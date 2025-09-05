#pragma once

struct tagRDATA;
typedef struct tagRDATA RUNDATA;
typedef	RUNDATA* LPRDATA;

class cSurface;
typedef	cSurface* LPSURFACE;

struct CopyAdapter {
    LPRDATA rdPtr = nullptr;

    CopyAdapter(LPRDATA p) :rdPtr(p) {};
    virtual ~CopyAdapter() = default;

    // return true if need to create new texture
    virtual bool InitTexture(LPSURFACE& pSf,
        const int width, const int height);
    virtual bool CopyTexture(LPSURFACE pDst,
        const unsigned char* pData, const int width, const int height) = 0;
};