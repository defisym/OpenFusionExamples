#pragma once

#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>

extern "C" {
#include <libavformat/avformat.h>
}

// Read/seek cursor over caller-owned media data. The source must outlive the view.
class MemView {
private:
    int64_t bufferIndex = 0;
    const uint8_t* pSrcBuf = nullptr;
    int srcBufSize = 0;

public:
    MemView(const uint8_t* source, int size) : pSrcBuf(source), srcBufSize(size) {}

    const uint8_t* getSrc() const { return pSrcBuf; }
    int getSrcSize() const { return srcBufSize; }

    int read(uint8_t* buffer, int size) {
        if (size <= 0) return AVERROR(EINVAL);
        const auto remaining = srcBufSize - bufferIndex;
        if (!remaining) return AVERROR_EOF;
        const int count = static_cast<int>((std::min)(remaining, static_cast<int64_t>(size)));
        std::memcpy(buffer, pSrcBuf + bufferIndex, count);
        bufferIndex += count;
        return count;
    }

    int64_t seek(int64_t offset, int whence) {
        if (whence == AVSEEK_SIZE) return srcBufSize;
        const int origin = whence & ~AVSEEK_FORCE;
        int64_t base = 0;
        switch (origin) {
        case SEEK_SET: break;
        case SEEK_CUR: base = bufferIndex; break;
        case SEEK_END: base = srcBufSize; break;
        default: return AVERROR(EINVAL);
        }
        if (offset < -base || offset > srcBufSize - base) return AVERROR(EINVAL);
        bufferIndex = base + offset;
        return bufferIndex;
    }
};
