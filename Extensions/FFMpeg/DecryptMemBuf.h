#pragma once

#include "MemBuf.h"
#include "Encryption/Core.h"

// Plugin-side glue only. DecryptReader remains an independent encryption
// component with no FFmpeg types, error codes or probing responsibilities.
class DecryptMemBuf final : public MemBuf {
    encryption::DecryptReader reader;
public:
    explicit DecryptMemBuf(encryption::DecryptReader source) : reader(std::move(source)) {}
    int read(std::uint8_t* output, int capacity) noexcept override {
        if (!output || capacity <= 0) return AVERROR(EINVAL);
        try {
            const auto count = reader.read(output, static_cast<std::size_t>(capacity));
            return count ? static_cast<int>(count) : AVERROR_EOF;
        } catch (...) { return AVERROR(EIO); }
    }
    std::int64_t seek(std::int64_t offset, int whence) noexcept override {
        const auto result = seekPosition(offset, whence,
            static_cast<std::int64_t>(reader.tell()), static_cast<std::int64_t>(reader.size()));
        if (result >= 0 && !(whence & AVSEEK_SIZE)) {
            try { reader.seek(static_cast<std::uint64_t>(result)); }
            catch (...) { return AVERROR(EIO); }
        }
        return result;
    }
    std::unique_ptr<MemBuf> clone() const override {
        return std::make_unique<DecryptMemBuf>(std::move(*reader.clone()));
    }
};
