#pragma once

#include "DXGI.h"

#include <vector>

struct Adapter {
    std::vector<ComPtr<IDXGIAdapter3>> pAdapters = {};
    UINT AdapterOrdinal = 0;

    inline HRESULT EnumAdapters(IDXGIFactory2* pDXGIFactory) {
        HRESULT hr;

        pAdapters.clear();
        AdapterOrdinal = 0;

        ComPtr<IDXGIAdapter> pTempAdapter;
        while (pDXGIFactory->EnumAdapters(AdapterOrdinal, &pTempAdapter) != DXGI_ERROR_NOT_FOUND) {
            ComPtr<IDXGIAdapter3> pDXGIAdapter = nullptr;
            hr = pTempAdapter->QueryInterface(IID_PPV_ARGS(&pDXGIAdapter));
            
            if (FAILED(hr)) {              
                return hr;
            }

            pAdapters.emplace_back(pDXGIAdapter);
            ++AdapterOrdinal;
        }

        return S_OK;
    }

	Adapter() = default;
    Adapter(IDXGIFactory2* pDXGIFactory) {
		if(EnumAdapters(pDXGIFactory) == S_OK){return;}
        throw D3DException("Failed to query IDXGIAdapter3 interface from selected adapter.");
    }

    ~Adapter() = default;
};

