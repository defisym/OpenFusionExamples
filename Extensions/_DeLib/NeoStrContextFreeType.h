#pragma once

#include "NeoStrContext.h"
#include "NeoStrDefinitionFreeType.h"

struct NeoStrContextFreeType :public NeoStrContext {
    FT_Library library = {};
    FT_Error error = {};

    bool Initialize() override;
    void Shutdown() override;
};