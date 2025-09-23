#pragma once

#include <cassert>
#include <algorithm>
#include <functional>

#include "Buffer.h"

template<typename Type>
struct RingBuffer :Buffer<Type> {
    size_t readIndex = 0;
    size_t writeIndex = 0;
    size_t elementCount = 0;

    // TODO: Overflow Not Protected!
    virtual size_t GetElementCount() { return elementCount; }

    RingBuffer() = default;
    RingBuffer(const size_t sz) :Buffer<Type>(sz) {}

    using Callback = std::function<void(Type*)>;

    // ------------------------
    // Read & Write logic are almost the same
    // but allow some spaghetti code here
    // ------------------------

    // copy pData to buffer
    virtual void WriteData(const Type* pData, const size_t dataSize) {
        const auto remain = this->_sz - writeIndex;
        const auto write = (std::min)(dataSize, remain);
        const auto left = dataSize - write;

        const auto pStart = this->_pBuf + writeIndex;
        memcpy(pStart, pData, sizeof(Type) * write);

        elementCount += write;
        writeIndex += write;
        if (writeIndex == this->_sz) { writeIndex = 0; }

        if (left == 0) { return; }
        RingBuffer::WriteData(pData + write, left);
    }

    // get buffer at current index and forward sz
    // caller should grantee that pointer and offset are valid
    // then write data
    virtual void WriteData(const size_t sz, const Callback& cb) {
        assert(sz + writeIndex <= this->_sz);
        auto pStart = this->_pBuf + writeIndex;
        cb(pStart);

        elementCount += sz;
        writeIndex += sz;
        if (writeIndex == this->_sz) { writeIndex = 0; }
    }

    // copy buffer to pBuf
    virtual void ReadData(Type* pBuf, const size_t sz) {
        const auto remain = this->_sz - readIndex;
        const auto read = (std::min)(sz, remain);
        const auto left = sz - read;

        const auto pStart = this->_pBuf + readIndex;
        memcpy(pBuf, pStart, sizeof(Type) * read);

        elementCount -= read;
        readIndex += read;
        if (readIndex == this->_sz) { readIndex = 0; }

        if (left == 0) { return; }
        RingBuffer::ReadData(pBuf + read, left);
    }

    // get buffer at current index and forward sz
    // caller should grantee that pointer and offset are valid
    // then read data
    //
    // return nullptr if data not enough (readIndex + sz > writeIndex)
    virtual void ReadData(const size_t sz, const Callback& cb) {
        assert(sz + readIndex <= this->_sz);
        //if (readIndex + sz > writeIndex) { cb(nullptr); return; }
        if (elementCount == 0) { cb(nullptr); return; }

        auto pStart = this->_pBuf + readIndex;
        cb(pStart);

        elementCount -= sz;
        readIndex += sz;
        if (readIndex == this->_sz) { readIndex = 0; }
    }

    virtual void ResetIndex() { writeIndex = 0; readIndex = 0; }

    virtual void DiscardUnreadBuffer() { readIndex = writeIndex; elementCount = 0; }
};
