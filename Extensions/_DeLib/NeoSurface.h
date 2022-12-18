#pragma once

//------------
// Wrapper for plain surface functions
//------------

#include "Surface.h"

#include "Fusion.h"
#include "FusionUtilities.h"

constexpr auto FLAG_MIXALPHA = 0x1000;

class NeoSurface :cSurface {
private:
    LPRDATA rdPtr = nullptr;

public:
    //------------
    // Construt & Destructor
    //------------
    NeoSurface(LPRDATA rdPtr) {
        this->rdPtr = rdPtr;
    }
    ~NeoSurface() {}

    //------------
    // File
    //------------
    inline void nSf_SavetoClipBoard(HWND Handle = NULL) {
        ::__SavetoClipBoard(this, Handle, false);
    }

    inline void nSf_SavetoFile(LPCWSTR FilePath, LPCWSTR DefaultFilterName = nullptr) {
        ::__SavetoFile(rdPtr, this, FilePath, DefaultFilterName, false);
    }

    inline void nSf_LoadFromClipBoard(int width, int height, bool NoStretch, bool HighQuality, HWND Handle = NULL) {
        ::_LoadFromClipBoard(this, width, height, NoStretch, HighQuality, Handle);
    }

#ifdef _NO_REF
    inline bool nSf_LoadFromFile(LPCTSTR FilePath, int width, int height, bool NoStretch, bool HighQuality) {
        return ::_LoadFromFile(this, FilePath, rdPtr, width, height, NoStretch, HighQuality);
    }

    inline bool nSf_LoadFromMemFile(LPBYTE pData, DWORD dataSz, int width, int height, bool NoStretch, bool HighQuality) {
        return ::_LoadFromMemFile(this, pData, dataSz, rdPtr, width, height, NoStretch, HighQuality);
    }
#endif

    //------------
    // Info
    //------------
    inline bool nSf_IsHWA() {
        return ::IsHWA(this);
    }

    inline bool nSf_IsTransparent() {
        return ::IsTransparent(this);
    }

    inline SfCoef nSf_GetSfCoef() {
        return ::GetSfCoef(this);
    }

    inline void nSf_ReleaseSfCoef(SfCoef coef) {
        return ::ReleaseSfCoef(this, coef);
    }

    inline void nSf_IteratePixel(std::function<void(int, int, const SfCoef, BYTE*, BYTE*)> process) {
        IteratePixel(this, process);
    }

    // in byte
    inline uint64_t nSf_GetEstimateSize() {
        return static_cast<uint64_t>(this->GetWidth()) * this->GetHeight()
            * (static_cast<uint64_t>(this->GetDepth()) << 3);
    }

    // in MB
    inline uint64_t nSf_GetEstimateSizeMB() {
        return nSf_GetEstimateSize() << 20;
    }

    //------------
    // Create
    //------------
    inline void nSf_CreateHWASurface(int depth, int width, int height, int type) {
        ::CreateHWASurface(rdPtr, this, depth, width, height, type);
    }

    inline void nSf_CreateBitmapSurface(int depth, int width, int height) {
        ::CreateSurface(this, depth, width, height);
    }

    inline void nSf_CreateBlankBitmapSurface(int depth, int width, int height) {
        ::CreateBlankSurface(this);
    }

    //------------
    // Convert
    //------------
    inline void nSf_ConverToBitmap() {
        ::ConvertToBitmap_NRef(rdPtr, this);
    }

    inline void nSf_ConverToHWATexture() {
        ::ConvertToHWATexture_NRef(rdPtr, this);
    }

    //------------
    // Operation
    //------------
    inline void nSf_AddAlpha(BYTE coef = 255) {
        ::_AddAlpha(this, coef);
    }

#ifdef _NO_REF
    inline void nSf_StackBlur(int radius, float scale, int divide) {
        ::StackBlur(this, radius, scale, divide);
    }
#endif

    inline bool nSf_MixAlpha(LPSURFACE pDst, int destX, int destY) {
        return ::MixAlpha(this, pDst, destX, destY);
    }

    //------------
    // Blit (With Mix Alpha)
    //------------
private:
    inline BOOL MixAlphaHelper(cSurface FAR& dest, int destX, int destY, DWORD dwBlitFlags
        , std::function<BOOL(DWORD)> callback) const {
        auto bMixAlpha = dwBlitFlags & FLAG_MIXALPHA;
        dwBlitFlags = dwBlitFlags & ~FLAG_MIXALPHA;

        auto result = callback(dwBlitFlags);

        if (!result) {
            return result;
        }

        if (bMixAlpha) {
            result = ::MixAlpha((LPSURFACE)this, &dest, destX, destY);
        }

        return result;
    }
public:
    // Blit surface to surface
    inline BOOL Blit(cSurface FAR& dest, int destX, int destY,
        BlitMode bm = BMODE_OPAQUE, BlitOp bo = BOP_COPY, LPARAM param = 0,
        DWORD dwBlitFlags = 0) const {
        return MixAlphaHelper(dest, destX, destY, dwBlitFlags
            , [&](DWORD dwBlitFlags) {
                return this->cSurface::Blit(dest, destX, destY
                    , bm, bo, param
                    , dwBlitFlags);
            });
    }

    // Blit rectangle to surface
    inline BOOL Blit(cSurface FAR& dest, int destX, int destY,
        int srcX, int srcY, int srcWidth, int srcHeight,
        BlitMode bm /*= BMODE_OPAQUE*/, BlitOp bo = BOP_COPY, LPARAM param = 0,
        DWORD dwBlitFlags = 0) const {
        return MixAlphaHelper(dest, destX, destY, dwBlitFlags
            , [&](DWORD dwBlitFlags) {
                return this->cSurface::Blit(dest, destX, destY
                    , srcX, srcY,srcWidth,srcHeight
                    , bm, bo, param
                    , dwBlitFlags);
            });
    }
};