#pragma once

#include <functional>
#include <string_view>

class NeoStr
{
private:
	HDC hdc;
	COLORREF color;
	HFONT hFont;

	DWORD dwDTFlags;
	
	struct StrSize {
		long width;
		long height;
	};

public:
	NeoStr(DWORD dwAlignFlags, COLORREF color, HFONT hFont) {
		this->hdc = GetDC(NULL);
		SelectObject(this->hdc, hFont);
		
		this->color = color;
		this->hFont = hFont;
		this->dwDTFlags = dwAlignFlags | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL;
	}
	~NeoStr() {
		ReleaseDC(NULL, this->hdc);
	}

	inline StrSize GetStrSize(LPCWSTR pStr) {
		RECT change = { 0,0,65535,1 };

		DrawTextW(
			hdc,
			pStr,
			wcslen(pStr),
			&change
			, this->dwDTFlags | DT_CALCRECT
		);

		return StrSize{ change.right - change.left,change.bottom - change.top };
	}

	inline void Display(LPSURFACE pDst, LPCWSTR pText, LPRECT pRc
		, BlitMode bm = BMODE_TRANSP, BlitOp bo = BOP_COPY, LPARAM boParam = 0, int bAntiA = 0
		, DWORD dwLeftMargin = 0, DWORD dwRightMargin = 0, DWORD dwTabSize = 8) {

		auto height = pDst->DrawText(pText, wcslen(pText), pRc
			, this->dwDTFlags, this->color, this->hFont,
			bm, bo, boParam, bAntiA);

		return;
	}
};

