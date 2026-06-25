#include "NeoStrContextFreeType.h"

#ifdef ENABLE_FREETYPE

bool NeoStrContextFreeType::Initialize() {
    if (bInitialized) { return true; }

    error = FT_Init_FreeType(&library);
    bInitialized = (error == FT_Err_Ok);

    return bInitialized;
}

void NeoStrContextFreeType::Shutdown() {
    error = FT_Done_FreeType(library);
}

#endif // ENABLE_FREETYPE