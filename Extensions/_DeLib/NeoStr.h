#pragma once

//#define _FONTEMBEDDEBUG
//#define _CONSOLE

#include <map>
#include <vector>
#include <functional>
#include <string_view>

#include <boost/regex.hpp>

using boost::wregex;
using boost::wsregex_token_iterator;
using boost::wsmatch;

using boost::regex_match;
using boost::regex_search;
using boost::regex_replace;

using RegexFlag = boost::regex_constants::syntax_option_type;
using boost::regex_constants::icase;
using boost::regex_constants::ECMAScript;
using boost::regex_constants::optimize;

//#include <FusionUtilities.h>
#include <RandGenerator.h>

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
constexpr auto EMPTY_CHAR = L' ';

constexpr auto DEFAULT_PARAM = L" ";

constexpr auto SHAKE_RANDOM_RANGE = 1000;

constexpr auto FORMAT_IGNORE_UNKNOWN = 0b00000001;
constexpr auto FORMAT_IGNORE_INCOMPLETE = 0b00000010;

constexpr auto FORMAT_IGNORE_DEFAULTFLAG = FORMAT_IGNORE_UNKNOWN | FORMAT_IGNORE_INCOMPLETE;
//constexpr auto FORMAT_IGNORE_DEFAULTFLAG = FORMAT_IGNORE_INCOMPLETE;

constexpr auto FORMAT_OPERATION_GetRawStringByFilteredStringLength = 0b00000001;

constexpr auto FORMAT_INVALID_ICON = (DWORD)-1;

//#define MEASURE_GDI_PLUS

class NeoStr {
private:
	HDC hdc;
	COLORREF dwTextColor;

	//HFONT hFont;
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
	size_t previousFlag = -1;

	LPWSTR pRawText = nullptr;
	LPWSTR pText = nullptr;

	wchar_t regstr[2] = { L'\0' };
	wregex* pCJK = nullptr;
	wregex* pEmpty = nullptr;

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
	wchar_t measureBaseStr[2] = { L'.',L'.' };

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

	bool bShake = false;

	unsigned short shakeTimer = 0;
	RandGenerator<int>* pShakeRandGen = nullptr;

	enum class ShakeType {
		ShakeType_None = 0,
		ShakeType_X,
		ShakeType_Y,
		ShakeType_Random,
	};

	struct ShakeControl {
		ShakeType shakeType = ShakeType::ShakeType_None;

		double amplitude = 1.0;
		// higher, faster
		double timerCoef = 1.0;
		// offset between characters
		// mul 360 = actual offset
		double charOffset = 1.0 / 6.0;
	};

	struct CharPos {
		long x = 0;
		long y = 0;
		long maxWidth = 0;
		long totalHeight = 0;

		// Shake
		ShakeControl shakeControl;
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
	//CharSizeCache charSzCache;

	struct CharSizeCacheItem {
		HDC hdc = NULL;
		HFONT hFont = NULL;
		TEXTMETRIC tm = {0};

		CharSizeCache cache;
	};

	bool bExternalCache = true;

	using LogFontHash = size_t;
public:
	using CharSizeCacheWithFont = std::map<LogFontHash, CharSizeCacheItem>;
private:
	CharSizeCacheWithFont* pCharSzCacheWithFont;

public:
	using FontCache = std::map<LogFontHash, Font*>;
private:
	FontCache* pFontCache;

	StrSize defaultCharSz;

	struct FormatBasic {
		size_t start;
		//size_t end;

		size_t startWithNewLine;
	};

	struct FormatColor {
		size_t start;
		//size_t end;

		size_t startWithNewLine;

		Color color;
	};

	std::vector<FormatColor> colorFormat;
	std::vector<Color> colorStack;

	struct FormatICon {
		size_t start;
		//size_t end;

		size_t startWithNewLine;

		DWORD hImage;

		size_t x;
		size_t y;
	};

	std::vector<FormatICon> iConFormat;

	struct IConDisplay {
		float iConOffsetX = 0;
		float iConOffsetY = 0;

		float iConScale = 1.0;

		bool iConResample = false;
	};

	IConDisplay iConDisplay = { 0,0,1.0,false };

	struct FormatIConDisplay {
		size_t start;
		//size_t end;

		size_t startWithNewLine;

		IConDisplay iConDisplay;
	};

	std::vector<FormatIConDisplay> iConDisplayFormat;
	std::vector<IConDisplay> iConDisplayStack;

	struct FormatFont {
		size_t start;
		//size_t end;

		size_t startWithNewLine;

		LOGFONT logFont;
	};

	std::vector<FormatFont> fontFormat;
	std::vector<LOGFONT> logFontStack;

	struct FormatShake {
		size_t start = 0;
		//size_t end;

		size_t startWithNewLine = 0;

		ShakeControl shakeControl;
	};

	std::vector<FormatShake> shakeFormat;
	std::vector<ShakeControl> shakeStack;

public:
	using IConLib = std::map<DWORD, LPSURFACE>;
	using ControlParams = std::vector<std::wstring_view>;
	using IConParamParser = std::function<DWORD(ControlParams&, IConLib&)>;

	struct IConData {
		IConLib* pIConLib = nullptr;
		LPRO pCaller = nullptr;
		LPRO pIConObject = nullptr;
		LPSURFACE pDefaultICon = nullptr;
		IConParamParser iconParamParser = nullptr;

		using ResizeCacheKeyHash = size_t;

		struct ResizeCacheKey {
			LPSURFACE pSrc = nullptr;
			int width = 0;
			int height = 0;

			inline ResizeCacheKeyHash GetHash() {
				return  std::hash<void*>()(pSrc) ^std::hash<int>()(width) ^ std::hash<int>()(height);
			}
		};		

		std::map<ResizeCacheKeyHash, LPSURFACE> resizeCache;
				
		IConData() {
			pIConLib = new IConLib;
		}

		~IConData() {
			ReleaseIConLib();

			delete pIConLib;
			pIConLib = nullptr;

			delete pDefaultICon;
			pDefaultICon = nullptr;
		}

		inline void ResetCaller() {
			this->pCaller = nullptr;

			this->pIConObject = nullptr;
			this->iconParamParser = nullptr;
		}

		inline bool CheckCallerValidity() {
			if (this->pCaller == nullptr || IsDestroyed((LPRO)this->pCaller)) {
				this->ResetCaller();

				return false;
			}

			return true;
		}

		// linked object changed, need to reset lib
		inline bool NeedUpdateICon(LPRO pObject) {
			return pIConObject != pObject;
		}

		// link to active -> pObject == LPRO
		// link to other object -> pObject == Callback Identifier
		// pIConObject == nullptr -> No ICon
		inline void UpdateICon(LPRO pObject, IConParamParser parser) {
			if (NeedUpdateICon(pObject)) {
				ReleaseIConLib();
			}			

			// always update callback, make sure the caller is valid
			this->pIConObject = pObject;
			this->iconParamParser = parser;
		}

