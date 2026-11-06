#pragma once
// Deliberately tiny DX11 stand-ins for testing CaptureCompletion's control flow.
// They do not simulate GPU execution and are never on the plugin include path.
#include <cstdint>
#include <vector>
using HRESULT = std::int32_t;
using BOOL = int;
using UINT = unsigned;
constexpr HRESULT S_OK = 0, S_FALSE = 1, E_UNEXPECTED = -1, E_POINTER = -2, E_INVALIDARG = -3, E_FAIL = -4;
constexpr BOOL FALSE = 0, TRUE = 1;
constexpr int ERROR_BUSY = 170;
#define HRESULT_FROM_WIN32(x) (-static_cast<HRESULT>(x))
#define FAILED(x) ((x) < 0)
constexpr int D3D11_QUERY_EVENT = 0;
constexpr UINT D3D11_ASYNC_GETDATA_DONOTFLUSH = 1;
enum D3D11_DEVICE_CONTEXT_TYPE { D3D11_DEVICE_CONTEXT_IMMEDIATE, D3D11_DEVICE_CONTEXT_DEFERRED };
struct D3D11_QUERY_DESC { int Query; UINT MiscFlags; };
struct MockRef {
    int refs = 1;
    void AddRef() { ++refs; }
    void Release() { --refs; }
};
struct ID3D11Query : MockRef {};
inline int mockSleeps = 0;
inline void Sleep(int) { ++mockSleeps; }
struct ID3D11Device : MockRef {
    ID3D11Query event;
    HRESULT result = S_OK;
    int allocations = 0;
    HRESULT CreateQuery(const D3D11_QUERY_DESC* desc, ID3D11Query** out) {
        ++allocations;
        if (desc->Query != D3D11_QUERY_EVENT || desc->MiscFlags != 0) return E_FAIL;
        if (FAILED(result)) return result;
        event.AddRef();
        *out = &event;
        return S_OK;
    }
};
struct ID3D11DeviceContext : MockRef {
    D3D11_DEVICE_CONTEXT_TYPE type = D3D11_DEVICE_CONTEXT_IMMEDIATE;
    struct Reply { HRESULT hr; BOOL done; };
    std::vector<Reply> replies;
    std::vector<int> operations;
    unsigned polls = 0;
    bool correctFlags = true;
    auto GetType() const { return type; }
    void End(ID3D11Query*) { operations.push_back(1); }
    void Flush() { operations.push_back(2); }
    HRESULT GetData(ID3D11Query*, void* data, UINT size, UINT flags) {
        correctFlags &= size == sizeof(BOOL) && flags == D3D11_ASYNC_GETDATA_DONOTFLUSH;
        Reply reply = polls < replies.size() ? replies[polls] : Reply{S_OK, TRUE};
        ++polls;
        *static_cast<BOOL*>(data) = reply.done;
        return reply.hr;
    }
};
