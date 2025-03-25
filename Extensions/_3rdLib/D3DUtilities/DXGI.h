#pragma once

#include <dxgi1_4.h>
#pragma comment(lib,"DXGI.lib")

#include "D3DException.h"
#include "D3DDefinition.h"

// define this macro if dynamic link is needed
//#define DYNAMIC_LINK_DXGI

struct DXGI {
#ifdef DYNAMIC_LINK_DXGI
    HINSTANCE hDXGI = nullptr;
#endif

    ComPtr<IDXGIFactory2> pDXGIFactory = nullptr;
    UINT dxgiFactoryFlags = 0;

    DXGI(UINT flags = 0) {
#ifdef DYNAMIC_LINK_DXGI
        wchar_t rootDir[MAX_PATH] = {};
        GetCurrentDirectory(MAX_PATH - 1, rootDir);

        wchar_t dllPath[2 * MAX_PATH] = {};
        wsprintf(dllPath, L"%s\\%s", rootDir, L"Modules\\DXGI.DLL");

        hDXGI = LoadLibrary(dllPath);

        if (hDXGI == nullptr) {
            MSGBOX(L"Load Failed");
        }
#endif

        dxgiFactoryFlags = flags;
        auto hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pDXGIFactory));
        if (FAILED(hr)) {
            throw D3DException("Failed to create DXGI Factory, hr={:#10x}", hr);
        }
    }

    ~DXGI() {
#ifdef DYNAMIC_LINK_DXGI
        if (hDXGI != nullptr) {
            FreeLibrary(hDXGI);
            hDXGI = nullptr;
        }
#endif
    }
};