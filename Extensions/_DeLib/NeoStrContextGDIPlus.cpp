#include "NeoStrContextGDIPlus.h"

bool NeoStrContextGDIPlus::Initialize() {
    if (gdiInitialized) { return true; }

    status = Gdiplus::GdiplusStartup(&gdiplusToken
        , &gdiplusStartupInput
        , NULL);
    gdiInitialized = status == Gdiplus::Status::Ok;

    return gdiInitialized;
}

void NeoStrContextGDIPlus::Shutdown() {
    Gdiplus::GdiplusShutdown(gdiplusToken);
}
