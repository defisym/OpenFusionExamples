#pragma once

#if NEOPIC_CAPTURE_BACKEND == 0
struct FrameCapture {
	LPSURFACE pCaptureSf = nullptr;
	HDC pCaptureDC = nullptr;

	HWND hFrameWindowHandle = nullptr;
	HDC hdcWindow = nullptr;

	int frameWidth = -1;
	int frameHeight = -1;
#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
    HRESULT lastResult = S_OK;
    void SetBenchmarking(bool) {}
    HRESULT GetLastResult() const { return lastResult; }
#endif

	FrameCapture(LPRDATA rdPtr) {
		this->hFrameWindowHandle = rdPtr->rHo.hoAdRunHeader->rhHEditWin;
	}
	~FrameCapture() {
		ReleaseDC();
		ReleaseSurface();
	}

	inline void ReleaseDC() {
		if (hdcWindow == nullptr) { return; }

		::ReleaseDC(hFrameWindowHandle, hdcWindow);
		hdcWindow = nullptr;
	}


	inline void NewDC() {
		if (hdcWindow == nullptr) {
			hdcWindow = GetDC(hFrameWindowHandle);
		}
	}

	inline void ReleaseSurface() {
		if (pCaptureSf == nullptr) { return; }

		if (pCaptureDC != nullptr) {
			pCaptureSf->ReleaseDC(pCaptureDC);
			pCaptureDC = nullptr;
		}

		delete pCaptureSf;
		pCaptureSf = nullptr;
	}

	inline bool NewSurface(LPRDATA rdPtr) {
		RECT frameRect;
        if (!::GetWindowRect(hFrameWindowHandle, &frameRect)) return false;
		frameWidth = frameRect.right - frameRect.left;
		frameHeight = frameRect.bottom - frameRect.top;

        int outputWidth = frameWidth, outputHeight = frameHeight;
#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
        // Compare the same area at the same logical output size as GPU backends.
        outputWidth = rdPtr->rHo.hoAdRunHeader->rh3.rh3WindowSx;
        outputHeight = rdPtr->rHo.hoAdRunHeader->rh3.rh3WindowSy;
#else
        (void)rdPtr;
#endif
        if (frameWidth <= 0 || frameHeight <= 0 || outputWidth <= 0 || outputHeight <= 0) return false;
		if (pCaptureSf == nullptr
			|| pCaptureSf->GetWidth() != outputWidth
			|| pCaptureSf->GetHeight() != outputHeight) {
			ReleaseSurface();
			
			pCaptureSf = CreateSurface(24, outputWidth, outputHeight);
            if (!pCaptureSf || !pCaptureSf->IsValid()) return false;
			_AddAlpha(pCaptureSf);

			pCaptureDC = pCaptureSf->GetDC();
#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
            const auto app = rdPtr->rHo.hoAdRunHeader->rhApp;
            const bool resample = app->m_pHdr2 && (app->m_pHdr2->dwOptions & AH2OPT_RESAMPLESTRETCH);
            if (pCaptureDC) {
                SetStretchBltMode(pCaptureDC, resample ? HALFTONE : COLORONCOLOR);
                SetBrushOrgEx(pCaptureDC, 0, 0, nullptr);
            }
#endif
		}
        return pCaptureDC != nullptr;
	}

    bool Capture(LPRDATA rdPtr) {
        NewDC();
        bool ok = hdcWindow && NewSurface(rdPtr);
        if (ok) ok = StretchBlt(pCaptureDC, 0, 0, pCaptureSf->GetWidth(), pCaptureSf->GetHeight(),
            hdcWindow, 0, 0, frameWidth, frameHeight, SRCCOPY) != FALSE;
#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
        // GDI batching must not look faster than an already GPU-complete result.
        if (ok) ok = GdiFlush() != FALSE;
        const DWORD error = ok ? ERROR_SUCCESS : GetLastError();
        lastResult = ok ? S_OK : (error ? HRESULT_FROM_WIN32(error) : E_FAIL);
#endif
        return ok;
    }

};

#else
#include "FrameCaptureDX11.h"
#endif
