#include "AudioRuntime.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <SoundTouch.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <vector>
namespace {
std::mutex channels;
struct PCM {
    std::mutex mutex;
    OFEAudio_PCMCallback pull = nullptr;
    void* pullUser = nullptr;
    int frequency = 0, channel = -1;
    Mix_Chunk* chunk = nullptr;
    SDL_AudioStream* conversion = nullptr;
    std::vector<uint8_t> silence = std::vector<uint8_t>(4096, 0);
    struct Sample { int16_t left, right; double end; };
    std::vector<Sample> ring;
    size_t read = 0, size = 0;
    soundtouch::SoundTouch tempo;
    double speed = 1, position = 0, writePosition = 0, inputEnd = -1;
    float volume = 1;
    bool dsp = false, flushed = false, running = false;
    std::vector<float> floats = std::vector<float>(8192);
    std::vector<int16_t> output = std::vector<int16_t>(8192);
    PCM() {
        uint16_t format; int count;
        if (!OFEAudio_GetSpec(&frequency, &format, &count) || format != AUDIO_S16SYS || count != 2)
            throw std::runtime_error("PCM output requires an acquired S16 stereo Mixer X device");
        ring.resize(static_cast<size_t>(frequency) * 2);
        tempo.setSampleRate(44100); tempo.setChannels(2);
        tempo.setSetting(SETTING_SEQUENCE_MS, 20); tempo.setSetting(SETTING_SEEKWINDOW_MS, 10);
        conversion = SDL_NewAudioStream(AUDIO_S16SYS, 2, 44100, format, 2, frequency);
        if (!conversion) throw std::runtime_error(SDL_GetError());
        chunk = Mix_QuickLoad_RAW(silence.data(), static_cast<Uint32>(silence.size()));
        if (!chunk) { SDL_FreeAudioStream(conversion); throw std::runtime_error(Mix_GetError()); }
    }
    ~PCM() {
        if (channel >= 0) { Mix_HaltChannel(channel); Mix_UnregisterAllEffects(channel); }
        if (chunk) Mix_FreeChunk(chunk);
        if (conversion) SDL_FreeAudioStream(conversion);
    }
    void drain() {
        for (;;) {
            size_t free = ring.size() - size;
            if (!free) return;
            if (SDL_AudioStreamAvailable(conversion) < 4 && dsp) {
                unsigned count = tempo.receiveSamples(floats.data(), static_cast<unsigned>(std::min<size_t>(free, 4096)));
                if (!count) return;
                for (size_t i = 0; i < count * 2; ++i) output[i] = static_cast<int16_t>(std::clamp(floats[i] * 32768.f, -32768.f, 32767.f));
                if (SDL_AudioStreamPut(conversion, output.data(), count * 4) < 0) throw std::runtime_error(SDL_GetError());
            }
            int count = SDL_AudioStreamGet(conversion, output.data(), static_cast<int>(std::min<size_t>(free, 4096)) * 4);
            if (count < 0) throw std::runtime_error(SDL_GetError());
            if (!count) return;
            for (int i = 0; i < count / 4; ++i) {
                writePosition += speed * 1000. / frequency;
                ring[(read + size++) % ring.size()] = {output[i*2], output[i*2+1], writePosition};
            }
        }
    }
    static void SDLCALL render(int, void* data, int bytes, void* user) {
        auto& self = *static_cast<PCM*>(user);
        std::lock_guard lock(self.mutex);
        auto dst = static_cast<int16_t*>(data); std::memset(data, 0, bytes);
        if (!self.running) return;
        if (self.pull) {
            try {
                if (self.frequency == 44100) self.pull(self.pullUser, data, bytes);
                else {
                    while (SDL_AudioStreamAvailable(self.conversion) < bytes) {
                        self.pull(self.pullUser, self.output.data(), 4096);
                        if (SDL_AudioStreamPut(self.conversion, self.output.data(), 4096) < 0) break;
                    }
                    SDL_AudioStreamGet(self.conversion, data, bytes);
                }
            } catch (...) {
                // Exceptions cannot unwind through SDL's audio thread.
                std::memset(data, 0, bytes);
                self.running = false;
            }
            return;
        }
        const double step = self.speed * 1000. / self.frequency;
        for (int i = 0; i < bytes / 4; ++i) {
            while (self.size && self.ring[self.read].end < self.position - step * .5) {
                self.read = (self.read + 1) % self.ring.size(); --self.size;
            }
            if (self.size && self.ring[self.read].end <= self.position + step * 1.5) {
                const auto sample = self.ring[self.read];
                dst[i*2] = static_cast<int16_t>(sample.left * self.volume); dst[i*2+1] = static_cast<int16_t>(sample.right * self.volume);
                self.position = std::max(self.position, sample.end);
                self.read = (self.read + 1) % self.ring.size(); --self.size;
            } else self.position += step; // Silence/underruns advance the same device clock.
        }
    }
};
PCM& pcm(void* value) { if (!value) throw std::runtime_error("PCM stream is closed"); return *static_cast<PCM*>(value); }
}
extern "C" void* OFEAudio_CreatePCM() {
    std::lock_guard lock(channels);
    auto stream = std::make_unique<PCM>();
    int channel = Mix_PlayChannel(-1, stream->chunk, -1);
    if (channel < 0) { int count = Mix_AllocateChannels(-1); Mix_AllocateChannels(count + 8); channel = Mix_PlayChannel(-1, stream->chunk, -1); }
    if (channel < 0) throw std::runtime_error(Mix_GetError());
    stream->channel = channel;
    // A looping silent chunk supplies an independent Mixer X channel, as on Windows.
    // The effect replaces only this channel's samples; music-stream playback is untouched.
    if (!Mix_RegisterEffect(channel, PCM::render, nullptr, stream.get())) throw std::runtime_error(Mix_GetError());
    return stream.release();
}
extern "C" void* OFEAudio_CreatePullPCM(OFEAudio_PCMCallback callback, void* user) {
    if (!callback) throw std::runtime_error("PCM callback is required");
    auto stream = static_cast<PCM*>(OFEAudio_CreatePCM());
    std::lock_guard lock(stream->mutex);
    stream->pull = callback;
    stream->pullUser = user;
    return stream;
}
extern "C" void OFEAudio_DestroyPCM(void* value) { std::lock_guard lock(channels); delete static_cast<PCM*>(value); }
extern "C" void OFEAudio_PCMWrite(void* value, const void* bytes, size_t count, double timestamp) {
    auto& self = pcm(value); std::lock_guard lock(self.mutex);
    if (count % 4 || !std::isfinite(timestamp)) throw std::runtime_error("Invalid stereo PCM input");
    if (self.flushed) throw std::runtime_error("Cannot write PCM after EOF");
    self.drain();
    if (self.inputEnd < 0 || std::abs(timestamp - self.inputEnd) > 100) self.writePosition = timestamp;
    self.inputEnd = timestamp + (count / 4) * 1000. / 44100.;
    if (self.dsp) {
        auto input = static_cast<const int16_t*>(bytes);
        for (size_t offset = 0; offset < count / 4;) {
            auto frames = std::min<size_t>(4096, count / 4 - offset);
            for (size_t i = 0; i < frames * 2; ++i) self.floats[i] = input[offset * 2 + i] / 32768.f;
            self.tempo.putSamples(self.floats.data(), static_cast<unsigned>(frames)); offset += frames; self.drain();
        }
    } else {
        if (SDL_AudioStreamPut(self.conversion, bytes, static_cast<int>(count)) < 0) throw std::runtime_error(SDL_GetError());
        self.drain();
    }
}
extern "C" void OFEAudio_PCMFlush(void* value) {
    auto& self = pcm(value); std::lock_guard lock(self.mutex);
    if (!self.flushed) { if (self.dsp) self.tempo.flush(); self.flushed = true; }
    self.drain();
    if (!self.dsp || self.tempo.numSamples() == 0) SDL_AudioStreamFlush(self.conversion);
    self.drain();
}
extern "C" void OFEAudio_PCMReset(void* value, double position) {
    auto& self = pcm(value); std::lock_guard lock(self.mutex);
    self.tempo.clear(); SDL_AudioStreamClear(self.conversion); self.read = self.size = 0;
    self.position = self.writePosition = position; self.inputEnd = -1; self.flushed = false;
}
extern "C" void OFEAudio_PCMState(void* value, int running, float volume, double speed) {
    auto& self = pcm(value); std::lock_guard lock(self.mutex);
    if (!std::isfinite(speed) || speed <= 0 || speed > 32) throw std::runtime_error("Invalid PCM tempo");
    self.running = running != 0;
    self.speed = speed; self.tempo.setTempo(speed);
    self.dsp = self.dsp || speed != 1.;
    self.volume = std::clamp(volume, 0.f, 1.f);
}
extern "C" double OFEAudio_PCMClock(void* value) {
    auto& self = pcm(value); std::lock_guard lock(self.mutex); return self.position;
}
extern "C" size_t OFEAudio_PCMPending(void* value) {
    auto& self = pcm(value); std::lock_guard lock(self.mutex); self.drain();
    return self.size + SDL_AudioStreamAvailable(self.conversion) / 4 + self.tempo.numSamples();
}
