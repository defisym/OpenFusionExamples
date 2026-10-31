#include "Mixer.h"
#include <jni.h>
#include <codecvt>
#include <locale>
#include <mutex>
#include <unordered_map>
#include <stdexcept>
namespace {
std::mutex mutex;
std::weak_ptr<ofe::audio::Mixer> sharedMixer;
struct Client { uint64_t token; std::shared_ptr<ofe::audio::Mixer> engine; bool isolated; };
std::unordered_map<jlong, Client> clients;
jlong serial = 1;
std::string string(JNIEnv* env, jstring value) {
    if (!value) return {};
    auto data = env->GetStringChars(value, nullptr); if (!data) throw std::bad_alloc();
    std::u16string text(reinterpret_cast<const char16_t*>(data), env->GetStringLength(value));
    env->ReleaseStringChars(value, data);
    if (text.find(u'\0') != std::u16string::npos) throw std::invalid_argument("NUL in audio string");
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(text);
}
jstring string(JNIEnv* env, const std::string& value) {
    auto text = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(value);
    return env->NewString(reinterpret_cast<const jchar*>(text.data()), static_cast<jsize>(text.size()));
}
void fail(JNIEnv* env, const std::exception& error) {
    if (!env->ExceptionCheck()) env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), error.what());
}
ofe::audio::Mixer& get(jlong id) {
    auto it = clients.find(id);
    if (it == clients.end()) throw std::runtime_error("SDL Mixer client is closed");
    return *it->second.engine;
}
}
#define JNI_NAME(name) Java__1DeLib_SDLMixer_##name
namespace {
jlong create(JNIEnv* env, bool isolated) {
    try {
        std::lock_guard lock(mutex);
        auto token = OFEAudio_Acquire(); if (!token) throw std::runtime_error(SDL_GetError());
        try {
            auto engine = isolated ? nullptr : sharedMixer.lock();
            if (!engine) engine = std::make_shared<ofe::audio::Mixer>();
            if (!isolated) sharedMixer = engine;
            auto id = serial++; clients.emplace(id, Client{token, std::move(engine), isolated}); return id;
        } catch (...) { OFEAudio_Release(token); throw; }
    } catch (const std::exception& error) { fail(env, error); return 0; }
}
}
extern "C" JNIEXPORT jlong JNICALL JNI_NAME(nCreate)(JNIEnv* env, jclass) { return create(env, false); }
extern "C" JNIEXPORT jlong JNICALL JNI_NAME(nCreateIsolated)(JNIEnv* env, jclass) { return create(env, true); }
extern "C" JNIEXPORT void JNICALL JNI_NAME(nDestroy)(JNIEnv* env, jclass, jlong id) {
    try { std::lock_guard lock(mutex); auto it = clients.find(id); if (it == clients.end()) return;
        auto token = it->second.token; clients.erase(it); OFEAudio_Release(token);
    } catch (const std::exception& error) { fail(env, error); }
}
extern "C" JNIEXPORT jlong JNICALL JNI_NAME(nPlay)(JNIEnv* env, jclass, jlong id, jstring name, jstring path,
    jint channel, jboolean exclusive, jint loops, jint fade) {
    try { std::lock_guard lock(mutex); return get(id).play(string(env, name), string(env, path), channel, exclusive, loops, fade); }
    catch (const std::exception& error) { fail(env, error); return 0; }
}
extern "C" JNIEXPORT void JNICALL JNI_NAME(nAction)(JNIEnv* env, jclass, jlong id, jint op,
    jint channel, jboolean exclusive, jdouble a, jdouble b, jstring text) {
    try { std::lock_guard lock(mutex); get(id).action(op, channel, exclusive, a, b, string(env, text)); }
    catch (const std::exception& error) { fail(env, error); }
}
extern "C" JNIEXPORT jboolean JNICALL JNI_NAME(nCondition)(JNIEnv* env, jclass, jlong id, jint op,
    jint channel, jboolean exclusive, jstring name) {
    try { std::lock_guard lock(mutex); return get(id).condition(op, channel, exclusive, string(env, name)); }
    catch (const std::exception& error) { fail(env, error); return false; }
}
extern "C" JNIEXPORT jdouble JNICALL JNI_NAME(nNumber)(JNIEnv* env, jclass, jlong id, jint op,
    jint channel, jboolean exclusive, jstring name) {
    try { std::lock_guard lock(mutex); return get(id).number(op, channel, exclusive, string(env, name)); }
    catch (const std::exception& error) { fail(env, error); return 0; }
}
extern "C" JNIEXPORT jstring JNICALL JNI_NAME(nName)(JNIEnv* env, jclass, jlong id, jint channel) {
    try { std::lock_guard lock(mutex); return string(env, get(id).channelName(channel)); }
    catch (const std::exception& error) { fail(env, error); return nullptr; }
}
extern "C" JNIEXPORT void JNICALL JNI_NAME(nCache)(JNIEnv* env, jclass, jlong id, jstring name, jstring path, jboolean replace) {
    try { std::lock_guard lock(mutex); get(id).cache(string(env, name), string(env, path), replace); }
    catch (const std::exception& error) { fail(env, error); }
}
extern "C" JNIEXPORT void JNICALL JNI_NAME(nErase)(JNIEnv* env, jclass, jlong id, jstring name) {
    try { std::lock_guard lock(mutex); get(id).erase(string(env, name)); }
    catch (const std::exception& error) { fail(env, error); }
}
extern "C" JNIEXPORT jstring JNICALL JNI_NAME(nMemoryName)(JNIEnv* env, jclass, jlong id,
    jstring access, jstring binary, jlong offset, jlong size) {
    try { std::lock_guard lock(mutex); return string(env, get(id).memoryName(string(env, access), string(env, binary), offset, size)); }
    catch (const std::exception& error) { fail(env, error); return nullptr; }
}
extern "C" JNIEXPORT jlongArray JNICALL JNI_NAME(nUpdate)(JNIEnv* env, jclass, jlong id) {
    try {
        std::lock_guard lock(mutex); auto& engine = get(id); engine.update(); auto voices = engine.liveVoices();
        std::vector<jlong> ids(voices.begin(), voices.end()); auto out = env->NewLongArray(static_cast<jsize>(ids.size()));
        if (out && !ids.empty()) env->SetLongArrayRegion(out, 0, static_cast<jsize>(ids.size()), ids.data());
        return out;
    } catch (const std::exception& error) { fail(env, error); return nullptr; }
}
extern "C" JNIEXPORT void JNICALL JNI_NAME(nSuspend)(JNIEnv* env, jclass, jlong id, jboolean suspend) {
    try { std::lock_guard lock(mutex); auto& engine = get(id); const auto& client = clients.at(id);
        if (client.isolated) engine.setSuspended(suspend);
        OFEAudio_SetSuspended(client.token, suspend); }
    catch (const std::exception& error) { fail(env, error); }
}
