#pragma once
#include "EncryptionCore.h"
extern "C" {
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libavutil/mem.h>
}
#include <cerrno>

namespace encryption {
// Owns the AVIO allocation and its independent plaintext cursor. The source can
// be shared with a thumbnail decoder, but each AVFormatContext gets its own AVIO.
class DecryptAvio {
    DecryptReader reader_;
    AVIOContext* context_ = nullptr;
    static int read_packet(void* opaque, std::uint8_t* output, int capacity) noexcept {
        try {
            if (capacity <= 0) return AVERROR(EINVAL);
            const auto count = static_cast<DecryptAvio*>(opaque)->reader_.read(output,
                static_cast<std::size_t>(capacity));
            return count ? static_cast<int>(count) : AVERROR_EOF;
        } catch (...) { return AVERROR(EIO); }
    }
    static std::int64_t seek_packet(void* opaque, std::int64_t offset, int whence) noexcept {
        try {
            auto& reader = static_cast<DecryptAvio*>(opaque)->reader_;
            if (whence & AVSEEK_SIZE) return static_cast<std::int64_t>(reader.size());
            whence &= ~AVSEEK_FORCE;
            std::int64_t base = 0;
            switch (whence) {
            case SEEK_SET: break;
            case SEEK_CUR: base = static_cast<std::int64_t>(reader.tell()); break;
            case SEEK_END: base = static_cast<std::int64_t>(reader.size()); break;
            default: return AVERROR(EINVAL);
            }
            if (offset > 0 && base > (std::numeric_limits<std::int64_t>::max)() - offset)
                return AVERROR(EINVAL);
            if (offset < 0 && offset < -base) return AVERROR(EINVAL);
            reader.seek(static_cast<std::uint64_t>(base + offset));
            return base + offset;
        } catch (...) { return AVERROR(EINVAL); }
    }
public:
    explicit DecryptAvio(DecryptReader reader) : reader_(std::move(reader)) {
        auto* buffer = static_cast<unsigned char*>(av_malloc(32768));
        if (!buffer) throw std::bad_alloc();
        context_ = avio_alloc_context(buffer, 32768, 0, this, read_packet, nullptr, seek_packet);
        if (!context_) { av_free(buffer); throw std::bad_alloc(); }
        context_->seekable = AVIO_SEEKABLE_NORMAL;
    }
    DecryptAvio(const DecryptAvio&) = delete;
    DecryptAvio& operator=(const DecryptAvio&) = delete;
    ~DecryptAvio() {
        // FFmpeg can replace the original buffer while probing.
        av_freep(&context_->buffer);
        avio_context_free(&context_);
    }
    AVIOContext* get() const noexcept { return context_; }
    std::unique_ptr<DecryptAvio> clone() const {
        return std::make_unique<DecryptAvio>(std::move(*reader_.clone()));
    }
    bool open(AVFormatContext** format, const AVInputFormat* input_format = nullptr) {
        *format = avformat_alloc_context();
        if (!*format) return false;
        (*format)->pb = context_;
        (*format)->flags |= AVFMT_FLAG_CUSTOM_IO;
        return avformat_open_input(format, nullptr, input_format, nullptr) >= 0;
    }
};
} // namespace encryption
