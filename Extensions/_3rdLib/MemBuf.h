#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
extern "C" {
#include <libavformat/avio.h>
#include <libavutil/error.h>
}
#include <cerrno>

constexpr auto MEM_BUFFER_SIZE = 4 * 1024;

// Simple borrowed-memory reader. Derived readers supply read/seek without
// changing the player's AVIO callbacks. FFmpeg owns its own scratch buffer.
class MemBuf {
    std::int64_t bufferIndex = 0;
    const std::uint8_t* pSrcBuf = nullptr;
    std::int64_t srcBufSize = 0;
protected:
    MemBuf() = default;
    static std::int64_t seekPosition(std::int64_t offset, int whence,
                                    std::int64_t position, std::int64_t size) noexcept {
        if (whence == AVSEEK_SIZE || whence == (AVSEEK_SIZE | AVSEEK_FORCE)) return size;
        std::int64_t base = 0;
        switch (whence & ~AVSEEK_FORCE) {
        case SEEK_SET: break;
        case SEEK_CUR: base = position; break;
        case SEEK_END: base = size; break;
        default: return AVERROR(EINVAL);
        }
        if (offset < -base || offset > size - base) return AVERROR(EINVAL);
        return base + offset;
    }
public:
    MemBuf(const std::uint8_t* source, std::size_t size) : pSrcBuf(source) {
        if ((!source && size) || size > static_cast<std::uint64_t>((std::numeric_limits<std::int64_t>::max)()))
            throw std::invalid_argument("Invalid memory input");
        srcBufSize = static_cast<std::int64_t>(size);
    }
    virtual ~MemBuf() = default;
    const std::uint8_t* getSrc() const { return pSrcBuf; }
    std::int64_t getSrcSize() const { return srcBufSize; }
    virtual int read(std::uint8_t* output, int capacity) noexcept {
        if (!output || capacity <= 0) return AVERROR(EINVAL);
        const auto count = static_cast<int>((std::min)(srcBufSize - bufferIndex, static_cast<std::int64_t>(capacity)));
        if (!count) return AVERROR_EOF;
        std::memcpy(output, pSrcBuf + bufferIndex, static_cast<std::size_t>(count));
        bufferIndex += count;
        return count;
    }
    virtual std::int64_t seek(std::int64_t offset, int whence) noexcept {
        const auto result = seekPosition(offset, whence, bufferIndex, srcBufSize);
        if (result >= 0 && !(whence & AVSEEK_SIZE)) bufferIndex = result;
        return result;
    }
    // Fresh cursor for preview decoding; source memory must outlive all clones.
    virtual std::unique_ptr<MemBuf> clone() const {
        return std::make_unique<MemBuf>(pSrcBuf, static_cast<std::size_t>(srcBufSize));
    }
};
