#pragma once

#include <cstring>
#include <new>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/mem.h>
}

// 4KB
constexpr auto MEM_BUFFER_SIZE = 4 * 1024;

class MemBuf {
private:
	int64_t bufferIndex = 0;

	uint8_t* pBuf = nullptr;
	int bufSize = 0;
    bool ownsBuffer = true;

	uint8_t* pSrcBuf = nullptr;
	int srcBufSize = 0;

public:
    void releaseBuffer() { ownsBuffer = false; }
	MemBuf(uint8_t* pSrcBuf, int srcBufSize, int bufSize = MEM_BUFFER_SIZE) {
		this->pSrcBuf = pSrcBuf;
		this->srcBufSize = srcBufSize;

		this->pBuf = static_cast<uint8_t*>(av_malloc(bufSize));
        if (!pBuf) throw std::bad_alloc();
		this->bufSize = bufSize;
	}

	~MemBuf() {
		if (ownsBuffer) av_free(this->pBuf);
	}

	inline const uint8_t* getSrc() const {
		return this->pSrcBuf;
	}

	inline int getSrcSize() const {
		return srcBufSize;
	}

	inline uint8_t* get() const {
		return pBuf;
	}

	inline int getSize() const {
		return bufSize;
	}

	inline int read(uint8_t* buf, int buf_size) {
		auto dataLeft = srcBufSize - bufferIndex;
		//memset(buf, 0, buf_size);

		if (dataLeft >= buf_size) {
			memcpy(buf, this->pSrcBuf + bufferIndex, buf_size);
			bufferIndex += buf_size;

			return buf_size;
		}
		else if (dataLeft > 0) {
			memcpy(buf, this->pSrcBuf + bufferIndex, (size_t)dataLeft);
			bufferIndex += dataLeft;

			return (size_t)dataLeft;
		}
		else {
			return AVERROR_EOF;
		}
	}

	inline int64_t seek(int64_t offset, int whence) {
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
