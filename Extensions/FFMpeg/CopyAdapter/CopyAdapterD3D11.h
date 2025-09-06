#pragma once

#include <memory>

#include "CopyAdapter.h"

#include "Macro.h"
#include "D3D/D3DHandler.h"

struct CopyAdapterD3D11 final :CopyAdapter {
    inline static std::unique_ptr<D3DSharedHandler> pD3DSharedHandler = nullptr;
    std::unique_ptr<D3DLocalHandler> pD3DLocalHandler = nullptr;

    CopyAdapterD3D11(LPRDATA p);
    ~CopyAdapterD3D11() = default;

    bool InitTexture(LPSURFACE& pSf,
        const int width, const int height) override;
    bool CopyTexture(LPSURFACE pRTTSf,
        const unsigned char* pData, const int width, const int height) override;
};