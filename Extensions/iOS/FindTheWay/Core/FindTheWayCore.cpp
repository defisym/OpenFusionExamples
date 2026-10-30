#include "FindTheWayCore.h"
#include <algorithm>
#include <memory>
#include <vector>
#include <mutex>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include "FindTheWayFusion.h"

namespace {
using Map = FindTheWay::FindTheWayFusion;
std::mutex registry_mutex;
std::unordered_map<FTWMap, std::shared_ptr<Map>> maps;
FTWMap next_handle = 1;
FTWMap publish(std::shared_ptr<Map> map) {
    std::lock_guard lock(registry_mutex);
    if (next_handle == std::numeric_limits<FTWMap>::max())
        throw std::overflow_error("Handle space exhausted");
    const auto id = next_handle;
    maps.emplace(id, std::move(map));
    ++next_handle;
    return id;
}
FTWCoord narrow(FindTheWay::Coord coord) {
    if (coord.x < INT32_MIN || coord.x > INT32_MAX || coord.y < INT32_MIN || coord.y > INT32_MAX)
        throw std::overflow_error("Coordinate exceeds ABI range");
    return {static_cast<int32_t>(coord.x), static_cast<int32_t>(coord.y)};
}

struct InvalidHandle {};
std::shared_ptr<Map> acquire(FTWMap handle) {
    std::lock_guard lock(registry_mutex);
    const auto it = maps.find(handle);
    if (it == maps.end()) throw InvalidHandle{};
    return it->second;
}
template<class F> FTWStatus boundary(F&& fn) noexcept {
    try { fn(); return FTW_OK; }
    catch (const InvalidHandle&) { return FTW_INVALID_HANDLE; }
    catch (const std::bad_alloc&) { return FTW_OUT_OF_MEMORY; }
    catch (FindTheWay::Exception) { return FTW_INVALID_ARGUMENT; }
    catch (const std::invalid_argument&) { return FTW_INVALID_ARGUMENT; }
    catch (...) { return FTW_CORE_ERROR; }
}
FindTheWay::MapType validate(const Map& map, int32_t x, int32_t y, int32_t layer) {
    if (x < 0 || y < 0 || static_cast<size_t>(x) >= map.GetMapWidth()
        || static_cast<size_t>(y) >= map.GetMapHeight() || layer < 0 || layer > 2)
        throw std::invalid_argument("Invalid cell or layer");
    return static_cast<FindTheWay::MapType>(layer);
}
}
extern "C" {
uint32_t ftw_abi_version(void) { return 1; }
FTWStatus ftw_map_create(int32_t width, int32_t height, FTWMap* output) {
    if (!output || width <= 0 || height <= 0
        || static_cast<int64_t>(width)*height > INT32_MAX) return FTW_INVALID_ARGUMENT;
    return boundary([&] {
        auto map = std::make_shared<Map>(width, height);
        *output = publish(std::move(map));
    });
}
FTWStatus ftw_map_release(FTWMap handle) {
    return boundary([&] { std::lock_guard lock(registry_mutex); maps.erase(handle); });
}
FTWStatus ftw_map_set_cost(FTWMap handle, int32_t x, int32_t y, int32_t cost, int32_t layer) {
    return boundary([&] {
        const auto map = acquire(handle);
        const auto type = validate(*map, x, y, layer);
        map->SetMap(x, y, cost, type);
    });
}
FTWStatus ftw_map_get_cost(FTWMap handle, int32_t x, int32_t y, int32_t layer, int32_t* output) {
    if (!output) return FTW_INVALID_ARGUMENT;
    return boundary([&] {
        const auto map = acquire(handle);
        const auto type = validate(*map, x, y, layer);
        *output = map->GetMap(x, y, type);
    });
}
FTWStatus ftw_map_find_path(FTWMap handle, FTWCoord start, FTWCoord goal,
                            uint32_t flags, uint64_t* count) {
    if (!count || (flags & ~uint32_t(7))) return FTW_INVALID_ARGUMENT;
    return boundary([&] {
        const auto map = acquire(handle);
        validate(*map, start.x, start.y, FTW_MAP);
        validate(*map, goal.x, goal.y, FTW_MAP);
        const FindTheWay::Coord from{start.x, start.y}, to{goal.x, goal.y};
        if (flags & FTW_FIND_FORCE)
            map->ForceFind(from, to, flags & FTW_FIND_DIAGONAL, flags & FTW_FIND_CHECK_CORNERS);
        else
            map->Find(from, to, flags & FTW_FIND_DIAGONAL, flags & FTW_FIND_CHECK_CORNERS);
        *count = map->GetPathValidity() ? map->GetDistance() : 0;
    });
}
FTWStatus ftw_map_copy_path(FTWMap handle, FTWCoord* buffer, uint64_t capacity,
                            uint64_t* count) {
    if (!count || (!buffer && capacity)) return FTW_INVALID_ARGUMENT;
    bool too_small = false;
    const auto status = boundary([&] {
        const auto map = acquire(handle);
        const uint64_t required = map->GetPathValidity() ? map->GetDistance() : 0;
        if (buffer && capacity >= required) {
            std::vector<FTWCoord> points;
            points.reserve(required);
            for (uint64_t i = 0; i < required; ++i) {
                const auto x = map->GetCoordAtPath(i, FindTheWay::CoordType::X);
                const auto y = map->GetCoordAtPath(i, FindTheWay::CoordType::Y);
                if (x < INT32_MIN || x > INT32_MAX || y < INT32_MIN || y > INT32_MAX)
                    throw std::overflow_error("Path coordinate exceeds ABI range");
                points.push_back({static_cast<int32_t>(x), static_cast<int32_t>(y)});
            }
            std::copy(points.begin(), points.end(), buffer);
        } else if (buffer) too_small = true;
        *count = required;
    });
    return status == FTW_OK && too_small ? FTW_BUFFER_TOO_SMALL : status;
}

FTWStatus ftw_map_create_serialized(const char* bytes, uint64_t length, FTWMap* output) {
    if (!output || !bytes || !length || length > INT32_MAX) return FTW_INVALID_ARGUMENT;
    return boundary([&] {
        std::wstring encoded;
        encoded.reserve(static_cast<size_t>(length));
        for (uint64_t i=0; i<length; ++i) {
            const auto value=static_cast<unsigned char>(bytes[i]);
            if (!value || value>127) throw std::invalid_argument("Map format must be ASCII");
            encoded.push_back(value);
        }
        auto map=std::make_shared<Map>(encoded);
        if (map->GetMapWidth()>INT32_MAX || map->GetMapHeight()>INT32_MAX
            || map->GetMapWidth()*map->GetMapHeight()>INT32_MAX)
            throw std::invalid_argument("Map dimensions exceed ABI range");
        *output=publish(std::move(map));
    });
}
FTWStatus ftw_map_export(FTWMap handle, char* buffer, uint64_t capacity, uint64_t* length) {
    if (!length || (!buffer && capacity)) return FTW_INVALID_ARGUMENT;
    bool too_small=false;
    const auto status=boundary([&] {
        const auto map=acquire(handle);
        const auto encoded=map->GetMap();
        if (buffer && capacity>=encoded.size()) {
            for (wchar_t value : encoded) if (value<0 || value>127)
                throw std::logic_error("Non-ASCII map serialization");
            for (size_t i=0;i<encoded.size();++i) buffer[i]=static_cast<char>(encoded[i]);
        } else if (buffer) too_small=true;
        *length=encoded.size();
    });
    return status==FTW_OK && too_small ? FTW_BUFFER_TOO_SMALL : status;
}
FTWStatus ftw_map_set_grid(FTWMap handle, int32_t isometric, int32_t width,
                           int32_t height, int32_t x, int32_t y) {
    if ((isometric!=0 && isometric!=1) || width<=0 || height<=0 || width>65535
        || height>65535 || x<0 || y<0) return FTW_INVALID_ARGUMENT;
    return boundary([&] {
        const auto map=acquire(handle);
        map->SetIsometric(isometric!=0);
        map->SetGridSize(isometric ? Map::GetIsometricGridSize(width,height) : width,x,y);
    });
}
FTWStatus ftw_map_convert_coord(FTWMap handle, FTWCoord input, int32_t to_grid, FTWCoord* output) {
    if (!output || (to_grid!=0 && to_grid!=1)) return FTW_INVALID_ARGUMENT;
    return boundary([&] {
        const auto map=acquire(handle);
        const FindTheWay::Coord coord{input.x,input.y};
        *output=narrow(to_grid ? map->GetGridCoord(coord) : map->GetRealCoord(coord));
    });
}

}
