#include "FindTheWayCore.h"
#include <algorithm>
#include <memory>
#include <vector>
#include <mutex>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include "FindTheWayFusion.h"
#include "AOE.h"
#include <codecvt>
#include <locale>

namespace {
struct Map : FindTheWay::FindTheWayFusion {
    FindTheWay::CoordSet ally, enemy, zoc;
    int32_t offset_x=0, offset_y=0;
    uint64_t updates=0; FTWMap handle=0; FTWUpdateCallback callback=nullptr; void* context=nullptr;
    template<class... T> explicit Map(T&&... args) : FindTheWayFusion(std::forward<T>(args)...) {
        SetUpdateMapCallBack([](void* p) { auto m=static_cast<Map*>(p); ++m->updates; auto callback=m->callback; if(callback) callback(m->context,m->handle); },this);
    }
};
std::wstring path_name(const char* name) {
    return name ? std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.from_bytes(name) : L"";
}
std::string utf8(const std::wstring& text) {
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(text);
}
std::mutex registry_mutex;
std::unordered_map<FTWMap, std::shared_ptr<Map>> maps;
FTWMap next_handle = 1;
FTWMap publish(std::shared_ptr<Map> map) {
    std::lock_guard lock(registry_mutex);
    if (next_handle == std::numeric_limits<FTWMap>::max())
        throw std::overflow_error("Handle space exhausted");
    const auto id = next_handle;
    map->handle=id;
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
uint32_t ftw_abi_version(void) { return 2; }
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
        || height>65535) return FTW_INVALID_ARGUMENT;
    return boundary([&] {
        const auto map=acquire(handle);
        map->SetIsometric(isometric!=0);
        // Keep signed offsets at the ABI boundary. The shared core stores size_t;
        // translating the origin avoids LLP64/LP64 unsigned arithmetic differences.
        map->SetGridSize(isometric ? Map::GetIsometricGridSize(width,height) : width,0,0);
        map->offset_x=x;map->offset_y=y;
    });
}
FTWStatus ftw_map_convert_coord(FTWMap handle, FTWCoord input, int32_t to_grid, FTWCoord* output) {
    if (!output || (to_grid!=0 && to_grid!=1)) return FTW_INVALID_ARGUMENT;
    return boundary([&] {
        const auto map=acquire(handle);
        FindTheWay::Coord coord{input.x,input.y};
        if(to_grid) {
            coord.x-=map->offset_x;coord.y-=map->offset_y;
            *output=narrow(map->GetGridCoord(coord));
        } else {
            coord=map->GetRealCoord(coord);coord.x+=map->offset_x;coord.y+=map->offset_y;
            *output=narrow(coord);
        }
    });
}


