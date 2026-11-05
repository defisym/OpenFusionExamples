// Compile the original Windows playback class, without JNI or the Android engine.
#include "FFMpeg.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
void check(bool ok, const char* message) { if (!ok) throw std::runtime_error(message); }
int main(int argc, char** argv) {
    check(argc == 3, "video and delayed audio fixtures required");
    FFMpegOptions options;
    options.outputFormat = AV_PIX_FMT_RGBA;
    options.SetThreadCount(2);
    std::vector<uint8_t> block(4096);
    auto pcm = [&](FFMpeg& player) {
        return player.audio_fillData(block.data(), block.size(), std::memset,
            [](void* dst, const void* src, size_t count, int) { std::memcpy(dst, src, count); });
    };
    FFMpeg player(std::string(argv[1]), options);
    int count = 0;
    size_t bytes = 0;
    const auto frame = [&](const unsigned char* data, int stride, int height) {
        check(data && stride == 64*4 && height == 48, "tightly packed shared software conversion"); ++count;
    };
    player.set_pause(false);
    for (int budget = 0; budget < 1000; ++budget) {
        player.get_nextFrame(frame, false);
        bytes += pcm(player);
        if (player.get_finishState() && player.audio_finished()) break;
    }
    check(count == 12, "shared decoder drains delayed video frames");
    check(bytes >= 44100*4, "shared filter and resampler drain");
    player.set_pause(true);
    check(player.seek(400, AVSEEK_FLAG_BACKWARD, true, frame) >= 0, "fractional second seek");
    check(player.get_videoPosition() >= 400 && player.get_videoPosition() < 600, "shared seek target");
    check(player.seek(0, AVSEEK_FLAG_BACKWARD, true, frame) >= 0, "rewind after EOF");
    count = 0;
    player.get_nextFrame(frame, false);
    check(count == 1 && player.get_videoPosition() == 0, "preview zero does not consume first playback frame");
    player.seek(0, AVSEEK_FLAG_BACKWARD, true, frame);
    player.get_nextFrame([&](const unsigned char*, int, int) { player.set_volume(25); }, false);
    check(player.get_volume() == 25, "frame callbacks may reenter control methods");
    check(player.seek(200, AVSEEK_FLAG_BACKWARD, true,
        [&](const unsigned char*, int, int) { player.set_volume(50); }) >= 0,
        "seek callbacks may reenter control methods");
    check(player.get_volume() == 50, "seek callback control update");
    player.set_pause(false); player.get_pausedTime();
    player.set_pause(true, false);
    check(player.get_pausedTime() == 0, "state-only pause leaves timing metadata unchanged");
    FFMpeg delayed(std::string(argv[2]), options);
    delayed.set_pause(false);
    check(pcm(delayed) == static_cast<int>(block.size()), "delayed audio callback");
    check(std::all_of(block.begin(), block.end(), [](uint8_t value) { return value == 0; }),
        "delayed audio starts with output silence");
    check(delayed.get_playbackClock() < 100, "delayed audio does not skip the video timeline");
    bool found = false;
    for (int i = 0; i < 30; ++i) {
        pcm(delayed);
        if (std::any_of(block.begin(), block.end(), [](uint8_t value) { return value != 0; })) {
            check(delayed.get_playbackClock() >= 350, "audible samples retain relative start offset");
            found = true; break;
        }
    }
    check(found, "delayed source eventually renders audio");
    std::ifstream input(argv[1], std::ios::binary);
    std::vector<unsigned char> memory((std::istreambuf_iterator<char>(input)), {});
    const auto original = memory;
    {
        MemView view(memory.data(), static_cast<int>(memory.size()));
        uint8_t first[4] = {};
        check(view.read(first, sizeof(first)) == sizeof(first)
            && std::equal(std::begin(first), std::end(first), memory.begin()), "borrowed source read");
        check(view.seek(0, AVSEEK_SIZE) == static_cast<int64_t>(memory.size()), "view size query");
        check(view.seek(-4, SEEK_END) == static_cast<int64_t>(memory.size()) - 4, "view seek from end");
        check(view.read(first, sizeof(first)) == sizeof(first), "read source tail");
        check(view.read(first, sizeof(first)) == AVERROR_EOF, "view EOF");
        check(view.seek(1, SEEK_CUR) == AVERROR(EINVAL), "view rejects out-of-bounds seek");
        check(view.seek(0, SEEK_SET | AVSEEK_FORCE) == 0, "view rewind");
    }
    {
        FFMpeg inMemory(memory.data(), memory.size(), options);
        count = 0; inMemory.get_nextFrame(frame, false);
        check(count == 1, "shared custom AVIO file source");
        check(inMemory.seek(500, AVSEEK_FLAG_BACKWARD, true, frame) >= 0, "custom AVIO seek");
        check(inMemory.get_videoFrame(600, true, frame) >= 0, "independent memory preview context");
        check(inMemory.seek(0, AVSEEK_FLAG_BACKWARD, true, frame) >= 0, "memory rewind");
        inMemory.set_pause(false);
        count = 0;
        size_t memoryBytes = 0;
        for (int budget = 0; budget < 1000; ++budget) {
            inMemory.get_nextFrame(frame, false);
            memoryBytes += pcm(inMemory);
            if (inMemory.get_finishState() && inMemory.audio_finished()) break;
        }
        check(count == 12 && memoryBytes >= 44100*4, "memory source plays and drains to EOF");
    }
    check(memory == original, "caller retains source data after view/player destruction");
    for (int i = 0; i < 20; ++i) {
        try { FFMpeg invalid(std::string(argv[1])+".missing", options); check(false, "invalid open must throw"); }
        catch (const FFMpegException&) {}
        try { FFMpeg invalid(memory.data(), 1, options); check(false, "invalid memory source must throw"); }
        catch (const FFMpegException&) {}
    }
    std::cout << "PASS: original C++ core, video/filter/resampler drain, accurate seek, custom AVIO and failure cleanup (" << bytes << " bytes)\n";
}