		inline void ReleaseIConLib() {
			for (auto& it : *this->pIConLib) {
				delete it.second;
			}

			this->pIConLib->clear();

			for (auto& it : this->resizeCache) {
				delete it.second;
			}

			this->resizeCache.clear();
		}

		inline void GetDefaultICon() {
			if (this->pDefaultICon == nullptr) {
				LPSURFACE proto = nullptr;
				GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);

				this->pDefaultICon = new cSurface;
				this->pDefaultICon->Create(128, 128, proto);

				_AddAlpha(this->pDefaultICon, 175);

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
		}
	};

private:
	bool bExternalIConData = false;
	IConData* pIConData = nullptr;

	inline const auto GetIConData() {
		return pIConData;
	}

	//IConLib* pIConLib = nullptr;
	//LPRO pIConObject = nullptr;
	//LPSURFACE pDefaultICon = nullptr;
	//IConParamParser iconParamParser = nullptr;

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

	inline bool RegexCore(wregex* pRegex, wchar_t wChar) {
		regstr[0] = wChar;

		return regex_match(regstr, *pRegex);
	}

	inline bool NotCJK(wchar_t wChar) {
		return RegexCore(this->pCJK, wChar);
	}

	inline bool NotEmpty(wchar_t wChar) {
		return !RegexCore(this->pEmpty, wChar);
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
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
				*pClsid = pImageCodecInfo[j].Clsid;
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
		// read only
		, FontCache* pFontCache = nullptr
		, CharSizeCacheWithFont* pCharSzCacheWithFont = nullptr		
		, IConData* pIConData = nullptr
		, PrivateFontCollection* pFontCollection = nullptr
		, bool needGDIPStartUp = true
	) {
		this->needGDIPStartUp = needGDIPStartUp;

		if (this->needGDIPStartUp) {
			Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		}

		//this->hdc = GetDC(rdPtr->rHo.hoAdRunHeader->rhHEditWin);
		this->hdc = GetDC(NULL);
		SelectObject(this->hdc, hFont);

		//this->hFont = hFont;
		GetObject(hFont, sizeof(LOGFONT), &this->logFont);
		GetTextMetrics(hdc, &this->tm);

		// https://www.jianshu.com/p/fcbc5cd06f39
		this->pCJK = new wregex(L"[\\u2E80-\\uA4CF\\uF900-\\uFAFF\\uFE10-\\uFE1F\\uFE30-\\uFE4F\\uFF00-\\uFFEF]"
			, ECMAScript | optimize);
		this->pEmpty = new wregex(L"[\\s]"
			, ECMAScript | optimize);

		this->pFontCollection = pFontCollection;

		if (!pFontCache || !pCharSzCacheWithFont) {
			this->bExternalCache = false;
		}

		if (this->bExternalCache) {
			this->pFontCache = pFontCache;
		}
		else {
			Alloc(this->pFontCache);
		}

		this->pFont = GetFontPointerWithCache(this->logFont);

		this->SetColor(color);
		this->dwDTFlags = dwAlignFlags | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL;

		this->strPos.reserve(20);		
		
		this->iConFormat.reserve(DEFAULT_FORMAT_RESERVE);
		
		this->iConDisplayStack.reserve(DEFAULT_FORMAT_RESERVE);
		this->iConDisplayFormat.reserve(DEFAULT_FORMAT_RESERVE);

		this->colorStack.reserve(DEFAULT_FORMAT_RESERVE);
		this->colorFormat.reserve(DEFAULT_FORMAT_RESERVE);		

		this->fontFormat.reserve(DEFAULT_FORMAT_RESERVE);
		this->logFontStack.reserve(DEFAULT_FORMAT_RESERVE);

		this->shakeFormat.reserve(DEFAULT_FORMAT_RESERVE);
		this->shakeStack.reserve(DEFAULT_FORMAT_RESERVE);

		this->pShakeRandGen = new RandGenerator<int>(-1 * SHAKE_RANDOM_RANGE, SHAKE_RANDOM_RANGE);

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
		if (this->bExternalCache) {
			this->pCharSzCacheWithFont = pCharSzCacheWithFont;
		}
		else {
			Alloc(this->pCharSzCacheWithFont);
		}

		this->defaultCharSz = this->GetCharSizeWithCache(DEFAULT_CHARACTER, this->logFont);

		if (pIConData != nullptr) {
			bExternalIConData = true;
			this->pIConData = pIConData;
		}
		else {
			this->pIConData = new IConData;
		}
	}

	~NeoStr() {
		//SelectObject(this->hdc, (HFONT)NULL);
		ReleaseDC(NULL, this->hdc);

		delete this->pCJK;
		this->pCJK = nullptr;

		delete this->pEmpty;
		this->pEmpty = nullptr;

#ifdef MEASURE_GDI_PLUS		
		delete this->pMeasure;
		this->pMeasure = nullptr;
#endif

		if (!this->bExternalCache) {
			Release(this->pCharSzCacheWithFont);
			Release(this->pFontCache);
		}

		if (!bExternalIConData) {
			delete this->pIConData;
			this->pIConData = nullptr;
		}

		delete this->pShakeRandGen;
		this->pShakeRandGen = nullptr;

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

		if (this->needGDIPStartUp) {
			Gdiplus::GdiplusShutdown(gdiplusToken);
		}
	}

	inline static void Alloc(CharSizeCacheWithFont*& pCharSzCacheWithFont) {
		pCharSzCacheWithFont = new CharSizeCacheWithFont;
	}

	inline static void Alloc(FontCache*& pFontCache) {
		pFontCache = new FontCache;
	}

	inline static void Release(CharSizeCacheWithFont*& pCharSzCacheWithFont) {
		for (auto& it : *pCharSzCacheWithFont) {
			//SelectObject(it.second.hdc, (HFONT)NULL);
			ReleaseDC(NULL, it.second.hdc);
			DeleteObject(it.second.hFont);
		}

		delete pCharSzCacheWithFont;
		pCharSzCacheWithFont = nullptr;
	}

