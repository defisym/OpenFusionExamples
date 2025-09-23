#pragma once

#ifndef OFE_BUFFER_H
#define OFE_BUFFER_H

#include <cstring>

template<typename Type>
struct Buffer {  // NOLINT(cppcoreguidelines-special-member-functions)
	Type* _pBuf = nullptr;
	size_t _sz = 0;
	size_t _capacity = 0;

    Buffer() = default;
    Buffer(const size_t sz) { Alloc(sz); Reset(); }
	virtual ~Buffer() { Release(); }

	void Release() {
		delete[] _pBuf;
		_pBuf = nullptr;
		_capacity = 0;
	}

	virtual bool Alloc(const size_t sz) {
		_sz = sz;
		if (_capacity >= sz) { return false; }

		Release();
		_pBuf = new Type[sz];
		_capacity = sz;
		return true;
	}
    virtual bool Extend(const size_t sz, int val = 0) {
		_sz = sz;
		if (_capacity >= sz) { return false; }

        auto pBuf = new Type[sz];
        memset(pBuf, val, sizeof(Type) * sz);
        memcpy(pBuf, _pBuf, sizeof(Type) * _capacity);

        Release(); 
		_pBuf = pBuf;
		_capacity = sz;
        return true;
    }

	// wrapper of memmove, to shift the elements in buffer
	void* Shift(const size_t start, const size_t elementCount) {
		return memmove(_pBuf, _pBuf + start,
		sizeof(Type) * elementCount);
	}

	virtual void Reset(int val = 0) {
		memset(_pBuf, val, sizeof(Type) * _capacity);
	}

    void Copy(const Buffer* pSrc) {
        Alloc(pSrc->_sz);   // size will be updated in alloc
        memcpy(this->_pBuf, pSrc->_pBuf, sizeof(Type) * pSrc->_sz);
    }

    Type* GetBuffer() { return _pBuf; }
};

template<typename Type = ORLProcessValueType>
void CopyToBuffer(Buffer<Type>* pBuf, const Type* pSrc, const size_t sz) {
    pBuf->Alloc(sz);   // size will be updated in alloc
	pBuf->Reset();
    memcpy(pBuf->_pBuf, pSrc, sizeof(Type) * sz);
}

// convert if possible
template<typename Source, typename Type = ORLProcessValueType>
void CopyToBuffer(Buffer<Type>* pBuf, const Source& src, const size_t sz) {
    CopyToBuffer(pBuf, static_cast<const Type*>(src), sz);
}

template<typename Type = ORLProcessValueType>
struct IndexBuffer :Buffer<Type> {
	bool bFilled = false;
	size_t index = 0;

    IndexBuffer() = default;
    IndexBuffer(const size_t sz) :Buffer<Type>(sz) {}

	[[nodiscard]] bool Filled() const { return bFilled; }

	// return true if filled this time
	bool AddData(const Type* pData, const size_t dataSize) {
		memcpy(this->_pBuf + index,
		   pData,
		   sizeof(Type) * dataSize);
		index += dataSize;
		if (index >= this->_sz) { bFilled = true; }

		return bFilled;
	}

	void Reset(int val = 0) {
		Buffer<Type>::Reset(val);
		ResetState();
	}

    void Copy(const IndexBuffer* pSrc) {
        Buffer<Type>::Copy(pSrc);
		CopyState(pSrc);
    }

	void ResetState() {
		bFilled = false;
		index = 0;
	}

	void CopyState(const IndexBuffer* pSrc) {
		this->bFilled = pSrc->bFilled;
		this->index = pSrc->index;
	}

    // return current pointer
    Type* GetData() { return this->GetBuffer() + index; }
};

#endif // OFE_BUFFER_H
