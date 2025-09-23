#pragma once

#include "RingBuffer.h"
#include "LockHelper.h"

template<typename Type, LockConcept LockType = SDL_SpinLock>
struct ThreadSafeRingBuffer :private RingBuffer<Type> {
    Lock<LockType> lock;
    using BufferLockHelper = typename GetLockHelper<LockType>::Type;
    using Callback = typename RingBuffer<Type>::Callback;

    ThreadSafeRingBuffer() = default;
    ThreadSafeRingBuffer(const size_t sz) :RingBuffer<Type>(sz) {}

    ~ThreadSafeRingBuffer() override {
        const auto lockHelper = BufferLockHelper(lock);
        RingBuffer<Type>::Release();
    }

    size_t GetElementCount() override {
        const auto lockHelper = BufferLockHelper(lock);
        return RingBuffer<Type>::GetElementCount();
    }

    size_t GetWriteIndex() {
        const auto lockHelper = BufferLockHelper(lock);
        return RingBuffer<Type>::writeIndex;
    }

    size_t GetReadIndex() {
        const auto lockHelper = BufferLockHelper(lock);
        return RingBuffer<Type>::readIndex;
    }

    void WriteData(const Type* pBuf, const size_t sz) override {
        const auto lockHelper = BufferLockHelper(lock);
        RingBuffer<Type>::WriteData(pBuf, sz);
    }
    void WriteData(const size_t sz, const Callback& cb) override {
        const auto lockHelper = BufferLockHelper(lock);
        RingBuffer<Type>::WriteData(sz, cb);
    }
    void ReadData(Type* pBuf, const size_t sz) override {
        const auto lockHelper = BufferLockHelper(lock);
        RingBuffer<Type>::ReadData(pBuf, sz);
    }
    void ReadData(const size_t sz, const Callback& cb) override {
        const auto lockHelper = BufferLockHelper(lock);
        RingBuffer<Type>::ReadData(sz, cb);
    }
    void ResetIndex() override {
        const auto lockHelper = BufferLockHelper(lock);
        RingBuffer<Type>::ResetIndex();
    }
    void DiscardUnreadBuffer() override {
        const auto lockHelper = BufferLockHelper(lock);
        RingBuffer<Type>::DiscardUnreadBuffer();
    }

    bool Alloc(const size_t sz) override {
        const auto lockHelper = BufferLockHelper(lock);
        return RingBuffer<Type>::Alloc(sz);
    }
    bool Extend(const size_t sz, int val = 0) override {
        const auto lockHelper = BufferLockHelper(lock);
        return RingBuffer<Type>::Extend(sz, val);
    }
    void Reset(int val = 0) override {
        const auto lockHelper = BufferLockHelper(lock);
        RingBuffer<Type>::Reset(val);
    }
};