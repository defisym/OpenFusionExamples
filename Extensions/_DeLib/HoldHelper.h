#pragma once

// HoldHelper
// keep old value then restore it by RAII
// NOTE: grantee that T is copyable

template<typename T>
struct HoldHelper {
	T* pDest = nullptr;
	T old = {};

	HoldHelper(T* p) :pDest(p), old(*p) {}
    ~HoldHelper() { *pDest = old; }
};
