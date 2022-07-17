#pragma once

#include <map>
#include <vector>
#include <functional>
#include <string_view>

//#include <FusionUtilities.h>

#ifdef _DEBUG		
#include <assert.h>
#endif

#define _GDIPLUS	// display with GDI plus

//#define _PATH		// draw outline, slow in GDI
//#define _DRAWTODC	// draw to DC, display incorrectly sometimes

#define _USE_HWA	// use hardware acceleration

#ifdef _GDIPLUS	
// compatible with MMF
#undef Font
#undef fpFont

#include <gdiplus.h>
#pragma comment(lib,"Gdiplus")

#include <gdiplusheaders.h>

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
using Gdiplus::RectF;
using Gdiplus::PointF;
using Gdiplus::Region;
using Gdiplus::Bitmap;
using Gdiplus::BitmapData;
using Gdiplus::ImageLockMode;
using Gdiplus::ImageCodecInfo;
using Gdiplus::PrivateFontCollection;
using Gdiplus::InstalledFontCollection;
using Gdiplus::FontFamily;
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
	LOGFONT logFont;

	TEXTMETRIC tm;
	int nRowSpace = 0;
	int nColSpace = 0;

	DWORD dwDTFlags;

	bool bClip = true;

	unsigned short borderOffsetX = 0;
	unsigned short borderOffsetY = 0;

	bool bVerticalAlignOffset;

	int renderWidth = 0;
	int renderHeight = 0;

	Gdiplus::TextRenderingHint textRenderingHint = Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias;
	Gdiplus::SmoothingMode smoothingMode = Gdiplus::SmoothingMode::SmoothingModeHighQuality;
	Gdiplus::PixelOffsetMode pixelOffsetMode = Gdiplus::PixelOffsetMode::PixelOffsetModeHalf;

	bool bOutLine = false;
	BYTE nOutLinePixel = 1;
	COLORREF dwOutLineColor = RGB(255, 255, 0);

	bool bShadow = false;
	BYTE nShadowOffsetX = 0;
	BYTE nShadowOffsetY = 0;

#ifdef _GDIPLUS		
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	PrivateFontCollection* pFontCollection = nullptr;
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

	StrSize* pStrSizeArr = nullptr;

	long rcWidth = 0;
	long rcHeight = 0;

	long totalWidth = 0;
	long totalHeight = 0;

	long startY = 0;

	LPSURFACE pMemSf = nullptr;

	int hotSpotX = 0;
	int hotSpotY = 0;

	float xScale = 1.0;
	float yScale = 1.0;

	int angle = 0;

#ifdef _USE_HWA
	int hwaType = 0;
	int hwaDriver = 0;
	bool preMulAlpha = false;

	LPSURFACE pHwaSf = nullptr;
#endif

	std::map<wchar_t, StrSize> charSzCache;

#ifdef _GDIPLUS

#else
	inline COLORREF BlackEscape(COLORREF input) {
		return input == BLACK ? RGB(8, 0, 0) : input;
	}
