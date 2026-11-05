#include "FFMpeg.h"
#include <iostream>
#include <set>
#include <stdexcept>

namespace {
bool grabbing = false, checkGrabbed = false;
std::set<int64_t> grabbedAudio;
int submittedGrabbed = 0;
int64_t seekTarget = -1;
bool dropFirstAudioPts = false;
int laterAudioPts = 0;
void check(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}
int pcm(FFMpeg& player, std::vector<uint8_t>& output) {
    return player.audio_fillData(output.data(), static_cast<int>(output.size()), std::memset,
        [](void* dst, const void* src, size_t size, int) { std::memcpy(dst, src, size); });
}
bool audible(const std::vector<uint8_t>& output) {
    return std::any_of(output.begin(), output.end(), [](uint8_t value) { return value != 0; });
}
}

extern "C" int __real_av_read_frame(AVFormatContext*, AVPacket*);
extern "C" int __real_avcodec_send_packet(AVCodecContext*, const AVPacket*);
extern "C" int __real_avcodec_receive_frame(AVCodecContext*, AVFrame*);
extern "C" int __real_avformat_seek_file(AVFormatContext*, int, int64_t, int64_t, int64_t, int);
extern "C" int __wrap_av_read_frame(AVFormatContext* context, AVPacket* packet) {
    const int result = __real_av_read_frame(context, packet);
    if (grabbing && result >= 0 && packet->pos >= 0
        && context->streams[packet->stream_index]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        grabbedAudio.insert(packet->pos);
    return result;
}
extern "C" int __wrap_avcodec_send_packet(AVCodecContext* context, const AVPacket* packet) {
    if (checkGrabbed && packet && context->codec_type == AVMEDIA_TYPE_AUDIO
        && grabbedAudio.contains(packet->pos)) ++submittedGrabbed;
    return __real_avcodec_send_packet(context, packet);
}
extern "C" int __wrap_avcodec_receive_frame(AVCodecContext* context, AVFrame* frame) {
    const int result = __real_avcodec_receive_frame(context, frame);
    if (result >= 0 && context->codec_type == AVMEDIA_TYPE_AUDIO) {
        if (dropFirstAudioPts) {
            frame->best_effort_timestamp = AV_NOPTS_VALUE;
            dropFirstAudioPts = false;
        } else if (frame->best_effort_timestamp != AV_NOPTS_VALUE) ++laterAudioPts;
    }
    return result;
}
extern "C" int __wrap_avformat_seek_file(AVFormatContext* context, int stream,
    int64_t min, int64_t target, int64_t max, int flags) {
    seekTarget = target;
    return __real_avformat_seek_file(context, stream, min, target, max, flags);
}

int main(int argc, char** argv) {
    check(argc == 4 || argc == 5, "A/V MP4, MPEG-TS, byte offset and optional case required");
    const std::string scenario = argc == 5 ? argv[4] : "all";
    const auto run = [&](const char* name) { return scenario == "all" || scenario == name; };
    av_log_set_level(AV_LOG_ERROR);
    FFMpegOptions options;
    options.SetThreadCount(1);
    const auto frame = [](const unsigned char*, int, int) {};
    std::vector<uint8_t> block(4096);

    // Negative flags select a different seek API, not different timestamp units.
    if (run("units")) {
        FFMpeg player(std::string(argv[1]), options);
        check(player.set_videoPosition(500, -AVSEEK_FLAG_BACKWARD) >= 0, "negative-flags time seek");
        AVFormatContext* context = nullptr;
        check(avformat_open_input(&context, argv[1], nullptr, nullptr) >= 0, "open reference");
        check(avformat_find_stream_info(context, nullptr) >= 0, "reference stream info");
        const int video = av_find_best_stream(context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
        const double origin = context->start_time == AV_NOPTS_VALUE ? 0.0
            : static_cast<double>(context->start_time) / AV_TIME_BASE;
        const auto expected = static_cast<int64_t>((0.5 + origin) / av_q2d(context->streams[video]->time_base));
        avformat_close_input(&context);
        check(seekTarget == expected, "time seek must convert milliseconds to stream ticks");
    }

    // A real byte seek must return audible samples, not discard them until byteOffset/1000 seconds.
    if (run("bytes")) for (int flags : {AVSEEK_FLAG_BYTE, -AVSEEK_FLAG_BYTE}) {
        FFMpeg player(std::string(argv[2]), options);
        player.set_pause(false);
        check(pcm(player, block) > 0 && audible(block), "initial filter graph exists at default tempo");
        check(player.set_videoPosition(std::stoll(argv[3]), flags) >= 0, "byte seek accepted");
        const auto provisionalPosition = player.get_videoPosition();
        check(pcm(player, block) > 0 && audible(block), "byte seek retains audible PCM");
        check(provisionalPosition == 0, "byte offset must not be exposed as milliseconds");
        check(player.get_videoPosition() >= 0 && player.get_videoPosition() < 4000,
            "byte seek clock anchors to decoded timestamp");
        check(player.set_videoPosition(0, AVSEEK_FLAG_BACKWARD) >= 0, "time rewind after byte seek");
        check(pcm(player, block) > 0 && audible(block), "rewind recreates the filter graph");
    }

    if (run("late-pts")) {
        int64_t reference = 0;
        for (bool missing : {false, true}) {
            FFMpeg player(std::string(argv[2]), options);
            player.set_pause(false);
            check(player.set_videoPosition(std::stoll(argv[3]), AVSEEK_FLAG_BYTE) >= 0,
                "byte seek before missing-PTS test");
            dropFirstAudioPts = missing;
            laterAudioPts = 0;
            for (int i = 0; i < 10; ++i) check(pcm(player, block) > 0, "missing PTS preserves PCM");
            check(laterAudioPts > 0, "fixture supplies later valid audio timestamps");
            if (!missing) reference = player.get_videoPosition();
            else check(reference > 100 && std::abs(player.get_videoPosition() - reference) <= 1,
                "later valid audio PTS must recover the byte-seek anchor");
        }
    }

    // Forward grabbing intentionally discards audio, even when it uses the main demuxer.
    if (run("grab")) {
        FFMpeg player(std::string(argv[1]), options);
        check(player.set_videoPosition(0, AVSEEK_FLAG_BACKWARD) >= 0, "rewind before grab");
        grabbing = true;
        check(player.goto_videoPosition(1000, frame) >= 0, "grab target frame");
        grabbing = false;
        check(!grabbedAudio.empty(), "grab fixture must cross audio packets");
        player.set_pause(false);
        checkGrabbed = true;
        bool resumed = false;
        // Later clock commits can insert silence before the first post-grab sample.
        for (int budget = 0; budget < 128 && !resumed; ++budget)
            resumed = pcm(player, block) > 0 && audible(block);
        check(resumed, "playback resumes after grab");
        checkGrabbed = false;
        check(submittedGrabbed == 0, "grabbed audio must not enter the playback queue");
    }

    // Drain fully, then seek with unchanged tempo: an EOF filter must be recreated.
    if (run("eof")) {
        FFMpeg player(std::string(argv[1]), options);
        player.set_pause(false);
        bool ended = false;
        for (int budget = 0; budget < 1000; ++budget) {
            if (pcm(player, block) == 0) { ended = true; break; }
        }
        check(ended, "audio reaches EOF");
        check(player.set_videoPosition(500, AVSEEK_FLAG_BACKWARD) >= 0, "time seek after EOF");
        check(pcm(player, block) > 0 && audible(block), "EOF seek resumes audible audio");
    }
    if (run("tempo")) {
        FFMpeg player(std::string(argv[1]), options);
        player.set_audioTempo(1.23f);
        player.set_pause(false);
        int64_t bytes = 0;
        bool ended = false;
        for (int budget = 0; budget < 1000; ++budget) {
            const int count = pcm(player, block);
            if (!count) { ended = true; break; }
            bytes += count;
        }
        check(ended, "fractional-tempo audio reaches EOF");
        // The fixture is four seconds. Allow codec padding and atempo window rounding,
        // but reject rounding the requested 1.23 to 1.2 (about 3,600 extra samples).
        check(std::abs(bytes / 4.0 - 4.0 * TARGET_SAMPLE_RATE / 1.23) < 2048,
            "filter must use the same fractional tempo as the audio clock");
    }
    std::cout << "PASS: audio seek regression (" << scenario << ")\n";
}
