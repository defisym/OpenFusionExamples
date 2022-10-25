#pragma once

//#define _FONTEMBEDDEBUG
//#define _CONSOLE

#include <map>
#include <vector>
#include <functional>
#include <string_view>

//#include <FusionUtilities.h>

#ifdef _DEBUG		
#include <assert.h>
#endif

// compatible with MMF
#undef Font
#undef fpFont

#pragma region _GDIPLUS_INIT

//#define _BLUR

#ifdef _BLUR
#define GDIPVER 0x0110
#endif

#include <gdiplus.h>
#pragma comment(lib,"Gdiplus")

#include <gdiplusheaders.h>

#ifdef _BLUR
#include <gdipluseffects.h>
#endif

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

#ifdef _BLUR
using Gdiplus::Blur;
using Gdiplus::BlurParams;
#endif

#pragma endregion

inline RECT operator+(RECT rA, RECT rB) {
	return RECT { rA.left + rB.left
				,rA.top + rB.top
				,rA.right + rB.right
				,rA.bottom + rB.bottom };
}

constexpr auto DEFAULT_EBORDER_OFFSET = 20;
constexpr auto DEFAULT_FORMAT_RESERVE = 10;

constexpr auto DEFAULT_CHARACTER = L'　';

//#define MEASURE_GDI_PLUS

class NeoStr {
private:
	HDC hdc;
	COLORREF dwTextColor;

	HFONT hFont;
	LOGFONT logFont;

	Font* pFont = nullptr;

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

	bool bTextValid = true;
	size_t pTextLen = 0;

	LPWSTR pRawText = nullptr;
	LPWSTR pText = nullptr;

	// Set to false if app have a shared GDI plus environment
	bool needGDIPStartUp = true;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	Bitmap* pBitmap = nullptr;
	PrivateFontCollection* pFontCollection = nullptr;

#ifdef MEASURE_GDI_PLUS
	Graphics* pMeasure;
	PointF origin = { 0, 0 };
	
	StringFormat stringFormat;

	SIZE measureBaseSize = { 0 };
	wchar_t measureBaseStr[2]= { L'.',L'.' };
	
	wchar_t measureStr[3] = { L'.',L'.',L'.' };
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

	struct CharPos {
		long x;
		long y;
		long maxWidth;
		long totalHeight;
	};

	CharPos* pCharPosArr = nullptr;

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

	int hwaType = 0;
	int hwaDriver = 0;
	bool preMulAlpha = false;

	LPSURFACE pHwaSf = nullptr;

	std::wstring notAtStart = L"!%),.:;>?]}¢¨°·ˇˉ―‖’”…‰′″›℃∶、。〃〉》」』】〕〗〞︶︺︾﹀﹄﹚﹜﹞！＂％＇），．：；？］｀｜｝～￠";
	std::wstring notAtEnd = L"$([{£¥·‘“〈《「『【〔〖〝﹙﹛﹝＄（．［｛￡￥";

	using CharSizeCache = std::map<wchar_t, StrSize>;
	CharSizeCache charSzCache;

	struct CharSizeCacheSecond {
		HDC hdc;
		CharSizeCache cache;
	};

	using LogFontHash = size_t;
	using CharSizeCacheWithFont = std::map<LogFontHash, CharSizeCacheSecond>;
	CharSizeCacheWithFont charSzCacheWithFont;
	
	StrSize defaultCharSz;

	struct FormatColor {
		size_t start;
		//size_t end;

		Color color;
	};

	std::vector<FormatColor> colorFormat;
	std::vector<Color> colorStack;

	struct FormatICon {
		size_t start;
		//size_t end;

		int animation;
		int direction;
		int frame;

		DWORD hImage;

		size_t x;
		size_t y;
	};

	std::vector<FormatICon> iConFormat;
	std::map<DWORD, LPSURFACE> iConLib;

	struct FormatFont {
		size_t start;
		//size_t end;

		size_t startWithNewLine;

		LOGFONT logFont;
	};

	std::vector<FormatFont> FontFormat;
	std::vector<LOGFONT> logFontStack;

	std::map<LogFontHash, Font*> FontCache;

	float iConOffsetX = 0;
	float iConOffsetY = 0;

	float iConScale = 1.0;

	bool iConResample = false;
	
	npAppli appli = nullptr;
	LPRO pIConActive = nullptr;
	LPSURFACE pDefaultICon = nullptr;

	inline bool CheckMatch(wchar_t wChar, std::wstring& data) {
		for (auto& wChartoCheck : data) {
			if (wChartoCheck == wChar) {
				return true;
			}
		}

		return false;
	}

	inline bool NotAtStart(wchar_t wChar) {
		return CheckMatch(wChar, notAtStart);
	}

	inline bool NotAtEnd(wchar_t wChar) {
		return CheckMatch(wChar, notAtEnd);
	}

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

#ifdef _DEBUG
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
#endif