	inline static void Release(FontCache*& pFontCache) {
		for (auto& it : *pFontCache) {
			delete it.second;
		}

		delete pFontCache;
		pFontCache = nullptr;
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

		if (logFont.lfWeight >= FW_SEMIBOLD) {
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
		auto it = pFontCache->find(logFontHash);

		if (it != pFontCache->end()) {
			return it->second;
		}
		else {
			auto newFont = GetFontPointer(logFont);
			(*pFontCache)[logFontHash] = newFont;

			return newFont;
		}
	}

	// Color conversion
	// #AARRGGBB
	inline Color GetColor(std::wstring_view hex) {
		return GetColor(_h2d(hex.data(), hex.size()), true);
	}
	
	// Color conversion
	inline Color GetColor(DWORD color, bool bAlpha = false) {
		if (bAlpha) {
			auto A = (BYTE)((color >> 24) & 0xFF);
			auto R = (BYTE)((color >> 16) & 0xFF);
			auto G = (BYTE)((color >> 8) & 0xFF);
			auto B = (BYTE)((color) & 0xFF);

			return Color(A, R, G, B);
		}
		else {
			return Color(255, GetRValue(color), GetGValue(color), GetBValue(color));
		}
	}

	// Color conversion
	inline DWORD GetDWORD(Color color) {
		auto A = color.GetAlpha();
		auto R = color.GetRed();
		auto G = color.GetGreen();
		auto B = color.GetBlue();

		return (A << 24) | (R << 16) | (G << 8) | (B);
	}

	// Get display color
	//inline COLORREF GetColor() {
	//	return this->dwTextColor;
	//}

	inline StrSize GetDefaultCharSize() {
		return this->defaultCharSz;
	}

	inline bool GetShakeUpdateState() {
		return this->bShake;
	}

	inline ShakeType GetShakeTypeByName(std::wstring_view& param) {
		do {
			if (StringViewIEqu(param, L"None")) {
				return ShakeType::ShakeType_None;
			}

			if (StringViewIEqu(param, L"X")) {
				return ShakeType::ShakeType_X;
			}

			if (StringViewIEqu(param, L"Y")) {
				return ShakeType::ShakeType_Y;
			}

			if (StringViewIEqu(param, L"Random") || StringViewIEqu(param, L"R")) {
				return ShakeType::ShakeType_Random;
			}
		} while (0);

		return ShakeType::ShakeType_None;
	}

	inline void GetShakePosition(const ShakeControl& shakeControl, double timer, float& x, float& y, const StrSize* charSz) {
		auto t = RAD(timer);

		switch (shakeControl.shakeType)
		{
		case ShakeType::ShakeType_None: {
			break;
		}
		case ShakeType::ShakeType_X: {
			x = float(x + shakeControl.amplitude * charSz->width * sin(t));

			break;
		}
		case ShakeType::ShakeType_Y: {
			y = float(y + shakeControl.amplitude * charSz->height * sin(t));

			break;
		}
		case ShakeType::ShakeType_Random: {
			auto randomX = this->pShakeRandGen->GenerateRandNumber() / (1.0 * SHAKE_RANDOM_RANGE);
			auto randomY = this->pShakeRandGen->GenerateRandNumber() / (1.0 * SHAKE_RANDOM_RANGE);

			auto wa = shakeControl.amplitude * charSz->width;
			auto ha = shakeControl.amplitude * charSz->height;

			x = float(x + wa * randomX);
			y = float(y + ha * randomY);

			break;
		}
		default:
			break;
		}
	}

	inline void SetIConOffset(float iConOffsetX = 0, float iConOffsetY = 0) {
		this->iConDisplay.iConOffsetX = iConOffsetX;
		this->iConDisplay.iConOffsetY = iConOffsetY;
	}

	inline void SetIConScale(float iConScale = 1.0) {
		this->iConDisplay.iConScale = iConScale;
	}

	inline void SetIConResample(bool iConResample=false){
		this->iConDisplay.iConResample = iConResample;
	}

	inline void LinkObject(LPRO pObject, IConParamParser parser) {
		if (!this->bExternalIConData) {
			this->pIConData->UpdateICon(pObject, parser);
		}
	}

	inline void SetHWA(int type, int driver, bool preMulAlpha) {
		this->hwaType = type;
		this->hwaDriver = driver;
		this->preMulAlpha = preMulAlpha;
	}

	inline void SetColor(DWORD color) {
		auto bColorChanged = this->dwTextColor != color;

		if (bColorChanged && !this->colorFormat.empty()) {
			// no color format, update default
			if (this->colorFormat.size() == 1) {
				this->colorFormat.at(0).color = GetColor(color);
			}
			// has color format, update format
			else {
				this->GetFormat(this->pRawText, this->previousFlag, true);
			}
		}

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

//	// deprecated
//	inline StrSize GetCharSizeRaw(wchar_t wChar) {
//#ifdef MEASURE_GDI_PLUS		
//		SIZE sz = { 0 };
//		GetTextExtentPoint32(hdc, &wChar, 1, &sz);
//
//		RectF boundRect;
//
//		this->measureStr[1] = wChar;
//
//		this->pMeasure->MeasureString(this->measureStr, 3, pFont, origin, &stringFormat, &boundRect);
//
//		auto curWidth = long(boundRect.GetRight() - boundRect.GetLeft());
//		auto curHeight = long(boundRect.GetBottom() - boundRect.GetTop());		
//
//		sz.cx = curWidth - measureBaseSize.cx;
//		//sz.cy = curHeight;
//		sz.cy = this->measureBaseSize.cy;
//
//		sz.cy -= (this->tm.tmExternalLeading);
//#else
//		SIZE sz = { 0 };
//		GetTextExtentPoint32(hdc, &wChar, 1, &sz);
//#endif
//		//ABC abc;
//		//GetCharABCWidths(hdc, wChar, wChar, &abc);
//
//		//sz.cx -= this->tm
//		//sz.cy -= (this->tm.tmInternalLeading + this->tm.tmExternalLeading);
//
//		return *(StrSize*)&sz;
//	}
//	
//	// deprecated
//	inline StrSize GetCharSizeWithCache(wchar_t wChar) {
//		auto it = charSzCache.find(wChar);
//
//		if (it != charSzCache.end()) {
//			return it->second;
//		}
//		else {
//			auto sz = GetCharSizeRaw(wChar);
//			charSzCache [wChar] = sz;
//
//			return sz;
//		}
//	}	

	inline StrSize GetCharSizeRaw(wchar_t wChar, HDC hdc) {
		SIZE sz = { 0 };
		GetTextExtentPoint32(hdc, &wChar, 1, &sz);

		return *(StrSize*)&sz;
	}

	inline auto GetCharSizeCacheIt(LOGFONT logFont) {
		auto logFontHash = LogFontHasher(logFont);
		auto it = pCharSzCacheWithFont->find(logFontHash);

		return it;
	}

	inline StrSize GetCharSizeWithCache(wchar_t wChar, LOGFONT logFont) {
		auto logFontHash = LogFontHasher(logFont);
		auto it = pCharSzCacheWithFont->find(logFontHash);

		if (it != pCharSzCacheWithFont->end()) {
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

			TEXTMETRIC tm;
			GetTextMetrics(hdc, &tm);

			(*pCharSzCacheWithFont)[logFontHash] = { hdc,hFont,tm };

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

	inline const LPWSTR GetRawText() {
		return this->pRawText;
	}

	inline const LPWSTR GetText() {
		return this->pText;
	}

	inline void GetFormat(LPCWSTR pStr
		, size_t flags = FORMAT_IGNORE_DEFAULTFLAG
		, bool bForced = false		// force refresh
		, size_t operation = 0
		, size_t operationParam = 0) {
		this->bTextValid = true;

		if (pStr == nullptr) {
			this->bTextValid = false;

			return;
		}

		size_t pInputLen = wcslen(pStr);

		if (pInputLen == 0) {
			this->bTextValid = false;

			return;
		}

		if (operation != 0) {
			bForced = true;
		}

		if (!bForced 
			&& this->pRawText != nullptr && StrEqu(pStr, this->pRawText)
			&& flags == previousFlag) {
			return;
		}

		previousFlag = flags;

		// won't copy if this->pRawText is input
		if (this->pRawText != pStr) {
			delete[] this->pRawText;
			this->pRawText = nullptr;

			this->pRawText = new wchar_t[pInputLen + 1];
			memset(pRawText, 0, sizeof(wchar_t) * (pInputLen + 1));

			memcpy(pRawText, pStr, sizeof(wchar_t) * (pInputLen + 1));
		}

		delete[] this->pText;
		this->pText = nullptr;

		this->pText = new wchar_t[pInputLen + 1];
		memset(pText, 0, sizeof(wchar_t) * (pInputLen + 1));	

		// ---------------
		// Format Control
		// ---------------
		// 
		// /[ -> [
		//	escaped
		// 
		// [Command] if not match follows, will be displayed as untouched
		//	depend on your flag settings
		// 
		// [^]
		//	ignore all formats after this
		// 
		// [^-]
		//	ignore all formats except [ICon] after this
		//	icon controls like [IConOffsetX] are also ignored
		// 
		// [!^]
		//	stop ignore all formats
		// 
		// [!]
		//	reset all format to what you set in properties and events
		// 
		// [ICon = Direction, Frame]
		//	insert icon based on linked active.
		//	if param is less than two, will be referred from left.
		//	e.g., one param will be treated as frame.
		//	
		//	ICon will be resized to the font size it inserted at
		//	
		//	*if you are linking to object
		//	*you need to retrieve each param
		//	*then handle it by your self
		//	*you need to set a key for it for retrieving
		//	*and value as pointer to a surface
		// 
		// [IConOffsetX = 0.0][/IConOffsetX]
		//	ICon Offset X
		//	! = reset to default
		//	+/- = add/minus to current
		// 
		// [IConOffsetY = 0.0][/IConOffsetY]
		//	ICon Offset Y
		//	! = reset to default
		//	+/- = add/minus to current
		// 
		// [IConScale = 1.0][/IConScale]
		//	ICon Scale
		//	! = reset to default
		//	+/- = add/minus to current
		// 
		// [IConResample = 1][/IConResample]
		//	ICon Resample, 1 = Enable, 0 = Disable
		//	! = reset to default
		// 
		// [Shake = Type, Amplitude, TimerCoef, CharOffset]
		//	control shake.
		//	if param is less than four, will be referred from right.
		//	e.g., one param -> Type, two params -> Type & Amplitude, etc.
		// 
		//	type: shake type, default is None, can be X/Y/Random. X/Y is based on cosine.
		//	amplitude: default is 1.0, relative to character size. size = 30, amplitude = 0.5,
		//			   will shake in range of [ -15 ~ 15 ]
		//  timer coef: default is 1.0, shake faster when higher.
		//	char offset: default is 1.0 / 6.0, relative to 360 degree. charOffset = 0.2, actual offset is 72 degree
		//				 determines the movement interval of adjacent characters.
		//				 only works in X/Y mode ( cosine )
		// 
		// [Color = #FFFFFFFF][/Color]
		// [C = #FFFFFFFF][/C]
		//	color, hex AARRGGBB, or A, R, G, B
		//	if RGB param is less than four, then it's referred in the following order:
		//		R
		//		R G
		//		R G B
		//		A R G B
		//	! = reset to default
		//	+/- = add/minus to current
		// 
		// [Font = FontName][/Font]
		// [F = FontName][/F]
		//	! = reset to default
		// 
		// [Size = FontSize][/Size]
		// [S = FontSize][/S]
		//	! = reset to default
		//	+/- = add/minus to current
		// 
		// [Bold][/Bold]
		// [B][/B]
		// 
		// [!Bold][/!Bold]
		// [!B][/!B]
		//	set to non-bold
		// 
		// [Italic][/Italic]
		// [I][/I]
		// 
		// [!Italic][/!Italic]
		// [!I][/!I]
		//	set to non-italic
		// 
		// [Underline][/Underline]
		// [U][/U]
		// 
		// [!Underline][/!Underline]
		// [!U][/!U]
		//	set to non-underline
		// 
		// [StrikeOut][/StrikeOut]
		// [S][/S]
		// 
		// [!StrikeOut][/!StrikeOut]
		// [!S][/!S]
		//	set to non-strike out

		auto pCurChar = pRawText;
		auto pSavedChar = pText;

		ControlParams controlParams;
		controlParams.reserve(4);

		this->iConFormat.clear();

		this->iConDisplayStack.clear();
		this->iConDisplayStack.emplace_back(this->iConDisplay);

		this->iConDisplayFormat.clear();
		this->iConDisplayFormat.emplace_back(FormatIConDisplay{ 0,0,iConDisplayStack.back() });
		
		this->colorStack.clear();
		this->colorStack.emplace_back(GetColor(this->dwTextColor));

		this->colorFormat.clear();
		this->colorFormat.emplace_back(FormatColor{ 0,0,colorStack.back() });		

		this->logFontStack.clear();
		this->logFontStack.emplace_back(this->logFont);

		this->fontFormat.clear();
		this->fontFormat.emplace_back(FormatFont{ 0,0,logFontStack.back() });

		this->bShake = false;

		this->shakeStack.clear();
		this->shakeStack.emplace_back(ShakeControl());

		this->shakeFormat.clear();
		this->shakeFormat.emplace_back(FormatShake{ 0,0,shakeStack.back() });


		auto GetRawStringByFilteredStringLength = [&]() {
			auto offset = size_t(pSavedChar - pText);

			if ((operation & FORMAT_OPERATION_GetRawStringByFilteredStringLength)
				&& offset + 1 > operationParam) {
				pCurChar[0] = L'\0';
				
#ifdef _DEBUG
				pSavedChar[0] = L'@';
				pSavedChar[1] = L'\0';
#endif

				throw FORMAT_OPERATION_GetRawStringByFilteredStringLength;
			}
		};

		size_t newLineCount = 0;

		bool bIgnoreUnknown = flags & FORMAT_IGNORE_UNKNOWN;
		bool bIgnoreIncomplete = flags & FORMAT_IGNORE_INCOMPLETE;

		bool bIgnoreFormat = false;
		bool bIgnoreFormatExceptICon = false;

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
				if (bIgnoreIncomplete && bEndOfText) {					
					break;
				}

				if (!bEndOfText) {
					auto bEndOfRegion = (pCurChar + 1)[0] == L'/';

					// handle format
					bool bValidCommand = true;

					do {
						size_t equal = 0;

						while ((pCurChar + equal)[0] != L'=') {
							if (equal == end) {
								break;
							}

							equal++;
						}

						using ParamParserCallback = std::function<void(std::wstring_view&)>;

						// parse by ',', from right
						auto ParseParamCore = [](std::wstring_view& paramParser, ParamParserCallback callBack) {
							auto start = paramParser.find_last_of(L',');

							if (start == std::wstring::npos) {
								callBack(paramParser);

								return false;
							}
							else {
								auto paramStr = paramParser.substr(start + 1);
								paramParser = paramParser.substr(0, start);

								callBack(paramStr);

								return true;
							}
						};

						std::wstring_view controlStr = GetTrimmedStr(pCurChar + bEndOfRegion + 1, max(0, (long long)equal - bEndOfRegion - 1));
						std::wstring_view controlParam = !bEndOfRegion
							? GetTrimmedStr(pCurChar + equal + 1, max(0, (long long)end - (long long)equal - 1))
							: controlStr;

						if (controlParam.size() == 0) {
							controlParam = DEFAULT_PARAM;
						}

						size_t savedLength = pSavedChar - pText - newLineCount * 2;
						size_t savedLengthWithNewLine = pSavedChar - pText;

						if (StringViewIEqu(controlStr, L"^")) {
							bIgnoreFormat = true;
						}

						if (StringViewIEqu(controlStr, L"^-")) {
							bIgnoreFormat = true;
							bIgnoreFormatExceptICon = true;
						}

						if (StringViewIEqu(controlStr, L"!^")) {
							bIgnoreFormat = false;
							bIgnoreFormatExceptICon = false;
						}
						
						if (StringViewIEqu(controlStr, L"ICon")) {
							if (bIgnoreFormat && !bIgnoreFormatExceptICon) {
								break;
							}

							DWORD hImage = FORMAT_INVALID_ICON;

							do {
								if (this->pIConData->pIConObject == nullptr || this->pIConData->iconParamParser == nullptr) {
									break;
								}

								auto& paramParser = controlParam;
								controlParams.clear();

								do {

								} while (ParseParamCore(paramParser
									, [&controlParams](std::wstring_view& param) {
										controlParams.emplace_back(GetTrimmedStr(param));
									}));

								hImage = pIConData->iconParamParser(controlParams, *this->pIConData->pIConLib);
							} while (0);

							this->iConFormat.emplace_back(FormatICon{ savedLength
																		, savedLengthWithNewLine
																		, hImage });

							// space for icon
							pSavedChar[0] = DEFAULT_CHARACTER;
							pSavedChar++;

							break;
						}

#pragma region StackedFormat
						auto UpdateFormat = [&](auto& format, auto& content) {
								auto CopyFormat = [](FormatBasic* pFormat, auto& content) {
									auto pData = (std::remove_reference_t<decltype(content)>*)(pFormat + 1);
									memcpy(pData, &content, sizeof(content));
								};
								
								auto pLastFormat = (FormatBasic*)(&format.back());

								// if equal, replace last format
								if (savedLength == pLastFormat->start) {
									CopyFormat(pLastFormat, content);
								}
								else {
									std::remove_reference_t<decltype(format[0])> newFormat = format.front();
									auto pNewFormat = (FormatBasic*)(&newFormat);

									pNewFormat->start = savedLength;
									pNewFormat->startWithNewLine = savedLengthWithNewLine;		

									CopyFormat(pNewFormat, content);

									format.emplace_back(newFormat);
								}
							};

						// reset all
						if (StringViewIEqu(controlStr, L"!")) {
								if (bIgnoreFormat) {
									break;
								}

								auto Reset = [&](auto& stack, auto& format) {
									std::remove_reference_t<decltype(stack[0])> first = stack.front();
									
									stack.clear();
									stack.emplace_back(first);

									UpdateFormat(format, first);
								};

								Reset(shakeStack, shakeFormat);
								Reset(colorStack, colorFormat);
								Reset(logFontStack, fontFormat);

								break;
							}

						auto StackManager = [&](auto& stack, auto& format, auto callBack) {
								if (!bEndOfRegion) {
									std::remove_reference_t<decltype(stack[0])> newFormat = stack.back();

									callBack(newFormat);

									stack.emplace_back(newFormat);
								}
								else {
									// protect for default one
									if (stack.size() > 1) {
										stack.pop_back();
									}
								}

								UpdateFormat(format, stack.back());
							};

						// callback should calculate the diff size
						auto DiffManager = [&](auto oldValue, auto callBack) {
								bool bAdd = false;
								bool bMinus = false;

								if (controlParam.front() == L'+') {
									controlParam = controlParam.substr(1);
									bAdd = true;
								}

								if (controlParam.front() == L'-') {
									controlParam = controlParam.substr(1);
									bAdd = true;
									bMinus = true;
								}

								auto sizeDiff = callBack(controlParam);

								return !bAdd
									? sizeDiff
									: (bMinus ? -1 : +1) * sizeDiff + oldValue;
							};

						if (StringViewIEqu(controlStr, L"IConOffsetX")) {
								if (bIgnoreFormat) {
									break;
								}

								StackManager(iConDisplayStack, iConDisplayFormat, [&](IConDisplay& iConDisplay) {
									// Reset
									if (StringViewIEqu(controlParam, L"!")) {
										iConDisplay.iConOffsetX = this->iConDisplayStack.front().iConOffsetX;

										return;
									}

									iConDisplay.iConOffsetX = DiffManager(iConDisplay.iConOffsetX, [&](std::wstring_view& controlParam) {
										auto size = _stof(controlParam);
										return size;
										});
									});

								break;
							}

						if (StringViewIEqu(controlStr, L"IConOffsetY")) {
								if (bIgnoreFormat) {
									break;
								}

								StackManager(iConDisplayStack, iConDisplayFormat, [&](IConDisplay& iConDisplay) {
									// Reset
									if (StringViewIEqu(controlParam, L"!")) {
										iConDisplay.iConOffsetY = this->iConDisplayStack.front().iConOffsetY;

										return;
									}

									iConDisplay.iConOffsetY = DiffManager(iConDisplay.iConOffsetY, [&](std::wstring_view& controlParam) {
										auto size = _stof(controlParam);
										return size;
										});
									});

								break;
							}

						if (StringViewIEqu(controlStr, L"IConScale")) {
								if (bIgnoreFormat) {
									break;
								}

								StackManager(iConDisplayStack, iConDisplayFormat, [&](IConDisplay& iConDisplay) {
									// Reset
									if (StringViewIEqu(controlParam, L"!")) {
										iConDisplay.iConScale = this->iConDisplayStack.front().iConScale;

										return;
									}

									iConDisplay.iConScale = DiffManager(iConDisplay.iConScale, [&](std::wstring_view& controlParam) {
										auto size = _stof(controlParam);
										return size;
										});
									});

								break;
							}

						if (StringViewIEqu(controlStr, L"IConResample")) {
								if (bIgnoreFormat) {
									break;
								}

								StackManager(iConDisplayStack, iConDisplayFormat, [&](IConDisplay& iConDisplay) {
									// Reset
									if (StringViewIEqu(controlParam, L"!")) {
										iConDisplay.iConResample = this->iConDisplayStack.front().iConResample;

										return;
									}

									iConDisplay.iConResample = _stoi(controlParam);
									});

								break;
							}

						if (StringViewIEqu(controlStr, L"Shake")) {
								if (bIgnoreFormat) {
									break;
								}

								StackManager(shakeStack, shakeFormat, [&](ShakeControl& shakeControl) {
									auto& paramParser = controlParam;
									controlParams.clear();

									do {

									} while (ParseParamCore(paramParser
										, [&controlParams](std::wstring_view& param) {
											controlParams.emplace_back(GetTrimmedStr(param));
										}));

									do {
										if (!controlParams.empty()) {
											shakeControl.shakeType = GetShakeTypeByName(controlParams.back());
											controlParams.pop_back();
										}

										if (!controlParams.empty()) {
											shakeControl.amplitude = _stod(controlParams.back());
											controlParams.pop_back();
										}

										if (!controlParams.empty()) {
											shakeControl.timerCoef = _stod(controlParams.back());
											controlParams.pop_back();
										}

										if (!controlParams.empty()) {
											shakeControl.charOffset = _stod(controlParams.back());
											controlParams.pop_back();
										}
									} while (0);

									if (shakeControl.shakeType != ShakeType::ShakeType_None) {
										this->bShake = true;
									}

									shakeControl.charOffset *= 360;
									});

								break;
							}

						if (StringViewIEqu(controlStr, L"Color")
							|| StringViewIEqu(controlStr, L"C")) {
								if (bIgnoreFormat) {
									break;
								}

								StackManager(colorStack, colorFormat, [&](Color& color) {
									// Reset
									if (StringViewIEqu(controlParam, L"!")) {
										color = colorStack.front();

										return;
									}

									auto result = DiffManager(GetDWORD(colorStack.back()), [&](std::wstring_view& controlParam) {
										// hex
										if ((controlParam[0] == L'#') 
											|| (controlParam[0] == L'0' 
												&& (controlParam[1] == L'x' || controlParam[1] == L'X'))) {
											return _h2d(controlParam.data(), controlParam.size());
										}
										// dec
										else {
											auto& paramParser = controlParam;
											controlParams.clear();

											do {

											} while (ParseParamCore(paramParser
												, [&controlParams](std::wstring_view& param) {
													controlParams.emplace_back(GetTrimmedStr(param));
												}));

											DWORD result = 0;
																					
											auto size = controlParams.size();
											auto alpha = 0xFF;

											if (size == 4) {
												alpha = _stoi(controlParams.back());
												controlParams.pop_back();
											}
											
											// R
											// R G
											// R G B
											// A R G B
											for (auto i = 0; i < 3; i++) {
												result = result << 8;
												
												if (!controlParams.empty()) {
													result |= _stoi(controlParams.back());

													controlParams.pop_back();
												}
											}	

											result |= alpha << 24;

											return result;
										}										
										});

									color = GetColor(result, true);
									});

								break;
							}

#pragma region FontControl
						using FontFormatControlCallback = std::function<void(LOGFONT& logFont)>;

						auto FontFormatControl = [&](FontFormatControlCallback callBack) {
							StackManager(logFontStack, fontFormat, callBack);
						};

						if (StringViewIEqu(controlStr, L"Font")
							|| StringViewIEqu(controlStr, L"F")) {
								if (bIgnoreFormat) {
									break;
								}

								FontFormatControl([&](LOGFONT& newLogFont) {
									// Reset
									if (StringViewIEqu(controlParam, L"!")) {
										newLogFont.lfHeight = this->logFontStack.front().lfHeight;

										return;
									}

									memset(newLogFont.lfFaceName, 0
										, LF_FACESIZE * sizeof(WCHAR));
									memcpy(newLogFont.lfFaceName, controlParam.data()
										, min(LF_FACESIZE, controlParam.size()) * sizeof(WCHAR));
									});

								break;
							}

						if (StringViewIEqu(controlStr, L"Size")
							|| StringViewIEqu(controlStr, L"S")) {
								if (bIgnoreFormat) {
									break;
								}

								FontFormatControl([&](LOGFONT& newLogFont) {									
									// Reset
									if (StringViewIEqu(controlParam, L"!")) {
										newLogFont.lfHeight = this->logFontStack.front().lfHeight;

										return;
									}

									newLogFont.lfHeight = DiffManager(newLogFont.lfHeight, [&](std::wstring_view& controlParam) {
										auto size = _stoi(controlParam);
										auto newSize = -1 * MulDiv(size
											, GetDeviceCaps(this->hdc, LOGPIXELSY)
											, 72);

										return newSize;
										});
										});

								break;
							}

						if (StringViewIEqu(controlStr, L"Bold")
							|| StringViewIEqu(controlStr, L"B")) {
								if (bIgnoreFormat) {
									break;
								}

								FontFormatControl([&](LOGFONT& newLogFont) {
									newLogFont.lfWeight = FW_BOLD;
									});								

								break;
							}

						if (StringViewIEqu(controlStr, L"!Bold")
							|| StringViewIEqu(controlStr, L"!B")) {
								if (bIgnoreFormat) {
									break;
								}

								FontFormatControl([&](LOGFONT& newLogFont) {
									newLogFont.lfWeight = FW_NORMAL;
									});

								break;
							}

						if (StringViewIEqu(controlStr, L"Italic")
							|| StringViewIEqu(controlStr, L"I")) {
								if (bIgnoreFormat) {
									break;
								}

								FontFormatControl([&](LOGFONT& newLogFont) {
									newLogFont.lfItalic = TRUE;
									});

								break;
							}

						if (StringViewIEqu(controlStr, L"!Italic")
							|| StringViewIEqu(controlStr, L"!I")) {
								if (bIgnoreFormat) {
									break;
								}

								FontFormatControl([&](LOGFONT& newLogFont) {
									newLogFont.lfItalic = FALSE;
									});

								break;
							}

						if (StringViewIEqu(controlStr, L"Underline")
							|| StringViewIEqu(controlStr, L"U")) {
								if (bIgnoreFormat) {
									break;
								}

								FontFormatControl([&](LOGFONT& newLogFont) {
									newLogFont.lfUnderline = TRUE;
									});

								break;
							}

						if (StringViewIEqu(controlStr, L"!Underline")
							|| StringViewIEqu(controlStr, L"!U")) {
								if (bIgnoreFormat) {
									break;
								}

								FontFormatControl([&](LOGFONT& newLogFont) {
									newLogFont.lfUnderline = FALSE;
									});

								break;
							}

						if (StringViewIEqu(controlStr, L"StrikeOut")
							|| StringViewIEqu(controlStr, L"S")) {
								if (bIgnoreFormat) {
									break;
								}

								FontFormatControl([&](LOGFONT& newLogFont) {
									newLogFont.lfStrikeOut = TRUE;
									});

								break;
							}

						if (StringViewIEqu(controlStr, L"!StrikeOut")
							|| StringViewIEqu(controlStr, L"!S")) {
								if (bIgnoreFormat) {
									break;
								}

								FontFormatControl([&](LOGFONT& newLogFont) {
									newLogFont.lfStrikeOut = FALSE;
									});

								break;
							}
#pragma endregion
#pragma endregion

						bValidCommand = false;
					} while (0);

					// copy invalid command
					if (!bIgnoreUnknown && !bValidCommand) {
						memcpy(pSavedChar, pCurChar, sizeof(wchar_t)* (end + 1));
						pSavedChar += end + 1;
					}

					// include ']'
					pCurChar += end + 1;

					GetRawStringByFilteredStringLength();
					
					continue;
				}
			}

			pSavedChar[0] = pCurChar[0];
			
			pCurChar++;
			pSavedChar++;
	  
			GetRawStringByFilteredStringLength();
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
			return { this->defaultCharSz.width / 2,this->defaultCharSz.height / 2 };
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

		bool bInWord = false;
		size_t notCJKStart = -1;
		size_t notCJKStartWidth = -1;

		size_t notAtStartCharPos = -1;
		bool bPunctuationSkip = false;

		auto fontIt = this->fontFormat.begin();
		auto localLogFont = fontIt->logFont;

		for (size_t pChar = 0; pChar < pTextLen; ) {
			bool bNewLineBegin = true;

			bool newLine = false;		// newline
			bool skipLine = false;		// current line only has /r/n
			bool bNewLineHandled = false;

			long curWidth = 0;
			long curHeight = 0;

			pCharStart = pChar;

			// ignore all empty contents followed in the end of line
			auto IsNewLine = [](const wchar_t& cur, const wchar_t& next) {
				return (cur == L'\r' && next == L'\n');
			};
			auto EscapeEmpty = [&]() {
				while (true) {
					auto curChar = (pText + pChar)[0];
					auto nextChar = (pText + pChar)[1];

					if (curChar == L'\0') {
						break;
					}

					if (NotEmpty(curChar)) {
						break;
					}

					if (IsNewLine(curChar, nextChar)) {
						pChar += 2;

						break;
					}

					pChar += 1;
				}
			};

			while (true) {
				auto pCurChar = pText + pChar;

				auto curChar = pCurChar [0];
				auto nextChar = pCurChar [1];

				if (fontIt != this->fontFormat.end()
					&& pChar >= fontIt->startWithNewLine) {
					localLogFont = fontIt->logFont;

					fontIt++;
				}

				// word break
				auto bCurNotCJK = NotCJK(curChar);

				auto InWord = [&]() {
					bInWord = true;

					notCJKStart = pChar;
					notCJKStartWidth = curWidth;
				};

				auto OutWord = [&]() {
					bInWord = false;

					//notCJKStart = -1;
					//notCJKStartWidth = -1;
				};

				auto WB_AbleToNextLine = [&]() {return (curWidth - notCJKStartWidth) < (size_t)rcWidth; };
				auto WB_Backword = [&]() {
					pChar = notCJKStart;
					curWidth = notCJKStartWidth;

					OutWord();
				};

				if (bCurNotCJK && !bInWord) {
					InWord();
				}

				if (!bCurNotCJK && bInWord) {
					OutWord();
				}

				// size
				pStrSizeArr[pChar] = GetCharSizeWithCache(curChar, localLogFont);
				auto charSz = &pStrSizeArr [pChar];

				curWidth += charSz->width;
				curHeight = max(curHeight, charSz->height);

				bNewLineHandled = false;

				// handle new line state and add pointer
				auto HandleNewLine = [&]() {
					newLine = true;
					skipLine = (pChar == pCharStart);
					bNewLineHandled = true;

					if ((notAtStartCharPos + 1) == pChar) {
						bPunctuationSkip = true;
					}

					pChar += 2;
				};

				if (curWidth > rcWidth) {
					//bool bTooNarrow = this->defaultCharSz.width > rcWidth;
					bool bTooNarrow = charSz->width > rcWidth;

					if (bTooNarrow) {
						if (IsNewLine(curChar, nextChar)) {
							HandleNewLine();

							break;
						}
						else {
							pChar += 1;

							break;
						}
					}

					// word break
					if (bInWord) {
						// merge
						auto bMerge = false;

						//if (nextChar != L'\0') {
						//	auto nextNextChar = pCurChar[2];
						//	bMerge = NotCJK(nextChar) && !NotCJK(nextNextChar);
						//}

						if (bMerge) {
							pChar += 2;

							break;
						}
						// next line
						else if (WB_AbleToNextLine()) {
							WB_Backword();

							break;
						}
						// too narrow, do nothing
						else {

						}
					}

					// punctuation
					auto pPreviousChar = pText + pChar - 1;
					auto PreviousChar = pPreviousChar[0];

					auto previousCharSz = &pStrSizeArr[pChar - 1];

					auto Punc_Backward = [&]() {
						if (NotCJK(PreviousChar)) {
							if (WB_AbleToNextLine()) {
								WB_Backword();
							}

							return;
						}

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
								Punc_Backward();
							}

							break;
						}
					}
					else {
						if (pChar != pCharStart) {
							if (NotAtEnd(PreviousChar)) {
								Punc_Backward();
							}
						}

						break;
					}
				}

				if (IsNewLine(curChar, nextChar)) {
					HandleNewLine();

					break;
				}

				pChar++;

				if (pChar > pTextLen) {
					break;
				}

				curWidth += nColSpace;
			}

			if (!skipLine) {
#ifdef _DEBUG
				std::wstring str(pText + pChar);
#endif // _DEBUG

				if (!bNewLineHandled) {
					EscapeEmpty();
				}

#ifdef _DEBUG
				std::wstring strEscaped(pText + pChar);
#endif // _DEBUG

				auto end = min(pChar, pTextLen) - 2 * newLine;
				
				if (end <= pCharStart) {
					end = pCharStart;
				}

				// totalWidth not used
				totalWidth = curWidth;

				this->strPos.emplace_back(StrPos {
					pCharStart,			// start
					end,				// end
					end - pCharStart,	// length				
					curWidth,			// width
					curHeight,			// height;
					0,					// x
					totalHeight,		// y
					});

#ifdef _DEBUG
				std::wstring strLine(pText + pCharStart, end - pCharStart);
#endif // _DEBUG

				maxWidth = curWidth != 0
					? max(maxWidth, curWidth - nColSpace)
					: maxWidth;
			}

			if (!bPunctuationSkip) {
				// empty line (skipLine == true) also need to add height
				// ...unless it's following a 'not at start' punctuation
				totalHeight += (curHeight + nRowSpace);
			}
			else {
				bPunctuationSkip = false;
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
		auto fontIt = this->fontFormat.begin();

		auto shakeIt = this->shakeFormat.begin();
		auto localShakeFormat = this->bShake
			? *shakeIt
			: FormatShake();
		
		if (this->bShake) {
			shakeTimer++;
		}

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
				charSz = &pStrSizeArr[offset];

				pCharPosArr[offset] = CharPos{ x + pStrSizeArr[curStrPos.start].width / 8
											,this->startY + curStrPos.y
											,0,0 };

#pragma region FORMAT_IT				
				auto positionX = (float)(x) + (float)(this->borderOffsetY);
				auto positionY = (float)(curStrPos.y) + (float)(this->borderOffsetY);

				if (colorIt != this->colorFormat.end()
					&& totalChar >= colorIt->start) {
					solidBrush.SetColor(colorIt->color);

					colorIt++;
				}

				if (fontIt != this->fontFormat.end()
					&& totalChar >= fontIt->start) {
					this->pFont = GetFontPointerWithCache(fontIt->logFont);

					fontIt++;
				}

				if (shakeIt != this->shakeFormat.end()
					&& totalChar >= shakeIt->start) {
					localShakeFormat = *shakeIt;

					shakeIt++;
				}

				if (this->bShake) {
					double offset = (totalChar - localShakeFormat.start) * localShakeFormat.shakeControl.charOffset
						+ this->shakeTimer * localShakeFormat.shakeControl.timerCoef;

					GetShakePosition(localShakeFormat.shakeControl, offset, positionX, positionY, charSz);
				}

				// use updated position
				if (iConIt != this->iConFormat.end()
					&& totalChar >= iConIt->start) {
					iConIt->x = (size_t)positionX;
					iConIt->y = (size_t)positionY;

					iConIt++;
				}
#pragma endregion

				//if (!clip(x, (this->startY + curStrPos.y), charSz)) {
				if (!clip((int)positionX - this->borderOffsetX, (this->startY + (int)positionY - this->borderOffsetY), charSz)) {
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
		//pBitmap->Save(L"F:\\NeoStr.png", &pngClsid, NULL);
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

		auto flags = this->iConDisplay.iConResample
			? STRF_RESAMPLE | STRF_RESAMPLE_TRANSP
			: 0;

		flags |= STRF_COPYALPHA;

		fontIt = this->fontFormat.begin();
		auto iConDisplayIt = this->iConDisplayFormat.begin();

		auto UpdateIt = [](auto& it,auto itEnd, size_t start) {
			while (it != itEnd && it->start <= start) {
				it++;
			}

			it--;
		};

		for (auto& it : this->iConFormat) {			
			UpdateIt(fontIt, this->fontFormat.end(), it.start);
			UpdateIt(iConDisplayIt, this->iConDisplayFormat.end(), it.start);

			bool bEnd = fontIt == this->fontFormat.end();

			const auto& charSize = !bEnd
				? this->GetCharSizeWithCache(DEFAULT_CHARACTER, fontIt->logFont)
				: this->defaultCharSz;
			const auto& tm = !bEnd
				? GetCharSizeCacheIt(fontIt->logFont)->second.tm
				: this->tm;

			const auto& iConDisplay = iConDisplayIt != this->iConDisplayFormat.end()
				? iConDisplayIt->iConDisplay
				: this->iConDisplay;

			StrSize iConSize = charSize;

			iConSize.width = int(iConSize.width * iConDisplay.iConScale);
			iConSize.height = int(iConSize.height * iConDisplay.iConScale);

			auto iConXOffset = int(iConDisplay.iConOffsetX * charSize.width
				+ (static_cast<double>(charSize.width) - iConSize.width) / 2
				+ charSize.width / 6.0);
			auto iConYOffset = int(iConDisplay.iConOffsetY * charSize.width
				+ (static_cast<double>(charSize.height) - iConSize.height) / 2
				+ (charSize.height - charSize.width)
				- tm.tmDescent /*- tm.tmExternalLeading*/);

			LPSURFACE pSf = nullptr;			
			auto bFound = it.hImage != FORMAT_INVALID_ICON;

			if (bFound) {
				auto IConLibIt = this->pIConData->pIConLib->find(it.hImage);
				pSf = IConLibIt->second;
			}
			else {
				this->pIConData->GetDefaultICon();
				pSf = this->pIConData->pDefaultICon;
			}

			LPSURFACE pStrecthSf = nullptr;

			auto key = IConData::ResizeCacheKey{ pSf , iConSize.width, iConSize.width }.GetHash();
			auto resizeCacheIt = this->pIConData->resizeCache.find(key);

			if (resizeCacheIt == this->pIConData->resizeCache.end()) {
				pStrecthSf = CreateSurface(32, iConSize.width, iConSize.width);
				pStrecthSf->CreateAlpha();

				auto ret = pSf->Stretch(*pStrecthSf, flags);

				this->pIConData->resizeCache[key] = pStrecthSf;
			}
			else {
				pStrecthSf = resizeCacheIt->second;
			}
			
			auto flags2 = this->iConDisplay.iConResample
				? BLTF_ANTIA
				: 0;

			pStrecthSf->Blit(*pMemSf
				, it.x + iConXOffset, it.y + iConYOffset
				, BMODE_TRANSP, BOP_COPY, 0, flags2);

			MixAlpha(pStrecthSf, pMemSf, it.x + iConXOffset, it.y + iConYOffset);

			//auto ret = pSf->Stretch(*pMemSf
			//	, it.x + iConXOffset, it.y + iConYOffset
			//	, iConSize.width, iConSize.width
			//	, BMODE_OPAQUE, BOP_COPY, 0, flags);

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
