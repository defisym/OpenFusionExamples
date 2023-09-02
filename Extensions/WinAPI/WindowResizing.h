#pragma once

#include "WindowsMessageBase.h"

struct WindowResizing:WindowsMessageBase {
	bool bWindowResizing = false;
	bool bMainlyChangedX = false;

	POINT CurrentFrameSize = {};

	WindowResizing() = default;
	~WindowResizing() = default;

	static inline POINT GetSize(HWND hwnd) {
		RECT CurrentRect = InitRect();
		::GetWindowRect(hwnd, &CurrentRect);

		return { CurrentRect.right - CurrentRect.left,
			CurrentRect.bottom - CurrentRect.top };
	}

	inline void EnterResizing(HWND hwnd) {
		if (bWindowResizing) { return; }

#ifdef _DEBUG
		OutputDebugStringA("Resizing\r\n");
#endif

		bWindowResizing = true;
		bTriggerCallback = false;

		CurrentFrameSize = GetSize(hwnd);
	}

	inline void ExitResizing(HWND hwnd) {
		if (!bWindowResizing) { return; }

#ifdef _DEBUG
		OutputDebugStringA("Resize Complete\r\n");
#endif

		bWindowResizing = false;
		bTriggerCallback = true;

		const auto CurrentSize = GetSize(hwnd);

		if (CurrentSize == CurrentFrameSize) {
			bTriggerCallback = false;
#ifdef _DEBUG
			OutputDebugStringA("Not Changed\r\n");
#endif

			return;
		}

		const auto offsetX = static_cast<double>(std::abs(CurrentSize.x - CurrentFrameSize.x))/ CurrentFrameSize.x;
		const auto offsetY = static_cast<double>(std::abs(CurrentSize.y - CurrentFrameSize.y))/ CurrentFrameSize.x;

		bMainlyChangedX = offsetX > offsetY;
	}
};
