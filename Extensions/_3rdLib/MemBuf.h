#pragma once

// #include <queue>

extern "C" {
#include <libavformat/avformat.h>
}

// 4KB
constexpr auto MEM_BUFFER_SIZE = 4 * 1024;

class MemBuf {
private:
	int64_t bufferIndex = 0;

	uint8_t* pBuf = nullptr;
	int bufSize = 0;

	uint8_t* pSrcBuf = nullptr;
	int srcBufSize = 0;

public:
	MemBuf(uint8_t* pSrcBuf, int srcBufSize, int bufSize = MEM_BUFFER_SIZE) {
		this->pSrcBuf = pSrcBuf;
		this->srcBufSize = srcBufSize;

		this->pBuf = new uint8_t[bufSize];
		this->bufSize = bufSize;
	}

	~MemBuf() {
		delete[] this->pBuf;
	}

	inline uint8_t* get() {
		return pBuf;
	}

	inline int getSize() {
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
		switch (whence) {
		case SEEK_SET:
			bufferIndex = 0 + offset;

			return bufferIndex;
		case SEEK_CUR:
			bufferIndex = bufferIndex + offset;

			return bufferIndex;
		case SEEK_END:
			bufferIndex = srcBufSize + offset;

			return bufferIndex;
		case AVSEEK_SIZE:
			return -1;
		default:
			return -1;
		}
	}
};