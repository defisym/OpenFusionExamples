#pragma once

struct FrameCapture {
	LPSURFACE pCaptureSf = nullptr;
	HDC pCaptureDC = nullptr;

	HWND hFrameWindowHandle = nullptr;
	HDC hdcWindow = nullptr;

	int frameWidth = -1;
	int frameHeight = -1;

	FrameCapture(HWND hFrameWindowHandle) {
		this->hFrameWindowHandle = hFrameWindowHandle;
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

	inline void NewSurface() {
		RECT frameRect;
		::GetWindowRect(hFrameWindowHandle, &frameRect);
		frameWidth = frameRect.right - frameRect.left;
		frameHeight = frameRect.bottom - frameRect.top;

		if (pCaptureSf == nullptr
			|| pCaptureSf->GetWidth() != frameWidth
			|| pCaptureSf->GetHeight() != frameHeight) {
			ReleaseSurface();
			
			pCaptureSf = CreateSurface(24, frameWidth, frameHeight);
			_AddAlpha(pCaptureSf);

			pCaptureDC = pCaptureSf->GetDC();
		}
	}

	inline void Capture() {
		NewDC();
		NewSurface();
		StretchBlt(pCaptureDC, 0, 0, frameWidth, frameHeight
			, hdcWindow, 0, 0, frameWidth, frameHeight
			, SRCCOPY);
		//_AddAlpha(pCaptureSf);

#ifdef _DEBUG
		//__SavetoClipBoard(pCaptureSf);
#endif // _DEBUG
	}
};
