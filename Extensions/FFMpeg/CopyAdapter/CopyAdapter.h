#pragma once

#pragma warning(disable : 4819)

extern "C" {
#include <libavcodec/avcodec.h>
}

#ifdef _WIN32
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
#else
#include <cstddef>
#include <cstdint>
#include <memory>
struct AndroidCopyTarget {
    uint8_t* pixels = nullptr;
    size_t capacity = 0;
    int width = 0, height = 0, stride = 0;
    bool premultiply = false;
    int program = 0, external = 0, framebuffer = 0, vbo = 0;
    float transform[16] = {};
};
struct CopyAdapter {
    virtual ~CopyAdapter() = default;
    virtual bool CopyTexture(const AndroidCopyTarget&, const unsigned char*) = 0;
};
std::unique_ptr<CopyAdapter> CopyAdapterFactory(AVHWDeviceType type);
#endif
