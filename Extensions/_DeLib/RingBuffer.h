#pragma once

#include <cstdint>
#include <cstring>

struct RingBuffer {
	uint8_t* pBuffer = nullptr;
	size_t bufferSz = 0;

	explicit RingBuffer(const size_t sz) :bufferSz(sz) {
		AllocBuffer();
	}

	virtual ~RingBuffer() {
		ReleaseBuffer();
	}

	void AllocBuffer() {
		pBuffer = new uint8_t[bufferSz];
		memset(pBuffer, 0, sizeof(uint8_t) * bufferSz);
	}

	void ReleaseBuffer() {
		delete[] pBuffer;
		pBuffer = nullptr;
	}

	// ------------------------
	// Read & Write logic are almost the same
	// but allow some spaghetti code here
	// ------------------------

	size_t writeIndex = 0;

	virtual void WriteData(const uint8_t* pBuf, const size_t sz) {
		const auto remain = bufferSz - writeIndex;
		const auto write = (std::min)(sz, remain);
		const auto left = sz - write;

		const auto pStart = pBuffer + writeIndex;
		memcpy(pStart, pBuf, sizeof(uint8_t) * write);

		writeIndex += write;
		if (writeIndex == bufferSz) { writeIndex = 0; }

		if (left == 0) { return; }
		RingBuffer::WriteData(pBuf + write, left);
	}

	size_t readIndex = 0;

	virtual void ReadData(uint8_t* pBuf, const size_t sz) {
		const auto remain = bufferSz - readIndex;
		const auto read = (std::min)(sz, remain);
		const auto left = sz - read;

		const auto pStart = pBuffer + readIndex;
		memcpy(pBuf, pStart, sizeof(uint8_t) * read);

		readIndex += read;
		if (readIndex == bufferSz) { readIndex = 0; }

		if (left == 0) { return; }
		RingBuffer::ReadData(pBuf + read, left);
	}
};
