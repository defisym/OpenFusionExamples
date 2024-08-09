#pragma once

#include <SDL_atomic.h>
#pragma comment(lib, "SDL2.lib")

#include "RingBuffer.h"
#include "LockHelper.h"

template<typename DataType>
struct ThreadSafeRingBuffer :public RingBuffer<DataType> {
	SDL_SpinLock lock = 0;

	explicit ThreadSafeRingBuffer(const size_t sz) :RingBuffer<DataType>(sz) {}

	~ThreadSafeRingBuffer() override {
		const auto lockHelper = LockHelper(&lock);
		RingBuffer<DataType>::ReleaseBuffer();
	}

	void WriteData(const DataType* pBuf, const size_t sz) override {
		const auto lockHelper = LockHelper(&lock);
		RingBuffer<DataType>::WriteData(pBuf, sz);
	}
	void ReadData(DataType* pBuf, const size_t sz) override {
		const auto lockHelper = LockHelper(&lock);
		RingBuffer<DataType>::ReadData(pBuf, sz);
	}
};