	inline size_t LogFontHasher(LOGFONT logFont) {
		constexpr auto HASHER_MAGICNUMBER = 0x9e3779b9;
		constexpr auto HASHER_MOVE = [](size_t seed) { return HASHER_MAGICNUMBER + (seed << 6) + (seed >> 2); };

		size_t seed = 65535;

		seed ^= logFont.lfHeight + HASHER_MOVE(seed);
		seed ^= logFont.lfWidth + HASHER_MOVE(seed);
		seed ^= logFont.lfEscapement + HASHER_MOVE(seed);
		seed ^= logFont.lfOrientation + HASHER_MOVE(seed);
		seed ^= logFont.lfWeight + HASHER_MOVE(seed);
		seed ^= logFont.lfItalic + HASHER_MOVE(seed);
		seed ^= logFont.lfUnderline + HASHER_MOVE(seed);
		seed ^= logFont.lfStrikeOut + HASHER_MOVE(seed);
		seed ^= logFont.lfCharSet + HASHER_MOVE(seed);
		seed ^= logFont.lfOutPrecision + HASHER_MOVE(seed);
		seed ^= logFont.lfClipPrecision + HASHER_MOVE(seed);
		seed ^= logFont.lfQuality + HASHER_MOVE(seed);
		seed ^= logFont.lfPitchAndFamily + HASHER_MOVE(seed);

		for (auto i = 0; i < LF_FACESIZE; i++) {
			seed ^= logFont.lfFaceName[i] + HASHER_MOVE(seed);
		}

		return seed;
	}

public:
	NeoStr(DWORD dwAlignFlags, COLORREF color
		, HFONT hFont
		, bool needGDIPStartUp = true
		, PrivateFontCollection* pFontCollection = nullptr) {
		this->needGDIPStartUp = needGDIPStartUp;

		if (this->needGDIPStartUp) {
			Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		}

		//this->hdc = GetDC(rdPtr->rHo.hoAdRunHeader->rhHEditWin);
		this->hdc = GetDC(NULL);
		SelectObject(this->hdc, hFont);		

		this->hFont = hFont;
		GetObject(this->hFont, sizeof(LOGFONT), &this->logFont);
		GetTextMetrics(hdc, &this->tm);

		this->pFontCollection = pFontCollection;
		this->pFont = GetFontPointerWithCache(this->logFont);

		this->SetColor(color);
		this->dwDTFlags = dwAlignFlags | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL;

		this->strPos.reserve(20);		
		
		this->colorStack.reserve(DEFAULT_FORMAT_RESERVE);
		this->colorFormat.reserve(DEFAULT_FORMAT_RESERVE);
		this->iConFormat.reserve(DEFAULT_FORMAT_RESERVE);

#ifdef MEASURE_GDI_PLUS		
		this->pMeasure = new Graphics(hdc);

		//this->stringFormat.GenericDefault();
		this->stringFormat.GenericTypographic();
		
		RectF boundRect;
		this->pMeasure->MeasureString(L"..", 2, pFont, origin, &stringFormat, &boundRect);

		this->measureBaseSize = { long(boundRect.GetRight() - boundRect.GetLeft()),long(boundRect.GetBottom() - boundRect.GetTop()) };
		this->measureBaseSize.cy = long(this->pFont->GetHeight(this->pMeasure));
#endif

		// add a default char to return default value when input text is empty
		//this->GetCharSizeWithCache(L'露');
		this->defaultCharSz = this->GetCharSizeWithCache(DEFAULT_CHARACTER);
	}

	~NeoStr() {
		ReleaseDC(NULL, this->hdc);

		for (auto& it : charSzCacheWithFont) {
			ReleaseDC(NULL, it.second.hdc);
		}

#ifdef MEASURE_GDI_PLUS		
		delete this->pMeasure;
		this->pMeasure = nullptr;
#endif

		for (auto& it : FontCache) {
			delete it.second;
		}

		delete[] this->pRawText;
		this->pRawText = nullptr;

		delete[] this->pText;
		this->pText = nullptr;

		delete[] this->pStrSizeArr;
		this->pStrSizeArr = nullptr;

		delete[] this->pCharPosArr;
		this->pCharPosArr = nullptr;

		delete this->pMemSf;
		this->pMemSf = nullptr;

		delete this->pBitmap;
		this->pBitmap = nullptr;

		delete this->pHwaSf;
		this->pHwaSf = nullptr;

		delete this->pDefaultICon;
		this->pDefaultICon = nullptr;

		for (auto& it : this->iConLib) {
			delete it.second;
		}

		if (this->needGDIPStartUp) {
			Gdiplus::GdiplusShutdown(gdiplusToken);
		}
	}

	//https://stackoverflow.com/questions/42595856/fonts-added-with-addfontresourceex-are-not-working-in-gdi
	//https://docs.microsoft.com/zh-cn/windows/win32/api/gdiplusheaders/nf-gdiplusheaders-privatefontcollection-addfontfile
	//https://www.codeproject.com/Articles/42041/How-to-Use-a-Font-Without-Installing-it
	//https://blog.csdn.net/analogous_love/article/details/45845971
	inline void EmbedFont(LPCWSTR pFontFile) {
		this->pFontCollection->AddFontFile(pFontFile);
		auto count = this->pFontCollection->GetFamilyCount();

		return;
	}

	inline static void EmbedFont(LPCWSTR pFontFile, PrivateFontCollection& fontCollection) {
		fontCollection.AddFontFile(pFontFile);

		return;
	}

