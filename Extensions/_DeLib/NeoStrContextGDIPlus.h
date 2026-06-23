#pragma once

#include "NeoStrContext.h"
#include "NeoStrDefinitionGDIPlus.h"

struct NeoStrContextGDIPlus :public NeoStrContext {
    GdiplusStartupInput gdiplusStartupInput = {};
    ULONG_PTR gdiplusToken = {};
    Gdiplus::Status status = {};

    bool Initialize() override;
    void Shutdown() override;
};