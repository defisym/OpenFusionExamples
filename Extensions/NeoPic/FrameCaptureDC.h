#pragma once
#include "Common.h"

class DcFrameCapture : public FrameCapture {
    std::unique_ptr<cSurface> bitmap;
    HDC bitmapDC = nullptr;
    HWND window = nullptr;
    HDC windowDC = nullptr;

    void ReleaseBitmap() {
        if (bitmapDC) bitmap->ReleaseDC(bitmapDC);
        bitmapDC = nullptr;
        bitmap.reset();
    }
protected:
    SIZE OutputSize(LPRDATA, SIZE sourceSize) const override { return sourceSize; }

    HRESULT CopyBitmap(LPRDATA rdPtr) {
        RECT rect{};
        if (!GetWindowRect(window, &rect)) return LastWin32Error();
        const SIZE source{rect.right - rect.left, rect.bottom - rect.top};
        const auto output = OutputSize(rdPtr, source);
        if (source.cx <= 0 || source.cy <= 0 || output.cx <= 0 || output.cy <= 0) return E_INVALIDARG;
        if (!windowDC) windowDC = GetDC(window);
        if (!windowDC) return LastWin32Error();
        if (!bitmap || bitmap->GetWidth() != output.cx || bitmap->GetHeight() != output.cy || !bitmapDC) {
            ReleaseBitmap();
            bitmap.reset(CreateSurface(24, output.cx, output.cy));
            if (!bitmap || !bitmap->IsValid()) return E_FAIL;
            _AddAlpha(bitmap.get());
            bitmapDC = bitmap->GetDC();
            if (!bitmapDC) return E_FAIL;
            // At native size this is a 1:1 copy. A custom output size may scale.
            const auto app = rdPtr->rHo.hoAdRunHeader->rhApp;
            const bool resample = app->m_pHdr2 && (app->m_pHdr2->dwOptions & AH2OPT_RESAMPLESTRETCH);
            SetStretchBltMode(bitmapDC, resample ? HALFTONE : COLORONCOLOR);
            SetBrushOrgEx(bitmapDC, 0, 0, nullptr);
        }
        return StretchBlt(bitmapDC, 0, 0, output.cx, output.cy,
            windowDC, 0, 0, source.cx, source.cy, SRCCOPY) ? S_OK : LastWin32Error();
    }

    HRESULT CloneBitmap(LPRDATA rdPtr, std::unique_ptr<cSurface>& output) {
        output.reset(CreateCloneSurface(rdPtr, bitmap.get()));
        return output && output->IsValid() ? S_OK : E_FAIL;
    }

    HRESULT CaptureSurface(LPRDATA rdPtr, std::unique_ptr<cSurface>& output) override {
        const HRESULT hr = CopyBitmap(rdPtr);
        return FAILED(hr) ? hr : CloneBitmap(rdPtr, output);
    }

    static HRESULT LastWin32Error() {
        const DWORD error = GetLastError();
        return error ? HRESULT_FROM_WIN32(error) : E_FAIL;
    }
public:
    explicit DcFrameCapture(LPRDATA rdPtr) : FrameCapture(CaptureBackend::Dc),
        window(rdPtr->rHo.hoAdRunHeader->rhHEditWin) {}
    ~DcFrameCapture() override {
        ReleaseBitmap();
        if (windowDC) ::ReleaseDC(window, windowDC);
    }
};
