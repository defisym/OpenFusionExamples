#include "FFMpeg.h"
#include "CopyAdapter.h"
#include "AudioRuntime.h"
#include <jni.h>
#include <cstring>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <unordered_map>
#ifdef __ANDROID__
#include <android/log.h>
#include <cstdio>
extern "C" {
#include <libavcodec/jni.h>
}
#endif
namespace {
std::mutex registryMutex;
struct Session {
    std::unique_ptr<FFMpeg> player;
    std::vector<uint8_t> pixels;
    bool hasFrame = false;
    uint64_t client = 0;
    void* pcm = nullptr;
    bool playing = false, suspended = false;
    float volume = 1;
    double speed = 1;
    ~Session() { if (pcm) OFEAudio_DestroyPCM(pcm); if (client) OFEAudio_Release(client); }
    FrameDataCallBack frame() {
        return [this](const unsigned char* data, int stride, int height) {
            if (data) {
                const int width = player->get_width();
                pixels.resize(static_cast<size_t>(width) * height * 4);
                for (int row = 0; row < height; ++row)
                    std::memcpy(pixels.data() + static_cast<size_t>(row) * width * 4,
                        data + static_cast<ptrdiff_t>(row) * stride, width * 4);
            }
            hasFrame = true;
        };
    }
    void initAudio() {
        if (!player->has_audio()) return;
        client = OFEAudio_Acquire();
        if (!client) throw std::runtime_error("Cannot acquire shared Mixer X output");
        pcm = OFEAudio_CreatePullPCM([](void* user, void* bytes, int count) {
            auto& self = *static_cast<Session*>(user);
            self.player->audio_fillData(static_cast<uint8_t*>(bytes), count, std::memset,
                [](void* dst, const void* src, size_t size, int volume) {
                    SDL_MixAudioFormat(static_cast<uint8_t*>(dst), static_cast<const uint8_t*>(src),
                        AUDIO_S16SYS, static_cast<Uint32>(size), volume);
                });
        }, this);
        state();
    }
    void state() {
        player->set_pause(!playing || suspended);
        player->set_sdl_volume(static_cast<int>(std::clamp(volume, 0.f, 1.f) * SDL_MIX_MAXVOLUME));
        player->set_audioTempo(static_cast<float>(speed));
        if (!pcm) return;
        OFEAudio_PCMState(pcm, playing && !suspended, 1, 1);
        OFEAudio_SetSuspended(client, suspended || !playing);
    }
};
std::unordered_map<jlong, std::shared_ptr<Session>> players;
jlong nextId = 1;
std::shared_ptr<Session> get(jlong id) {
    std::lock_guard lock(registryMutex);
    auto it = players.find(id);
    if (it == players.end()) throw std::runtime_error("FFmpeg player is closed");
    return it->second;
}
std::string utf8(JNIEnv* env, jstring value) {
    if (!value) return {};
    // FFmpeg filenames are UTF-8, not JNI modified UTF-8.
    const jchar* chars = env->GetStringChars(value, nullptr);
    if (!chars) throw std::bad_alloc();
    std::string out;
    for (jsize i = 0, size = env->GetStringLength(value); i < size; ++i) {
        uint32_t c = chars[i];
        if (c >= 0xd800 && c <= 0xdbff && i + 1 < size && chars[i+1] >= 0xdc00 && chars[i+1] <= 0xdfff)
            c = 0x10000 + ((c - 0xd800) << 10) + (chars[++i] - 0xdc00);
        else if (c >= 0xd800 && c <= 0xdfff) c = 0xfffd;
        if (!c) { env->ReleaseStringChars(value, chars); throw std::runtime_error("NUL in FFmpeg filename"); }
        if (c < 0x80) out += static_cast<char>(c);
        else if (c < 0x800) { out += static_cast<char>(0xc0 | (c >> 6)); out += static_cast<char>(0x80 | (c & 63)); }
        else {
            if (c >= 0x10000) out += static_cast<char>(0xf0 | (c >> 18));
            out += static_cast<char>((c >= 0x10000 ? 0x80 : 0xe0) | ((c >> 12) & (c >= 0x10000 ? 63 : 15)));
            out += static_cast<char>(0x80 | ((c >> 6) & 63)); out += static_cast<char>(0x80 | (c & 63));
        }
    }
    env->ReleaseStringChars(value, chars); return out;
}
void fail(JNIEnv* env, const char* message) {
    if (!env->ExceptionCheck()) env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), message);
}
}
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
#ifdef __ANDROID__
    if (av_jni_set_java_vm(vm, nullptr) < 0) return JNI_ERR;
    av_log_set_callback([](void*, int level, const char* format, va_list args) {
        if (level > AV_LOG_WARNING) return;
        char text[1024]; vsnprintf(text, sizeof(text), format, args);
        __android_log_write(level <= AV_LOG_ERROR ? ANDROID_LOG_ERROR : ANDROID_LOG_WARN, "OFE.FFMpeg.Native", text);
    });
#else
    (void)vm;
