#pragma once

#include "NeoStrContext.h"
#include "NeoStrDefinitionFreeType.h"

#ifdef ENABLE_FREETYPE

struct NeoStrContextFreeType :public NeoStrContext {
    FT_Library library = {};
    FT_Error error = {};

    bool Initialize() override;
    void Shutdown() override;
};

#endif // ENABLE_FREETYPE