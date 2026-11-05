#include "FFMpeg.h"
#include <iostream>
#include <stdexcept>

// The runner permits access to the internal clock for this test only; production
// headers gain no test hooks. A deterministic timer avoids scheduler-dependent checks.
int64_t testTime = 1000000;
extern "C" int64_t __wrap_av_gettime_relative() { return testTime; }
void check(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}
void near(double actual, double expected, const char* message) {
    if (std::abs(actual - expected) > 1e-6) {
        std::cerr << "actual=" << actual << " expected=" << expected << '\n';
        throw std::runtime_error(message);
    }
}
int fill(FFMpeg& player, std::vector<uint8_t>& block) {
    return player.audio_fillData(block.data(), static_cast<int>(block.size()), std::memset,
        [](void* dst, const void* src, size_t count, int) { std::memcpy(dst, src, count); });
}

int main(int argc, char** argv) {
    check(argc == 2 || argc == 3, "four-second A/V fixture and optional scenario required");
    const std::string scenario = argc == 3 ? argv[2] : "all";
    const auto run = [&](const char* name) { return scenario == "all" || scenario == name; };
    FFMpegOptions options;
    options.SetThreadCount(1);
    const double bytesPerSecond = TARGET_SAMPLE_RATE * TARGET_CHANNEL_NUMBER * 2;

    if (run("buffered")) for (float tempo : {0.25f, 1.0f, 1.23f, 4.0f}) {
        for (int size : {4, 512, 4096}) {
            FFMpeg player(std::string(argv[1]), options);
            player.set_audioTempo(tempo);
            player.set_pause(false);
            std::vector<uint8_t> block(size);
            double submitted = 0;
            for (int i = 0; i < 5; ++i) {
                const int count = fill(player, block);
                check(count == size, "fixture must fill callback");
                near(player.get_audioClock(), submitted, "buffered PCM must not advance playback clock");
                submitted += count * tempo / bytesPerSecond;
                testTime += 1000000;
                near(player.get_audioClock(), submitted, "clock stops at submitted samples");
            }
        }
    }

    if (run("partial")) {
        FFMpeg player(std::string(argv[1]), options);
        player.set_pause(false);
        std::vector<uint8_t> block(1000000);
        const int count = fill(player, block);
        check(count > 0 && count < static_cast<int>(block.size()), "callback must end partially filled at EOF");
        const double end = count / bytesPerSecond;
        testTime += 10000000;
        near(player.get_audioClock(), end, "unfilled EOF tail must not advance media clock");
        check(fill(player, block) == 0, "EOF callback returns no PCM");
        testTime += 10000000;
        near(player.get_audioClock(), end, "empty callback must not advance media clock");
    }

    if (run("tempo-pause")) {
        FFMpeg player(std::string(argv[1]), options);
        player.set_pause(false);
        std::vector<uint8_t> block(4096);
        check(fill(player, block) == 4096, "initial callback");
        testTime += 5000;
        near(player.get_audioClock(), 0.005, "initial interpolation");
        player.set_audioTempo(2.0f);
        near(player.get_audioClock(), 0.005, "tempo must not retime already submitted PCM");
        testTime += 5000;
        near(player.get_audioClock(), 0.010, "old callback retains its tempo");
        player.set_pause(true);
        testTime += 500000;
        near(player.get_audioClock(), 0.010, "pause freezes clock");
        player.set_pause(false);
        testTime += 5000;
        near(player.get_audioClock(), 0.015, "resume preserves remaining callback duration");
    }
    std::cout << "PASS: submitted-audio clock (" << scenario << ")\n";
}