FTWStatus ftw_map_set_lists(FTWMap h, const FTWLists* lists) {
    if (!lists || (!lists->ally && lists->ally_count) || (!lists->enemy && lists->enemy_count)
        || (!lists->zoc && lists->zoc_count)) return FTW_INVALID_ARGUMENT;
    return boundary([&] {
        auto m=acquire(h);
        auto copy=[](const FTWCoord* p,uint64_t n) {
            if(n>INT32_MAX) throw std::invalid_argument("List too large");
            FindTheWay::CoordSet out; out.reserve(n);
            for(uint64_t i=0;i<n;++i) out.emplace_back(p[i].x,p[i].y);
            return out;
        };
        auto a=copy(lists->ally,lists->ally_count), e=copy(lists->enemy,lists->enemy_count), z=copy(lists->zoc,lists->zoc_count);
        m->ally=std::move(a); m->enemy=std::move(e); m->zoc=std::move(z);
    });
}
FTWStatus ftw_map_find_ex(FTWMap h, FTWCoord start, FTWCoord goal, uint32_t flags,
                         uint32_t ignore, const char* name, uint64_t* count) {
    if(!count || (flags&~7u)) return FTW_INVALID_ARGUMENT;
    return boundary([&] {
        auto m=acquire(h); const auto saved=path_name(name);
        FindTheWay::Coord a{start.x,start.y},b{goal.x,goal.y};
        if(flags&FTW_FIND_FORCE) m->ForceFind(a,b,flags&1,flags&2,&m->ally,&m->enemy,&m->zoc,ignore);
        else m->Find(a,b,flags&1,flags&2,&m->ally,&m->enemy,&m->zoc,ignore);
        if(!saved.empty()) m->SaveLastPath(saved);
        *count=m->GetPathValidity()?m->GetDistance():0;
    });
}
FTWStatus ftw_map_named_path(FTWMap h,const char* name,FTWCoord* output,uint64_t capacity,uint64_t* count) {
    if(!count || (!output && capacity)) return FTW_INVALID_ARGUMENT;
    bool small=false;
    auto status=boundary([&] {
        auto m=acquire(h); auto n=path_name(name); auto key=n.empty()?nullptr:&n;
        uint64_t size=m->GetPathValidity(key)?m->GetDistance(key):0;
        if(output && capacity>=size) {
            std::vector<FTWCoord> points; points.reserve(size);
            for(uint64_t i=0;i<size;++i) points.push_back(narrow({m->GetCoordAtPath(i,FindTheWay::CoordType::X,key),m->GetCoordAtPath(i,FindTheWay::CoordType::Y,key)}));
            std::copy(points.begin(),points.end(),output);
        } else if(output) small=true;
        *count=size;
    });
    return status==FTW_OK && small?FTW_BUFFER_TOO_SMALL:status;
}
FTWStatus ftw_map_area(FTWMap h,const FTWAreaOptions* o,uint64_t* rings,uint64_t* attack) {
    if(!o || !rings || !attack || o->range<0 || o->start_range<0 || o->attack_range<0) return FTW_INVALID_ARGUMENT;
    return boundary([&] {
        auto m=acquire(h); size_t extra=65536;
        FindTheWay::Coord start{o->start.x,o->start.y};
        if(o->line) m->CalcLineArea(start,o->range,o->start_range,&m->ally,&m->enemy,&m->zoc,o->ignore,o->attack);
        else m->CalcArea(start,o->range,o->start_range,&m->ally,&m->enemy,&m->zoc,o->ignore,o->attack,o->all_range,o->attack_range,&extra);
        *rings=m->GetArea().size(); *attack=extra;
    });
}
FTWStatus ftw_map_copy_area(FTWMap h,FTWAreaPoint* output,uint64_t capacity,uint64_t* count) {
    if(!count || (!output && capacity)) return FTW_INVALID_ARGUMENT;
    bool small=false;
    auto status=boundary([&] {
        auto m=acquire(h); const auto& area=m->GetArea();
        std::vector<FTWAreaPoint> points;
        for(size_t i=0;i<area.size();++i) for(auto c:area[i]) points.push_back({narrow(c),i});
        if(output && capacity>=points.size()) std::copy(points.begin(),points.end(),output);
        else if(output) small=true;
        *count=points.size();
    });
    return status==FTW_OK && small?FTW_BUFFER_TOO_SMALL:status;
}
FTWStatus ftw_map_line_range(FTWMap h,FTWCoord start,int32_t dir,int32_t range,uint32_t ignore,int32_t attack,int32_t* result) {
    if(!result || range<0) return FTW_INVALID_ARGUMENT;
    return boundary([&] { auto m=acquire(h); *result=m->GetAbleLineRange({start.x,start.y},range,static_cast<size_t>(dir),&m->ally,&m->enemy,&m->zoc,ignore,attack); });
}
FTWStatus ftw_generate_zoc(FTWCoord start,FTWCoord* output,uint64_t capacity,uint64_t* count) {
    if(!count || (!output && capacity)) return FTW_INVALID_ARGUMENT;
    bool small=false;
    auto status=boundary([&] {
        FindTheWay::CoordSet coords; Map::GenerateZoc({start.x,start.y},&coords,false);
        std::vector<FTWCoord> points; for(auto c:coords) points.push_back(narrow(c));
        if(output && capacity>=points.size()) std::copy(points.begin(),points.end(),output);
        else if(output) small=true;
        *count=points.size();
    }); return status==FTW_OK && small?FTW_BUFFER_TOO_SMALL:status;
}
FTWStatus ftw_map_aoe(FTWMap h,FTWCoord start,uint32_t dir,uint32_t type,uint32_t ignore,FTWCoord* output,uint64_t capacity,uint64_t* count) {
    if(!count || (!output && capacity)) return FTW_INVALID_ARGUMENT;
    bool small=false;
    auto status=boundary([&] {
        auto m=acquire(h); AOE::AOEClass aoe; FindTheWay::CoordSet coords;
        aoe.GetAOE({start.x,start.y},dir,type,&coords);
        auto flags=Map::ParseIgnoreFlag(ignore);
        std::vector<FTWCoord> points;
        for(auto c:coords) {
            if(m->GetMap(c.x,c.y,FindTheWay::MapType::MAP)==255) continue;
            if(!std::get<3>(flags) && std::find(m->ally.begin(),m->ally.end(),c)!=m->ally.end()) continue;
            if(!std::get<4>(flags) && std::find(m->enemy.begin(),m->enemy.end(),c)!=m->enemy.end()) continue;
            points.push_back(narrow(c));
        }
        if(output && capacity>=points.size()) std::copy(points.begin(),points.end(),output);
        else if(output) small=true;
        *count=points.size();
    }); return status==FTW_OK && small?FTW_BUFFER_TOO_SMALL:status;
}
FTWStatus ftw_map_stash(FTWMap h,int32_t enable) {
    return boundary([&] {auto m=acquire(h);m->SetStash(enable!=0);if(!enable)m->ClearStash();});
}
FTWStatus ftw_map_clear_costs(FTWMap h,int32_t cost,int32_t layer) {
    if(layer<1 || layer>2) return FTW_INVALID_ARGUMENT;
    return boundary([&]{acquire(h)->ClearMap(static_cast<FindTheWay::MapType>(layer),cost);});
}
FTWStatus ftw_map_dimensions(FTWMap h,int32_t* w,int32_t* height) {
    if(!w || !height) return FTW_INVALID_ARGUMENT;
    return boundary([&]{auto m=acquire(h); *w=m->GetMapWidth(); *height=m->GetMapHeight();});
}
FTWStatus ftw_map_display(FTWMap h,int32_t layer,int32_t show,const char* name,char* output,uint64_t capacity,uint64_t* count) {
    if(!count || (!output && capacity) || layer<0 || layer>2) return FTW_INVALID_ARGUMENT;
    bool small=false;
    auto status=boundary([&] {
        auto m=acquire(h); auto n=path_name(name);
        auto text=utf8(m->OutPutMapStr(static_cast<FindTheWay::MapType>(layer),show,n.empty()?nullptr:&n));
        if(output && capacity>=text.size()) std::copy(text.begin(),text.end(),output);
        else if(output) small=true;
        *count=text.size();
    });return status==FTW_OK && small?FTW_BUFFER_TOO_SMALL:status;
}
FTWStatus ftw_map_take_updates(FTWMap h,uint64_t* count) {
    if(!count) return FTW_INVALID_ARGUMENT;
    return boundary([&]{auto m=acquire(h);*count=m->updates;m->updates=0;});
}
FTWStatus ftw_map_set_callback(FTWMap h,FTWUpdateCallback callback,void* context) {
    return boundary([&]{auto m=acquire(h);m->callback=callback;m->context=context;});
}

FTWStatus ftw_map_sample_cost(FTWMap h,int32_t x,int32_t y,int32_t layer,int32_t* cost) {
    if(!cost || layer<0 || layer>2) return FTW_INVALID_ARGUMENT;
    return boundary([&]{auto m=acquire(h);*cost=m->GetMap(static_cast<size_t>(x),static_cast<size_t>(y),static_cast<FindTheWay::MapType>(layer));});
}

}
