#pragma once

#include <vector>
#include <functional>
#include <string_view>

#ifdef _DEBUG		
#include <assert.h>
#endif

inline RECT operator+(RECT rA, RECT rB) {
	return RECT{ rA.left + rB.left
				,rA.top + rB.top
				,rA.right + rB.right
				,rA.bottom + rB.bottom };
}

class NeoStr
{
private:
	HDC hdc;
	COLORREF color;
	HFONT hFont;

	DWORD dwDTFlags;

	bool bOutLine = false;
	bool bShadow = false;

	COLORREF dwOutLineColor = RGB(255, 255, 0);

	struct StrPos {
		size_t start;
		size_t end;
		size_t length;
		long width;
		long height;
		long x;
		long y;
	};
	
	std::vector<StrPos> strPos;
	
	struct StrSize {
		long width;
		long height;
	};

	inline int GetStartPosX(long totalWidth, long rcWidth) const {
		//DT_LEFT | DT_CENTER | DT_RIGHT
		if (this->dwDTFlags & DT_LEFT) {
			return 0;
		}
		if (this->dwDTFlags & DT_CENTER) {
			return ((rcWidth - totalWidth) >> 1);
		}
		if (this->dwDTFlags & DT_RIGHT) {
			return rcWidth - totalWidth;
		}
		
		return 0;
	}

	inline int GetStartPosY(long totalHeight, long rcHeight) const {
		//	DT_TOP | DT_VCENTER | DT_BOTTOM
		if (this->dwDTFlags & DT_TOP) {
			return 0;
		}
		if (this->dwDTFlags & DT_VCENTER) {
			return ((rcHeight - totalHeight) >> 1);
		}
		if (this->dwDTFlags & DT_BOTTOM) {
			return rcHeight - totalHeight;
		}

		return 0;
	}

public:
	struct CharPos {
		long x;
		long y;
	};

	NeoStr(DWORD dwAlignFlags, COLORREF color, HFONT hFont) {
		this->hdc = GetDC(NULL);
		SelectObject(this->hdc, hFont);
		
		this->color = color;
		this->hFont = hFont;
		this->dwDTFlags = dwAlignFlags | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL;
		
		this->strPos.reserve(20);
	}
	~NeoStr() {
		ReleaseDC(NULL, this->hdc);
	}

	inline void EnableOutLine(DWORD color) {
		this->bOutLine = true;
		this->dwOutLineColor = color;
	}

	inline void DisableOutLine() {
		this->bOutLine = false;
	}

	inline StrSize GetCharSize(wchar_t wChar) {
		SIZE sz;
		GetTextExtentPoint32(hdc, &wChar, 1, &sz);

		return StrSize{ sz.cx,sz.cy };
		// return GetStrSize(&wChar, 1);
	}

	inline StrSize GetStrSize(LPCWSTR pStr, size_t pStrLen = -1) {
		RECT change = { 0,0,65535,1 };

		DrawTextW(
			hdc,
			pStr,
			pStrLen == -1 ? wcslen(pStr): pStrLen,
			&change
			, this->dwDTFlags | DT_CALCRECT
		);

		return StrSize{ change.right - change.left,change.bottom - change.top };
	}

	inline void Display(LPSURFACE pDst, LPCWSTR pText, LPRECT pRc
		, BlitMode bm = BMODE_TRANSP, BlitOp bo = BOP_COPY, LPARAM boParam = 0, int bAntiA = 0
		, DWORD dwLeftMargin = 0, DWORD dwRightMargin = 0, DWORD dwTabSize = 8) {

		auto height = pDst->DrawText(pText, wcslen(pText), pRc
			, this->dwDTFlags, this->color, this->hFont
			, bm, bo, boParam, bAntiA);

		return;
	}

