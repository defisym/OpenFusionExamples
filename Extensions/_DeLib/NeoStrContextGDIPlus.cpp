#include "NeoStrContextGDIPlus.h"

#ifdef ENABLE_GDIPLUS

bool NeoStrContextGDIPlus::Initialize() {
    if (bInitialized) { return true; }

    status = Gdiplus::GdiplusStartup(&gdiplusToken
        , &gdiplusStartupInput
        , NULL);
    bInitialized = (status == Gdiplus::Status::Ok);

    return bInitialized;
}

void NeoStrContextGDIPlus::Shutdown() {
    if (!bInitialized) { return; }

    Gdiplus::GdiplusShutdown(gdiplusToken);
}

#endif // ENABLE_GDIPLUS