#include "EncryptionAvio.h"
#include "EncryptionLegacy.h"
extern "C" {
#include <libavcodec/packet.h>
}
#include <iostream>
#define CHECK(x) do { if (!(x)) throw std::runtime_error(#x); } while (false)
using namespace encryption;
class MemorySource final : public RandomAccessSource {
    Bytes bytes_;
public:
    std::uint64_t transferred = 0;
    explicit MemorySource(Bytes bytes) : bytes_(std::move(bytes)) {}
    std::uint64_t size() const override { return bytes_.size(); }
    std::size_t read_at(std::uint64_t offset, Byte* output, std::size_t capacity) override {
        CHECK(offset <= bytes_.size());
        auto count = (std::min)(capacity, bytes_.size() - static_cast<std::size_t>(offset));
        if (count) std::memcpy(output, bytes_.data() + offset, count);
        transferred += count;
        return count;
    }
};
void word(Bytes& data, std::uint32_t value, int count) {
    while (count--) { data.push_back(static_cast<Byte>(value)); value >>= 8; }
}
int main() {
    AVFormatContext* format = nullptr;
    AVFormatContext* second = nullptr;
    AVPacket* packet = av_packet_alloc();
    std::unique_ptr<DecryptAvio> input, clone;
    try {
        // Real PCM WAV demuxing over the same encrypted AVIO used by the plugin.
        const std::uint32_t samples = 8000 * 30;
        Bytes wav{'R','I','F','F'};
        word(wav, samples*2+36, 4); wav.insert(wav.end(), {'W','A','V','E','f','m','t',' '});
        word(wav, 16, 4); word(wav, 1, 2); word(wav, 1, 2); word(wav, 8000, 4);
        word(wav, 16000, 4); word(wav, 2, 2); word(wav, 16, 2); wav.insert(wav.end(), {'d','a','t','a'});
        word(wav, samples*2, 4);
        for (std::uint32_t i = 0; i < samples; ++i) word(wav, i % 100, 2);
        const auto key = legacy::derive(u"abc");
        auto source = std::make_shared<MemorySource>(transform_buffer(key, Mode::encrypt, wav.data(), wav.size()));
        CHECK(packet != nullptr);
        input = std::make_unique<DecryptAvio>(DecryptReader(source, key));
        CHECK(source->transferred == 32);
        CHECK(input->open(&format));
        CHECK(avformat_find_stream_info(format, nullptr) >= 0);
        CHECK(format->nb_streams == 1 && format->streams[0]->codecpar->codec_id == AV_CODEC_ID_PCM_S16LE);
        CHECK(avio_size(format->pb) == static_cast<std::int64_t>(wav.size()));
        CHECK(avio_seek(format->pb, -1, SEEK_SET) < 0);
        CHECK(avio_seek(format->pb, (std::numeric_limits<std::int64_t>::max)(), SEEK_END) < 0);
        CHECK(av_seek_frame(format, 0, 0, AVSEEK_FLAG_BACKWARD) >= 0);
        std::uint64_t total = 0;
        while (av_read_frame(format, packet) >= 0) {
            total += packet->size;
            av_packet_unref(packet);
        }
        CHECK(total == samples * 2);
        CHECK(av_seek_frame(format, 0, 12000, AVSEEK_FLAG_BACKWARD) >= 0);
        CHECK(av_read_frame(format, packet) >= 0 && packet->pts == 12000);
        av_packet_unref(packet);
        clone = input->clone();
        CHECK(clone->open(&second));
        CHECK(avformat_find_stream_info(second, nullptr) >= 0);
        CHECK(av_read_frame(second, packet) >= 0);
        av_packet_unref(packet);
        // The secondary demuxer must not disturb the primary cursor.
        CHECK(av_read_frame(format, packet) >= 0 && packet->pts > 12000);
        av_packet_unref(packet);
        avformat_close_input(&second);
        avformat_close_input(&format);
        std::cout << "PASS: FFmpeg encrypted WAV demux, full payload, size, seek and independent secondary decoder\n";
        av_packet_free(&packet);
        return 0;
    } catch (const std::exception& error) {
        avformat_close_input(&second); avformat_close_input(&format); av_packet_free(&packet);
        std::cerr << error.what() << '\n'; return 1;
    }
}
