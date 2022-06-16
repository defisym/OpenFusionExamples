#pragma once

#include <vector>
#include <functional>
#include <string_view>

#ifdef _DEBUG		
#include <assert.h>
#endif

#define _GDIPLUS

#ifdef _GDIPLUS	
// compatible with MMF
#undef Font
#undef fpFont

#include <gdiplus.h>
#pragma comment(lib,"Gdiplus")

using Gdiplus::GdiplusStartupInput;
using Gdiplus::Graphics;
using Gdiplus::GraphicsPath;
using Gdiplus::StringFormat;
using Gdiplus::StringAlignment;
using Gdiplus::FillMode;
using Gdiplus::Color;
using Gdiplus::Font;
using Gdiplus::Pen;
using Gdiplus::LineJoin;
using Gdiplus::SolidBrush;
using Gdiplus::Rect;
using Gdiplus::Bitmap;
using Gdiplus::BitmapData;
using Gdiplus::ImageLockMode;
using Gdiplus::ImageCodecInfo;
using Gdiplus::GetImageEncodersSize;
#endif

inline RECT operator+(RECT rA, RECT rB) {
	return RECT { rA.left + rB.left
				,rA.top + rB.top
				,rA.right + rB.right
				,rA.bottom + rB.bottom };
}

class NeoStr {
private:
	HDC hdc;
	COLORREF dwTextColor;
	HFONT hFont;

	DWORD dwDTFlags;

	bool bClip = true;

	bool bOutLine = false;
	BYTE nOutLinePixel = 1;
	COLORREF dwOutLineColor = RGB(255, 255, 0);

	bool bShadow = false;
	BYTE nShadowOffsetX = 0;
	BYTE nShadowOffsetY = 0;

#ifdef _GDIPLUS		
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
#endif

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

	inline COLORREF BlackEscape(COLORREF input) {
		return input == BLACK ? RGB(8, 0, 0) : input;
	}

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

#ifdef _GDIPLUS
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes
		ImageCodecInfo* pImageCodecInfo = NULL;
		GetImageEncodersSize(&num, &size);
		if (size == 0)
			return -1;  // Failure
		pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == NULL)
			return -1;  // Failure
		GetImageEncoders(num, size, pImageCodecInfo);
		for (UINT j = 0; j < num; ++j) {
			if (wcscmp(pImageCodecInfo [j].MimeType, format) == 0) {
				*pClsid = pImageCodecInfo [j].Clsid;
				free(pImageCodecInfo);
				return j;  // Success
			}
		}
		free(pImageCodecInfo);
		return -1;  // Failure
	}

#endif // _GDIPLUS