	inline static bool FontCollectionHasFont(LPWSTR pFaceName
		, Gdiplus::FontCollection* pFontCollection) {
		if (pFontCollection == nullptr) {
			return false;
		}

		int n = pFontCollection->GetFamilyCount();

		if (n == 0) {
			return false;
		}

		FontFamily* ffs = new FontFamily [n];

		int found;
		pFontCollection->GetFamilies(n, ffs, &found);

		if (found == 0) {
			return false;
		}

		wchar_t name [LF_FACESIZE] { 0 };

		LANGID language = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);

		bool has = false;
		bool hasSuffixRegular = false;
		//bool hasSuffixNormal = false;

		std::wstring withRegular = (std::wstring)pFaceName + (std::wstring)L" Regular";
		//std::wstring withNormal = (std::wstring)pFaceName + (std::wstring)L" Normal";

		for (int i = 0; i < n; i++) {
			has = false;
			hasSuffixRegular = false;
			//hasSuffixNormal = false;

			auto hasName = [&] (LANGID language = (LANGID)0U) {
				memset(name, 0, LF_FACESIZE * sizeof(wchar_t));
				ffs [i].GetFamilyName(name, language);

				has |= (_wcsicmp(name, pFaceName) == 0);

				if (has) {
					return;
				}

				hasSuffixRegular |= (_wcsicmp(name, withRegular.c_str()) == 0);

				if (hasSuffixRegular) {
					return;
				}

				//hasSuffixNormal |= (_wcsicmp(name, withNormal.c_str()) == 0);
			};

			hasName();
			hasName(language);

			if (has || hasSuffixRegular/* || hasSuffixNormal*/) {
				if (hasSuffixRegular) {
					wcscpy_s(pFaceName, LF_FACESIZE, withRegular.c_str());
				}
				//if (hasSuffixNormal) {
				//	wcscpy_s(pFaceName, LF_FACESIZE, withNormal.c_str());
				//}

				break;
			}
		}

		delete[] ffs;