	inline CharPos DisplayPerChar(LPSURFACE pDst, LPCWSTR pText, LPRECT pRc
		, size_t nRowSpace = 0, size_t nColSpace = 0
		, BlitMode bm = BMODE_TRANSP, BlitOp bo = BOP_COPY, LPARAM boParam = 0, int bAntiA = 0
		, DWORD dwLeftMargin = 0, DWORD dwRightMargin = 0, DWORD dwTabSize = 8) {

		this->strPos.clear();
		
		size_t pTextLen = wcslen(pText);
		auto pStrSizeArr = new StrSize[pTextLen + 1];
		memset(pStrSizeArr, 0, sizeof(StrSize) * (pTextLen+1));
		
		long rcWidth = pRc->right - pRc->left;
		long rcHeight = pRc->bottom - pRc->top;

		DWORD dwETOFlags = TA_TOP | TA_LEFT;

		RECT clipRc = { 0,0,65535,65535 };

		size_t pCharStart = 0;
		
		long totalWidth = 0;
		long totalHeight = 0;

		for (size_t pChar = 0; pChar < pTextLen; ) {
			bool newLine = false;
			bool skipLine = false;
			
			long curWidth = 0;
			long curHeight = 0;

			pCharStart = pChar;

			while (curWidth < rcWidth
				&& pChar <= pTextLen) {
				auto pCurChar = pText + pChar;
				
				auto curChar = pCurChar[0];
				auto nextChar = pCurChar[1];

				if (curChar == L'\r' && nextChar == L'\n') {
					newLine = true;
					skipLine = (curWidth == 0);
					pChar+=2;
					
					break;
				}

				pStrSizeArr[pChar] = GetCharSize(curChar);
				auto charSz = &pStrSizeArr[pChar];

				totalWidth = curWidth;
				
				curWidth += (charSz->width + nColSpace);
				curHeight = max(curHeight, charSz->height);
				
				if (curWidth > rcWidth) {					
					continue;
				}

				pChar++;
			}

			if (!skipLine) {
				auto end = min(pChar, pTextLen) - 2 * newLine;

				this->strPos.emplace_back(StrPos{
					pCharStart,
					end,
					end - pCharStart,
					totalWidth,
					curHeight,
					0,
					totalHeight,
					});
				
				totalHeight += (curHeight + nRowSpace);
			}
		}

		const auto& lastStrPos = strPos.back();
		int y = GetStartPosY(totalHeight - nRowSpace, rcHeight);
		
		auto lastCharPos = CharPos{ 0,0 };

#define MEMDC

#ifdef MEMDC
		auto dwInvertColor = WHITE - this->color;
		auto pMemSf = CreateSurface(24, rcWidth, totalHeight - nRowSpace);
		pMemSf->Fill(BLACK);
		
		auto hMemDc = pMemSf->GetDC();
		SelectObject(hMemDc, this->hFont);
		SetTextColor(hMemDc, this->color);
		//SetTextColor(hMemDc, WHITE);
		SetBkMode(hMemDc, TRANSPARENT);

		BeginPath(hMemDc);

		for (auto& curStrPos : this->strPos) {

#ifdef _DEBUG
			std::wstring str(pText + curStrPos.start, curStrPos.length);
#endif // _DEBUG

			int x = GetStartPosX(curStrPos.width - nColSpace, rcWidth);
			StrSize* charSz = nullptr;

			for (size_t curChar = 0; curChar < curStrPos.length; curChar++) {
				auto offset= curStrPos.start + curChar;
				auto pCurChar = pText + offset;
				charSz = &pStrSizeArr[offset];
				
				TextOut(hMemDc, x, y + curStrPos.y, pCurChar, 1);

				x += (charSz->width + nColSpace);
			}

			// lastCharPos = CharPos{ x,y + curStrPos.y };
			lastCharPos = CharPos{ x + (charSz->width >> 1)
				,y + curStrPos.y + (charSz->height >> 1) };
		}

		EndPath(hMemDc);

		auto hStrokePen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
		//auto hStrokePen = ExtCreatePen(PS_GEOMETRIC| PS_SOLID| PS_JOIN_ROUND, 1, RGB(0, 255, 0));
		SelectObject(hMemDc, hStrokePen);
		//StrokePath(hMemDc);
		
		auto hFillBrush = CreateSolidBrush(this->color);
		SelectObject(hMemDc, hFillBrush);
		//FillPath(hMemDc);
		
		StrokeAndFillPath(hMemDc);

		delete[] pStrSizeArr;

#ifdef _DEBUG
		// _SavetoClipBoard(pMemSf, false);
#endif // _DEBUG

		pMemSf->ReleaseDC(hMemDc);

		//IteratePixel(pMemSf, [](int x, int y, int offset, BYTE* src, BYTE* temp) {
		//	auto A= src[3];
		//	auto R = src[2];
		//	auto G = src[1];
		//	auto B = src[0];

		//	printf("%d %d %d %d\n", A, R, G, B);
		//	});
		
		if (pDst != nullptr) {
			pMemSf->Blit(*pDst, pRc->left, pRc->top, bm, bo, boParam, bAntiA);
		}

		delete pMemSf;
		
		return lastCharPos;

#else
		//LOGFONT outLineLogFont;
		//GetObject(this->hFont, sizeof(LOGFONT), &outLineLogFont);
		//outLineLogFont.lfWeight += 200;
		//
		//HFONT outLineHFont = CreateFontIndirect(&outLineLogFont);

		for (auto& curStrPos : this->strPos) {
			
#ifdef _DEBUG
			std::wstring str(pText + curStrPos.start, curStrPos.length);
#endif // _DEBUG
			
			int x = GetStartPosX(curStrPos.width - nColSpace, rcWidth);
			StrSize charSz = { 0,0 };

			for (size_t curChar = 0; curChar < curStrPos.length; curChar++) {
				auto pCurChar = pText + curStrPos.start + curChar;			
				charSz = GetCharSize(*pCurChar);

				RECT curRc = { pRc->left + x
					, pRc->top + y + curStrPos.y
					, pRc->left + x + charSz.width
					, pRc->top + y + curStrPos.y + curStrPos.height };
				
				if (pDst != nullptr) {
					//pDst->DrawText(pCurChar, 1, &curRc
					//	, 0, this->dwOutLineColor, outLineHFont
					//	, bm, bo, boParam, bAntiA);
					
					//auto pDstDC = pDst->GetDC();

					auto outLineRect = curRc + RECT{ -1, -1, -1, -1 };

					pDst->DrawText(pCurChar, 1, &outLineRect
						, 0, this->dwOutLineColor, this->hFont
						, bm, bo, boParam, bAntiA);

					outLineRect = curRc + RECT{ 1, 1, 1, 1 };

					pDst->DrawText(pCurChar, 1, &outLineRect
						, 0, this->dwOutLineColor, this->hFont
						, bm, bo, boParam, bAntiA);

					outLineRect = curRc + RECT{ -1, 1, -1, 1 };

					pDst->DrawText(pCurChar, 1, &outLineRect
						, 0, this->dwOutLineColor, this->hFont
						, bm, bo, boParam, bAntiA);

					outLineRect = curRc + RECT{ 1, -1, 1, -1 };

					pDst->DrawText(pCurChar, 1, &outLineRect
						, 0, this->dwOutLineColor, this->hFont
						, bm, bo, boParam, bAntiA);

					auto height = pDst->DrawText(pCurChar, 1, &curRc
						, 0, this->color, this->hFont
						//, this->dwDTFlags, this->color, this->hFont
						, bm, bo, boParam, bAntiA);
				}
				
				x += (charSz.width + nColSpace);
			}
			
			// lastCharPos = CharPos{ x,y + curStrPos.y };
			lastCharPos = CharPos{ x + (charSz.width >> 1)
				,y + curStrPos.y + (charSz.height >> 1) };
		}

		return lastCharPos;
#endif // MEMDC
		
	}
};

