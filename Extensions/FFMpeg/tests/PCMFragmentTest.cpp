#include "FFMpeg.h"
#include <iostream>
#include <stdexcept>
#include <vector>

// Output must not depend on how SDL divides a request across decoded PCM blocks.
int main(int argc, char** argv) {
    if (argc != 2) throw std::runtime_error("Audio/video fixture required");
    FFMpeg large(to_wide_string(argv[1])), fragmented(to_wide_string(argv[1]));
    large.set_pause(false); fragmented.set_pause(false);
    std::vector<uint8_t> actual(100000), expected(actual.size());
    auto mix = [](void* dst, const void* src, size_t count, int volume) {
        SDL_MixAudioFormat(static_cast<uint8_t*>(dst), static_cast<const uint8_t*>(src),
            AUDIO_S16SYS, static_cast<Uint32>(count), volume);
    };
    large.audio_fillData(actual.data(), static_cast<int>(actual.size()), std::memset, mix);
    for (size_t i = 0; i < expected.size(); i += 4)
        fragmented.audio_fillData(expected.data()+i, 4, std::memset, mix);
    if (std::all_of(expected.begin(), expected.end(), [](uint8_t value) { return value == 0; }))
        throw std::runtime_error("Fixture produced no audible PCM");
    if (actual != expected) throw std::runtime_error("PCM changes with callback fragmentation");
    std::cout << "PASS: PCM callback fragmentation preserves sample bytes\n";
}