		return has || hasSuffixRegular/* || hasSuffixNormal*/;
	}

	inline static int GetFontStyle(LOGFONT& logFont) {
		int fontStyle = Gdiplus::FontStyleRegular;

		if (logFont.lfWeight >= 600) {
			fontStyle = fontStyle | Gdiplus::FontStyleBold;
		}
		if (logFont.lfItalic) {
			fontStyle = fontStyle | Gdiplus::FontStyleItalic;
		}
		if (logFont.lfUnderline) {
			fontStyle = fontStyle | Gdiplus::FontStyleUnderline;
		}
		if (logFont.lfStrikeOut) {
			fontStyle = fontStyle | Gdiplus::FontStyleStrikeout;
		}

		return fontStyle;
	}
		
	inline Font GetFont(LOGFONT logFont) {
		//auto bTest = StrIEqu(this->logFont.lfFaceName, L"思源黑体 CN");
		auto bFound = FontCollectionHasFont(logFont.lfFaceName, this->pFontCollection);

#ifdef _FONTEMBEDDEBUG
		if (!bFound) {
			MSGBOX((std::wstring)this->logFont.lfFaceName + (std::wstring)L" Not Found");
		}
#endif // _FONTEMBEDDEBUG

		return Font(logFont.lfFaceName
			, (float)abs(logFont.lfHeight)
			, GetFontStyle(logFont)
			, Gdiplus::UnitWorld
			, bFound ? this->pFontCollection
			: nullptr);
	}

	inline Font* GetFontPointer(LOGFONT logFont) {
		auto bFound = FontCollectionHasFont(logFont.lfFaceName, this->pFontCollection);
		
		return new Font(logFont.lfFaceName
			, (float)abs(logFont.lfHeight)
			, GetFontStyle(logFont)
			, Gdiplus::UnitWorld
			, bFound ? this->pFontCollection
			: nullptr);
	}

	inline Font* GetFontPointerWithCache(LOGFONT logFont) {
		auto logFontHash = LogFontHasher(logFont);
		auto it = FontCache.find(logFontHash);

		if (it != FontCache.end()) {
			return it->second;
		}
		else {
			auto newFont = GetFontPointer(logFont);
			FontCache[logFontHash] = newFont;

			return newFont;
		}
	}

	// #AARRGGBB
	inline Color GetColor(std::wstring_view hex) {
		auto dec = _h2d(hex.data(), hex.size());

		auto A = (BYTE)((dec >> 24) & 0xFF);
		auto R = (BYTE)((dec >> 16) & 0xFF);
		auto G = (BYTE)((dec >> 8) & 0xFF);
		auto B = (BYTE)((dec) & 0xFF);		
		
		return Color(A, R, G, B);
	}

	inline Color GetColor(DWORD color) {
		return 	Color(255, GetRValue(color), GetGValue(color), GetBValue(color));
	}

	inline StrSize GetDefaultCharSize() {
		return this->defaultCharSz;
	}

	inline void SetIConOffset(float iConOffsetX = 0, float iConOffsetY = 0) {
		this->iConOffsetX = iConOffsetX;
		this->iConOffsetY = iConOffsetY;
	}

	inline void SetIConScale(float iConScale = 1.0) {
		this->iConScale = iConScale;
	}

	inline void SetIConResample(bool iConResample=false){
		this->iConResample=iConResample;
	}

	inline void LinkActive(LPRO pObject) {
		this->pIConActive = pObject;
	}

	inline void SetAppli(npAppli appli) {
		this->appli = appli;
	}

	inline void SetHWA(int type, int driver, bool preMulAlpha) {
		this->hwaType = type;
		this->hwaDriver = driver;
		this->preMulAlpha = preMulAlpha;
	}

	inline void SetColor(DWORD color) {
		this->dwTextColor = color;
	}

	inline void SetBorderOffset(unsigned short borderOffsetX, unsigned short borderOffsetY) {
		this->borderOffsetX = borderOffsetX;
		this->borderOffsetY = borderOffsetX;
	}

	// set row/col space
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

	//inline void SetOutLine(BYTE outLinePixel, DWORD color) {
	//	this->bOutLine = outLinePixel;
	//	this->nOutLinePixel = outLinePixel;

	//	this->dwOutLineColor = color;
	//}

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

	// deprecated
	inline StrSize GetCharSizeRaw(wchar_t wChar) {
#ifdef MEASURE_GDI_PLUS		
		SIZE sz = { 0 };
		GetTextExtentPoint32(hdc, &wChar, 1, &sz);

		RectF boundRect;

		this->measureStr[1] = wChar;

		this->pMeasure->MeasureString(this->measureStr, 3, pFont, origin, &stringFormat, &boundRect);

		auto curWidth = long(boundRect.GetRight() - boundRect.GetLeft());
		auto curHeight = long(boundRect.GetBottom() - boundRect.GetTop());		

		sz.cx = curWidth - measureBaseSize.cx;
		//sz.cy = curHeight;
		sz.cy = this->measureBaseSize.cy;

		sz.cy -= (this->tm.tmExternalLeading);
#else
		SIZE sz = { 0 };
		GetTextExtentPoint32(hdc, &wChar, 1, &sz);
#endif
		//ABC abc;
		//GetCharABCWidths(hdc, wChar, wChar, &abc);

		//sz.cx -= this->tm
		//sz.cy -= (this->tm.tmInternalLeading + this->tm.tmExternalLeading);

		return *(StrSize*)&sz;
	}
	
	// deprecated
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

	inline StrSize GetCharSizeRaw(wchar_t wChar, HDC hdc) {
		SIZE sz = { 0 };
		GetTextExtentPoint32(hdc, &wChar, 1, &sz);

		return *(StrSize*)&sz;
	}

	inline StrSize GetCharSizeWithCache(wchar_t wChar, LOGFONT logFont) {
		auto logFontHash = LogFontHasher(logFont);
		auto it = charSzCacheWithFont.find(logFontHash);

		if (it != charSzCacheWithFont.end()) {
			auto& cacheSecond = it->second;
			auto& charCache = cacheSecond.cache;

			auto charIt = charCache.find(wChar);
			if (charIt != charCache.end()) {
				return charIt->second;
			}
			else {
				auto sz = GetCharSizeRaw(wChar, cacheSecond.hdc);
				charCache[wChar] = sz;

				return sz;
			}
		}
		else {
			auto hdc = GetDC(NULL);
			auto hFont = CreateFontIndirect(&logFont);			
			SelectObject(hdc, hFont);

			charSzCacheWithFont[logFontHash] = { hdc };

			return GetCharSizeWithCache(wChar,logFont);
		}
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

	inline void GetFormat(LPCWSTR pStr) {
		if (pStr == nullptr) {
			this->bTextValid = false;

			return;
		}

		size_t pInputLen = wcslen(pStr);

		if (pInputLen == 0) {
			this->bTextValid = false;

			return;
		}

		delete[] this->pRawText;
		this->pRawText = nullptr;

		delete[] this->pText;
		this->pText = nullptr;

		this->pRawText = new wchar_t[pInputLen + 1];
		memset(pRawText, 0, sizeof(wchar_t) * (pInputLen + 1));

		this->pText = new wchar_t[pInputLen + 1];
		memset(pText, 0, sizeof(wchar_t) * (pInputLen + 1));

		memcpy(pRawText, pStr, sizeof(wchar_t) * (pInputLen + 1));

		// Format
		// [ICon = Direction, Frame]
		// [Color = #FFFFFFFF][/Color]

		auto pCurChar = pRawText;
		auto pSavedChar = pText;

		this->colorStack.clear();
		this->colorStack.emplace_back(GetColor(this->dwTextColor));

		this->colorFormat.clear();
		this->colorFormat.emplace_back(FormatColor{ 0,colorStack.back() });

		this->iConFormat.clear();

		this->logFontStack.clear();
		this->logFontStack.emplace_back(this->logFont);

		this->FontFormat.clear();
		this->FontFormat.emplace_back(FormatFont{ 0,0,logFontStack.back() });

		size_t newLineCount = 0;

		while (true) {
			// End
			if ((pCurChar - pRawText) == pInputLen) {
				break;
			}

			auto curChar = pCurChar[0];
			auto nextChar = pCurChar[1];

			// new line
			if (curChar == L'\r' && nextChar == L'\n') {
				newLineCount++;
			}

			// Escape
			if (curChar == L'\\' && nextChar == L'[') {
				pSavedChar[0] = L'[';

				pCurChar += 2;
				pSavedChar++;

				continue;
			}

			// Parse Format
			if (curChar == L'[') {
				size_t end = 0;

				bool bEndOfText = false;

				while ((pCurChar + end)[0] != L']') {
					if ((pCurChar - pRawText) + end == pInputLen) {
						bEndOfText = true;

						break;
					}

					end++;
				}

				// get format
				//std::wstring_view controlStrDebug(pCurChar + 1, end - 1);
				if (!bEndOfText) {
					auto bEndOfRegion = (pCurChar + 1)[0] == L'/';

					// handle format
					do {
						size_t equal = 0;

						while ((pCurChar + equal)[0] != L'=') {
							if (equal == end) {
								break;
							}
							equal++;
						}

						auto GetTrimmedStr = [](LPWSTR pStart, size_t length) {
							while (pStart[0] == L' ') {
								pStart++;
								length--;
							}

							while ((pStart + length - 1)[0] == L' ') {
								length--;
							}

							return std::wstring_view(pStart, length);
						};
						auto StringViewIEqu = [](std::wstring_view& str, LPCWSTR pStr) {
							auto length = wcslen(pStr);

							if (length != str.size()) {
								return false;
							}

							for (size_t i = 0; i < length; i++) {
								if (ChrCmpIW(str[i], pStr[i]) != 0) {
									return false;
								}
							}

							return true;
						};
						auto StringViewToInt = [GetTrimmedStr](std::wstring_view& str) {
							return _stoi(GetTrimmedStr(const_cast<wchar_t*>(str.data()), str.size()).data());
						};

						std::wstring_view controlStr = GetTrimmedStr(pCurChar + bEndOfRegion + 1, equal - bEndOfRegion - 1);
						std::wstring_view controlParam = !bEndOfRegion
							? GetTrimmedStr(pCurChar + equal + 1, end - equal - 1)
							: controlStr;

						do {
							size_t savedLength = pSavedChar - pText - newLineCount * 2;
							size_t savedLengthWithNewLine = pSavedChar - pText;

							if (StringViewIEqu(controlStr, L"Color")
								|| StringViewIEqu(controlStr, L"C")) {
								size_t savedLengthLocal = savedLength;

								if (!bEndOfRegion) {
									colorStack.emplace_back(GetColor(controlParam));
								}
								else {
									colorStack.pop_back();
								}

								auto& lastColorFormat = this->colorFormat.back();

								// if equal, replace last format
								if (savedLengthLocal == lastColorFormat.start) {
									lastColorFormat.color = colorStack.back();
								}
								else {
									this->colorFormat.emplace_back(FormatColor{ savedLengthLocal,colorStack.back() });
								}

								break;
							}

							if (StringViewIEqu(controlStr, L"ICon")) {
								size_t savedLengthLocal = savedLength;

								int frame = 0;
								int direction = 0;
								int animation = 0;

								auto paramParser = controlParam;

								do {
									auto ParseParam = [StringViewToInt, &paramParser](int& data) {
										auto start = paramParser.find_last_of(L',');

										if (start == std::wstring::npos) {
											data = StringViewToInt(paramParser);

											return false;
										}
										else {
											auto paramStr = paramParser.substr(start + 1);
											paramParser = paramParser.substr(0, start);

											data = StringViewToInt(paramParser);

											return true;
										}
									};

									if (!ParseParam(frame)) {
										break;
									}

									if (!ParseParam(direction)) {
										break;
									}

									if (!ParseParam(animation)) {
										break;
									}
								} while (0);

								DWORD hImage = -1;

								do {
									if (this->pIConActive == nullptr || this->appli == nullptr) {
										break;
									}

									if (animation < 0 || direction < 0 || frame < 0) {
										break;
									}

									auto pRoa = &this->pIConActive->roa;

									if (pRoa->raAnimOffset->anOffsetToDir[direction] < 0) {
										break;
									}

									auto pDir = &pRoa->raAnimDirOffset[direction];

									if (frame >= pDir->adNumberOfFrame) {
										break;
									}

									hImage = pDir->adFrame[frame];
								} while (0);

								this->iConFormat.emplace_back(FormatICon{ savedLength
																			, animation
																			, direction
																			, frame
																			, hImage });

								// space for icon
								pSavedChar[0] = DEFAULT_CHARACTER;
								pSavedChar++;

								break;
							}

							using FontFormatControlCallback = std::function<void(LOGFONT& logFont)>;

							auto FontFormatControl = [&](FontFormatControlCallback callBack) {
								size_t savedLengthLocal = savedLength;

								if (!bEndOfRegion) {
									LOGFONT newLogFont = logFontStack.back();

									callBack(newLogFont);

									logFontStack.emplace_back(newLogFont);
								}
								else {
									logFontStack.pop_back();
								}

								auto& lastFontFormat = this->FontFormat.back();

								// if equal, replace last format
								if (savedLengthLocal == lastFontFormat.start) {
									lastFontFormat.logFont = logFontStack.back();
								}
								else {
									this->FontFormat.emplace_back(FormatFont{ savedLengthLocal
										,savedLengthWithNewLine
										,logFontStack.back() });
								}
							};

							if (StringViewIEqu(controlStr, L"Font")
								|| StringViewIEqu(controlStr, L"F")) {
								FontFormatControl([&](LOGFONT& newLogFont) {
									memset(newLogFont.lfFaceName, 0
										, LF_FACESIZE * sizeof(WCHAR));
									memcpy(newLogFont.lfFaceName, controlParam.data()
										, min(LF_FACESIZE, controlParam.size()) * sizeof(WCHAR));
									});

								break;
							}

							if (StringViewIEqu(controlStr, L"Size")
								|| StringViewIEqu(controlStr, L"S")) {
								FontFormatControl([&](LOGFONT& newLogFont) {
									auto size = StringViewToInt(controlParam);;
									auto newSize = -1 * MulDiv(size
										, GetDeviceCaps(this->hdc, LOGPIXELSY)
										, 72);

									newLogFont.lfHeight = newSize;
										});

								break;
							}

							if (StringViewIEqu(controlStr, L"Bold")
								|| StringViewIEqu(controlStr, L"B")) {
								FontFormatControl([&](LOGFONT& newLogFont) {
									newLogFont.lfWeight = FW_EXTRABOLD;
									});								

								break;
							}

							if (StringViewIEqu(controlStr, L"Italic")
								|| StringViewIEqu(controlStr, L"I")) {
								FontFormatControl([&](LOGFONT& newLogFont) {
									newLogFont.lfItalic = TRUE;
									});

								break;
							}

							if (StringViewIEqu(controlStr, L"Underline")
								|| StringViewIEqu(controlStr, L"U")) {
								FontFormatControl([&](LOGFONT& newLogFont) {
									newLogFont.lfUnderline = TRUE;
									});

								break;
							}

							if (StringViewIEqu(controlStr, L"StrikeOut")
								|| StringViewIEqu(controlStr, L"S")) {
								FontFormatControl([&](LOGFONT& newLogFont) {
									newLogFont.lfStrikeOut = TRUE;
									});

								break;
							}
						} while (0);
					} while (0);

					// include ']'
					pCurChar += end + 1;

					continue;
				}
			}

			pSavedChar[0] = pCurChar[0];
			
			pCurChar++;
			pSavedChar++;
		}

		pTextLen = wcslen(pText);

		if (pTextLen == 0) {
			this->bTextValid = false;

			return;
		}

		return;
	}

	inline CharPos CalculateRange(LPRECT pRc) {
		this->strPos.clear();

		if (!this->bTextValid) {
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

		size_t notAtStartCharPos = -1;
		bool bPunctationSkip = false;

		auto fontIt = this->FontFormat.begin();
		auto localLogFont = fontIt->logFont;

		for (size_t pChar = 0; pChar < pTextLen; ) {
			bool newLine = false;		// newline
			bool skipLine = false;		// current line only has /r/n

			long curWidth = 0;
			long curHeight = 0;

			pCharStart = pChar;

			while (true) {
				auto pCurChar = pText + pChar;

				auto curChar = pCurChar [0];
				auto nextChar = pCurChar [1];

				if (fontIt != this->FontFormat.end()
					&& pChar >= fontIt->startWithNewLine) {
					localLogFont = fontIt->logFont;

					fontIt++;
				}

				pStrSizeArr[pChar] = GetCharSizeWithCache(curChar, localLogFont);
				auto charSz = &pStrSizeArr [pChar];

				totalWidth = curWidth;

				curWidth += charSz->width;
				curHeight = max(curHeight, charSz->height);

				auto HandleNewLine = [&]() {
					newLine = true;
					skipLine = (pChar == pCharStart);

					if ((notAtStartCharPos + 1) == pChar) {
						bPunctationSkip = true;
					}
				};

				if (curWidth > rcWidth) {
					//bool bTooNarrow = this->defaultCharSz.width > rcWidth;
					bool bTooNarrow = charSz->width > rcWidth;

					if (bTooNarrow) {
						if (curChar == L'\r' && nextChar == L'\n') {
							HandleNewLine();
							pChar += 2;

							break;
						}
						else {
							pChar += 1;

							break;
						}
					}

					auto pPreviousChar = pText + pChar - 1;
					auto PreviousChar = pPreviousChar[0];

					auto previousCharSz = &pStrSizeArr[pChar - 1];

					auto Backward = [&]() {
						if (curWidth - (previousCharSz->width + nColSpace + charSz->width) <= 0) {
							return;
						}

						pChar--;

						curWidth -= charSz->width;

						curWidth -= previousCharSz->width;
						curWidth -= nColSpace;
					};

					if (NotAtStart(curChar)) {
						notAtStartCharPos = pChar;

						if (NotAtStart(nextChar)) {
							if (pChar != pCharStart) {
								Backward();
							}

							break;
						}
					}
					else {
						if (pChar != pCharStart) {
							if (NotAtEnd(PreviousChar)) {
								Backward();
							}
						}

						break;
					}
				}

				if (curChar == L'\r' && nextChar == L'\n') {
					HandleNewLine();
					pChar += 2;

					break;
				}

				pChar++;

				if (pChar > pTextLen) {
					break;
				}

				curWidth += nColSpace;
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
			}

			if (!bPunctationSkip) {
				// empty line (skipLine == true) also need to add height
				// ...unless it's following a 'not at start' punctuation
				totalHeight += (curHeight + nRowSpace);
			}
			else {
				bPunctationSkip = false;
			}
		}

		const auto& lastStrPos = strPos.back();
		const auto lastCharSize = &pStrSizeArr [lastStrPos.start + lastStrPos.length - 1];

		this->startY = GetStartPosY(totalHeight - nRowSpace, rcHeight);

		auto lastCharPos = CharPos {
			GetStartPosX(lastStrPos.width, rcWidth) - lastCharSize->width / 4
			+ lastStrPos.width + (lastCharSize->width >> 1)
			,startY + lastStrPos.y + (lastCharSize->height >> 1)
			,maxWidth
			,totalHeight - nRowSpace };

		return lastCharPos;
	}

	inline void RenderPerChar(LPRECT pRc) {
		if (!this->bTextValid) {
			return;
		}

		delete[] this->pCharPosArr;
		this->pCharPosArr = nullptr;

		pCharPosArr = new CharPos [pTextLen + 1];
		memset(pCharPosArr, 0, sizeof(CharPos) * (pTextLen + 1));

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

			pMemSf = CreateSurface(32, width, height);

			delete this->pBitmap;
			this->pBitmap = nullptr;

			this->pBitmap = new Bitmap(width, height, PixelFormat32bppARGB);
		}

		pMemSf->Fill(BLACK);
		pMemSf->CreateAlpha();

#ifdef _DEBUG
		auto type = pMemSf->GetType();
#endif

		Graphics g(pBitmap);
		
		g.Clear(Color(0, 0, 0, 0));

		g.SetTextRenderingHint(this->textRenderingHint);
		g.SetSmoothingMode(this->smoothingMode);
		g.SetPixelOffsetMode(this->pixelOffsetMode);

		//Color fontColor(255, 50, 150, 250);
		Color fontColor(255, GetRValue(this->dwTextColor), GetGValue(this->dwTextColor), GetBValue(this->dwTextColor));
		SolidBrush solidBrush(fontColor);

		RECT displayRc = { 0,0,(LONG)this->renderWidth, (LONG)this->renderHeight };

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

		size_t totalChar = 0;

		auto colorIt = this->colorFormat.begin();
		auto iConIt = this->iConFormat.begin();
		auto fontIt = this->FontFormat.begin();

		for (auto& curStrPos : this->strPos) {
#ifdef _DEBUG
			std::wstring str(pText + curStrPos.start, curStrPos.length);
#endif // _DEBUG

			StrSize* charSz = nullptr;

			int x = GetStartPosX(curStrPos.width, rcWidth);
			x -= pStrSizeArr [curStrPos.start].width / 8;

			for (size_t curChar = 0; curChar < curStrPos.length; curChar++, totalChar++) {
				auto offset = curStrPos.start + curChar;
				auto pCurChar = pText + offset;
				charSz = &pStrSizeArr [offset];

				pCharPosArr[offset] = CharPos{ x + pStrSizeArr[curStrPos.start].width / 8
											,this->startY + curStrPos.y
											,0,0 };

				if (!clip(x, (this->startY + curStrPos.y), charSz)) {
					auto positionX = (float)(x) + (float)(this->borderOffsetY);
					auto positionY = (float)(curStrPos.y) + (float)(this->borderOffsetY);
		
					if (colorIt!= this->colorFormat.end()
						&& totalChar >= colorIt->start) {
						solidBrush.SetColor(colorIt->color);
						
						colorIt++;
					}

					if (iConIt != this->iConFormat.end()
						&& totalChar >= iConIt->start) {
						iConIt->x = (size_t)positionX;
						iConIt->y = (size_t)positionY;

						iConIt++;
					}

					if (fontIt != this->FontFormat.end()
						&& totalChar >= fontIt->start) {
						this->pFont = GetFontPointerWithCache(fontIt->logFont);

						fontIt++;
					}

					//Gdiplus::FontStyle style = (Gdiplus::FontStyle)this->pFont->GetStyle();
					auto status = g.DrawString(pCurChar, 1, pFont
						, PointF(positionX, positionY), &solidBrush);
				}

				x += (charSz->width + nColSpace);
			}
		}

#ifdef _BLUR
		Blur blur;
		BlurParams bp = { 0 };
		RECT rc = { 0,0,pBitmap->GetWidth(),pBitmap->GetHeight() };

		bp.expandEdge = false;
		bp.radius = 5;

		blur.SetParameters(&bp);

		pBitmap->ApplyEffect(&blur, &rc);
#endif

#ifdef _DEBUG
		//CLSID pngClsid;
		//GetEncoderClsid(L"image/png", &pngClsid);
		//bitmap.Save(L"F:\\NeoStr.png", &pngClsid, NULL);
#endif // _DEBUG

		BitmapData bitmapData;
		auto bitmapRect = Rect(0, 0, width, height);
		auto lockBitsRet = pBitmap->LockBits(&bitmapRect, ImageLockMode::ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);
		unsigned int* pRawBitmap = (unsigned int*)bitmapData.Scan0;   // for easy access and indexing

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

		ReleaseSfCoef(pMemSf, sfCoef);

		pBitmap->UnlockBits(&bitmapData);
			
		auto iConSize = this->defaultCharSz;

		iConSize.width = int(iConSize.width * this->iConScale);
		iConSize.height = int(iConSize.height * this->iConScale);

		auto iConXOffset = int(this->iConOffsetX * this->defaultCharSz.width
			+ (this->defaultCharSz.width - iConSize.width) / 2
			+ this->defaultCharSz.width / 6.0);
		auto iConYOffset = int(this->iConOffsetY * this->defaultCharSz.width
			+ (this->defaultCharSz.height - iConSize.height) / 2
			+ (this->defaultCharSz.height - this->defaultCharSz.width)
			- this->tm.tmDescent /*- this->tm.tmExternalLeading*/);

		auto flags = this->iConResample
			? STRF_RESAMPLE | STRF_RESAMPLE_TRANSP | STRF_COPYALPHA
			: STRF_COPYALPHA;

		for (auto& it : this->iConFormat) {			
			LPSURFACE pSf = nullptr;			
			auto bFound = it.hImage != -1;

			if (bFound) {
				auto IConLibIt = this->iConLib.find(it.hImage);

				if (IConLibIt == this->iConLib.end()) {
					cSurface imageSurface;
					LockImageSurface(this->appli, it.hImage, imageSurface);

					auto pIConSf = CreateSurface(32, imageSurface.GetWidth(), imageSurface.GetHeight());
					//pIConSf->SetTransparentColor(imageSurface.GetTransparentColor());
					//_AddAlpha(pIConSf);

					imageSurface.Blit(*pIConSf);
					//imageSurface.Blit(*pIConSf, 0, 0, BMODE_OPAQUE, BOP_COPY, 0, BLTF_ANTIA | BLTF_COPYALPHA);

					UnlockImageSurface(imageSurface);

					this->iConLib[it.hImage] = pIConSf;

					pSf = pIConSf;
				}
				else {
					pSf = IConLibIt->second;
				}
			}
			else {
				if (this->pDefaultICon == nullptr) {
					LPSURFACE proto = nullptr;
					GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);

					this->pDefaultICon = new cSurface;
					this->pDefaultICon->Create(128, 128, proto);

					_AddAlpha(this->pDefaultICon,175);

					this->pDefaultICon->Fill(RGB(200, 200, 200));

					this->pDefaultICon->Line(0, 0, 127, 0);
					this->pDefaultICon->Line(0, 0, 0, 127);
					this->pDefaultICon->Line(127, 127, 127, 0);
					this->pDefaultICon->Line(127, 127, 0, 127);

					this->pDefaultICon->Line(0, 0, 127, 127);
					this->pDefaultICon->Line(127, 0, 0, 127);

#ifdef _DEBUG
					//_SavetoClipBoard(this->pDefaultICon, false);
#endif // _DEBUG
				}

				pSf = this->pDefaultICon;
			}

			auto ret = pSf->Stretch(*pMemSf
				, it.x + iConXOffset, it.y + iConYOffset
				, iConSize.width, iConSize.width
				, BMODE_OPAQUE, BOP_COPY, 0, flags);

#ifdef _DEBUG
			//_SavetoClipBoard(pSf, false);
			//_SavetoClipBoard(pMemSf, false);
#endif // _DEBUG		
		}

		delete pHwaSf;
		pHwaSf = nullptr;

		pHwaSf = CreateHWASurface(32, width, height, this->hwaType, this->hwaDriver);

		if (this->preMulAlpha) {
			pMemSf->PremultiplyAlpha();		// only needed in DX11 premultiplied mode
		}

		pMemSf->Blit(*pHwaSf);
	}

	inline CharPos GetCharPos(size_t pos) {
		auto invalid = CharPos { -1, -1, -1, -1 };

		if (pCharPosArr == nullptr) {
			return invalid;
		}

		size_t pTextLen = wcslen(pText);

		if (pTextLen == 0) {
			return invalid;
		}

		if (pTextLen <= pos) {
			return invalid;
		}

		return pCharPosArr [pos];
	}

	inline void DisplayPerChar(LPSURFACE pDst, LPRECT pRc
		, BlitMode bm = BMODE_TRANSP, BlitOp bo = BOP_COPY, LPARAM boParam = 0, int bAntiA = 0
		, DWORD dwLeftMargin = 0, DWORD dwRightMargin = 0, DWORD dwTabSize = 8) {
		if (!this->bTextValid) {
			return;
		}

		if (pDst != nullptr && pHwaSf != nullptr && pMemSf != nullptr) {
			auto pSf = pHwaSf;

			int xOffset = -this->borderOffsetX;
			int yOffset = -this->borderOffsetY + this->startY;

			POINT hotSpot = { this->hotSpotX - xOffset,this->hotSpotY - yOffset };

			int xPos = pRc->left + this->hotSpotX;
			int yPos = pRc->top + this->hotSpotY;

			pSf->BlitEx(*pDst, (float)xPos, (float)yPos, this->xScale, this->yScale
				, 0, 0, pSf->GetWidth(), pSf->GetHeight(), &hotSpot, (float)this->angle
				, bm, bo, boParam, bAntiA);

		}
	}
};

// compatible with MMF
#define Font FontW
#define fpFont fpFontW
