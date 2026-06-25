#pragma once

#include "NeoStrContext.h"
#include "NeoStrDefinitionGDIPlus.h"

#ifdef ENABLE_GDIPLUS

struct NeoStrContextGDIPlus :public NeoStrContext {
    GdiplusStartupInput gdiplusStartupInput = {};
    ULONG_PTR gdiplusToken = {};
    Gdiplus::Status status = {};

    bool Initialize() override;
    void Shutdown() override;
};

#endif // ENABLE_GDIPLUS