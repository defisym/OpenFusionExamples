#include "FFMpeg.h"
#include <cerrno>
#include <iostream>
#include <stdexcept>
#include <utility>

// Linker wrappers inject source/codec errors without adding production test hooks.
struct Probe {
    bool active = false, queuedVideo = false, receivedBeforeSend = false;
    int readError = 0, sendError = 0, reads = 0, sends = 0, receives = 0;
} probe;

extern "C" int __real_av_read_frame(AVFormatContext*, AVPacket*);
extern "C" int __real_avcodec_send_packet(AVCodecContext*, const AVPacket*);
extern "C" int __real_avcodec_receive_frame(AVCodecContext*, AVFrame*);

extern "C" int __wrap_av_read_frame(AVFormatContext* context, AVPacket* packet) {
    if (probe.active) {
        ++probe.reads;
        if (probe.readError) return std::exchange(probe.readError, 0);
    }
    const int result = __real_av_read_frame(context, packet);
    if (probe.active && result >= 0
        && context->streams[packet->stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        probe.queuedVideo = true;
    return result;
}

extern "C" int __wrap_avcodec_send_packet(AVCodecContext* context, const AVPacket* packet) {
    if (probe.active && context->codec_type == AVMEDIA_TYPE_VIDEO) {
        ++probe.sends;
        probe.queuedVideo = false;
        if (probe.sendError) return std::exchange(probe.sendError, 0);
    }
    return __real_avcodec_send_packet(context, packet);
}

extern "C" int __wrap_avcodec_receive_frame(AVCodecContext* context, AVFrame* frame) {
    if (probe.active && context->codec_type == AVMEDIA_TYPE_VIDEO) {
        ++probe.receives;
        if (probe.queuedVideo) probe.receivedBeforeSend = true;
    }
    return __real_avcodec_receive_frame(context, frame);
}

void check(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

int main(int argc, char** argv) {
    check(argc == 2, "audio/video fixture required");
    FFMpegOptions options;
    options.flag = FFMpegFlag_ForceNoAudio;
    options.SetThreadCount(2);
    for (int scenario = 0; scenario < 4; ++scenario) {
        probe = {};
        FFMpeg player(std::string(argv[1]), options);
        player.set_audioTempo(1);
        int frames = 0;
        auto frame = [&](const unsigned char*, int, int) { ++frames; };
        probe.readError = scenario == 1 ? AVERROR(EAGAIN) : scenario == 2 ? AVERROR(EIO) : 0;
        probe.sendError = scenario == 3 ? AVERROR(EPERM) : 0;
        probe.active = true;
        const int result = player.get_nextFrame(frame);
        if (scenario == 1) {
            check(result == 0 && frames == 0 && probe.reads == 1
                && probe.sends == 0 && probe.receives == 0 && !player.get_finishState(),
                "temporary source starvation defers one tick without finishing or spinning");
        } else if (scenario >= 2) {
            const int error = scenario == 2 ? AVERROR(EIO) : AVERROR(EPERM);
            check(result == error && result != END_OF_QUEUE && frames == 0 && !player.get_finishState(),
                "real IO/codec errors remain distinct from queue states and EOF");
        }
        if (scenario <= 1) {
            for (int budget = 0; frames == 0 && budget < 100; ++budget) player.get_nextFrame(frame);
            check(frames == 1 && probe.sends > 0 && !probe.receivedBeforeSend,
                "queued packets are submitted before receiving; playback resumes after EAGAIN");
        }
        probe.active = false;
    }
    std::cout << "PASS: Send First, deferred input, IO/codec error propagation and EOF separation\n";
}
