#pragma once

#include <cstring>

template<typename DataType>
struct RingBuffer {
	DataType* pBuffer = nullptr;
	size_t bufferSz = 0;

	explicit RingBuffer(const size_t sz) :bufferSz(sz) {
		AllocBuffer();
	}

	virtual ~RingBuffer() {
		ReleaseBuffer();
	}

    void AllocBuffer() {
        pBuffer = new DataType[bufferSz];
        ResetBuffer();
    }

	void ReleaseBuffer() {
		delete[] pBuffer;
		pBuffer = nullptr;
	}

    virtual void ResetBuffer() {
        memset(pBuffer, 0, sizeof(DataType) * bufferSz);
    }

	// ------------------------
	// Read & Write logic are almost the same
	// but allow some spaghetti code here
	// ------------------------
	virtual void ResetIndex() {
		writeIndex = 0;
		readIndex = 0;
	}

	size_t writeIndex = 0;

	virtual void WriteData(const DataType* pBuf, const size_t sz) {
		const auto remain = bufferSz - writeIndex;
		const auto write = (std::min)(sz, remain);
		const auto left = sz - write;

		const auto pStart = pBuffer + writeIndex;
		memcpy(pStart, pBuf, sizeof(DataType) * write);

		writeIndex += write;
		if (writeIndex == bufferSz) { writeIndex = 0; }

		if (left == 0) { return; }
		RingBuffer::WriteData(pBuf + write, left);
	}

	size_t readIndex = 0;

	virtual void ReadData(DataType* pBuf, const size_t sz) {
		const auto remain = bufferSz - readIndex;
		const auto read = (std::min)(sz, remain);
		const auto left = sz - read;

		const auto pStart = pBuffer + readIndex;
		memcpy(pBuf, pStart, sizeof(DataType) * read);

		readIndex += read;
		if (readIndex == bufferSz) { readIndex = 0; }

		if (left == 0) { return; }
		RingBuffer::ReadData(pBuf + read, left);
	}
};
