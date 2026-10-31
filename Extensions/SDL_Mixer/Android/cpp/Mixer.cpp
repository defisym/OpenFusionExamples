#include "Mixer.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
namespace ofe::audio {
namespace {
std::atomic<uint64_t> epochSerial{1};
void channelValid(int channel) { if (channel < 0 || channel > 4095) throw std::invalid_argument("Channel must be in 0..4095"); }
void checked(int result) { if (result < 0) throw std::runtime_error(Mix_GetError()); }
int volume(int value) { return std::clamp(value, 0, 100); }
}
Mixer::Mixer() : epoch(epochSerial.fetch_add(1, std::memory_order_relaxed)) {}
Voice::~Voice() {
    if (music) { Mix_HaltMusicStream(music); Mix_FreeMusic(music); }
    if (rw) SDL_RWclose(rw);
}
Mixer::~Mixer() { voices.clear(); }
Settings& Mixer::settings(int channel, bool excl) { channelValid(channel); return (excl ? exclusiveSettings : mixingSettings)[channel]; }
Voice* Mixer::current(int channel) {
    auto it = exclusive.find(channel); if (it == exclusive.end()) return nullptr;
    for (auto& voice : voices) if (voice->id == it->second) return voice.get();
    return nullptr;
}
void Mixer::apply(Voice& voice) {
    const auto& state = settings(voice.channel, voice.exclusive);
    Mix_VolumeMusicStream(voice.music, state.volume * master * MIX_MAX_VOLUME / 10000);
    if (state.spatialMode == 1) {
        if (!Mix_SetMusicEffectPanning(voice.music, state.left, state.right)) throw std::runtime_error(Mix_GetError());
        if (state.distance && !Mix_SetMusicEffectDistance(voice.music, state.distance)) throw std::runtime_error(Mix_GetError());
    } else if (state.spatialMode == 2 && !Mix_SetMusicEffectPosition(voice.music, state.angle, state.distance)) throw std::runtime_error(Mix_GetError());
    else if (state.distance && !Mix_SetMusicEffectDistance(voice.music, state.distance)) throw std::runtime_error(Mix_GetError());
}
void SDLCALL Mixer::pitchCallback(Mix_Music*, void* buffer, int length, void* data) {
    try {
    auto& voice = *static_cast<Voice*>(data);
    // Fixed scratch capacity; SoundTouch state is retained between callbacks and is private to this voice.
    const int frames = length / (2 * sizeof(int16_t));
    if (frames <= 0 || static_cast<size_t>(frames * 2) > voice.input.size()) return;
    auto src = static_cast<int16_t*>(buffer);
    for (int i = 0; i < frames * 2; ++i) voice.input[i] = src[i] / 32768.f;
    voice.pitch->putSamples(voice.input.data(), frames);
    const auto count = voice.pitch->receiveSamples(voice.output.data(), frames);
    std::memset(buffer, 0, length);
    for (size_t i = 0; i < count * 2; ++i) src[i] = static_cast<int16_t>(std::clamp(voice.output[i] * 32768.f, -32768.f, 32767.f));
    } catch (...) { std::memset(buffer, 0, length); }
}
uint64_t Mixer::play(const std::string& name, const std::string& path, int channel, bool excl, int loops, int fade) {
    try { return playPrepared(name, path, channel, excl, loops, fade); }
    catch (...) {
        if (excl && channel >= 0 && channel <= 4095) action(3, channel, true, fade);
        throw;
    }
}
uint64_t Mixer::playPrepared(const std::string& name, const std::string& path, int channel, bool excl, int loops, int fade) {
    auto& state = settings(channel, excl);
    auto voice = std::make_unique<Voice>(); voice->name = name; voice->channel = channel; voice->exclusive = excl;
    auto view = views.find(name);
    if (view != views.end()) {
        voice->bytes = view->second.bytes.lock();
        if (!voice->bytes) throw std::runtime_error("Binary memory view has expired");
        voice->name = view->second.access; voice->binary = view->second.binary;
        voice->rw = SDL_RWFromConstMem(voice->bytes->data() + view->second.offset, static_cast<int>(view->second.size));
    } else voice->rw = SDL_RWFromFile(path.c_str(), "rb");
    if (!voice->rw) throw std::runtime_error(SDL_GetError());
    voice->music = Mix_LoadMUS_RW(voice->rw, 0);
    if (!voice->music) throw std::runtime_error(Mix_GetError());
    voice->id = serial++;
    apply(*voice);
    if (!excl) {
        auto& record = records[voice->name]; auto now = std::chrono::steady_clock::now();
        if (now - record.time > std::chrono::seconds(1)) record.count = 0;
        if (state.scoreEnabled && record.count && !state.score.empty()) {
            voice->pitch = std::make_unique<soundtouch::SoundTouch>();
            int frequency, channels; uint16_t format; OFEAudio_GetSpec(&frequency, &format, &channels);
            voice->pitch->setSampleRate(frequency); voice->pitch->setChannels(channels);
            voice->pitch->setPitchSemiTones(state.score[record.count % state.score.size()] - state.base);
            voice->input.resize(65536); voice->output.resize(65536);
            if (!Mix_RegisterMusicEffect(voice->music, pitchCallback, nullptr, voice.get())) throw std::runtime_error(Mix_GetError());
        }
        record.time = now; ++record.count;
    }
    auto old = excl ? current(channel) : nullptr;
    if (old && Mix_PausedMusicStream(old->music)) { Mix_HaltMusicStream(old->music); old = nullptr; }
    int repeat = loops <= 0 ? -1 : loops;
    if (fade < 0) { if (old) Mix_HaltMusicStream(old->music); checked(Mix_PlayMusicStream(voice->music, repeat)); }
    else if (old) {
        // Pinned Mixer X's CrossFade helper combines FadeOut's 1-success and FadeIn's
        // 0-success return values incorrectly. Use the same two primitives directly.
        Mix_FadeOutMusicStream(old->music, fade);
        checked(Mix_FadeInMusicStream(voice->music, repeat, fade));
    }
    else checked(Mix_FadeInMusicStream(voice->music, repeat, fade));
    if (suspended) Mix_PauseMusicStream(voice->music);
    auto id = voice->id; voices.push_back(std::move(voice));
    if (excl) exclusive[channel] = id;
    return id;
}
void Mixer::setSuspended(bool value) {
    suspended = value;
    for (auto& voice : voices) {
        if (suspended || voice->userPaused) Mix_PauseMusicStream(voice->music);
        else Mix_ResumeMusicStream(voice->music);
    }
}
void Mixer::update() {
    for (auto& [channel, id] : exclusive) {
        auto voice = current(channel); const auto& state = exclusiveSettings[channel];
        if (!voice || !state.ab || Mix_PausedMusicStream(voice->music)) continue;
        double end = state.end == 0 ? Mix_MusicDuration(voice->music) : state.end;
        if (end > state.start && Mix_GetMusicPosition(voice->music) >= end) checked(Mix_SetMusicPositionStream(voice->music, state.start));
    }
    voices.erase(std::remove_if(voices.begin(), voices.end(), [](const auto& voice) {
        return !Mix_PlayingMusicStream(voice->music);
    }), voices.end());
    for (auto it = views.begin(); it != views.end();) { if (it->second.bytes.expired()) it = views.erase(it); else ++it; }
}
std::vector<uint64_t> Mixer::liveVoices() const { std::vector<uint64_t> ids; for (const auto& voice : voices) ids.push_back(voice->id); return ids; }
void Mixer::action(int op, int channel, bool excl, double a, double b, const std::string& text) {
    if (!std::isfinite(a) || !std::isfinite(b)) throw std::invalid_argument("Invalid audio parameter");
    if (op == 13) { master = volume(static_cast<int>(a)); for (auto& voice : voices) Mix_VolumeMusicStream(voice->music, settings(voice->channel, voice->exclusive).volume * master * MIX_MAX_VOLUME / 10000); return; }
    if (op == 7) { for (auto& voice : voices) { if (a < 0 || Mix_PausedMusicStream(voice->music)) Mix_HaltMusicStream(voice->music); else Mix_FadeOutMusicStream(voice->music, static_cast<int>(a)); } return; }
    auto& state = settings(channel, excl);
    if (op == 2) state.volume = volume(static_cast<int>(a));
    if (op == 8) { if (a < 0 || b < 0 || (b != 0 && b <= a)) throw std::invalid_argument("Invalid AB loop bounds"); state.ab = true; state.start = a; state.end = b; return; }
    if (op == 9) {
        std::string normalized = text; for (auto& ch : normalized) if (ch == '{' || ch == '}' || ch == ',') ch = ' ';
        std::istringstream input(normalized); std::vector<float> notes; float note;
        while (input >> note) { if (!std::isfinite(note)) throw std::invalid_argument("Invalid score"); notes.push_back(note); }
        if (!input.eof()) throw std::invalid_argument("Invalid score note");
        if (!notes.empty()) state.score = std::move(notes);
        else if (a != 0) throw std::invalid_argument("Score is empty");
        state.scoreEnabled = a != 0; state.base = b; return;
    }
    if (op == 14) { state.spatialMode = 1; state.left = static_cast<uint8_t>(static_cast<int>(a)); state.right = static_cast<uint8_t>(static_cast<int>(b)); }
    if (op == 15) { state.spatialMode = 2; state.angle = static_cast<int16_t>(static_cast<int>(a)); state.distance = static_cast<uint8_t>(static_cast<int>(b)); }
    if (op == 16) state.distance = static_cast<uint8_t>(static_cast<int>(a));
    for (auto& voice : voices) {
        if (voice->channel != channel || voice->exclusive != excl || (excl && current(channel) != voice.get())) continue;
        switch (op) {
            case 2: Mix_VolumeMusicStream(voice->music, state.volume * master * MIX_MAX_VOLUME / 10000); break;
            case 3: if (a < 0 || Mix_PausedMusicStream(voice->music)) Mix_HaltMusicStream(voice->music); else Mix_FadeOutMusicStream(voice->music, static_cast<int>(a)); break;
            case 4: voice->userPaused = true; Mix_PauseMusicStream(voice->music); break;
            case 5: voice->userPaused = false; if (!suspended) Mix_ResumeMusicStream(voice->music); break;
            case 6: checked(Mix_SetMusicPositionStream(voice->music, std::max(0., a))); break;
            case 14: if (!Mix_SetMusicEffectPanning(voice->music, state.left, state.right)) throw std::runtime_error(Mix_GetError()); break;
            case 15: if (!Mix_SetMusicEffectPosition(voice->music, state.angle, state.distance)) throw std::runtime_error(Mix_GetError()); break;
            case 16: if (!Mix_SetMusicEffectDistance(voice->music, state.distance)) throw std::runtime_error(Mix_GetError()); break;
            default: break;
        }
    }
}
bool Mixer::condition(int op, int channel, bool excl, const std::string& name) {
    if (op == 6) return unreferenced(name);
    if (op != 1 && op != 4) channelValid(channel);
    bool playing = false, paused = false, allPaused = true;
    for (auto& voice : voices) {
        bool active = Mix_PlayingMusicStream(voice->music) != 0;
        bool pause = voice->userPaused;
        if (op != 1 && op != 4 && (voice->channel != channel || voice->exclusive != excl || (excl && current(channel) != voice.get()))) continue;
        playing |= active; paused |= active && pause; if (active && !pause) allPaused = false;
    }
    switch (op) {
        case 0: return playing; case 1: return !playing;
        case 2: { auto voice = current(channel); return !voice || Mix_FadingMusicStream(voice->music) == MIX_NO_FADING; }
        case 3: return paused; case 4: return allPaused; case 5: return !playing || allPaused;
        default: return false;
    }
}
double Mixer::number(int op, int channel, bool excl, const std::string& name) {
    if (op == 9) return master;
    if (op == 4) { for (const auto& voice : voices) if (voice->exclusive == excl && voice->name == name && Mix_PlayingMusicStream(voice->music)) return voice->channel; return -1; }
    auto& state = settings(channel, excl);
    switch (op) {
        case 0: return state.volume; case 1: return condition(0, channel, excl);
        case 2: { auto voice = current(channel); return voice ? Mix_GetMusicPosition(voice->music) : -1; }
        case 3: { auto voice = current(channel); return voice ? Mix_MusicDuration(voice->music) : -1; }
        case 10: return state.left; case 11: return state.right; case 12: return state.angle; case 13: return state.distance;
        default: return 0;
    }
}
std::string Mixer::channelName(int channel) { channelValid(channel); auto voice = current(channel); return voice ? voice->name : ""; }
void Mixer::cache(const std::string& name, const std::string& path, bool replace) {
    if (!replace && binaries.count(name)) return;
    std::ifstream input(path, std::ios::binary | std::ios::ate);
    if (!input) throw std::runtime_error("Cannot open audio binary");
    auto size = input.tellg(); if (size <= 0 || size > std::numeric_limits<int>::max()) throw std::runtime_error("Invalid audio binary size");
    auto bytes = std::make_shared<std::vector<uint8_t>>(static_cast<size_t>(size)); input.seekg(0);
    if (!input.read(reinterpret_cast<char*>(bytes->data()), size)) throw std::runtime_error("Cannot read audio binary");
    binaries[name] = std::move(bytes);
}
bool Mixer::unreferenced(const std::string& name) const { if (name.empty()) return true; for (const auto& voice : voices) if (voice->binary == name) return false; return true; }
void Mixer::erase(const std::string& name) {
    for (auto it = binaries.begin(); it != binaries.end();) {
        if ((name.empty() || it->first == name) && unreferenced(it->first)) it = binaries.erase(it); else ++it;
    }
}
std::string Mixer::memoryName(const std::string& access, const std::string& binary, int64_t offset, int64_t size) {
    auto it = binaries.find(binary); if (it == binaries.end()) return "";
    if (offset < 0 || size < 0 || static_cast<uint64_t>(offset) > it->second->size() || static_cast<uint64_t>(size) > it->second->size() - offset) throw std::invalid_argument("Binary view outside loaded data");
    std::string id = "ofe-mixer-memory://" + std::to_string(epoch) + "/" + std::to_string(viewSerial++);
    views.emplace(id, View{it->second, access, binary, static_cast<size_t>(offset), static_cast<size_t>(size)}); return id;
}
}