#endif

	inline int GetStartPosX(long totalWidth, long rcWidth) const {
		//DT_LEFT | DT_CENTER | DT_RIGHT
		//if (this->dwDTFlags & DT_LEFT) {
		//	return 0;
		//}
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
		//if (this->dwDTFlags & DT_TOP) {
		//	//return 0;
		//	return this->tm.tmInternalLeading;
		//}
		if (this->dwDTFlags & DT_VCENTER) {
			//return ((rcHeight - totalHeight) >> 1);

			//remove offset to make exactly at the center
			//https://docs.microsoft.com/en-us/windows/win32/gdi/string-widths-and-heights

			auto verticalAlignOffset = bVerticalAlignOffset
				? this->tm.tmInternalLeading + this->tm.tmExternalLeading
				: 0;

			//return ((rcHeight - (totalHeight - (this->tm.tmInternalLeading + this->tm.tmExternalLeading))) >> 1);
			return ((rcHeight - (totalHeight - verticalAlignOffset)) >> 1);
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

		if (size == 0) {
			return -1;  // Failure
		}

		pImageCodecInfo = (ImageCodecInfo*)(malloc(size));

		if (pImageCodecInfo == NULL) {
			return -1;  // Failure
		}

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
		long maxWidth;
		long totalHeight;
	};

	NeoStr(DWORD dwAlignFlags, COLORREF color, HFONT hFont) {
		this->hdc = GetDC(NULL);
		SelectObject(this->hdc, hFont);

		this->SetColor(color);

		this->hFont = hFont;
		GetObject(this->hFont, sizeof(LOGFONT), &this->logFont);

		GetTextMetrics(hdc, &this->tm);
		//this->SetSpace();

		this->dwDTFlags = dwAlignFlags | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL;

		this->strPos.reserve(20);

#ifdef _GDIPLUS		
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		this->pFontCollection = new PrivateFontCollection;
#endif
		// add a default char to return default value when input text is empty
		this->GetCharSizeWithCache(L'露');
	}

	~NeoStr() {
		ReleaseDC(NULL, this->hdc);

		delete[] this->pStrSizeArr;
		this->pStrSizeArr = nullptr;

		delete this->pMemSf;
		this->pMemSf = nullptr;

#ifdef _USE_HWA
		delete this->pHwaSf;
		this->pHwaSf = nullptr;
#endif

#ifdef _GDIPLUS	
		delete this->pFontCollection;
		Gdiplus::GdiplusShutdown(gdiplusToken);
#endif	
	}

#ifdef _GDIPLUS	
	//https://stackoverflow.com/questions/42595856/fonts-added-with-addfontresourceex-are-not-working-in-gdi
	//https://docs.microsoft.com/zh-cn/windows/win32/api/gdiplusheaders/nf-gdiplusheaders-privatefontcollection-addfontfile
	inline void EmbedFont(LPCWSTR pFontFile) {
		this->pFontCollection->AddFontFile(pFontFile);
		auto count = this->pFontCollection->GetFamilyCount();
		
		return;
	}
#endif	

#ifdef _USE_HWA
	inline void SetHWA(int type, int driver, bool preMulAlpha) {
		this->hwaType = type;
		this->hwaDriver = driver;
		this->preMulAlpha = preMulAlpha;
	}
#endif

	inline void SetColor(DWORD color) {
#ifdef _GDIPLUS
		this->dwTextColor = color;
#else
		this->dwTextColor = BlackEscape(color);
#endif
	}

	inline void SetBorderOffset(unsigned short borderOffsetX, unsigned short borderOffsetY) {
		this->borderOffsetX = borderOffsetX;
		this->borderOffsetY = borderOffsetX;
	}

	inline void SetSpace(int nRowSpace = 0, int nColSpace = 0) {
		this->nRowSpace = nRowSpace /*+ this->tm.tmInternalLeading + this->tm.tmExternalLeading*/;
		this->nColSpace = nColSpace;
	}

	inline void SetSmooth(
		Gdiplus::TextRenderingHint textRenderingHint = Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias
		, Gdiplus::SmoothingMode smoothingMode = Gdiplus::SmoothingMode::SmoothingModeHighQuality
		, Gdiplus::PixelOffsetMode pixelOffsetMode = Gdiplus::PixelOffsetMode::PixelOffsetModeHalf) {
		this->textRenderingHint = textRenderingHint;
		this->smoothingMode = smoothingMode;
		this->pixelOffsetMode = pixelOffsetMode;
	}

	inline void SetAlign(DWORD dwAlign, bool bVerticalAlignOffset) {
		this->dwDTFlags = dwAlign | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL;
		this->bVerticalAlignOffset = bVerticalAlignOffset;
	}

	inline void SetClip(bool clip, int renderWidth, int renderHeight) {
		this->bClip = clip;
		this->renderWidth = renderWidth;
		this->renderHeight = renderHeight;
	}

	inline void SetOutLine(BYTE outLinePixel, DWORD color) {
		this->bOutLine = outLinePixel;
		this->nOutLinePixel = outLinePixel;

#ifdef _GDIPLUS
		this->dwOutLineColor = color;
#else
		this->dwOutLineColor = BlackEscape(color);
#endif
	}

	inline void SetHotSpot(int hotSpotX = 0, int hotSpotY = 0) {
		this->hotSpotX = hotSpotX;
		this->hotSpotY = hotSpotY;
	}

	inline void SetScale(float xScale = 1.0, float yScale = 1.0) {
		this->xScale = xScale;
		this->yScale = yScale;
	}

	inline void SetAngle(int angle = 0) {
		this->angle = angle;
	}

	inline StrSize GetCharSizeRaw(wchar_t wChar) {
		//Graphics g(hdc);
		//Font font(GetDC(NULL), this->hFont);
		//PointF origin(0, 0);
		//RectF boundRect;
		//g.MeasureString(&wChar, 1, &font, origin, &boundRect);

		//StringFormat stringFormat;
		//RectF layoutRect(0,0,65535,65535);
		//RectF boundRect;
		//Region region;
		//g.MeasureCharacterRanges(&wChar, 1, &font, layoutRect, &stringFormat, 1, &region);
		//g.MeasureString(&wChar, 1, &font, layoutRect, &stringFormat, &boundRect);

		SIZE sz;
		GetTextExtentPoint32(hdc, &wChar, 1, &sz);

		//TEXTMETRIC tm;
		//GetTextMetrics(hdc, &tm);

		//ABC abc;
		//GetCharABCWidths(hdc, wChar, wChar, &abc);

		//GetCharABCWidths()
		//TEXTMETRIC textMetric;
		//GetTextMetrics(hdc, &textMetric);

		//sz.cy -= (this->tm.tmInternalLeading + this->tm.tmExternalLeading);

		return *(StrSize*)&sz;
	}

	inline StrSize GetCharSizeWithCache(wchar_t wChar) {
		auto it = charSzCache.find(wChar);

		if (it != charSzCache.end()) {
			return it->second;
		}
		else {
			auto sz = GetCharSizeRaw(wChar);
			charSzCache [wChar] = sz;

			return sz;
		}
	}

#define GetCharSize(wChar) this->GetCharSizeWithCache(wChar)

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

	inline CharPos CalculateRange(LPCWSTR pText, LPRECT pRc) {
		this->strPos.clear();

		size_t pTextLen = wcslen(pText);

		if (pTextLen == 0) {
			auto it = charSzCache.cend();
			it--;

			return { it->second.width / 2,it->second.height / 2,0 };
		}

		delete[] this->pStrSizeArr;
		this->pStrSizeArr = nullptr;

		pStrSizeArr = new StrSize [pTextLen + 1];
		memset(pStrSizeArr, 0, sizeof(StrSize) * (pTextLen + 1));

		this->rcWidth = pRc->right - pRc->left;
		this->rcHeight = pRc->bottom - pRc->top;

		this->totalWidth = 0;
		this->totalHeight = 0;
		//this->totalHeight = this->tm.tmInternalLeading;

		size_t pCharStart = 0;
		long maxWidth = 0;

		bool bNegColSpace = nColSpace < 0;
		bool bNegRowSpace = nRowSpace < 0;

		for (size_t pChar = 0; pChar < pTextLen; ) {
			bool newLine = false;		// newline
			bool skipLine = false;		// current line only has /r/n

			long curWidth = 0;
			long curHeight = 0;

			pCharStart = pChar;

			while (true) {
				//while (curWidth < rcWidth
					//&& pChar <= pTextLen) {				
				auto pCurChar = pText + pChar;

				auto curChar = pCurChar [0];
				auto nextChar = pCurChar [1];

				//if (curChar == L'\r' && nextChar == L'\n') {
				//	newLine = true;
				//	//skipLine = (curWidth == 0);
				//	skipLine = (pChar == pCharStart);
				//	pChar += 2;

				//	break;
				//}

				pStrSizeArr [pChar] = GetCharSize(curChar);
				auto charSz = &pStrSizeArr [pChar];

				totalWidth = curWidth;

				//curWidth += (charSz->width + nColSpace);
				curWidth += charSz->width;
				curHeight = max(curHeight, charSz->height);

				if (curWidth > rcWidth) {
					break;
				}

				if (curChar == L'\r' && nextChar == L'\n') {
					newLine = true;
					//skipLine = (curWidth == 0);
					skipLine = (pChar == pCharStart);
					pChar += 2;

					break;
				}

				pChar++;

				if (pChar > pTextLen) {
					break;
				}

				curWidth += nColSpace;

				//if (curWidth > rcWidth) {
				//	continue;
				//}

				//curWidth += nColSpace;
				//pChar++;
			}

			if (!skipLine) {
				auto end = min(pChar, pTextLen) - 2 * newLine;

				this->strPos.emplace_back(StrPos {
					pCharStart,			// start
					end,				// end
					end - pCharStart,	// length
					totalWidth,			// width
					//curWidth,			// width
					curHeight,			// height;
					0,					// x
					totalHeight,		// y
					});

#ifdef _DEBUG
				std::wstring str(pText + pCharStart, end - pCharStart);
#endif // _DEBUG

				maxWidth = curWidth != 0
					? max(maxWidth, curWidth - nColSpace)
					: maxWidth;
				//maxWidth = max(maxWidth, curWidth);
				//maxWidth = max(maxWidth, totalWidth);
			}

			// empty line (skipLine == true) also need to add height
			totalHeight += (curHeight + nRowSpace);
		}

		const auto& lastStrPos = strPos.back();
		const auto lastCharSize = &pStrSizeArr [lastStrPos.start + lastStrPos.length - 1];

		this->startY = GetStartPosY(totalHeight - nRowSpace, rcHeight);

		auto lastCharPos = CharPos {
			//GetStartPosX(lastStrPos.width - nColSpace, rcWidth) + lastStrPos.width + (lastCharSize->width >> 1)
			GetStartPosX(lastStrPos.width, rcWidth) - lastCharSize->width / 4
			+ lastStrPos.width + (lastCharSize->width >> 1)
			,startY + lastStrPos.y + (lastCharSize->height >> 1)
			,maxWidth 
			,totalHeight - nRowSpace };

		return lastCharPos;
	}

	inline void RenderPerChar(LPCWSTR pText, LPRECT pRc) {
		size_t pTextLen = wcslen(pText);

		if (pTextLen == 0) {
			return;
		}

		rcWidth = pRc->right - pRc->left;
		rcHeight = pRc->bottom - pRc->top;

		char scale = 1;
		auto width = abs((rcWidth + this->borderOffsetX * 2) * scale);
		auto height = abs((totalHeight + this->borderOffsetY * 2) * scale);

		if (this->pMemSf == nullptr
			|| this->pMemSf->GetWidth() != width
			|| this->pMemSf->GetHeight() != height) {
			delete this->pMemSf;
			this->pMemSf = nullptr;

#ifdef _DRAWTODC
			pMemSf = CreateSurface(32, width, height);
#else
			//#ifdef _USE_HWA
			//			pMemSf = CreateHWASurface(32, width, height, this->hwaType, this->hwaDriver);
			//#else
			//			pMemSf = CreateSurface(32, width, height);
			//#endif
			pMemSf = CreateSurface(32, width, height);
#endif
		}

		pMemSf->Fill(BLACK);
		pMemSf->CreateAlpha();

#ifdef _DEBUG
		auto type = pMemSf->GetType();
#endif

		//LOGFONT drawLogFont;
		//GetObject(this->hFont, sizeof(LOGFONT), &drawLogFont);
		//drawLogFont.lfQuality = ANTIALIASED_QUALITY;
		//drawLogFont.lfQuality = NONANTIALIASED_QUALITY;
		//this->hFont = CreateFontIndirect(&drawLogFont);

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
		
		//Font font(GetDC(NULL), this->hFont);
		Font font2Plus(GetDC(NULL), this->hFont);
		
		auto pFontFamily = new FontFamily[max(5, this->pFontCollection->GetFamilyCount())];

		int nNumFound = 0;
		this->pFontCollection->GetFamilies(1, pFontFamily, &nNumFound);

		if (nNumFound == 0) {
			InstalledFontCollection ifc;
			ifc.GetFamilies(1, pFontFamily, &nNumFound);
		}

		//Font font(&pFontFamily[0], font2Plus.GetSize(), font2Plus.GetStyle(), font2Plus.GetUnit());

		Font font(&pFontFamily[0], font2Plus.GetSize(), font2Plus.GetStyle(), font2Plus.GetUnit());

		delete[] pFontFamily;

		g.SetTextRenderingHint(this->textRenderingHint);
		g.SetSmoothingMode(this->smoothingMode);
		g.SetPixelOffsetMode(this->pixelOffsetMode);

#else	
		SelectObject(hMemDc, this->hFont);
		SetTextColor(hMemDc, this->dwTextColor);
		SetBkMode(hMemDc, TRANSPARENT);
#endif

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

		RECT displayRc = { 0,0,(LONG)this->renderWidth, (LONG)this->renderHeight, };

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
			//int x = GetStartPosX(curStrPos.width - nColSpace, rcWidth);
			int x = GetStartPosX(curStrPos.width, rcWidth);
			x -= pStrSizeArr [curStrPos.start].width / 8;

			for (size_t curChar = 0; curChar < curStrPos.length; curChar++) {
				auto offset = curStrPos.start + curChar;
				auto pCurChar = pText + offset;
				charSz = &pStrSizeArr [offset];

				if (!clip(x, (this->startY + curStrPos.y), charSz)) {
#ifdef _GDIPLUS	
#ifdef  _PATH
					txtPath.AddString(pCurChar, 1,
						&pFontFamily, fontstyle, font.GetSize()
						, PointF((float)x, (float)((y + curStrPos.y) * scale)), &cStringFormat);
#else

#ifdef _DEBUG
					auto WX = (float)(x)+(float)(this->borderOffsetY);
					auto WY = (float)(curStrPos.y) + (float)(this->borderOffsetY);
#endif

					//auto pos = y + curStrPos.y;
					//g.DrawString(pCurChar, 1, &font, PointF((float)x, (float)(y + curStrPos.y)), &solidBrush);
					//g.DrawString(pCurChar, 1, &font, PointF((float)x, (float)(curStrPos.y)), &solidBrush);
					auto status = g.DrawString(pCurChar, 1, &font
						, PointF((float)(x)+(float)(this->borderOffsetY)
							, (float)(curStrPos.y) + (float)(this->borderOffsetY))
						, &solidBrush);
#endif
#else
					TextOut(hMemDc, x * scale, (y + curStrPos.y) * scale, pCurChar, 1);
#endif
				}

				x += (charSz->width + nColSpace);
			}
		}

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
		//CLSID pngClsid;
		//GetEncoderClsid(L"image/png", &pngClsid);
		//bitmap.Save(L"F:\\NeoStr.png", &pngClsid, NULL);
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

		//#ifdef _USE_HWA
		//		auto pHWASf = pMemSf->GetRenderTargetSurface();
		//		auto sfCoef = GetSfCoef(pHWASf);
		//#else
		//		auto sfCoef = GetSfCoef(pMemSf);
		//#endif

		auto sfCoef = GetSfCoef(pMemSf);

		auto lineSz = sfCoef.pitch;

		for (int y = 0; y < height; y++) {
			auto pData = sfCoef.pData + y * sfCoef.pitch;
			auto pBmp = pRawBitmap + (height - 1 - y) * bitmapData.Stride / 4;

			memcpy(pData, pBmp, lineSz);

			for (int x = 0; x < width; x++) {
				auto pAlphaData = sfCoef.pAlphaData + (height - 1 - y) * sfCoef.alphaPitch + x * sfCoef.alphaByte;
				auto curAlpha = pRawBitmap + (height - 1 - y) * bitmapData.Stride / 4 + x;
				pAlphaData [0] = (*curAlpha & 0xff000000) >> 24;
			}
		}

		//#ifdef _USE_HWA
		//		ReleaseSfCoef(pHWASf, sfCoef);
		//		pMemSf->ReleaseRenderTargetSurface(pHWASf);
		//#else
		//		ReleaseSfCoef(pMemSf, sfCoef);
		//#endif

		ReleaseSfCoef(pMemSf, sfCoef);

#ifdef _DEBUG
		//_SavetoClipBoard(pMemSf, false);
#endif // _DEBUG

#ifdef _USE_HWA
		delete pHwaSf;
		pHwaSf = nullptr;

		pHwaSf = CreateHWASurface(32, width, height, this->hwaType, this->hwaDriver);

		//pMemSf->DemultiplyAlpha();
		if (this->preMulAlpha) {
			pMemSf->PremultiplyAlpha();		// only needed in DX11 premultiplied mode
		}

		pMemSf->Blit(*pHwaSf);

		//pHwaSf->DemultiplyAlpha();
		//pHwaSf->PremultiplyAlpha();
#endif

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
		//				//pAlphaData [0] = (curColor & 0xff000000) >> 24;
		//#endif
		//
		//#ifdef _CONSOLE
		//				printf("%d %d %d %d\n", A, R, G, B);
		//#endif // _CONSOLE
		//				});
	}

	inline void Display(LPSURFACE pDst, LPCWSTR pText, LPRECT pRc
		, BlitMode bm = BMODE_TRANSP, BlitOp bo = BOP_COPY, LPARAM boParam = 0, int bAntiA = 0
		, DWORD dwLeftMargin = 0, DWORD dwRightMargin = 0, DWORD dwTabSize = 8) {

		auto height = pDst->DrawText(pText, wcslen(pText), pRc
			, this->dwDTFlags, this->dwTextColor, this->hFont
			, bm, bo, boParam, bAntiA);

		return;
	}

	inline void DisplayPerChar(LPSURFACE pDst, LPCWSTR pText, LPRECT pRc
		, BlitMode bm = BMODE_TRANSP, BlitOp bo = BOP_COPY, LPARAM boParam = 0, int bAntiA = 0
		, DWORD dwLeftMargin = 0, DWORD dwRightMargin = 0, DWORD dwTabSize = 8) {

		size_t pTextLen = wcslen(pText);

		if (pTextLen == 0) {
			return;
		}

		//CalculateRange(pText, pRc, nRowSpace, nColSpace);
		//RenderPerChar(pText, pRc, nRowSpace, nColSpace);

		if (pDst != nullptr

#ifdef _USE_HWA
			&& pHwaSf != nullptr
#else
			&& pMemSf != nullptr
#endif
			) {

#ifdef _USE_HWA
			auto pSf = pHwaSf;
#else
			auto pSf = pMemSf;
#endif

#ifdef _USE_HWA

			int xOffset = -this->borderOffsetX;
			int yOffset = -this->borderOffsetY + this->startY;

			POINT hotSpot = { this->hotSpotX - xOffset,this->hotSpotY - yOffset };

			//int xPos = pRc->left + xOffset;
			//int yPos = pRc->top + yOffset;

			int xPos = pRc->left + this->hotSpotX;
			int yPos = pRc->top + this->hotSpotY;

			pSf->BlitEx(*pDst, (float)xPos, (float)yPos, this->xScale, this->yScale
				, 0, 0, pSf->GetWidth(), pSf->GetHeight(), &hotSpot, (float)this->angle
				, bm, bo, boParam, bAntiA);
#else
			pSf->Blit(*pDst, xPos, yPos, bm, bo, boParam, bAntiA);
#endif

		}
	}
};

#ifdef _GDIPLUS	
// compatible with MMF
#define Font FontW
#define fpFont fpFontW
#endif

