#include "AudioRuntime.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <mutex>
#include <unordered_map>
#include <string>
#ifdef __ANDROID__
#include <jni.h>
#endif
namespace {
std::mutex mutex;
std::unordered_map<uint64_t, bool> clients;
uint64_t serial = 1;
bool ownsMixer = false, ownsAudio = false, ownsTimer = false, focused = true;
uint64_t failAcquire() {
    const std::string error = SDL_GetError();
    if (ownsMixer && Mix_QuerySpec(nullptr,nullptr,nullptr)) Mix_CloseAudio();
    if (ownsAudio) SDL_QuitSubSystem(SDL_INIT_AUDIO);
    if (ownsTimer) SDL_QuitSubSystem(SDL_INIT_TIMER);
    ownsMixer = ownsAudio = ownsTimer = false;
    SDL_SetError("%s", error.c_str()); return 0;
}
void updatePause() {
    bool suspended = !clients.empty();
    for (const auto& client : clients) suspended &= client.second;
    if (!clients.empty()) Mix_PauseAudio(!focused || suspended);
}
}
extern "C" unsigned OFEAudio_ABIVersion() { return 2; }
extern "C" uint64_t OFEAudio_Acquire() {
    std::lock_guard lock(mutex);
    if (clients.empty()) {
        SDL_SetMainReady();
#ifdef __ANDROID__
        // The stock JNI-backed driver works with Fusion's Activity on API 21+.
        // Explicit caller/environment hints retain priority over this default.
        SDL_SetHintWithPriority(SDL_HINT_AUDIODRIVER, "android", SDL_HINT_DEFAULT);
#endif
        ownsAudio = !SDL_WasInit(SDL_INIT_AUDIO); ownsTimer = !SDL_WasInit(SDL_INIT_TIMER);
        Uint32 flags = (ownsAudio ? SDL_INIT_AUDIO : 0) | (ownsTimer ? SDL_INIT_TIMER : 0);
        if (flags && SDL_InitSubSystem(flags) < 0) return failAcquire();
        ownsMixer = Mix_QuerySpec(nullptr, nullptr, nullptr) == 0;
        if (ownsMixer && Mix_OpenAudioDevice(44100, AUDIO_S16SYS, 2, 1024, nullptr, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE) < 0) {
            return failAcquire();
        }
        int frequency, channels; Uint16 format;
        Mix_QuerySpec(&frequency, &format, &channels);
        if (format != AUDIO_S16SYS || channels != 2) {
            SDL_SetError("OFE audio requires signed 16-bit stereo Mixer X output");
            return failAcquire();
        }
    }
    auto id = serial++; clients.emplace(id, false); updatePause(); return id;
}
extern "C" void OFEAudio_Release(uint64_t id) {
    std::lock_guard lock(mutex);
    if (!clients.erase(id)) return;
    if (clients.empty()) {
        if (ownsMixer) Mix_CloseAudio();
        if (ownsAudio) SDL_QuitSubSystem(SDL_INIT_AUDIO);
        if (ownsTimer) SDL_QuitSubSystem(SDL_INIT_TIMER);
        ownsMixer = ownsAudio = ownsTimer = false;
    } else updatePause();
}
extern "C" void OFEAudio_SetSuspended(uint64_t id, int suspended) {
    std::lock_guard lock(mutex);
    auto it = clients.find(id); if (it != clients.end()) it->second = suspended != 0;
    updatePause();
}
extern "C" void OFEAudio_SetFocus(int value) { std::lock_guard lock(mutex); focused = value != 0; updatePause(); }
extern "C" int OFEAudio_GetSpec(int* frequency, uint16_t* format, int* channels) { return Mix_QuerySpec(frequency, format, channels); }
#ifdef __ANDROID__
extern "C" JNIEXPORT void JNICALL Java__1DeLib_SDLAudioRuntime_nFocus(JNIEnv*, jclass, jboolean focus) { OFEAudio_SetFocus(focus); }
#endif
