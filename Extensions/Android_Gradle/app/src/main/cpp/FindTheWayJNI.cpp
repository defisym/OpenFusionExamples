#include <jni.h>
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include "FindTheWay.h"
#include "FindTheWayFusion.h"
#include "AOE.h"

namespace {
using namespace FindTheWay;
// Opaque IDs rather than exposed pointers: a released/stale handle can never dereference freed memory.
std::mutex registryMutex;
std::unordered_map<jlong, std::shared_ptr<FindTheWayFusion>> maps;
jlong nextID = 1;
struct PendingJavaException {};

void checkJava(JNIEnv* env) { if (env->ExceptionCheck()) { throw PendingJavaException{}; } }
void report(JNIEnv* env, const char* type, const char* message) {
    if (!env->ExceptionCheck()) { env->ThrowNew(env->FindClass(type), message); }
}
void translateException(JNIEnv* env) {
    try { throw; }
    catch (const PendingJavaException&) { /* retain the Java exception */ }
    catch (const std::bad_alloc&) { report(env, "java/lang/OutOfMemoryError", "FindTheWay native allocation failed"); }
    catch (const std::exception& e) { report(env, "java/lang/IllegalArgumentException", e.what()); }
    catch (Exception) { report(env, "java/lang/IllegalArgumentException", "Invalid FindTheWay map size or data"); }
    catch (...) { report(env, "java/lang/IllegalStateException", "FindTheWay native failure"); }
}
std::shared_ptr<FindTheWayFusion> lookup(jlong id) {
    std::lock_guard lock(registryMutex);
    auto it = maps.find(id);
    if (it == maps.end()) { throw std::logic_error("FindTheWay map is closed"); }
    return it->second;
}
std::vector<jint> ints(JNIEnv* env, jintArray array) {
    if (!array) { return {}; }
    std::vector<jint> values(env->GetArrayLength(array));
    env->GetIntArrayRegion(array, 0, static_cast<jsize>(values.size()), values.data());
    checkJava(env);
    return values;
}
jintArray result(JNIEnv* env, const std::vector<jint>& values) {
    if (values.size() > static_cast<size_t>(INT32_MAX)) { throw std::length_error("Result too large"); }
    auto array = env->NewIntArray(static_cast<jsize>(values.size()));
    checkJava(env);
    if (!values.empty()) { env->SetIntArrayRegion(array, 0, static_cast<jsize>(values.size()), values.data()); }
    checkJava(env);
    return array;
}
CoordSet coords(const std::vector<jint>& values, size_t begin = 0, size_t end = SIZE_MAX) {
    end = (std::min)(end, values.size());
    if (begin > end || (end - begin) % 2) { throw std::invalid_argument("Invalid coordinate array"); }
    CoordSet output;
    for (size_t i = begin; i < end; i += 2) { output.emplace_back(values[i], values[i + 1]); }
    return output;
}
std::vector<jint> flat(const CoordSet& values) {
    std::vector<jint> output;
    for (const auto& value : values) {
        output.push_back(static_cast<jint>(value.x));
        output.push_back(static_cast<jint>(value.y));
    }
    return output;
}
// Java strings are UTF-16; wchar_t is UTF-32 on Android/Linux and UTF-16 on Windows.
// Explicit conversion preserves supplementary characters and embedded NUL path names.
std::wstring wide(JNIEnv* env, jstring input) {
    if (!input) { return {}; }
    const auto length = env->GetStringLength(input);
    const jchar* data = env->GetStringChars(input, nullptr);
    checkJava(env);
    struct Release { JNIEnv* env; jstring input; const jchar* data;
        ~Release() { env->ReleaseStringChars(input, data); } } release{env, input, data};
    std::wstring output;
    for (jsize i = 0; i < length; ++i) {
        uint32_t cp = data[i];
        if constexpr (sizeof(wchar_t) > 2) {
            if (cp >= 0xD800 && cp <= 0xDBFF && i + 1 < length
                    && data[i + 1] >= 0xDC00 && data[i + 1] <= 0xDFFF) {
                cp = 0x10000 + ((cp - 0xD800) << 10) + data[++i] - 0xDC00;
            }
        }
        output.push_back(static_cast<wchar_t>(cp));
    }
    return output;
}
jstring string(JNIEnv* env, const std::wstring& input) {
    std::vector<jchar> output;
    for (wchar_t value : input) {
        uint32_t cp = static_cast<uint32_t>(value);
        if constexpr (sizeof(wchar_t) > 2) {
            if (cp > 0xFFFF) {
                cp -= 0x10000;
                output.push_back(static_cast<jchar>(0xD800 + (cp >> 10)));
                output.push_back(static_cast<jchar>(0xDC00 + (cp & 0x3FF)));
                continue;
            }
        }
        output.push_back(static_cast<jchar>(cp));
    }
    auto value = env->NewString(output.data(), static_cast<jsize>(output.size()));
    checkJava(env);
    return value;
}
MapType mapType(jint value) {
    if (value < 0 || value >= MAPTYPENUM) { throw std::invalid_argument("Invalid map type"); }
    return static_cast<MapType>(value);
}
// Keep the map alive through nested Fusion callbacks, including close/recreate during a callback.
// The callback holds only this invocation's JNI local reference, never a cached JNIEnv/global owner.
thread_local std::unordered_map<FindTheWayClass*, std::vector<UpdateMapCallBack>> callbackStack;
struct CallbackScope {
    FindTheWayClass& map;
    CallbackScope(FindTheWayClass& map, JNIEnv* env, jobject owner) : map(map) {
        auto clazz = env->GetObjectClass(owner);
        auto method = env->GetMethodID(clazz, "onMapUpdated", "()V");
        env->DeleteLocalRef(clazz);
        checkJava(env);
        auto fn = [env, owner, method](void*) {
            env->CallVoidMethod(owner, method);
            checkJava(env);
        };
        callbackStack[&map].emplace_back(fn);
        map.SetUpdateMapCallBack(fn, nullptr);
    }
    ~CallbackScope() {
        auto& stack = callbackStack.at(&map);
        stack.pop_back();
        if (stack.empty()) {
            map.SetUpdateMapCallBack(nullptr, nullptr);
            callbackStack.erase(&map);
        } else {
            map.SetUpdateMapCallBack(stack.back(), nullptr);
        }
    }
};
}

