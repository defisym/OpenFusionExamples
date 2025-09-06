#pragma once

#include "CopyAdapter.h"

struct CopyAdapterBitmap final :CopyAdapter {
    CopyAdapterBitmap(LPRDATA p) :CopyAdapter(p) {};
    ~CopyAdapterBitmap() = default;

    bool InitTexture(LPSURFACE& pSf,
        const int width, const int height) override;
    bool CopyTexture(LPSURFACE pMemSf,
        const unsigned char* pData, const int srcLineSz, const int reserved) override;
};