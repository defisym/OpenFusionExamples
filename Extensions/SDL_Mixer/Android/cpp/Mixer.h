#pragma once
#include "AudioRuntime.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <SoundTouch.h>
#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>
namespace ofe::audio {
struct Settings {
    int spatialMode = 0;
    int volume = 100, left = 255, right = 255, angle = 0, distance = 0;
    bool ab = false, scoreEnabled = false;
    double start = 0, end = 0, base = 0;
    std::vector<float> score{0,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1};
};
struct Voice {
    uint64_t id = 0;
    int channel = 0;
    bool exclusive = false, userPaused = false;
    std::string name, binary;
    Mix_Music* music = nullptr;
    SDL_RWops* rw = nullptr;
    std::shared_ptr<std::vector<uint8_t>> bytes;
    std::unique_ptr<soundtouch::SoundTouch> pitch;
    std::vector<float> input, output;
    ~Voice();
};
class Mixer final {
    struct View { std::weak_ptr<std::vector<uint8_t>> bytes; std::string access, binary; size_t offset, size; };
    struct Record { std::chrono::steady_clock::time_point time; size_t count = 0; };
    std::vector<std::unique_ptr<Voice>> voices;
    std::map<int, uint64_t> exclusive;
    std::map<int, Settings> exclusiveSettings, mixingSettings;
    std::map<std::string, std::shared_ptr<std::vector<uint8_t>>> binaries;
    std::map<std::string, View> views;
    std::map<std::string, Record> records;
    uint64_t serial = 1, viewSerial = 1, epoch;
    int master = 100;
    bool suspended = false;
    Settings& settings(int channel, bool excl);
    Voice* current(int channel);
    void apply(Voice& voice);
    uint64_t playPrepared(const std::string&, const std::string&, int, bool, int, int);
    static void SDLCALL pitchCallback(Mix_Music*, void*, int, void*);
public:
    Mixer();
    ~Mixer();
    Mixer(const Mixer&) = delete;
    uint64_t play(const std::string& name, const std::string& path, int channel, bool excl, int loops, int fade);
    void update();
    void setSuspended(bool value);
    std::vector<uint64_t> liveVoices() const;
    void action(int op, int channel, bool excl, double a = 0, double b = 0, const std::string& text = "");
    bool condition(int op, int channel, bool excl, const std::string& name = "");
    double number(int op, int channel, bool excl, const std::string& name = "");
    std::string channelName(int channel);
    void cache(const std::string& name, const std::string& path, bool replace);
    void erase(const std::string& name);
    bool unreferenced(const std::string& name) const;
    std::string memoryName(const std::string& access, const std::string& binary, int64_t offset, int64_t size);
};
}
