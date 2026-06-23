#include "NeoStrContextFreeType.h"

bool NeoStrContextFreeType::Initialize() {
    if (bInitialized) { return true; }

    error = FT_Init_FreeType(&library);
    bInitialized = (error == 0);

    return bInitialized;
}

void NeoStrContextFreeType::Shutdown() {
    error = FT_Done_FreeType(library);
}