public:
	struct CharPos {
		long x;
		long y;
	};

	NeoStr(DWORD dwAlignFlags, COLORREF color, HFONT hFont) {
		this->hdc = GetDC(NULL);
		SelectObject(this->hdc, hFont);

		this->dwTextColor = BlackEscape(color);
		this->hFont = hFont;
		this->dwDTFlags = dwAlignFlags | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL;

		this->strPos.reserve(20);

#ifdef _GDIPLUS		
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
#endif
	}
	~NeoStr() {
		ReleaseDC(NULL, this->hdc);

#ifdef _GDIPLUS	
		Gdiplus::GdiplusShutdown(gdiplusToken);
#endif	
	}

	inline void SetClip(bool clip) {
		this->bClip = clip;
	}

	inline void SetOutLine(BYTE outLinePixel, DWORD color) {
		this->bOutLine = outLinePixel;
		this->nOutLinePixel = outLinePixel;
		this->dwOutLineColor = BlackEscape(color);;
	}

	inline StrSize GetCharSize(wchar_t wChar) {
		SIZE sz;
		GetTextExtentPoint32(hdc, &wChar, 1, &sz);

		return *(StrSize*)&sz;
	}

	inline StrSize GetStrSize(LPCWSTR pStr, size_t pStrLen = -1) {
		RECT change = { 0,0,65535,1 };

		DrawTextW(
			hdc,
			pStr,
			pStrLen == -1 ? wcslen(pStr) : pStrLen,
			&change
			, this->dwDTFlags | DT_CALCRECT
		);

		return StrSize { change.right - change.left,change.bottom - change.top };
	}

	inline void Display(LPSURFACE pDst, LPCWSTR pText, LPRECT pRc
		, BlitMode bm = BMODE_TRANSP, BlitOp bo = BOP_COPY, LPARAM boParam = 0, int bAntiA = 0
		, DWORD dwLeftMargin = 0, DWORD dwRightMargin = 0, DWORD dwTabSize = 8) {

		auto height = pDst->DrawText(pText, wcslen(pText), pRc
			, this->dwDTFlags, this->dwTextColor, this->hFont
			, bm, bo, boParam, bAntiA);

		return;
	}

	inline CharPos DisplayPerChar(LPSURFACE pDst, LPCWSTR pText, LPRECT pRc
		, size_t nRowSpace = 0, size_t nColSpace = 0
		, BlitMode bm = BMODE_TRANSP, BlitOp bo = BOP_COPY, LPARAM boParam = 0, int bAntiA = 0
		, DWORD dwLeftMargin = 0, DWORD dwRightMargin = 0, DWORD dwTabSize = 8) {

		this->strPos.clear();

		size_t pTextLen = wcslen(pText);

		auto pStrSizeArr = new StrSize [pTextLen + 1];
		memset(pStrSizeArr, 0, sizeof(StrSize) * (pTextLen + 1));

		long rcWidth = pRc->right - pRc->left;
		long rcHeight = pRc->bottom - pRc->top;

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

				auto curChar = pCurChar [0];
				auto nextChar = pCurChar [1];

				if (curChar == L'\r' && nextChar == L'\n') {
					newLine = true;
					skipLine = (curWidth == 0);
					pChar += 2;

					break;
				}

				pStrSizeArr [pChar] = GetCharSize(curChar);
				auto charSz = &pStrSizeArr [pChar];

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

				this->strPos.emplace_back(StrPos {
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
		const auto lastCharSize = &pStrSizeArr [lastStrPos.start + lastStrPos.length - 1];

		int y = GetStartPosY(totalHeight - nRowSpace, rcHeight);

		auto lastCharPos = CharPos {
			GetStartPosX(lastStrPos.width - nColSpace, rcWidth) + lastStrPos.width + (lastCharSize->width >> 1)
			,y + lastStrPos.y + (lastCharSize->height >> 1) };

		if (pDst != nullptr) {
			LOGFONT drawLogFont;
			GetObject(this->hFont, sizeof(LOGFONT), &drawLogFont);
			//drawLogFont.lfQuality = ANTIALIASED_QUALITY;
			//drawLogFont.lfQuality = NONANTIALIASED_QUALITY;
			//this->hFont = CreateFontIndirect(&drawLogFont);

			char scale = 1;
			auto width = abs(rcWidth * scale);
			auto height = abs(long((totalHeight - nRowSpace) * scale));

			//#define _DRAWTODC

#ifdef _DRAWTODC
			auto pMemSf = CreateSurface(32, rcWidth * scale, (totalHeight - nRowSpace) * scale);
#else
			auto pMemSf = CreateSurface(32, width, height * scale);
#endif

			pMemSf->CreateAlpha();
			//pMemSf->Fill(BLACK);

#ifdef _DRAWTODC
			auto hMemDc = pMemSf->GetDC();
			Graphics g(hMemDc);
#else
#ifdef _GDIPLUS		
			Bitmap bitmap(width, height, PixelFormat32bppARGB);
			Graphics g(&bitmap);
#endif
#endif

#ifdef _GDIPLUS			
			g.Clear(Color(0, 0, 0, 0));

			//Color fontColor(255, 50, 150, 250);
			Color fontColor(255, GetRValue(this->dwTextColor), GetGValue(this->dwTextColor), GetBValue(this->dwTextColor));

			SolidBrush solidBrush(fontColor);
			Font font(GetDC(NULL), this->hFont);

			g.SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias);
			g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
			g.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);

#else	
			SelectObject(hMemDc, this->hFont);
			SetTextColor(hMemDc, this->dwTextColor);
			SetBkMode(hMemDc, TRANSPARENT);
#endif

#define _PATH

#ifdef  _PATH
#ifdef _GDIPLUS	
			GraphicsPath txtPath(FillMode::FillModeWinding);

			Gdiplus::FontFamily fontFamily;
			font.GetFamily(&fontFamily);

			Gdiplus::FontStyle fontstyle = (Gdiplus::FontStyle)font.GetStyle();

			StringFormat  cStringFormat;
			cStringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
#else
			BeginPath(hMemDc);
#endif
#endif

			RECT displayRc = { 0,0,(LONG)width, (LONG)height, };

			auto clip = [this, pRc, displayRc] (int startX, int startY, const StrSize* charSz)->bool {
				if (this->bClip == false) {
					return false;
				}

				RECT charRc = { pRc->left + startX
					,pRc->top + startY
					,pRc->left + startX + charSz->width
					,pRc->top + startY + charSz->height };

				return !(charRc.left < displayRc.right
					&& charRc.right > displayRc.left
					&& charRc.top < displayRc.bottom
					&& charRc.bottom > displayRc.top);
			};

			for (auto& curStrPos : this->strPos) {

#ifdef _DEBUG
				std::wstring str(pText + curStrPos.start, curStrPos.length);
#endif // _DEBUG

				StrSize* charSz = nullptr;
				int x = GetStartPosX(curStrPos.width - nColSpace, rcWidth);

				for (size_t curChar = 0; curChar < curStrPos.length; curChar++) {
					auto offset = curStrPos.start + curChar;
					auto pCurChar = pText + offset;
					charSz = &pStrSizeArr [offset];

					if (!clip(x, (y + curStrPos.y), charSz)) {
#ifdef _GDIPLUS	
#ifdef  _PATH
						txtPath.AddString(pCurChar, 1,
							&fontFamily, fontstyle, font.GetSize()
							, Gdiplus::PointF((float)x, (float)((y + curStrPos.y) * scale)), &cStringFormat);
#else
						g.DrawString(pCurChar, 1, &font, Gdiplus::PointF((float)x, (float)((y + curStrPos.y) * scale)), &solidBrush);
#endif
#else
						TextOut(hMemDc, x * scale, (y + curStrPos.y) * scale, pCurChar, 1);
#endif
					}

					x += (charSz->width + nColSpace);
				}
			}

			delete[] pStrSizeArr;

#ifdef  _PATH
#ifdef _GDIPLUS	
			if (this->bOutLine) {
				Pen borderPen(Color(255
					, GetRValue(this->dwOutLineColor), GetRValue(this->dwOutLineColor), GetRValue(this->dwOutLineColor))
					, this->nOutLinePixel);
				borderPen.SetLineJoin(LineJoin::LineJoinRound);

				g.DrawPath(&borderPen, &txtPath);
			}

			g.FillPath(&solidBrush, &txtPath);
#else
			EndPath(hMemDc);

			auto hFillBrush = CreateSolidBrush(this->dwTextColor);
			SelectObject(hMemDc, hFillBrush);

			if (this->bOutLine) {
				auto hStrokePen = CreatePen(PS_SOLID, this->nOutLinePixel, this->dwOutLineColor);
				//auto hStrokePen = ExtCreatePen(PS_GEOMETRIC| PS_SOLID| PS_JOIN_ROUND, , this->nOutLinePixel, this->dwOutLineColor);
				SelectObject(hMemDc, hStrokePen);
				//StrokePath(hMemDc);

				StrokeAndFillPath(hMemDc);

				DeleteObject(hStrokePen);
			}
			else {
				FillPath(hMemDc);
			}

			DeleteObject(hFillBrush);
#endif
#endif

#ifdef _DEBUG
			//_SavetoClipBoard(pMemSf, false);

			//CLSID pngClsid;
			//GetEncoderClsid(L"image/png", &pngClsid);
			//bitmap.Save(L"F:\\Mosaic2.png", &pngClsid, NULL);

#endif // _DEBUG

#ifdef _DRAWTODC
			pMemSf->ReleaseDC(hMemDc);
#else
#ifdef _GDIPLUS		
			BitmapData bitmapData;
			auto bitmapRect = Rect(0, 0, width, height);
			bitmap.LockBits(&bitmapRect, ImageLockMode::ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);
			unsigned int* pRawBitmap = (unsigned int*)bitmapData.Scan0;   // for easy access and indexing
#endif
#endif		
			auto sfCoef = GetSfCoef(pMemSf);

			auto lineSz = sfCoef.pitch;

			for (int y = 0; y < height; y++) {
				auto pData = sfCoef.pData + y * sfCoef.pitch;
				auto pBmp = pRawBitmap + (height - 1 - y) * bitmapData.Stride / 4;

				memcpy(pData, pBmp, lineSz);

				for (int x = 0; x < width; x++) {
					auto pAlphaData = sfCoef.pAlphaData + (height - 1 - y) * sfCoef.alphaPitch + x * sfCoef.alphaByte;
					auto curAlpha = pRawBitmap + (height - 1 - y) * bitmapData.Stride / 4 + x;
					pAlphaData[0] = (*curAlpha & 0xff000000) >> 24;
				}
			}
			
			ReleaseSfCoef(pMemSf, sfCoef);

//#define _CONSOLE
//			IteratePixel(pMemSf, [&] (int x, int y, const SfCoef sfCoef, BYTE* pData, BYTE* pAlphaData) {
//#ifdef _DRAWTODC
//				auto alphaPixel = sfCoef.pData + (height - 1 - y) * sfCoef.pitch + x * sfCoef.byte;
//				pData [0] = GetRValue(this->dwTextColor);
//				pData [1] = GetGValue(this->dwTextColor);
//				pData [2] = GetBValue(this->dwTextColor);
//				pAlphaData [0] = alphaPixel [3];
//#else
//				//unsigned int curColor = pRawBitmap [(height - 1 - y) * bitmapData.Stride / 4 + x];
//				//unsigned int curAlpha = pRawBitmap [y * bitmapData.Stride / 4 + x];
//
//				////int b = curColor & 0xff;
//				////int g = (curColor & 0xff00) >> 8;
//				////int r = (curColor & 0xff0000) >> 16;
//				////int a = (curColor & 0xff000000) >> 24;
//
//				//pData [0] = curColor & 0xff;
//				//pData [1] = (curColor & 0xff00) >> 8;
//				//pData [2] = (curColor & 0xff0000) >> 16;
//
//				//pAlphaData [0] = (curAlpha & 0xff000000) >> 24;
//#endif
//
//#ifdef _CONSOLE
//				printf("%d %d %d %d\n", A, R, G, B);
//#endif // _CONSOLE
//				});

			pMemSf->Blit(*pDst, pRc->left, pRc->top, bm, bo, boParam, bAntiA);

			delete pMemSf;
		}

		return lastCharPos;
	}
};

#ifdef _GDIPLUS	
// compatible with MMF
#define Font FontW
#define fpFont fpFontW
#endif

