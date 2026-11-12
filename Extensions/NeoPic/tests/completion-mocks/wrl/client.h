#pragma once
namespace Microsoft::WRL {
template<class T> class ComPtr {
    T* ptr = nullptr;
public:
    ~ComPtr() { Reset(); }
    T* Get() const { return ptr; }
    T* operator->() const { return ptr; }
    explicit operator bool() const { return ptr != nullptr; }
    T** GetAddressOf() { return &ptr; }
    ComPtr& operator=(T* value) {
        if (value) value->AddRef();
        Reset();
        ptr = value;
        return *this;
    }
    void Reset() { if (ptr) ptr->Release(); ptr = nullptr; }
};
}
