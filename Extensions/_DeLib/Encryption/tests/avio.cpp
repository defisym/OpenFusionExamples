#include "DecryptMemBuf.h"
#include "Encryption/KeyGen.h"
extern "C" {
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
#include <libavutil/mem.h>
}
#include <iostream>
#define CHECK(x) do { if (!(x)) throw std::runtime_error(#x); } while (false)
using namespace encryption;
class MemorySource final : public RandomAccessSource {
    Bytes bytes_;
public:
    std::uint64_t transferred = 0;
    bool fail = false;
    explicit MemorySource(Bytes bytes) : bytes_(std::move(bytes)) {}
    std::uint64_t size() const override { return bytes_.size(); }
    std::size_t read_at(std::uint64_t offset, Byte* output, std::size_t capacity) override {
        if (fail) throw std::runtime_error("Injected source failure");
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
// Consumer-side fixture: the player owns AVIO/probing, the reader only read/seek.
struct Demuxer {
    std::unique_ptr<MemBuf> reader;
    AVIOContext* io = nullptr;
    AVFormatContext* format = nullptr;
    explicit Demuxer(std::unique_ptr<MemBuf> source) : reader(std::move(source)) {
        auto* buffer = static_cast<Byte*>(av_malloc(MEM_BUFFER_SIZE));
        if (!buffer) throw std::bad_alloc();
        io = avio_alloc_context(buffer, MEM_BUFFER_SIZE, 0, reader.get(),
            [](void* opaque, Byte* output, int capacity) { return static_cast<MemBuf*>(opaque)->read(output, capacity); },
            nullptr,
            [](void* opaque, std::int64_t offset, int whence) { return static_cast<MemBuf*>(opaque)->seek(offset, whence); });
        if (!io) { av_free(buffer); throw std::bad_alloc(); }
    }
    ~Demuxer() {
        avformat_close_input(&format);
        av_freep(&io->buffer);
        avio_context_free(&io);
    }
    bool open(const AVInputFormat* type = nullptr) {
        if (!type) {
            if (av_probe_input_buffer2(io, &type, "", nullptr, 0, 0) < 0) return false;
            CHECK(type && std::string(type->name) == "wav");
            CHECK(avio_tell(io) == 0); // Probe bytes remain available to open/read.
        }
        format = avformat_alloc_context();
        if (!format) return false;
        format->iformat = type;
        format->pb = io;
        format->flags |= AVFMT_FLAG_CUSTOM_IO;
        return avformat_open_input(&format, nullptr, type, nullptr) >= 0;
    }
};
void demux(std::unique_ptr<MemBuf> source, const Bytes& wav, std::uint32_t samples) {
    Demuxer input(std::move(source));
    CHECK(input.open());
    auto* format = input.format;
    CHECK(avformat_find_stream_info(format, nullptr) >= 0);
    CHECK(format->nb_streams == 1 && format->streams[0]->codecpar->codec_id == AV_CODEC_ID_PCM_S16LE);
    CHECK(avio_size(format->pb) == static_cast<std::int64_t>(wav.size()));
    CHECK(avio_seek(format->pb, -1, SEEK_SET) < 0);
    CHECK(avio_seek(format->pb, (std::numeric_limits<std::int64_t>::max)(), SEEK_END) < 0);
    CHECK(av_seek_frame(format, 0, 0, AVSEEK_FLAG_BACKWARD) >= 0);
    const auto packet = std::unique_ptr<AVPacket, void(*)(AVPacket*)>(av_packet_alloc(),
        [](AVPacket* p) { av_packet_free(&p); });
    CHECK(packet != nullptr);
    std::size_t total = 0;
    while (av_read_frame(format, packet.get()) >= 0) {
        CHECK(total + packet->size <= samples * 2);
        CHECK(std::memcmp(packet->data, wav.data() + 44 + total, packet->size) == 0);
        total += packet->size;
        av_packet_unref(packet.get());
    }
    CHECK(total == samples * 2);
    CHECK(av_seek_frame(format, 0, 12000, AVSEEK_FLAG_BACKWARD) >= 0);
    CHECK(av_read_frame(format, packet.get()) >= 0 && packet->pts == 12000);
    av_packet_unref(packet.get());
    Demuxer preview(input.reader->clone());
    CHECK(preview.open(format->iformat));
    CHECK(avformat_find_stream_info(preview.format, nullptr) >= 0);
    CHECK(av_read_frame(preview.format, packet.get()) >= 0);
    av_packet_unref(packet.get());
    CHECK(av_read_frame(format, packet.get()) >= 0 && packet->pts > 12000);
}

void seek_contract(MemBuf& input, const Bytes& plain) {
    const auto size = static_cast<std::int64_t>(plain.size());
    CHECK(input.seek(7, SEEK_SET) == 7);
    CHECK(input.seek(0, AVSEEK_SIZE | AVSEEK_FORCE) == size);
    CHECK(input.seek(0, SEEK_CUR) == 7); // Size queries never move the cursor.
    CHECK(input.seek((std::numeric_limits<std::int64_t>::min)(), SEEK_CUR) < 0);
    CHECK(input.seek((std::numeric_limits<std::int64_t>::max)(), SEEK_END) < 0);
    CHECK(input.seek(1, AVSEEK_SIZE | SEEK_CUR) < 0);
    CHECK(input.seek(0, SEEK_CUR) == 7); // Rejected seeks also preserve position.
    CHECK(input.seek(-3, SEEK_END | AVSEEK_FORCE) == size - 3);
    Byte tail[3];
    CHECK(input.read(tail, 3) == 3);
    CHECK(std::memcmp(tail, plain.data() + plain.size() - 3, 3) == 0);
    CHECK(input.read(tail, 3) == AVERROR_EOF);
    CHECK(input.seek(1, SEEK_END) < 0);
    auto other = input.clone();
    CHECK(other->seek(0, SEEK_CUR) == 0);
    CHECK(input.seek(0, SEEK_CUR) == size);
    CHECK(input.seek(-size, SEEK_CUR) == 0);
}

void failure_contract(const KeyIv& key, const Bytes& wav) {
    auto source = std::make_shared<MemorySource>(transform_buffer(key, Mode::encrypt, wav.data(), wav.size()));
    DecryptMemBuf input(DecryptReader(source, key));
    source->fail = true;
    Byte bytes[16];
    CHECK(input.read(bytes, 16) == AVERROR(EIO)); // No exception crosses the AVIO callback.
    {
        Demuxer failed(std::make_unique<DecryptMemBuf>(DecryptReader(
            std::make_shared<MemorySource>(transform_buffer(key, Mode::encrypt, bytes, 16)), key)));
        CHECK(!failed.open()); // Unknown format fails during consumer-side probe.
        CHECK(failed.format == nullptr);
    }
    MemBuf empty(nullptr, 0);
    Byte byte;
    CHECK(empty.seek(0, AVSEEK_SIZE) == 0);
    CHECK(empty.read(&byte, 1) == AVERROR_EOF);
    CHECK(empty.seek(1, SEEK_SET) < 0);
    // Copyable option storage retains the prototype, not an AVIO or live cursor.
    std::shared_ptr<const MemBuf> original = std::make_shared<MemBuf>(wav.data(), wav.size());
    auto copy = original;
    original.reset();
    auto first = copy->clone();
    auto second = copy->clone();
    copy.reset();
    CHECK(first->seek(9, SEEK_SET) == 9);
    CHECK(second->seek(0, SEEK_CUR) == 0);
}

int main() {
    try {
        const std::uint32_t samples = 8000 * 30;
        Bytes wav{'R','I','F','F'};
        word(wav, samples*2+36, 4); wav.insert(wav.end(), {'W','A','V','E','f','m','t',' '});
        word(wav, 16, 4); word(wav, 1, 2); word(wav, 1, 2); word(wav, 8000, 4);
        word(wav, 16000, 4); word(wav, 2, 2); word(wav, 16, 2); wav.insert(wav.end(), {'d','a','t','a'});
        word(wav, samples*2, 4);
        for (std::uint32_t i = 0; i < samples; ++i) word(wav, i % 100, 2);
        const auto key = keygen::from_utf16(u"abc");
        auto source = std::make_shared<MemorySource>(transform_buffer(key, Mode::encrypt, wav.data(), wav.size()));
        auto encrypted = std::make_unique<DecryptMemBuf>(DecryptReader(source, key));
        CHECK(source->transferred == 32); // No full plaintext loading during construction.
        CHECK(encrypted->getSrc() == nullptr);
        seek_contract(*encrypted, wav);
        demux(std::move(encrypted), wav, samples);
        auto memory = std::make_unique<MemBuf>(wav.data(), wav.size());
        CHECK(memory->getSrc() == wav.data()); // Existing cache-reference identity.
        CHECK(memory->clone()->getSrc() == wav.data());
        seek_contract(*memory, wav);
        demux(std::move(memory), wav, samples);
        failure_contract(key, wav);
        std::cout << "PASS: MemBuf-derived decryption, explicit probe, memory/encrypted WAV payload, seek, independent preview and IO failures\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n'; return 1;
    }
}