#endif
    return JNI_VERSION_1_6;
}
#define JNI_NAME(name) Java__1DeLib_FFMpegPlayer_##name
extern "C" JNIEXPORT jlong JNICALL JNI_NAME(nOpen)(JNIEnv* env, jclass, jstring path,
    jobject surface, jint threads, jstring video, jstring audio, jboolean mute) {
    try {
        auto session = std::make_shared<Session>();
        FFMpegOptions options;
        options.platformSurface = surface;
        options.outputFormat = AV_PIX_FMT_RGBA;
        options.SetThreadCount(threads);
        options.videoCodecName = utf8(env, video);
        options.audioCodecName = utf8(env, audio);
        if (mute) options.flag |= FFMpegFlag_ForceNoAudio;
        if (surface) options.flag |= AV_HWDEVICE_TYPE_MEDIACODEC | FFMpegFlag_CopyToTexture;
        session->player = std::make_unique<FFMpeg>(utf8(env, path), options);
        session->initAudio();
        std::lock_guard lock(registryMutex); auto id = nextId++; players.emplace(id, std::move(session)); return id;
    } catch (const std::exception& e) { fail(env, e.what()); return 0; }
}
extern "C" JNIEXPORT void JNICALL JNI_NAME(nClose)(JNIEnv*, jclass, jlong id) {
    std::shared_ptr<Session> removed;
    { std::lock_guard lock(registryMutex); auto it = players.find(id);
      if (it != players.end()) { removed = std::move(it->second); players.erase(it); } }
}
extern "C" JNIEXPORT jlongArray JNICALL JNI_NAME(nInfo)(JNIEnv* env, jclass, jlong id) {
    try {
        auto session = get(id); auto p = session->player.get();
        jlong values[] = {p->get_width(), p->get_height(), p->get_videoDuration(), p->get_videoPosition(), p->get_hwDecodeState(), p->get_finishState(), session->hasFrame};
        auto out = env->NewLongArray(7); if (out) env->SetLongArrayRegion(out, 0, 7, values); return out;
    } catch (const std::exception& e) { fail(env, e.what()); return nullptr; }
}
extern "C" JNIEXPORT jint JNICALL JNI_NAME(nNext)(JNIEnv* env, jclass, jlong id, jlong target) {
    try { auto session = get(id); auto& p = *session->player;
        bool updated = false;
        const int result = p.get_nextFrame([&](const unsigned char* data, int stride, int height) {
            session->frame()(data, stride, height); updated = true;
        }, target != INT64_MAX);
        if (result < 0 && result != END_OF_QUEUE) throw FFMpegException(FFMpegException_DecodeFailed);
        return updated ? 1 : (target == INT64_MAX ? p.get_finishState() : p.playback_finished()) ? 2 : 0; }
    catch (const std::exception& e) { fail(env, e.what()); return -1; }
}
extern "C" JNIEXPORT void JNICALL JNI_NAME(nSeek)(JNIEnv* env, jclass, jlong id, jlong ms, jint flags, jboolean accurate) {
    try { auto session = get(id);
        if (session->pcm) OFEAudio_PCMState(session->pcm, 0, 1, 1);
        int result = session->player->seek(ms, flags, accurate, session->frame());
        if (session->pcm) OFEAudio_PCMReset(session->pcm, std::max<jlong>(0, ms));
        session->state();
        if (result < 0) throw std::runtime_error("FFmpeg seek failed"); }
    catch (const std::exception& e) { fail(env, e.what()); }
}
extern "C" JNIEXPORT void JNICALL JNI_NAME(nCopy)(JNIEnv* env, jclass, jlong id, jobject buffer, jboolean premultiply) {
    try { auto session = get(id); const auto& pixels = session->pixels; auto data = env->GetDirectBufferAddress(buffer);
        auto capacity = env->GetDirectBufferCapacity(buffer);
        if (!data || capacity < 0 || static_cast<uint64_t>(capacity) < pixels.size()) throw std::runtime_error("RGBA buffer too small");
        AndroidCopyTarget target;
        target.pixels = static_cast<uint8_t*>(data); target.capacity = static_cast<size_t>(capacity);
        target.width = session->player->get_width(); target.height = session->player->get_height();
        target.stride = target.width * 4; target.premultiply = premultiply;
        if (!CopyAdapterFactory(AV_HWDEVICE_TYPE_NONE)->CopyTexture(target, pixels.data()))
            throw std::runtime_error("Cannot copy decoded frame");
    } catch (const std::exception& e) { fail(env, e.what()); }
}
extern "C" JNIEXPORT void JNICALL JNI_NAME(nState)(JNIEnv* env, jclass, jlong id, jboolean playing,
    jboolean suspended, jfloat volume, jdouble speed) {
    try { auto p = get(id); p->playing = playing; p->suspended = suspended; p->volume = volume; p->speed = speed; p->state(); }
    catch (const std::exception& e) { fail(env, e.what()); }
}
extern "C" JNIEXPORT jdouble JNICALL JNI_NAME(nClock)(JNIEnv* env, jclass, jlong id) {
    try { auto p = get(id); return p->player->get_playbackClock(); }
    catch (const std::exception& e) { fail(env, e.what()); return -1.; }
}
extern "C" JNIEXPORT jboolean JNICALL JNI_NAME(nPending)(JNIEnv* env, jclass, jlong id) {
    try { auto p = get(id); return !p->player->audio_finished(); }
    catch (const std::exception& e) { fail(env, e.what()); return false; }
}
