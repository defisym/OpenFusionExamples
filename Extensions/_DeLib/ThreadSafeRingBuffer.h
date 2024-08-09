#pragma once

#include <SDL_atomic.h>
#pragma comment(lib, "SDL2.lib")

#include "RingBuffer.h"
#include "LockHelper.h"

struct ThreadSafeRingBuffer :public RingBuffer {
	SDL_SpinLock lock = 0;

	explicit ThreadSafeRingBuffer(const size_t sz) :RingBuffer(sz) {}

	~ThreadSafeRingBuffer() override {
		const auto lockHelper = LockHelper(&lock);
		RingBuffer::ReleaseBuffer();
	}

	void WriteData(const uint8_t* pBuf, const size_t sz) override {
		const auto lockHelper = LockHelper(&lock);
		RingBuffer::WriteData(pBuf, sz);
	}
	void ReadData(uint8_t* pBuf, const size_t sz) override {
		const auto lockHelper = LockHelper(&lock);
		RingBuffer::ReadData(pBuf, sz);
	}
};
