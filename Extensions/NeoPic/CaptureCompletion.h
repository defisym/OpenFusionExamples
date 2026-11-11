#pragma once
#include <d3d11.h>
#include <wrl/client.h>

// An event query marks GPU execution completion, not the validity of the pixels.
// Prepare before issuing GPU work; Submit afterwards; Wait for synchronous capture.
class CaptureCompletion {
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<ID3D11Query> query;
    bool submitted = false;
public:
    CaptureCompletion() = default;
    CaptureCompletion(const CaptureCompletion&) = delete;
    CaptureCompletion& operator=(const CaptureCompletion&) = delete;

    HRESULT Prepare(ID3D11Device* device, ID3D11DeviceContext* immediate) {
        if (!device || !immediate) return E_POINTER;
        if (query) return HRESULT_FROM_WIN32(ERROR_BUSY);
        if (immediate->GetType() != D3D11_DEVICE_CONTEXT_IMMEDIATE) return E_INVALIDARG;
        const D3D11_QUERY_DESC desc{D3D11_QUERY_EVENT, 0};
        const HRESULT hr = device->CreateQuery(&desc, query.GetAddressOf());
        if (FAILED(hr)) return hr;
        context = immediate;
        return S_OK;
    }

    void Submit() {
        context->End(query.Get());
        // Submit once so DONOTFLUSH polling can make progress even if Fusion is
        // idle. Flush submits asynchronously; it does not wait for GPU completion.
        context->Flush();
        submitted = true;
    }

    bool InFlight() const { return submitted; }

    HRESULT Poll() const {
        if (!submitted) return E_UNEXPECTED;
        BOOL done = FALSE;
        const HRESULT hr = context->GetData(query.Get(), &done, sizeof(done),
            D3D11_ASYNC_GETDATA_DONOTFLUSH);
        if (hr == S_OK) return done ? S_OK : S_FALSE;
        if (hr == S_FALSE || FAILED(hr)) return hr;
        return E_UNEXPECTED;
    }

    // Synchronous capture and teardown must not release a borrowed WGC frame
    // until the submitted work finishes. Flush submits; this actually waits.
    HRESULT Wait() const {
        HRESULT hr;
        while ((hr = Poll()) == S_FALSE) Sleep(1);
        return hr;
    }

    HRESULT WaitForTeardown() const { return Wait(); }

    void Reset() {
        submitted = false;
        query.Reset();
        context.Reset();
    }
};
