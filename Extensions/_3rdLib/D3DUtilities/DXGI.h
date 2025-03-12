#pragma once

#include <wrl.h>
#include <dxgi1_4.h>
#pragma comment(lib,"DXGI.lib")

#include "D3DException.h"

struct DXGI {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<IDXGIFactory2> pDXGIFactory = nullptr;
    UINT dxgiFactoryFlags = 0;

    DXGI(UINT flags = 0) {
        dxgiFactoryFlags = flags;
        auto hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pDXGIFactory));
        if (FAILED(hr)) {
            throw D3DException("Failed to create DXGI Factory, hr={:#10x}", hr);
        }
    }

    ~DXGI() = default;
};