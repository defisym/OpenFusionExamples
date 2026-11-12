#include "Common.h"
#include "FrameCaptureDC.h"
#include "FrameCaptureDX11.h"

SIZE FrameCapture::OutputSize(LPRDATA rdPtr, SIZE) const {
    const auto rh = rdPtr->rHo.hoAdRunHeader;
    return {rh->rh3.rh3WindowSx, rh->rh3.rh3WindowSy};
}

void FrameCapture::Report(HRESULT hr, const char* stage) const {
    if (!DiagnosticsEnabled()) return;
    char message[256];
    sprintf_s(message, "NeoPic capture backend=%u %s hr=0x%08lX\n",
        static_cast<unsigned>(backend), stage, static_cast<unsigned long>(hr));
    OutputDebugStringA(message);
}

HRESULT FrameCapture::CaptureAndLoad(LPRDATA rdPtr, SIZE* outputSize) {
    std::unique_ptr<cSurface> surface;
    HRESULT hr = CaptureSurface(rdPtr, surface);
    if (SUCCEEDED(hr) && (!surface || !surface->IsValid())) hr = E_FAIL;
    if (FAILED(hr)) {
        Report(hr, "failed (image unchanged)");
        return hr;
    }
    if (outputSize) *outputSize = {surface->GetWidth(), surface->GetHeight()};
    LoadFromPointer(rdPtr, L"_TempCapture", surface.release(), LoadFromPointerFlags_TakeOwnership);
    Report(S_OK, "ready (synchronous capture completion)");
    return S_OK;
}

std::unique_ptr<FrameCapture> CreateFrameCapture(LPRDATA rdPtr, CaptureBackend backend) {
    switch (backend) {
    case CaptureBackend::Secondary: return std::make_unique<SecondaryFrameCapture>(rdPtr);
    case CaptureBackend::WindowsGraphicsCapture: return std::make_unique<WgcFrameCapture>(rdPtr);
    default: return std::make_unique<DcFrameCapture>(rdPtr);
    }
}
