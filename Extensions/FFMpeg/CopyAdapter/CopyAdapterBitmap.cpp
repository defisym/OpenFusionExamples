#include "CopyAdapterBitmap.h"

#include "common.h"

bool CopyAdapterBitmap::InitTexture(LPSURFACE& pSf, 
    const int width, const int height) {
    if (CopyAdapter::TextureValid(pSf, width, height) && !IsHWA(pSf)) {
        return false;
    }

    delete pSf;
#ifdef VIDEO_ALPHA
    pSf = CreateSurface(32, width, height);
#else
    pSf = CreateSurface(24, width, height);
#endif

    _AddAlpha(pSf);

    return true;
}

bool CopyAdapterBitmap::CopyTexture(LPSURFACE pMemSf,
    const unsigned char* pData, const int srcLineSz, const int reserved) {
    // validate
    if (!CopyAdapter::CopyValid(pMemSf, pData)) { return false; }

    // pMemSf must have alpha channel, see `InitSurface`
    auto sfCoef = GetSfCoef(pMemSf);
    if (sfCoef.pData == nullptr || sfCoef.pAlphaData == nullptr) {
        return false;
    }

    auto lineSz = sfCoef.pitch;
    auto alphaSz = sfCoef.alphaSz / sfCoef.alphaByte;

    auto width = pMemSf->GetWidth();
    auto height = pMemSf->GetHeight();

    for (int y = 0; y < height; y++) {
        const auto line = (height - 1 - y);

        auto pMemData = sfCoef.pData + y * lineSz;
        auto pVideo = pData + line * srcLineSz;

        memcpy(pMemData, pVideo, lineSz);
#ifdef VIDEO_ALPHA
        // 32 bit: 4 bytes per pixel: blue, green, red, unused (0)
        const auto pSfAlphaOffset = sfCoef.pAlphaData + line * sfCoef.alphaPitch;
        const auto pBitmapAlphaOffset = pVideo + (PIXEL_BYTE - 1);

        for (int x = 0; x < width; x++) {
            auto pAlphaData = pSfAlphaOffset + x * sfCoef.alphaByte;
            const auto pCurAlpha = pBitmapAlphaOffset + x * PIXEL_BYTE;
            pAlphaData[0] = pCurAlpha[0];
        }
#endif
    }

#ifdef _DEBUG
    //_SavetoClipBoard(pMemSf, false);
#endif // _DEBUG

    ReleaseSfCoef(pMemSf, sfCoef);

#ifndef _MANUAL_PM    
    if (rdPtr->bPm) {
        pMemSf->PremultiplyAlpha();		// only needed in DX11 premultiplied mode
    }
#endif

    return true;
}