extern "C" JNIEXPORT jlong JNICALL Java__1DeLib_FindTheWayClass_nCreate(
        JNIEnv* env, jclass, jint width, jint height, jstring encoded) {
    try {
        std::shared_ptr<FindTheWayFusion> map;
        if (encoded) { map = std::make_shared<FindTheWayFusion>(wide(env, encoded)); }
        else {
            if (width <= 0 || height <= 0 || static_cast<int64_t>(width) * height > INT32_MAX) {
                throw std::invalid_argument("Invalid map dimensions");
            }
            map = std::make_shared<FindTheWayFusion>(width, height);
        }
        std::lock_guard lock(registryMutex);
        const jlong id = nextID++;
        maps.emplace(id, std::move(map));
        return id;
    } catch (...) { translateException(env); return 0; }
}
extern "C" JNIEXPORT void JNICALL Java__1DeLib_FindTheWayClass_nDestroy(JNIEnv*, jclass, jlong id) {
    std::lock_guard lock(registryMutex);
    maps.erase(id);
}
extern "C" JNIEXPORT jintArray JNICALL Java__1DeLib_FindTheWayClass_nCall(
        JNIEnv* env, jobject owner, jlong id, jint operation, jintArray args, jstring pathName,
        jintArray allies, jintArray enemies, jintArray zones) {
    try {
        auto mapOwner = lookup(id);
        auto& map = *mapOwner;
        CallbackScope callback(map, env, owner);
        auto a = ints(env, args);
        auto v = [&](size_t i) -> jint { return a.at(i); };
        auto ally = coords(ints(env, allies)), enemy = coords(ints(env, enemies)), zoc = coords(ints(env, zones));
        auto name = wide(env, pathName);
        const auto* path = name.empty() ? nullptr : &name;
        std::vector<jint> output;
        switch (operation) {
        case 0: map.SetIsometric(v(0) != 0); break;
        case 1: map.SetGridSize(static_cast<uint32_t>(v(0)), v(1), v(2)); break;
        case 2: {
            Coord c{v(1), v(2)}, value;
            switch (v(0)) {
            case 0: value = map.GetGridCoord(c); break;
            case 1: value = map.GetRealCoord(c); break;
            case 2: value = map.GetTraditionalGridCoord(c); break;
            case 3: value = map.GetTraditionalRealCoord(c); break;
            case 4: value = map.GetIsometricGridCoord(c); break;
            case 5: value = map.GetIsometricRealCoord(c); break;
            default: throw std::invalid_argument("Invalid conversion");
            }
            output = {static_cast<jint>(value.x), static_cast<jint>(value.y)}; break;
        }
        case 3: map.SetMap(v(0), v(1), v(2), mapType(v(3)), v(4)); break;
        case 4: map.ClearMap(mapType(v(0)), v(1), v(2)); break;
        case 5: output = {map.GetMap(v(0), v(1), mapType(v(2)), v(3))}; break;
        case 6: map.UpdateMap(); break;
        case 7:
            if (v(7)) { map.ForceFind({v(0), v(1)}, {v(2), v(3)}, v(4), v(5), &ally, &enemy, &zoc, v(6)); }
            else { map.Find({v(0), v(1)}, {v(2), v(3)}, v(4), v(5), &ally, &enemy, &zoc, v(6)); }
            break;
        case 8: output = {map.GetPathValidity(path)}; break;
        case 9: output = {static_cast<jint>(map.GetDistance(path))}; break;
        case 10: output = {static_cast<jint>(map.GetCoordAtPath(v(0), v(1) == 0 ? CoordType::X : CoordType::Y, path))}; break;
        case 11: map.SaveLastPath(name); break;
        case 12: output = {static_cast<jint>(map.GetMapWidth()), static_cast<jint>(map.GetMapHeight()),
            static_cast<jint>(map.GetGridWidth()), static_cast<jint>(map.GetGridHeight()),
            static_cast<jint>(map.GetGridOffsetX()), static_cast<jint>(map.GetGridOffsetY())}; break;
        case 13: output = {static_cast<jint>(map.GetAbleLineRange({v(0), v(1)}, v(2), v(3), &ally, &enemy, &zoc, v(4), v(5)))}; break;
        case 14: map.CalcLineArea({v(0), v(1)}, v(2), v(3), &ally, &enemy, &zoc, v(4), v(5)); break;
        case 15: {
            size_t extra = DEFAULT_EXTRARANGESTARTPOS;
            map.CalcArea({v(0), v(1)}, v(2), v(3), &ally, &enemy, &zoc, v(4), v(5), v(6), v(7), &extra, v(8));
            output = {static_cast<jint>(extra)}; break;
        }
        case 16:
            for (const auto& ring : map.GetArea()) {
                output.push_back(static_cast<jint>(ring.size()));
                auto values = flat(ring);
                output.insert(output.end(), values.begin(), values.end());
            }
            break;
        case 17:
            map.IterateMap(MapType::MAP, [&](size_t x, size_t y, MapCost) {
                auto c = v(0) ? map.GetRealCoord({x, y}) : Coord{x, y};
                output.push_back(static_cast<jint>(c.x));
                output.push_back(static_cast<jint>(c.y));
            });
            break;
        case 18:
            switch (v(0)) {
            case 0: output = {map.GetStash()}; break;
            case 1: map.SetStash(v(1)); break;
            case 2: map.ClearStash(); break;
            case 3: output = {static_cast<jint>(map.GetPathStashSize())}; break;
            case 4: output = {static_cast<jint>(map.GetAreaStashSize())}; break;
            default: throw std::invalid_argument("Invalid stash operation");
            }
            break;
        case 19: {
            const auto cell = map.GetMapData(v(0), v(1), mapType(v(2)), v(3));
            output = {cell.GetCost(), static_cast<jint>(cell.GetTerrainType()), cell.IsObstacle()};
            break;
        }
        case 20:
            if (v(2) < 0 || v(2) > 255 || (v(4) != 0 && v(4) != 1)) {
                throw std::invalid_argument("Invalid map cell");
            }
            map.SetMapData(v(0), v(1), MapData(static_cast<MapCost>(v(2)),
                static_cast<TerrainType>(v(3)), v(4) != 0), mapType(v(5)), v(6));
            break;
        default: throw std::invalid_argument("Unknown FindTheWay operation");
        }
        return result(env, output);
    } catch (...) { translateException(env); return nullptr; }
}
extern "C" JNIEXPORT jstring JNICALL Java__1DeLib_FindTheWayClass_nText(
        JNIEnv* env, jobject owner, jlong id, jint operation, jintArray args, jstring value,
        jintArray, jintArray, jintArray) {
    try {
        auto mapOwner = lookup(id);
        auto& map = *mapOwner;
        CallbackScope callback(map, env, owner);
        auto a = ints(env, args);
        auto name = wide(env, value);
        switch (operation) {
        case 0: return string(env, map.GetMap());
        case 1: return string(env, map.OutPutMapStr(mapType(a.at(0)), a.at(1), name.empty() ? nullptr : &name));
        case 3: map.SetMap(name); return string(env, L"");
        default: throw std::invalid_argument("Unknown text operation");
        }
    } catch (...) { translateException(env); return nullptr; }
}
extern "C" JNIEXPORT jintArray JNICALL Java__1DeLib_FindTheWayClass_nGeometry(
        JNIEnv* env, jclass, jint operation, jintArray args, jintArray input) {
    try {
        auto a = ints(env, args);
        std::vector<jint> output;
        switch (operation) {
        case 0: output = {static_cast<jint>(FindTheWayClass::GetIsometricGridSize(a.at(0), a.at(1)))}; break;
        case 1: {
            if (a.at(0) < 0 || a.at(1) < 0) { throw std::invalid_argument("Invalid frame dimensions"); }
            auto [w, h] = FindTheWayClass::CalcMapSize(a[0], a[1], static_cast<uint32_t>(a.at(2)), a.at(3));
            output = {static_cast<jint>(w), static_cast<jint>(h)}; break;
        }
        case 2: {
            auto values = ints(env, input);
            auto count = static_cast<size_t>(values.at(0));
            if (count > values.size() - 1) { throw std::invalid_argument("Invalid coordinate count"); }
            auto starts = coords(values, 1, 1 + count), target = coords(values, 1 + count);
            if (a.at(0) == 0) { FindTheWayClass::GenerateSet(starts.at(0), &target, a.at(1), a.at(2)); }
            else { FindTheWayClass::GenerateZoc(starts, &target, a.at(1), a.at(2)); }
            output = flat(target); break;
        }
        case 3: {
            AOE::AOEClass aoe;
            CoordSet target;
            aoe.GetAOE({a.at(0), a.at(1)}, a.at(2), a.at(3), &target);
            output = flat(target); break;
        }
        default: throw std::invalid_argument("Unknown geometry operation");
        }
        return result(env, output);
    } catch (...) { translateException(env); return nullptr; }
}
