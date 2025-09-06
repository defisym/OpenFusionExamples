#pragma once

#pragma warning(disable : 4819)

extern "C" {
#include <libavcodec/avcodec.h>
}

struct tagRDATA;
typedef struct tagRDATA RUNDATA;
typedef	RUNDATA* LPRDATA;

class cSurface;
typedef	cSurface* LPSURFACE;

struct CopyAdapter {
    LPRDATA rdPtr = nullptr;

    CopyAdapter(LPRDATA p) :rdPtr(p) {};
    virtual ~CopyAdapter() = default;

    // return true if new texture created
    virtual bool InitTexture(LPSURFACE& pSf,
        const int width, const int height) = 0;
    static bool TextureValid(LPSURFACE pSf, const int width, const int height);

    // return true if success
    virtual bool CopyTexture(LPSURFACE pDst,
        const unsigned char* pData, const int width, const int height) = 0;
    static bool CopyValid(LPSURFACE pDst, const unsigned char* pData);
};

// only check support, no fallback
bool CopyAdapterSupport(LPRDATA rdPtr, const AVHWDeviceType type);
// always fallback to bitmap
bool UpdateCopyAdapter(CopyAdapter*& pCopyAdapter, 
    LPRDATA rdPtr, const AVHWDeviceType type);