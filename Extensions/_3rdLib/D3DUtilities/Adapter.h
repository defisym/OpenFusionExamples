#pragma once

#include "DXGI.h"

#include <vector>

struct Adapter {
    DXGI& DXGIRef;

    std::vector<DXGI::ComPtr<IDXGIAdapter3>> pAdapters = {};
    UINT AdapterOrdinal = 0;

    inline HRESULT EnumAdapters() {
        HRESULT hr;
        DXGI::ComPtr<IDXGIAdapter> pTempAdapter;
        AdapterOrdinal = 0;

        while (DXGIRef.pDXGIFactory->EnumAdapters(AdapterOrdinal, &pTempAdapter) != DXGI_ERROR_NOT_FOUND) {
            DXGI::ComPtr<IDXGIAdapter3> pDXGIAdapter = nullptr;
            hr = pTempAdapter->QueryInterface(IID_PPV_ARGS(&pDXGIAdapter));
            
            if (FAILED(hr)) {              
                return hr;
            }

            pAdapters.emplace_back(pDXGIAdapter);
            ++AdapterOrdinal;
        }

        return S_OK;
    }

    Adapter(DXGI& DXGI) : DXGIRef(DXGI) {
        if (EnumAdapters() != S_OK) {
            throw D3DException("Failed to query IDXGIAdapter3 interface from selected adapter.");
        }

    }

    ~Adapter() = default;
};

