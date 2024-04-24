// ReSharper disable CppClangTidyClangDiagnosticShadow
// ReSharper disable CppTooWideScopeInitStatement

#pragma once

//#define _FONTEMBEDDEBUG
//#define _CONSOLE

#include <map>
#include <vector>
#include <functional>
#include <string_view>

//Boost
#define BOOST
#include "RegexHelper.h"

//#include <FusionUtilities.h>
#include <RandGenerator.h>

#ifdef _DEBUG		
#include <assert.h>
#endif

// compatible with MMF
#pragma push_macro("Font")
#pragma push_macro("fpFont")

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
#include <ranges>

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

inline RECT operator+(const RECT& rA, const RECT& rB) {
	return RECT { rA.left + rB.left
				,rA.top + rB.top
				,rA.right + rB.right
				,rA.bottom + rB.bottom };
}

constexpr auto CHAR_SPACE = L' ';
constexpr auto CHAR_EMSPACE = L'　';
constexpr auto CHAR_TAB = L'\t';

constexpr auto DEFAULT_EBORDER_OFFSET = 20;
constexpr auto DEFAULT_FORMAT_RESERVE = 10;

constexpr auto DEFAULT_CHARACTER = CHAR_EMSPACE;
constexpr auto EMPTY_CHAR = CHAR_SPACE;

constexpr auto DEFAULT_PARAM = L" ";

constexpr auto SHAKE_RANDOM_RANGE = 1000;

// ------------
// Format flag
// ------------
constexpr auto FORMAT_IGNORE_UNKNOWN = 0b00000001;
constexpr auto FORMAT_IGNORE_INCOMPLETE = 0b00000010;
// text is still valid if only empty char left (aka iCon placeholder only)
constexpr auto FORMAT_IGNORE_AllowEmptyCharStringAfterFormatParsing = 0b00000100;

//constexpr auto FORMAT_IGNORE_DEFAULTFLAG = FORMAT_IGNORE_INCOMPLETE;
constexpr auto FORMAT_IGNORE_DEFAULTFLAG = FORMAT_IGNORE_UNKNOWN | FORMAT_IGNORE_INCOMPLETE;

// ------------
// Format operation
// ------------
// will stop parsing and use exception to return
constexpr auto FORMAT_OPERATION_GetRawStringByFilteredStringLength = 0b00000001;

constexpr auto FORMAT_INVALID_ICON = static_cast<DWORD>(-1);

struct NeoStrFormatException final :std::exception {
private:
	int flag = 0;

public:
	NeoStrFormatException(char const* const pMsg, int flag = 0) noexcept
		:std::exception(pMsg) {
		this->flag = flag;
	}

	inline int GetFlag() const noexcept { return this->flag; }
};

// D3D limitation
// https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-limits
constexpr long D3D11_TEXTURE_SIZE = 16384;

//#define MEASURE_GDI_PLUS

class NeoStr {
private:
	HDC hdc;
	HGDIOBJ hOldObj;
	DWORD dwTextColor;

	HFONT hFont;
	LOGFONT logFont;

	Font* pFont = nullptr;

	TEXTMETRIC tm;
	int nRowSpace = 0;
	int nColSpace = 0;

	DWORD dwDTFlags;

	// add rectangle on each char, for debug
	bool bDrawRectangle = false;

	unsigned short borderOffsetX = 0;
	unsigned short borderOffsetY = 0;

	bool bVerticalAlignOffset;

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
	bool bPreviousForced = false;
	bool bFormatUpdated = false;
	RECT previousRC = {};

	size_t pTextLen = 0;
	size_t previousFlag = -1;

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

	struct CharSize {
		long width = 0;
		long height = 0;
	};

	CharSize* pCharSizeArr = nullptr;

	bool bShake = false;
	bool bChildShake = false;

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

	// original position of character (not affected by shake), relative to non-border offset start
	// CharPos must be calculated in Render as align command may change position
	CharPos* pCharPosArr = nullptr;
	CharPos previousCharPos = {};

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

	float angle = 0;

	int hwaDriver = 0;
	bool preMulAlpha = false;

	LPSURFACE pHwaSf = nullptr;

	std::wstring notAtStart = L"!%),.:;>?]}¢¨°·ˇˉ―‖’”…‰′″›℃∶、。〃〉》」』】〕〗〞︶︺︾﹀﹄﹚﹜﹞！＂％＇），．：；？］｀｜｝～￠";
	std::wstring notAtEnd = L"$([{£¥·‘“〈《「『【〔〖〝﹙﹛﹝＄（．［｛￡￥";

	using CharSizeCache = std::map<wchar_t, CharSize>;

	struct CharSizeCacheItem {
		HDC hdc = nullptr;
		HGDIOBJ hOldObj = nullptr;
		HFONT hFont = nullptr;
		TEXTMETRIC tm = {0};

		CharSizeCache cache;

		CharSizeCacheItem(CharSizeCacheItem& other) = delete;
		CharSizeCacheItem(CharSizeCacheItem&& other) = delete;
		CharSizeCacheItem& operator=(CharSizeCacheItem& other) = delete;
		CharSizeCacheItem& operator=(CharSizeCacheItem&& other) = delete;

		explicit CharSizeCacheItem(const LOGFONT& logFont) {
			hFont = CreateFontIndirect(&logFont);

			hdc = GetDC(nullptr);
			hOldObj = SelectObject(hdc, hFont);
			GetTextMetrics(hdc, &tm);
		}

		~CharSizeCacheItem() {
			SelectObject(hdc, hOldObj);
			ReleaseDC(nullptr, hdc);
			DeleteObject(hFont);
		}
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

	CharSize defaultCharSz;

	//------------
	// format zone
	//------------
public:
	using ControlParams = std::vector<std::wstring_view>;

private:
	//---------
	// basic
	//---------
	struct FormatBasic {
		size_t start;
		//size_t end;

		size_t startWithNewLine;

		// copy POD format except the basic part
		inline void CopyFormat(auto& content) {
			auto pData = reinterpret_cast<std::remove_reference_t<decltype(content)>*>(this + 1);
			memcpy(pData, &content, sizeof(content));
		}
	};

	//---------
	// non-stack based
	//			format is saved directly
	//---------

	// for trigger
	struct FormatTrigger :FormatBasic {
		// ------
		// updated during parsing
		// ------
		size_t triggerLength = 0;
		std::wstring triggerName;
		std::wstring content;

		// ------
		// updated during rendering
		// ------
		CharSize* pCharSizeArr = nullptr;
		CharPos* pCharPosArr = nullptr;

		FormatTrigger(size_t start, size_t startWithNewLine,
			const std::wstring& trigger) :FormatBasic{ start, startWithNewLine } {
			this->triggerName = trigger;
		}
		FormatTrigger(size_t start, size_t startWithNewLine,
			const std::wstring& trigger, const std::wstring& content)
		:FormatTrigger{ start, startWithNewLine,trigger } {
			this->content = content;
		}
	};

	std::vector<FormatTrigger> triggerFormat;

	using TriggerRect = std::pair<size_t, RECT>;
	std::vector<TriggerRect> triggerRect;
public:
	inline void GetTriggerRect() {
		triggerRect.clear();
		std::vector<size_t> validIndex;

		for (size_t triggerIdx = 0; triggerIdx < triggerFormat.size(); triggerIdx++) {
			const auto& trigger = triggerFormat[triggerIdx];
			validIndex.clear();

			auto GetChar = [&] (size_t index) {
				return pText[trigger.startWithNewLine + index];
				};
			auto ChangeLine = [] (WCHAR curChar) {
				return curChar == L'\r' || curChar == L'\n';
				};

			for (size_t idx = 0; idx < trigger.triggerLength; idx++) {			
				const auto curChar = GetChar(idx);
				if (ChangeLine(curChar)) { continue; }

				validIndex.emplace_back(idx);
			}

			long previousCharX = MIN_LONG;

			long minY = MAX_LONG;
			long maxY = -1;

			while (!validIndex.empty()) {
				for (size_t index = 0; index < validIndex.size(); index++) {
					const auto idx = validIndex[index];
					const auto& charPos = trigger.pCharPosArr[idx];
					const auto& charSize = trigger.pCharSizeArr[idx];
					const auto curChar = GetChar(idx);

					auto UpdateSize = [&] () {
						minY = (std::min)(minY, charPos.y);
						maxY = (std::max)(maxY, charPos.y + charSize.height);
					};

					if (previousCharX >= charPos.x ) {
						const auto& startCharPos = trigger.pCharPosArr[validIndex.front()];

						const auto lastCharIndex = validIndex[index == 0
							? validIndex.size() - 1
							: index - 1];
						const auto& lastCharPos = trigger.pCharPosArr[lastCharIndex];
						const auto& lastCharSize = trigger.pCharSizeArr[lastCharIndex];

						if (minY == MAX_LONG && maxY == -1) { UpdateSize(); }

						triggerRect.emplace_back(triggerIdx,
							RECT{ startCharPos.x, minY,
								lastCharPos.x + lastCharSize.width, maxY });

						minY = MAX_LONG;
						maxY = -1;

						const auto start = validIndex.begin();
						const auto end = index == 0
							? validIndex.end()
							: validIndex.begin() + (index - 1);

						if(start == end) {
							validIndex.erase(start);
						}
						else {
							validIndex.erase(start, end);
						}

						break;
					}

					UpdateSize();
					previousCharX = charPos.x;
				}
			}		
		}
	}

	inline size_t OverlapTrigger(int x, int y, const wchar_t* pTriggerName = nullptr) {
		const auto it = std::ranges::find_if(triggerRect, [&] (const TriggerRect& triggerRect) {
			const auto& [triggerIdx, rect] = triggerRect;
			const bool bInside = x > rect.left && x< rect.right
				&& y>rect.top && y < rect.bottom;
			const bool bNameMatch = pTriggerName != nullptr
				? StrEqu(pTriggerName, triggerFormat[triggerIdx].triggerName.c_str())
				: true;

			return bInside && bNameMatch;
		});

		if (it == triggerRect.end()) { return static_cast<size_t>(-1); }

		return it->first;
	}

	// no protection, you should use the index return from OverlapTrigger
	inline const FormatTrigger& GetCurrentTrigger(size_t index) {
		return this->triggerFormat[index];
	}

private:

	// for tag
	struct FormatTag :FormatBasic {
		// ------
		// updated during parsing
		// ------
		size_t rawStart;
		std::wstring callbackName;
		std::vector<std::wstring> callbackParams;

		// ------
		// updated during rendering
		// ------

		FormatTag(size_t start, size_t startWithNewLine,
			size_t rawStart, const ControlParams& controlParams) :FormatBasic{ start, startWithNewLine } {
			this->rawStart = rawStart;

			auto it = controlParams.begin();
			this->callbackName = { it->data(),it->size() };
			++it;

			for (; it != controlParams.end(); ++it) {
				this->callbackParams.emplace_back(it->data(), it->size());
			}
		}
	};

	std::vector<FormatTag> tagFormat;

	// for remark
	struct FormatRemark :FormatBasic {
		// ------
		// updated during parsing
		// ------

		// remark length
		size_t baseLength = 0;
		std::wstring remark;
		
		// ------
		// updated during rendering
		// ------
		CharSize* pCharSizeArr = nullptr;
		CharPos* pCharPosArr = nullptr;
		// 0 -> end of text
		size_t validLength = 0;

		NeoStr* pNeoStr = nullptr;

		FormatRemark(size_t start, size_t startWithNewLine,
			size_t baseLength, const std::wstring& remark) :FormatBasic{ start, startWithNewLine } {
			this->baseLength = baseLength;
			this->remark = remark;
		}

		FormatRemark(const FormatRemark&) = default;
		FormatRemark(FormatRemark&&) = default;
		FormatRemark& operator=(const FormatRemark&) = default;
		FormatRemark& operator=(FormatRemark&&) = default;

		~FormatRemark() { delete pNeoStr; }
	};

	// this vector is erased before format parsing, so old pointer is erased
	// and when parsing, vector resize doesn't affect pointer as it's not allocated
	std::vector<FormatRemark> remarkFormat;

	// for icon
	struct FormatICon :FormatBasic {
		// ------
		// updated during parsing
		// ------
		DWORD hImage;

		// ------
		// updated during rendering
		// ------
		size_t x = 0;
		size_t y = 0;
	};

	std::vector<FormatICon> iConFormat;

	//---------
	// stack based
	//		syntax: 
	//			format records where the format starts, for rendering
	//			stack records the format change during parsing
	//---------

	// for remark control
	struct RemarkDisplay {
		float remarkOffsetX = 0;
		float remarkOffsetY = 0;
	};

	RemarkDisplay remarkDisplay = { 0,0 };

	struct FormatRemarkDisplay :FormatBasic {
		RemarkDisplay remarkDisplay;
	};

	std::vector<FormatRemarkDisplay> remarkDisplayFormat;
	std::vector<RemarkDisplay> remarkDisplayStack;

	// for icon control
	struct IConDisplay {
		float iConOffsetX = 0;
		float iConOffsetY = 0;

		float iConScale = 1.0;

		bool iConResample = false;
	};

	IConDisplay iConDisplay = { 0,0,1.0,false };

	struct FormatIConDisplay :FormatBasic {
		IConDisplay iConDisplay;
	};

	std::vector<FormatIConDisplay> iConDisplayFormat;
	std::vector<IConDisplay> iConDisplayStack;

	// for align control
	struct FormatAlign :FormatBasic {
		DWORD dwDTFlags;
	};

	std::vector<FormatAlign> alignFormat;
	std::vector<DWORD> alignStack;

	struct CharOffsetDisplay {
		float charOffsetX = 0.0f;
		float charOffsetY = 0.0f;
	};

	CharOffsetDisplay charOffsetDisplay = { 0.0f,0.0f };

	struct FormatCharOffsetDisplay :FormatBasic {
		CharOffsetDisplay charOffsetDisplay;
	};

	std::vector<FormatCharOffsetDisplay> charOffsetFormat;
	std::vector<CharOffsetDisplay> charOffsetStack;

	// for shake control
	struct FormatShake :FormatBasic {
		ShakeControl shakeControl;
	};

	std::vector<FormatShake> shakeFormat;
	std::vector<ShakeControl> shakeStack;

	// for color control
	struct FormatColor :FormatBasic {
		Color color;
	};

	std::vector<FormatColor> colorFormat;
	std::vector<Color> colorStack;

	// for font control
	struct FormatFont :FormatBasic {
		LOGFONT logFont;
	};

	std::vector<FormatFont> fontFormat;
	std::vector<LOGFONT> logFontStack;

public:
	using IConLib = std::map<DWORD, LPSURFACE>;
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

			inline ResizeCacheKeyHash GetHash() const {
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
		inline bool NeedUpdateICon(const LPRO pObject) const {
			return pIConObject != pObject;
		}

		// link to active -> pObject == LPRO
		// link to other object -> pObject == Callback Identifier
		// pIConObject == nullptr -> No ICon
		inline void UpdateICon(const LPRO pObject, const IConParamParser& parser) {
			if (NeedUpdateICon(pObject)) {
				ReleaseIConLib();
			}			

			// always update callback, make sure the caller is valid
			this->pIConObject = pObject;
			this->iconParamParser = parser;
		}

		inline void ReleaseIConLib() {
			for (const auto& pIConSf : *this->pIConLib | std::views::values) {
				delete pIConSf;
			}

			this->pIConLib->clear();

			for (const auto& pIConResizeSf : this->resizeCache | std::views::values) {
				delete pIConResizeSf;
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

	inline const IConData* GetIConData() const {
		return pIConData;
	}

	static inline bool CheckMatch(const wchar_t wChar, const std::wstring& data) {
		for (auto& wChartoCheck : data) {
			if (wChartoCheck == wChar) {
				return true;
			}
		}

		return false;
	}

	inline bool NotAtStart(const wchar_t wChar) const {
		return CheckMatch(wChar, notAtStart);
	}

	inline bool NotAtEnd(const wchar_t wChar) const {
		return CheckMatch(wChar, notAtEnd);
	}

public:
	struct RegexHandler {
		wchar_t regstr[2] = { L'\0' };

		wregex* pCJK = nullptr;
		wregex* pEmpty = nullptr;
		wregex* pEmptyStr = nullptr;

		using RegexCache = std::map<wchar_t, bool>;

		RegexCache CJKCache;
		RegexCache EmptyCache;

		RegexHandler() {
			constexpr auto defaultRegexFlag = static_cast<RegexFlag>(ECMAScript | optimize);

			// https://www.jianshu.com/p/fcbc5cd06f39
			// filter -> match == no CJK, may miss some part, so add [a-zA-Z]
			this->pCJK = new wregex(L"[a-zA-Z\\u2E80-\\uA4CF\\uF900-\\uFAFF\\uFE10-\\uFE1F\\uFE30-\\uFE4F\\uFF00-\\uFFEF]"
				, defaultRegexFlag);
			this->pEmpty = new wregex(L"[\\s]"
				, defaultRegexFlag);
			this->pEmptyStr = new wregex(L"[\\s]*"
				, defaultRegexFlag);
		}
		~RegexHandler() {
			delete this->pCJK;
			this->pCJK = nullptr;

			delete this->pEmpty;
			this->pEmpty = nullptr;

			delete this->pEmptyStr;
			this->pEmptyStr = nullptr;
		}

		inline bool RegexCore(const wregex* pRegex, RegexCache* pCache, const wchar_t wChar) {
			regstr[0] = wChar;

			const auto it = pCache->find(wChar);

			if (it == pCache->end()) {
				const auto bRet = regex_match(regstr, *pRegex);
				(*pCache)[wChar] = bRet;

				return bRet;
			}
			else {
				return it->second;
			}

			//return regex_match(regstr, *pRegex);
		}

		inline bool NotCJK(const wchar_t wChar) {
			return RegexCore(this->pCJK, &this->CJKCache, wChar);
		}

		inline bool NotEmpty(const wchar_t wChar) {
			return !RegexCore(this->pEmpty, &this->EmptyCache, wChar);
		}

		inline bool NotEmpty(const wchar_t* pChar) const {
			return !regex_match(pChar, *this->pEmptyStr);
		}
	};

private:
	RegexHandler* pRegexCache;

#ifdef  _DEBUG
	// check a set of characters match regex
	//wchar_t wChars[] = { L'a',L'b', L'c', L'd' };
	//auto bMatch = CheckRegex(wChars, 4, &NeoStr::NotCJK);
	inline bool CheckRegex(const wchar_t* wChars, const size_t sz
		, bool (NeoStr::* regex)(wchar_t)) {
		auto bRet = true;

		for (size_t i = 0; i < sz; i++) {
			bRet &= (this->*regex)(wChars[i]);
		}

		return bRet;
	}
#endif //  _DEBUG

	inline int GetStartPosX(const long totalWidth, const long rcWidth) const {
		//DT_LEFT | DT_CENTER | DT_RIGHT
		//if (this->dwDTFlags & DT_LEFT) {
		//	return 0;
		//}
		if (this->dwDTFlags & DT_CENTER) {
			return ((rcWidth - totalWidth) / 2);
		}
		if (this->dwDTFlags & DT_RIGHT) {
			return rcWidth - totalWidth;
		}

		return 0;
	}

	inline int GetStartPosY(const long totalHeight, const long rcHeight) const {
		//	DT_TOP | DT_VCENTER | DT_BOTTOM
		//if (this->dwDTFlags & DT_TOP) {
		//	//return 0;
		//	return this->tm.tmInternalLeading;
		//}
		if (this->dwDTFlags & DT_VCENTER) {
			//remove offset to make exactly at the center
			//https://docs.microsoft.com/en-us/windows/win32/gdi/string-widths-and-heights

			const auto verticalAlignOffset = bVerticalAlignOffset
				                                 //? this->tm.tmInternalLeading + this->tm.tmExternalLeading
				                                 ? this->tm.tmDescent + this->tm.tmExternalLeading
				                                 : 0;

			//return ((rcHeight - totalHeight)  / 2);
			return ((rcHeight - (totalHeight - verticalAlignOffset)) / 2);
		}
		if (this->dwDTFlags & DT_BOTTOM) {
			return rcHeight - totalHeight;
		}

		return 0;
	}

#ifdef _DEBUG
	static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes
		ImageCodecInfo* pImageCodecInfo = nullptr;

		GetImageEncodersSize(&num, &size);

		if (size == 0) {
			return -1;  // Failure
		}

		pImageCodecInfo = (ImageCodecInfo*)(malloc(size));

		if (pImageCodecInfo == nullptr) {
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

	static inline size_t LogFontHasher(const LOGFONT& logFont) {
		constexpr auto HASHER_MAGICNUMBER = 0x9e3779b9;
		constexpr auto HASHER_MOVE = [](const size_t seed) { return HASHER_MAGICNUMBER + (seed << 6) + (seed >> 2); };

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
	NeoStr(const DWORD dwAlignFlags, const COLORREF color, const HFONT hFont,
		const NeoStr* pCache)
		:NeoStr(dwAlignFlags, color, hFont,
		false,
		{ pCache->pFontCache,
			pCache->pCharSzCacheWithFont,
			pCache->pRegexCache,
			pCache->pFontCollection },
			pCache->pIConData) {}

	struct NeoStrFontCache {
		FontCache* pFontCache = nullptr;
		CharSizeCacheWithFont* pCharSzCacheWithFont = nullptr;
		RegexHandler* pRegexCache = nullptr;
		PrivateFontCollection* pFontCollection = nullptr;

		inline bool CacheValid() const {
			return pFontCache && pCharSzCacheWithFont && pRegexCache;
		}

		inline void UpdateNeoStr(NeoStr* pNeoStr) const {
			pNeoStr->bExternalCache = true;

			pNeoStr->pFontCache = this->pFontCache;
			pNeoStr->pCharSzCacheWithFont = this->pCharSzCacheWithFont;
			pNeoStr->pRegexCache = this->pRegexCache;
		}
	};

	NeoStr(const DWORD dwAlignFlags, const COLORREF color, const HFONT hFont,
		const bool needGDIPStartUp = true,
		// read only
		const NeoStrFontCache& neoStrFontCache = {},
		IConData* pIConData = nullptr) {
		// ------
		// GDI Env
		// ------
		this->needGDIPStartUp = needGDIPStartUp;

		if (this->needGDIPStartUp) {
			Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
		}

		// ------
		// Basic Font
		// ------
		// duplicate font handle
		GetObject(hFont, sizeof(LOGFONT), &this->logFont);
		this->hFont = CreateFontIndirect(&this->logFont);

		//this->hdc = GetDC(rdPtr->rHo.hoAdRunHeader->rhHEditWin);
		this->hdc = GetDC(nullptr);
		this->hOldObj = SelectObject(this->hdc, hFont);
		GetTextMetrics(hdc, &this->tm);

		this->SetColor(color);
		this->dwDTFlags = dwAlignFlags | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL;

		this->strPos.reserve(20);

		// ------
		// Cache
		// ------

		// Font
		this->pFontCollection = neoStrFontCache.pFontCollection;
		this->bExternalCache = neoStrFontCache.CacheValid();

		if (this->bExternalCache) {
			neoStrFontCache.UpdateNeoStr(this);		
		}else {			
			Alloc(this->pFontCache);
			Alloc(this->pCharSzCacheWithFont);
			Alloc(this->pRegexCache);
		}

		this->pFont = GetFontPointerWithCache(this->logFont);

		// ICon
		this->bExternalIConData = pIConData != nullptr;
		this->pIConData = this->bExternalIConData ? pIConData : new IConData;

		// ------
		// Format
		// ------
		// non-stack based
		this->triggerFormat.reserve(DEFAULT_FORMAT_RESERVE);
		this->triggerRect.reserve(DEFAULT_FORMAT_RESERVE);

		this->tagFormat.reserve(DEFAULT_FORMAT_RESERVE);
		this->remarkFormat.reserve(DEFAULT_FORMAT_RESERVE);
		this->iConFormat.reserve(DEFAULT_FORMAT_RESERVE);

		// stack based
		this->charOffsetFormat.reserve(DEFAULT_FORMAT_RESERVE);
		this->charOffsetStack.reserve(DEFAULT_FORMAT_RESERVE);

		this->alignFormat.reserve(DEFAULT_FORMAT_RESERVE);
		this->alignStack.reserve(DEFAULT_FORMAT_RESERVE);

		this->remarkDisplayFormat.reserve(DEFAULT_FORMAT_RESERVE);
		this->remarkDisplayStack.reserve(DEFAULT_FORMAT_RESERVE);

		this->iConDisplayFormat.reserve(DEFAULT_FORMAT_RESERVE);
		this->iConDisplayStack.reserve(DEFAULT_FORMAT_RESERVE);

		this->colorFormat.reserve(DEFAULT_FORMAT_RESERVE);
		this->colorStack.reserve(DEFAULT_FORMAT_RESERVE);

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
		this->defaultCharSz = this->GetCharSizeWithCache(DEFAULT_CHARACTER, this->logFont);
	}

	~NeoStr() {
		SelectObject(this->hdc, this->hOldObj);
		ReleaseDC(nullptr, this->hdc);
		DeleteObject(this->hFont);

#ifdef MEASURE_GDI_PLUS		
		delete this->pMeasure;
		this->pMeasure = nullptr;
#endif

		if (!this->bExternalCache) {
			Release(this->pRegexCache);
			Release(this->pCharSzCacheWithFont);
			Release(this->pFontCache);
		}

		if (!this->bExternalIConData) {
			delete this->pIConData;
			this->pIConData = nullptr;
		}

		delete this->pShakeRandGen;
		this->pShakeRandGen = nullptr;

		delete[] this->pRawText;
		this->pRawText = nullptr;

		delete[] this->pText;
		this->pText = nullptr;

		delete[] this->pCharSizeArr;
		this->pCharSizeArr = nullptr;

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

	inline void CopyProperties(const NeoStr* pCopy) {
		this->SetAlign(pCopy->dwDTFlags, pCopy->bVerticalAlignOffset);
		this->SetSpace(pCopy->nRowSpace, pCopy->nColSpace);
		this->SetTabProperties(pCopy->tabContext.tabSize, pCopy->GetTabEm());

		this->LinkObject(pCopy->pIConData->pIConObject, pCopy->pIConData->iconParamParser);
		this->SetIConOffset(pCopy->iConDisplay.iConOffsetX, pCopy->iConDisplay.iConOffsetY);
		this->SetIConScale(pCopy->iConDisplay.iConScale);
		this->SetIConResample(pCopy->iConDisplay.iConResample);

		this->SetRemarkOffset(pCopy->remarkDisplay.remarkOffsetX, pCopy->remarkDisplay.remarkOffsetY);

		this->SetSmooth(
			pCopy->textRenderingHint,
			pCopy->smoothingMode,
			pCopy->pixelOffsetMode);
	}

	inline static void Release(CharSizeCacheWithFont*& pCharSzCacheWithFont) {
		delete pCharSzCacheWithFont;
		pCharSzCacheWithFont = nullptr;
	}

	inline static void Alloc(CharSizeCacheWithFont*& pCharSzCacheWithFont) {
		pCharSzCacheWithFont = new CharSizeCacheWithFont;
	}

	inline static void Release(FontCache*& pFontCache) {
		for (const auto& pFont : *pFontCache | std::views::values) {
			delete pFont;
		}

		delete pFontCache;
		pFontCache = nullptr;
	}
	
	inline static void Alloc(FontCache*& pFontCache) {
		pFontCache = new FontCache;
	}

	inline static void Release(RegexHandler*& pRegexHandler) {
		delete pRegexHandler;
		pRegexHandler = nullptr;
	}

	inline static void Alloc(RegexHandler*& pRegexHandler) {
		pRegexHandler = new RegexHandler;
	}

	//https://stackoverflow.com/questions/42595856/fonts-added-with-addfontresourceex-are-not-working-in-gdi
	//https://docs.microsoft.com/zh-cn/windows/win32/api/gdiplusheaders/nf-gdiplusheaders-privatefontcollection-addfontfile
	//https://www.codeproject.com/Articles/42041/How-to-Use-a-Font-Without-Installing-it
	//https://blog.csdn.net/analogous_love/article/details/45845971
	inline void EmbedFont(const LPCWSTR pFontFile) const {
		EmbedFont(pFontFile, *this->pFontCollection);
#ifdef _DEBUG
		if (this->pFontCollection != nullptr) {
			auto count = this->pFontCollection->GetFamilyCount();
		}
#endif
	}

	inline static void EmbedFont(const LPCWSTR pFontFile, PrivateFontCollection& fontCollection) {
		fontCollection.AddFontFile(pFontFile);
	}

	inline static bool FontCollectionHasFont(const LPWSTR pFaceName
		, const Gdiplus::FontCollection* pFontCollection) {
		if (pFontCollection == nullptr) {
			return false;
		}

		const int n = pFontCollection->GetFamilyCount();

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

		const LANGID language = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);

		bool has = false;
		bool hasSuffixRegular = false;
		//bool hasSuffixNormal = false;

		const std::wstring withRegular = (std::wstring)pFaceName + (std::wstring)L" Regular";
		//std::wstring withNormal = (std::wstring)pFaceName + (std::wstring)L" Normal";

		for (int i = 0; i < n; i++) {
			has = false;
			hasSuffixRegular = false;
			//hasSuffixNormal = false;

			auto hasName = [&] (const LANGID language = (LANGID)0U) {
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

	inline static int GetFontStyle(const LOGFONT& logFont) {
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
		
	inline Font GetFont(LOGFONT logFont) const {
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

	inline Font* GetFontPointer(LOGFONT logFont) const {
		const auto bFound = FontCollectionHasFont(logFont.lfFaceName, this->pFontCollection);
		
		return new Font(logFont.lfFaceName
			, (float)abs(logFont.lfHeight)
			, GetFontStyle(logFont)
			, Gdiplus::UnitWorld
			, bFound ? this->pFontCollection
			: nullptr);
	}

	inline Font* GetFontPointerWithCache(const LOGFONT& logFont) const {
		const auto logFontHash = LogFontHasher(logFont);
		const auto it = pFontCache->find(logFontHash);

		if (it != pFontCache->end()) {
			return it->second;
		}
		else {
			const auto newFont = GetFontPointer(logFont);
			(*pFontCache)[logFontHash] = newFont;

			return newFont;
		}
	}

	inline int GetFontSize(int size) const {
		return -1 * MulDiv(size,
			GetDeviceCaps(this->hdc, LOGPIXELSY),
			72);
	}

	// Color conversion
	// #AARRGGBB
	static inline Color GetColor(const std::wstring_view hex) {
		return GetColor(_h2d(hex.data(), hex.size()), true);
	}
	
	// Color conversion
	// bAlpha = false: Color is COLORREF
	static inline Color GetColor(const DWORD color, const bool bAlpha = false) {
		if (bAlpha) {
			const auto A = (BYTE)((color >> 24) & 0xFF);
			const auto R = (BYTE)((color >> 16) & 0xFF);
			const auto G = (BYTE)((color >> 8) & 0xFF);
			const auto B = (BYTE)((color) & 0xFF);

			return Color(A, R, G, B);
		}
		else {
			return Color(255, GetRValue(color), GetGValue(color), GetBValue(color));
		}
	}

	// Color conversion
	static inline DWORD GetDWORD(const Color color) {
		const auto A = color.GetAlpha();
		const auto R = color.GetRed();
		const auto G = color.GetGreen();
		const auto B = color.GetBlue();

		return (A << 24) | (R << 16) | (G << 8) | (B);
	}

	// get color ref
	static inline COLORREF GetColor(const DWORD color) {
		const auto R = (BYTE)((color >> 16) & 0xFF);
		const auto G = (BYTE)((color >> 8) & 0xFF);
		const auto B = (BYTE)((color) & 0xFF);

		return RGB(R, G, B);
	}

	inline CharSize GetDefaultCharSize() const {
		return this->defaultCharSz;
	}

	inline void SetShakeTimer(const unsigned short shakeTimer) {
		this->shakeTimer = shakeTimer;
	}

	inline bool GetShakeUpdateState() const {
		return this->bShake || this->bChildShake;
	}

	static inline ShakeType GetShakeTypeByName(const std::wstring_view& param) {
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
		} while (false);

		return ShakeType::ShakeType_None;
	}

	inline double GetShakeOffset(const FormatShake& shakeFormat, size_t totalChar) const {
		const double shakeOffset = (totalChar - shakeFormat.start) * shakeFormat.shakeControl.charOffset
			+ this->shakeTimer * shakeFormat.shakeControl.timerCoef;

		return shakeOffset;
	}

	inline void GetShakePosition(const ShakeControl& shakeControl, const double timer, float& x, float& y, const CharSize* charSz) const {
		const auto t = RAD(timer);

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
			const auto randomX = this->pShakeRandGen->GenerateRandNumber() / (1.0 * SHAKE_RANDOM_RANGE);
			const auto randomY = this->pShakeRandGen->GenerateRandNumber() / (1.0 * SHAKE_RANDOM_RANGE);

			const auto wa = shakeControl.amplitude * charSz->width;
			const auto ha = shakeControl.amplitude * charSz->height;

			x = float(x + wa * randomX);
			y = float(y + ha * randomY);

			break;
		}
		default:
			break;
		}
	}

	inline void SetRemarkOffset(const float remarkOffsetX = 0, const float remarkOffsetY = 0) {
		this->remarkDisplay.remarkOffsetX = remarkOffsetX;
		this->remarkDisplay.remarkOffsetY = remarkOffsetY;
	}

	inline void SetIConOffset(const float iConOffsetX = 0, const float iConOffsetY = 0) {
		this->iConDisplay.iConOffsetX = iConOffsetX;
		this->iConDisplay.iConOffsetY = iConOffsetY;
	}

	inline void SetIConScale(const float iConScale = 1.0) {
		this->iConDisplay.iConScale = iConScale;
	}

	inline void SetIConResample(const bool iConResample=false){
		this->iConDisplay.iConResample = iConResample;
	}

	inline void LinkObject(const LPRO pObject, const IConParamParser& parser) const {
		if (!this->bExternalIConData) {
			this->pIConData->UpdateICon(pObject, parser);
		}
	}

	inline void SetHWA(const int driver, const bool preMulAlpha) {
		this->hwaDriver = driver;
		this->preMulAlpha = preMulAlpha;
	}

	// set color
	// bAlpha = false: Color is COLORREF
	inline void SetColor(DWORD color, const bool bAlpha = false) {
		// COLORREF -> dwColor
		if (!bAlpha) { color = GetDWORD(GetColor(color, false)); }

		const auto bColorChanged = this->dwTextColor != color;

		if (bColorChanged && !this->colorFormat.empty()) {
			// no color format, update default
			if (this->colorFormat.size() == 1) {
				this->colorFormat.at(0).color = GetColor(color, true);
			}
			// has color format, update format
			// a full refresh is needed as there are diff commands
			else {
				this->GetFormat(this->pRawText, this->previousFlag, true);
			}
		}

		this->dwTextColor = color;
	}

	inline void SetBorderOffset(const unsigned short borderOffsetX, unsigned short borderOffsetY) {
		this->borderOffsetX = borderOffsetX;
		this->borderOffsetY = borderOffsetY;
	}

	// set row/col space
	inline void SetSpace(const int nRowSpace = 0, const int nColSpace = 0) {
		this->nRowSpace = nRowSpace /*+ this->tm.tmInternalLeading + this->tm.tmExternalLeading*/;
		this->nColSpace = nColSpace;
	}

	inline void SetSmooth(
		const Gdiplus::TextRenderingHint textRenderingHint = Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias
		, const Gdiplus::SmoothingMode smoothingMode = Gdiplus::SmoothingMode::SmoothingModeHighQuality
		, const Gdiplus::PixelOffsetMode pixelOffsetMode = Gdiplus::PixelOffsetMode::PixelOffsetModeHalf) {
		this->textRenderingHint = textRenderingHint;
		this->smoothingMode = smoothingMode;
		this->pixelOffsetMode = pixelOffsetMode;
	}

	inline void SetAlign(const DWORD dwAlign, const bool bVerticalAlignOffset) {
		this->dwDTFlags = dwAlign | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL;
		this->bVerticalAlignOffset = bVerticalAlignOffset;
	}

	// add rectangle on each char, for debug
	inline void SetDrawRectangle(const bool bRectangle) {
		this->bDrawRectangle = bRectangle;
	}

	//inline void SetOutLine(BYTE outLinePixel, DWORD color) {
	//	this->bOutLine = outLinePixel;
	//	this->nOutLinePixel = outLinePixel;

	//	this->dwOutLineColor = color;
	//}

	inline void SetHotSpot(const int hotSpotX = 0, const int hotSpotY = 0) {
		this->hotSpotX = hotSpotX;
		this->hotSpotY = hotSpotY;
	}

	inline void SetScale(const float xScale = 1.0, const float yScale = 1.0) {
		this->xScale = xScale;
		this->yScale = yScale;
	}

	inline void SetAngle(const float angle = 0) {
		this->angle = angle;
	}

//	// deprecated
//	inline CharSize GetCharSizeRaw(wchar_t wChar) {
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
//		return *(CharSize*)&sz;
//	}
//	
//	// deprecated
//	inline CharSize GetCharSizeWithCache(wchar_t wChar) {
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

private:
    struct TabContext {
        // settings
        int tabSize = 4;
		wchar_t spaceChar = CHAR_SPACE;

        // context
        int curWidth = 0;
        CharSize spaceCharSize = {};

		inline void UpdateSpaceChar(wchar_t spaceChar) {
			this->spaceChar = spaceChar;
		}

        inline void UpdateContext(int curWidth, CharSize spaceCharSize) {
            this->curWidth = curWidth;
            this->spaceCharSize = spaceCharSize;
        }

        // TODO handle case of end of line
        inline CharSize GetTabCharSize() const {
            // tab size depend on space size
            const auto actualTabSize = tabSize * spaceCharSize.width;
            CharSize tabCharSize = { actualTabSize, spaceCharSize.height };

            const auto div = static_cast<double>(curWidth) / actualTabSize;
            const auto floor =static_cast<int>(std::floor(div));
            const auto ceil = static_cast<int>(std::ceil(div));

            // if not enough, make it enough
            if(floor != ceil) {
                tabCharSize.width = ceil * actualTabSize - curWidth;
            }
            // if enough, return default size
            else {

            }

			return tabCharSize;
        }
    };

    TabContext tabContext;

public:
	inline void SetTabProperties(int tabSize = 4, bool bEM = false) {
		tabContext.tabSize = tabSize;
		tabContext.spaceChar = bEM
			? CHAR_EMSPACE
			: CHAR_SPACE;
    }

	inline bool GetTabEm() const {
		return tabContext.spaceChar == CHAR_EMSPACE;
	}

	static inline CharSize GetCharSizeRaw(const wchar_t wChar, const HDC hdc) {
		SIZE sz = { 0,0 };
		GetTextExtentPoint32(hdc, &wChar, 1, &sz);

		// special: shouldn't have width to fix non-left align offset		
		if (wChar == L'\r' || wChar == L'\n' || wChar == L'\0') {
			sz.cx = 0;
		}
		// general: has both width & height
		else {
			
		}

		return *(CharSize*)&sz;
	}

	inline auto GetCharSizeCacheIt(const LOGFONT& logFont) const {
		const auto logFontHash = LogFontHasher(logFont);
		auto it = pCharSzCacheWithFont->find(logFontHash);

		return it;
	}

	inline CharSize GetCharSizeWithCache(const wchar_t wChar, const LOGFONT& logFont) {
		const auto logFontHash = LogFontHasher(logFont);
		const auto it = pCharSzCacheWithFont->find(logFontHash);

		if (it != pCharSzCacheWithFont->end()) {
			auto& cacheSecond = it->second;
			auto& charCache = cacheSecond.cache;

			const auto charIt = charCache.find(wChar);
			if (charIt != charCache.end()) {
				return charIt->second;
			}
			else {
				const auto sz = GetCharSizeRaw(wChar, cacheSecond.hdc);
				charCache[wChar] = sz;

				return sz;
			}
		}
		else {
#ifdef COUNT_GDI_OBJECT
			GDIObjectCounter objectCounter;

			objectCounter.UpdateObjectCount();
#endif
			pCharSzCacheWithFont->emplace(logFontHash, logFont);

#ifdef COUNT_GDI_OBJECT
 			const auto count = objectCounter.objectCount;

			objectCounter.UpdateObjectCount();
			if (count < objectCounter.objectCount) {
				const auto info = std::format(L"Add when Cache\n");
				OutputDebugStringW(info.c_str());
		}
#endif
			return GetCharSizeWithCache(wChar,logFont);
		}
	}

	//inline CharSize GetStrSize(const LPCWSTR pStr, const size_t pStrLen = -1) const {
	//	RECT change = { 0,0,65535,1 };
	//
	//	DrawTextW(
	//		hdc,
	//		pStr,
	//		pStrLen == -1 ? wcslen(pStr) : pStrLen,
	//		&change
	//		, this->dwDTFlags | DT_CALCRECT
	//	);
	//
	//	return CharSize { change.right - change.left,change.bottom - change.top };
	//}

	inline LPCWSTR GetRawText() const {
		return this->pRawText;
	}

	inline LPCWSTR GetText() const {
		return this->pText;
	}

	inline void GetFormat(const LPCWSTR pStr
		, const size_t flags = FORMAT_IGNORE_DEFAULTFLAG
		, bool bForced = false		// force refresh
		, const size_t operation = 0
		, const size_t operationParam = 0) {
		// ------------
		// check validity & skip
		// ------------

		this->bTextValid = true;
		this->bPreviousForced = bForced;
		this->bFormatUpdated = false;

		// if valid, len = wcslen(pStr)
		auto TextValid = [&](const wchar_t* pStr, size_t* pLen,
			bool bAllowEmptyChar = false) {
			*pLen = 0;

			if (pStr == nullptr) {
				return false;
			}

			*pLen = wcslen(pStr);

			if (*pLen == 0) {
				return false;
			}

			if (!bAllowEmptyChar && !pRegexCache->NotEmpty(pStr)) {
				return false;
			}

			return true;
		};

		size_t pInputLen = 0;

		if (!TextValid(pStr, &pInputLen)) {
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
		bFormatUpdated = true;

		// won't copy if this->pRawText is input
		if (this->pRawText != pStr) {
			delete[] this->pRawText;
			this->pRawText = nullptr;

			this->pRawText = new wchar_t[pInputLen + 1];
			// if valid, len = wcslen(pStr), copy directly without memset to '\0'
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
		// \[ -> [
		//	escaped
		// 
		// [Command] if not match follows, will be displayed as untouched
		//	depend on your flag settings
		// 
		// [^ = Exclude, ...]
		//	ignore all formats after this exclude given ones
		// 
		// [!^]
		//	stop ignore all formats
		// 
		// [!]
		//	reset all format to what you set in properties and events
		//
		// [Values General]
		// 	! = reset to default
		//	+/- = add/minus to current
		//	\ = use given value directly, including sign
		//	e.g., current is 1.0
		//		-0.5 -> 1.0 - 0.5 = 0.5
		//		\+0.5 -> +0.5
		//		\-0.5 -> -0.5
		//
		// [Trigger = TriggerName, Content][/Trigger]
		//	Embraced characters are trigger rect
		//	As text may include ',', parse is started from left, unlike other formats
		//
		//	*if you use content, make sure it doesn't have open []
		//
		// [Tag = CallbackName, Params]
		//	Trigger callback when render to it, tags in remarks will be ignored
		//	*you need to retrieve each param
		//	*then handle it by your self
		//
		// [Remark = CharCount, Content]
		//	Insert remark that display over texts
		//	As text may include ',', parse is started from left, unlike other formats
		//	remark is rendered in another NeoStr class, with the same config (color, font, etc), but overrode position & render object
		//	and the font size is half of the parent object when it starts rendering
		//
		//	*if you use content, make sure it doesn't have open []
		//	*if remarked string changed line, then the render of its remark is skipped
		//	*if the font / size changed too drastically there will be overlap
		//	*position of remark is estimated by the mean size of remarked characters
		//
		// [RemarkOffsetX = 0.0][/RemarkOffsetX]
		//	Remark Offset X
		//	See [Values General]
		// 
		// [RemarkOffsetY = 0.0][/RemarkOffsetY]
		//	Remark Offset Y
		//	See [Values General]
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
		//	See [Values General]
		// 
		// [IConOffsetY = 0.0][/IConOffsetY]
		//	ICon Offset Y
		//	See [Values General]
		// 
		// [IConScale = 1.0][/IConScale]
		//	ICon Scale
		//	See [Values General]
		// 
		// [IConResample = 1][/IConResample]
		//	ICon Resample, 1 = Enable, 0 = Disable
		//	! = reset to default
		// 
		// [Align = LEFT]
		//  change align of different lines
		//
		// [CharOffsetX = 0.0][/CharOffsetX]
		//  Char Offset X
		//	See [Values General]
		// 
		// [CharOffsetY = 0.0][/CharOffsetY]
		//  Char Offset Y
		//	See [Values General]
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
		//	See [Values General]
		// 
		// [Font = FontName][/Font]
		// [F = FontName][/F]
		//	! = reset to default
		// 
		// [Size = FontSize][/Size]
		// [S = FontSize][/S]
		//	See [Values General]
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

		// SavedChar: no command & escaped
		auto pCurChar = pRawText;
		auto pSavedChar = pText;

		ControlParams controlParams;
		controlParams.reserve(4);

		// ------------
		// reset formats
		// ------------

		// non-stack based
		this->triggerFormat.clear();
		this->triggerRect.clear();

		this->tagFormat.clear();
		this->remarkFormat.clear();
		this->iConFormat.clear();

		// stack based
		this->charOffsetStack.clear();
		this->charOffsetStack.emplace_back(this->charOffsetDisplay);

		this->charOffsetFormat.clear();
		this->charOffsetFormat.emplace_back(FormatCharOffsetDisplay{ {0,0},charOffsetStack.back() });

		this->alignStack.clear();
		this->alignStack.emplace_back(this->dwDTFlags);

		this->alignFormat.clear();
		this->alignFormat.emplace_back(FormatAlign{ {0,0},alignStack.back() });

		this->remarkDisplayStack.clear();
		this->remarkDisplayStack.emplace_back(this->remarkDisplay);

		this->remarkDisplayFormat.clear();
		this->remarkDisplayFormat.emplace_back(FormatRemarkDisplay{ {0,0},remarkDisplayStack.back() });

		this->iConDisplayStack.clear();
		this->iConDisplayStack.emplace_back(this->iConDisplay);

		this->iConDisplayFormat.clear();
		this->iConDisplayFormat.emplace_back(FormatIConDisplay{ {0,0},iConDisplayStack.back() });
		
		this->colorStack.clear();
		this->colorStack.emplace_back(GetColor(this->dwTextColor, true));

		this->colorFormat.clear();
		this->colorFormat.emplace_back(FormatColor{ {0,0},colorStack.back() });

		this->logFontStack.clear();
		this->logFontStack.emplace_back(this->logFont);

		this->fontFormat.clear();
		this->fontFormat.emplace_back(FormatFont{ {0,0},logFontStack.back() });

		this->bShake = false;
		this->bChildShake = false;

		this->shakeStack.clear();
		this->shakeStack.emplace_back(ShakeControl());

		this->shakeFormat.clear();
		this->shakeFormat.emplace_back(FormatShake{ {0,0},shakeStack.back() });

		// function that used to GetRawStringByFilteredStringLength
		auto GetRawStringByFilteredStringLength = [&]() {
			const auto offset = size_t(pSavedChar - pText);

			if ((operation & FORMAT_OPERATION_GetRawStringByFilteredStringLength)
				&& offset + 1 > operationParam) {
				pCurChar[0] = L'\0';
				
#ifdef _DEBUG
				pSavedChar[0] = L'@';
				pSavedChar[1] = L'\0';
#endif
				throw NeoStrFormatException("Get Raw String By Filtered String Length", 
					FORMAT_OPERATION_GetRawStringByFilteredStringLength);
			}
		};

		// ------------
		// start parse
		// ------------
		size_t newLineCount = 0;

		const bool bIgnoreUnknown = flags & FORMAT_IGNORE_UNKNOWN;
		const bool bIgnoreIncomplete = flags & FORMAT_IGNORE_INCOMPLETE;

		bool bIgnoreFormat = false;
		ControlParams ignoreFormatExclude;

		while (true) {
			// End
			if ((pCurChar - pRawText) == pInputLen) {
				break;
			}

			const auto curChar = pCurChar[0];
			const auto nextChar = pCurChar[1];

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
				size_t nestCount = 0;

				bool bEndOfText = false;

				// get command and param ([Command = Param])
				while ((pCurChar + end)[0] != L']' || nestCount != 0) {
					// protect for end
					if ((pCurChar - pRawText) + end == pInputLen) {
						bEndOfText = true;

						break;
					}

					const auto pTest = pCurChar + end;

					// handle nest format contorl for params
					if ((pTest)[0] == L'\\' && (pTest + 1)[0] == L'[') {
						end += 2;
						continue;
					}

					if (end != 0 && (pTest)[0] == L'[') { nestCount++; }
					if (nestCount != 0 && (pTest)[0] == L']') { nestCount--; }

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

						// get command (Command = Param)
						while ((pCurChar + equal)[0] != L'=') {
							if (equal == end) {
								break;
							}

							equal++;
						}

						using ParamParserCallback = std::function<void(std::wstring_view&)>;

						// parse by ',' use a while loop to get all params
						auto ParseParamCore = [] (std::wstring_view& paramParser, bool bReverse, const ParamParserCallback& callBack) {
							const auto start = bReverse
								? paramParser.find_last_of(L',')
								: paramParser.find_first_of(L',');

							if (start == std::wstring::npos) {
								callBack(paramParser);

								return false;
							}

							const auto before = paramParser.substr(0, start);
							const auto after = paramParser.substr(start + 1);

							auto paramStr = bReverse ? after : before;
							callBack(paramStr);

							paramParser = bReverse ? before : after;

							return true;
						};

						// from left
						auto ParseParam = [ParseParamCore] (std::wstring_view& paramParser, const ParamParserCallback& callBack) {
							return ParseParamCore(paramParser, false, callBack);
							};

						// from right
						auto ParseParamReverse = [ParseParamCore](std::wstring_view& paramParser, const ParamParserCallback& callBack) {
							return ParseParamCore(paramParser, true, callBack);
						};

						// get command and param, handle end and equal sign
						std::wstring_view controlStr = GetTrimmedStr(pCurChar + bEndOfRegion + 1, (std::max)(0u, equal - bEndOfRegion - 1));
						std::wstring_view controlParam = !bEndOfRegion
							? GetTrimmedStr(pCurChar + equal + 1, (std::max)(0u, end - equal - 1))
							: controlStr;

						if (controlParam.empty()) {
							controlParam = DEFAULT_PARAM;
						}

						// length to ref format when calculating & rendering
						size_t savedLength = pSavedChar - pText - newLineCount * 2;
						size_t savedLengthWithNewLine = pSavedChar - pText;

						// ------------
						// parse commands & params
						// ------------
						auto CurrentCommandIgnored = [&] () {
							if (!bIgnoreFormat) { return false; }

							for (const auto& it : ignoreFormatExclude) {
								if (StringViewIEqu(controlStr, it)) { return false; }
							}

							return true;
							};

						if (StringViewIEqu(controlStr, L"^")) {
							bIgnoreFormat = true;
							ignoreFormatExclude.clear();

							auto& paramParser = controlParam;

							do {

							} while (ParseParam(paramParser
								, [&ignoreFormatExclude] (std::wstring_view& param) {
									ignoreFormatExclude.emplace_back(GetTrimmedStr(param));
								}));

							ignoreFormatExclude.erase(std::ranges::unique(ignoreFormatExclude).begin(), ignoreFormatExclude.end());

							break;
						}

						if (StringViewIEqu(controlStr, L"!^")) {
							bIgnoreFormat = false;
							ignoreFormatExclude.clear();

							break;
						}

						if (StringViewIEqu(controlStr, L"Trigger")) {
							// new trigger
							if (!bEndOfRegion) {
								auto& paramParser = controlParam;
								controlParams.clear();

								// no delimiter -> only name
								if (!ParseParam(paramParser
									, [&controlParams] (std::wstring_view& param) {
										controlParams.emplace_back(GetTrimmedStr(param));
									})) {
									this->triggerFormat.emplace_back(savedLength, savedLengthWithNewLine,
									std::wstring(controlParam));
								}
								// has param
								else {
									this->triggerFormat.emplace_back(savedLength, savedLengthWithNewLine,
										std::wstring(controlParams[0]),
										std::wstring(GetTrimmedStr(paramParser)));
								}

								break;
							}

							if (this->triggerFormat.empty()) { break; }

							// end of region, calculate length
							auto& lastTrigger = this->triggerFormat.back();
							lastTrigger.triggerLength = savedLengthWithNewLine - lastTrigger.startWithNewLine;
						}

						if (StringViewIEqu(controlStr, L"Tag")) {
							auto& paramParser = controlParam;
							controlParams.clear();

							do {

							} while (ParseParam(paramParser
								, [&controlParams] (std::wstring_view& param) {
									controlParams.emplace_back(GetTrimmedStr(param));
								}));

							if (controlParams.empty()) { break; }

							this->tagFormat.emplace_back(savedLength,
								savedLengthWithNewLine,
								static_cast<size_t>(pCurChar - pRawText),
								controlParams);

							break;
						}

						if (StringViewIEqu(controlStr, L"Remark")) {
							if (CurrentCommandIgnored()) { break; }

							auto& paramParser = controlParam;
							controlParams.clear();

							// invalid, no delimiter
							if(!ParseParam(paramParser
								, [&controlParams] (std::wstring_view& param) {
									controlParams.emplace_back(GetTrimmedStr(param));
								})) {
								break;
							}

							const auto length = _stoi(controlParams[0]);

							// invalid length
							if (length == 0) { break; }

							controlParams.emplace_back(GetTrimmedStr(paramParser));

							this->remarkFormat.emplace_back(savedLength,
								savedLengthWithNewLine,
								static_cast<size_t>(_stoi(controlParams[0])),
								std::wstring(controlParams[1]));

							break;
						}

						if (StringViewIEqu(controlStr, L"ICon")) {
							if (CurrentCommandIgnored()) { break; }

							DWORD hImage = FORMAT_INVALID_ICON;

							do {
								if (this->pIConData->pIConObject == nullptr || this->pIConData->iconParamParser == nullptr) {
									break;
								}

								auto& paramParser = controlParam;
								controlParams.clear();

								do {

								} while (ParseParamReverse(paramParser
									, [&controlParams](std::wstring_view& param) {
										controlParams.emplace_back(GetTrimmedStr(param));
									}));

								hImage = pIConData->iconParamParser(controlParams, *this->pIConData->pIConLib);
							} while (false);

							this->iConFormat.emplace_back(FormatICon{ {savedLength,
																		savedLengthWithNewLine},
																		hImage });

							// space for icon
							pSavedChar[0] = DEFAULT_CHARACTER;
							pSavedChar++;

							break;
						}

#pragma region StackedFormat
						// update format for last character
						auto UpdateFormat = [&](auto& format, auto& content) {
								auto pLastFormat = &format.back();

								// if equal, replace last format
								if (savedLength == pLastFormat->start) {
									pLastFormat->CopyFormat(content);
								}
								else {
									std::remove_reference_t<decltype(format[0])> newFormat = format.front();
									newFormat.start = savedLength;
									newFormat.startWithNewLine = savedLengthWithNewLine;

									newFormat.CopyFormat(content);

									format.emplace_back(newFormat);
								}
							};

						// reset all
						if (StringViewIEqu(controlStr, L"!")) {
							auto Reset = [&] (auto& stack, auto& format) {
								std::remove_reference_t<decltype(stack[0])> first = stack.front();

								stack.clear();
								stack.emplace_back(first);

								UpdateFormat(format, first);
								};

							// reset stack based here
							// note that icon or remark requires stack based
							// to align, so doesn't reset them here
							Reset(charOffsetStack, charOffsetFormat);
							Reset(alignStack, alignFormat);
							Reset(shakeStack, shakeFormat);
							Reset(colorStack, colorFormat);
							Reset(logFontStack, fontFormat);

							break;
						}

						// manage the stack
						// new element, call callback to update the one copied from last one
						// parse params there
						// or end of region, pop the stack, then update it
						auto StackManager = [&] (auto& stack, auto& format, auto callBack) {
							if (!bEndOfRegion) {
								// clone one here
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
						auto DiffManager = [&] (auto oldValue, auto callBack) {
							bool bAdd = false;
							bool bMinus = false;

							do {
								const auto& firstChar = controlParam.front();

								// escape
								if(firstChar == L'\\') {
									controlParam = controlParam.substr(1);

									break;
								}

								if (firstChar == L'+') {
									controlParam = controlParam.substr(1);
									bAdd = true;

									break;
								}

								if (firstChar == L'-') {
									controlParam = controlParam.substr(1);
									bAdd = true;
									bMinus = true;

									break;
								}
							} while (false);

							auto sizeDiff = callBack(controlParam);

							return !bAdd
								? sizeDiff
								: (bMinus ? -1 : +1) * sizeDiff + oldValue;
						};

						// ------------
						// remark
						// ------------

						if (StringViewIEqu(controlStr, L"RemarkOffsetX")) {
							if (CurrentCommandIgnored()) { break; }

							StackManager(remarkDisplayStack, remarkDisplayFormat, [&] (RemarkDisplay& remarkDisplay) {
								// Reset
								if (StringViewIEqu(controlParam, L"!")) {
									remarkDisplay.remarkOffsetX = this->remarkDisplayStack.front().remarkOffsetX;

									return;
								}

								remarkDisplay.remarkOffsetX = DiffManager(remarkDisplay.remarkOffsetX,
									[&] (const std::wstring_view& controlParam) {
										const auto size = _stof(controlParam);
										return size;
									});
								});

							break;
						}

						if (StringViewIEqu(controlStr, L"RemarkOffsetY")) {
							if (CurrentCommandIgnored()) { break; }

							StackManager(remarkDisplayStack, remarkDisplayFormat, [&] (RemarkDisplay& remarkDisplay) {
								// Reset
								if (StringViewIEqu(controlParam, L"!")) {
									remarkDisplay.remarkOffsetY = this->remarkDisplayStack.front().remarkOffsetY;

									return;
								}

								remarkDisplay.remarkOffsetY = DiffManager(remarkDisplay.remarkOffsetY,
									[&] (const std::wstring_view& controlParam) {
										const auto size = _stof(controlParam);
										return size;
									});
								});

							break;
						}

						// ------------
						// icon
						// ------------

						if (StringViewIEqu(controlStr, L"IConOffsetX")) {
							if (CurrentCommandIgnored()) { break; }

							StackManager(iConDisplayStack, iConDisplayFormat, [&] (IConDisplay& iConDisplay) {
								// Reset
								if (StringViewIEqu(controlParam, L"!")) {
									iConDisplay.iConOffsetX = this->iConDisplayStack.front().iConOffsetX;

									return;
								}

								iConDisplay.iConOffsetX = DiffManager(iConDisplay.iConOffsetX,
									[&] (const std::wstring_view& controlParam) {
										const auto size = _stof(controlParam);
										return size;
									});
								});

							break;
						}

						if (StringViewIEqu(controlStr, L"IConOffsetY")) {
							if (CurrentCommandIgnored()) { break; }

							StackManager(iConDisplayStack, iConDisplayFormat, [&] (IConDisplay& iConDisplay) {
								// Reset
								if (StringViewIEqu(controlParam, L"!")) {
									iConDisplay.iConOffsetY = this->iConDisplayStack.front().iConOffsetY;

									return;
								}

								iConDisplay.iConOffsetY = DiffManager(iConDisplay.iConOffsetY,
									[&] (const std::wstring_view& controlParam) {
									const auto size = _stof(controlParam);
									return size;
									});
								});

							break;
						}

						if (StringViewIEqu(controlStr, L"IConScale")) {
							if (CurrentCommandIgnored()) { break; }

							StackManager(iConDisplayStack, iConDisplayFormat, [&] (IConDisplay& iConDisplay) {
								// Reset
								if (StringViewIEqu(controlParam, L"!")) {
									iConDisplay.iConScale = this->iConDisplayStack.front().iConScale;

									return;
								}

								iConDisplay.iConScale = DiffManager(iConDisplay.iConScale,
									[&] (const std::wstring_view& controlParam) {
									const auto size = _stof(controlParam);
									return size;
									});
								});

							break;
						}

						if (StringViewIEqu(controlStr, L"IConResample")) {
							if (CurrentCommandIgnored()) { break; }

							StackManager(iConDisplayStack, iConDisplayFormat, [&] (IConDisplay& iConDisplay) {
								// Reset
								if (StringViewIEqu(controlParam, L"!")) {
									iConDisplay.iConResample = this->iConDisplayStack.front().iConResample;

									return;
								}

								iConDisplay.iConResample = _stoi(controlParam);
								});

							break;
						}


						// ------------
						// align
						// ------------

						if (StringViewIEqu(controlStr, L"Align")) {
							if (CurrentCommandIgnored()) { break; }

							StackManager(alignStack, alignFormat, [&] (DWORD& dwDTFlags) {
								// Reset
								if (StringViewIEqu(controlParam, L"!")) {
									dwDTFlags = alignStack.front();

									return;
								}

								const auto& paramParser = controlParam;
								controlParams.clear();

								const auto newAlign = _stoi(paramParser);
								dwDTFlags = (dwDTFlags & ~(DT_LEFT | DT_CENTER | DT_RIGHT)) | (newAlign);
								});

							break;
						}

						if (StringViewIEqu(controlStr, L"CharOffsetX")) {
							if (CurrentCommandIgnored()) { break; }

							StackManager(charOffsetStack, charOffsetFormat, [&] (CharOffsetDisplay& charPosDisplay) {
								// Reset
								if (StringViewIEqu(controlParam, L"!")) {
									charPosDisplay.charOffsetX = this->charOffsetStack.front().charOffsetX;

									return;
								}

								charPosDisplay.charOffsetX = DiffManager(charPosDisplay.charOffsetX,
									[&] (const std::wstring_view& controlParam) {
										const auto size = _stof(controlParam);
										return size;
									});
								});

							break;
						}

						if (StringViewIEqu(controlStr, L"CharOffsetY")) {
							if (CurrentCommandIgnored()) { break; }

							StackManager(charOffsetStack, charOffsetFormat, [&] (CharOffsetDisplay& charPosDisplay) {
								// Reset
								if (StringViewIEqu(controlParam, L"!")) {
									charPosDisplay.charOffsetY = this->charOffsetStack.front().charOffsetY;

									return;
								}

								charPosDisplay.charOffsetY = DiffManager(charPosDisplay.charOffsetY,
									[&] (const std::wstring_view& controlParam) {
										const auto size = _stof(controlParam);
										return size;
									});
								});

							break;
						}

						// ------------
						// shake
						// ------------

						if (StringViewIEqu(controlStr, L"Shake")) {
							if (CurrentCommandIgnored()) { break; }

							StackManager(shakeStack, shakeFormat, [&] (ShakeControl& shakeControl) {
								auto& paramParser = controlParam;
								controlParams.clear();

								do {

								} while (ParseParamReverse(paramParser
									, [&controlParams] (std::wstring_view& param) {
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
								} while (false);

								if (shakeControl.shakeType != ShakeType::ShakeType_None) {
									this->bShake = true;
								}

								shakeControl.charOffset *= 360;
								});

							break;
						}

						// ------------
						// color
						// ------------

						if (StringViewIEqu(controlStr, L"Color")
							|| StringViewIEqu(controlStr, L"C")) {
							if (CurrentCommandIgnored()) { break; }

							StackManager(colorStack, colorFormat, [&] (Color& color) {
								// Reset
								if (StringViewIEqu(controlParam, L"!")) {
									color = colorStack.front();

									return;
								}

								const auto result = DiffManager(GetDWORD(colorStack.back()), [&] (std::wstring_view& controlParam) {
									// hex
									if ((controlParam[0] == L'#')
										|| (controlParam[0] == L'0'
										&& (controlParam[1] == L'x' || controlParam[1] == L'X'))) {
										return _h2d(controlParam.data(), controlParam.size());
									}

									// dec
									auto& paramParser = controlParam;
									controlParams.clear();

									do {

									} while (ParseParamReverse(paramParser
															   , [&controlParams] (std::wstring_view& param) {
																   controlParams.emplace_back(GetTrimmedStr(param));
															   }));

									DWORD result = 0;

									const auto size = controlParams.size();
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
								});

								color = GetColor(result, true);
								});

							break;
						}

#pragma region FontControl
						// ------------
						// font
						// ------------

						using FontFormatControlCallback = std::function<void(LOGFONT& logFont)>;

						auto FontFormatControl = [&](const FontFormatControlCallback& callBack) {
							StackManager(logFontStack, fontFormat, callBack);
						};

						if (StringViewIEqu(controlStr, L"Font")
							|| StringViewIEqu(controlStr, L"F")) {
							if (CurrentCommandIgnored()) { break; }

							FontFormatControl([&] (LOGFONT& newLogFont) {
								// Reset
								if (StringViewIEqu(controlParam, L"!")) {
									constexpr auto nameSz = LF_FACESIZE * sizeof(WCHAR);
									memcpy(newLogFont.lfFaceName, this->logFontStack.front().lfFaceName, nameSz);

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
							if (CurrentCommandIgnored()) { break; }

							FontFormatControl([&] (LOGFONT& newLogFont) {
								// Reset
								if (StringViewIEqu(controlParam, L"!")) {
									newLogFont.lfHeight = this->logFontStack.front().lfHeight;

									return;
								}

								newLogFont.lfHeight = DiffManager(newLogFont.lfHeight, [&] (const std::wstring_view& controlParam) {
									const auto size = _stoi(controlParam);									
									const auto newSize = GetFontSize(size);

									return newSize;
									});
									});

							break;
						}

						if (StringViewIEqu(controlStr, L"Bold")
							|| StringViewIEqu(controlStr, L"B")) {
							if (CurrentCommandIgnored()) { break; }

							FontFormatControl([&] (LOGFONT& newLogFont) {
								newLogFont.lfWeight = FW_BOLD;
								});

							break;
						}

						if (StringViewIEqu(controlStr, L"!Bold")
							|| StringViewIEqu(controlStr, L"!B")) {
							if (CurrentCommandIgnored()) { break; }

							FontFormatControl([&] (LOGFONT& newLogFont) {
								newLogFont.lfWeight = FW_NORMAL;
								});

							break;
						}

						if (StringViewIEqu(controlStr, L"Italic")
							|| StringViewIEqu(controlStr, L"I")) {
							if (CurrentCommandIgnored()) { break; }

							FontFormatControl([&] (LOGFONT& newLogFont) {
								newLogFont.lfItalic = TRUE;
								});

							break;
						}

						if (StringViewIEqu(controlStr, L"!Italic")
							|| StringViewIEqu(controlStr, L"!I")) {
							if (CurrentCommandIgnored()) { break; }

							FontFormatControl([&] (LOGFONT& newLogFont) {
								newLogFont.lfItalic = FALSE;
								});

							break;
						}

						if (StringViewIEqu(controlStr, L"Underline")
							|| StringViewIEqu(controlStr, L"U")) {
							if (CurrentCommandIgnored()) { break; }

							FontFormatControl([&] (LOGFONT& newLogFont) {
								newLogFont.lfUnderline = TRUE;
								});

							break;
						}

						if (StringViewIEqu(controlStr, L"!Underline")
							|| StringViewIEqu(controlStr, L"!U")) {
							if (CurrentCommandIgnored()) { break; }

							FontFormatControl([&] (LOGFONT& newLogFont) {
								newLogFont.lfUnderline = FALSE;
								});

							break;
						}

						if (StringViewIEqu(controlStr, L"StrikeOut")
							|| StringViewIEqu(controlStr, L"S")) {
							if (CurrentCommandIgnored()) { break; }

							FontFormatControl([&] (LOGFONT& newLogFont) {
								newLogFont.lfStrikeOut = TRUE;
								});

							break;
						}

						if (StringViewIEqu(controlStr, L"!StrikeOut")
							|| StringViewIEqu(controlStr, L"!S")) {
							if (CurrentCommandIgnored()) { break; }

							FontFormatControl([&] (LOGFONT& newLogFont) {
								newLogFont.lfStrikeOut = FALSE;
								});

							break;
						}
#pragma endregion
#pragma endregion

						bValidCommand = false;
					} while (false);

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

		// ------------
		// valid check
		// ------------
		const bool bAllowEmptyChar = flags & FORMAT_IGNORE_AllowEmptyCharStringAfterFormatParsing;

		if (!TextValid(pText, &pTextLen, bAllowEmptyChar)) {
			this->bTextValid = false;
			return;
		}

		// ------------
		// after parse
		// ------------
		// handle open trigger tags
		if (!this->triggerFormat.empty() && this->triggerFormat.back().triggerLength == 0) {
			auto& lastTrigger = this->triggerFormat.back();
			lastTrigger.triggerLength = pTextLen - lastTrigger.startWithNewLine;
		}
	}

private:
	template<typename Format>
		requires std::is_base_of_v<FormatBasic, Format>
	struct IteratorHandler {
	private:
		using IT = decltype(static_cast<std::vector<Format>*>(nullptr)->begin());

	public:
		std::vector<Format>* pVector = nullptr;
		IT it;

		IteratorHandler(std::vector<Format>& vector) {
			pVector = &vector;
			it = vector.begin();
		}

		inline IT GetInit() {
			return pVector->begin();
		}

		inline bool End() {
			return it == pVector->end();
		}

		inline void Reset() {
			it = pVector->begin();
		}

		inline void Forward(size_t position, const std::function<void(IT it)>& callback) {
			if (it != pVector->end() && position >= it->start) {
				callback(it);

				++it;
			}
		}

		inline void ForwardWithNewLine(size_t position, const std::function<void(IT it)>& callback) {
			if (it != pVector->end() && position >= it->startWithNewLine) {
				callback(it);

				++it;
			}
		}

		inline void UpdateIt(size_t start) {
			while (it != pVector->end() && it->start <= start) {
				++it;
			}

			--it;
		}
	};

public:
	inline CharPos CalculateRange(LPRECT pRc) {
		const bool bRectNotChanged = pRc->bottom == previousRC.bottom
			&& pRc->top == previousRC.top
			&& pRc->left == previousRC.left
			&& pRc->right == previousRC.right;

		// skip calculate
		if (!this->bFormatUpdated && bRectNotChanged) {
			return previousCharPos;
		}

		previousRC = *pRc;

		this->strPos.clear();

		if (!this->bTextValid) {
			return { this->defaultCharSz.width / 2,this->defaultCharSz.height / 2 };
		}

		delete[] this->pCharSizeArr;
		this->pCharSizeArr = nullptr;

		pCharSizeArr = new CharSize[pTextLen + 1];

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

		auto fontItHandler = IteratorHandler(this->fontFormat);
		auto localLogFont = fontItHandler.GetInit()->logFont;

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

					if (pRegexCache->NotEmpty(curChar)) {
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

				auto curChar = pCurChar[0];
				auto nextChar = pCurChar[1];

				fontItHandler.ForwardWithNewLine(pChar, [&] (auto fontIt) {
					localLogFont = fontIt->logFont;
				});

				// word break
				auto bCurNotCJK = pRegexCache->NotCJK(curChar);

				auto InWord = [&] () {
					bInWord = true;

					notCJKStart = pChar;
					notCJKStartWidth = curWidth;
					};

				auto OutWord = [&] () {
					bInWord = false;

					//notCJKStart = -1;
					//notCJKStartWidth = -1;
					};

				auto WB_AbleToNextLine = [&] () {return (curWidth - notCJKStartWidth) < (size_t)rcWidth; };
				auto WB_Backword = [&] () {
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
				auto getCharSize = [&] () {
					if (curChar != CHAR_TAB) [[likely]] {
						return GetCharSizeWithCache(curChar, localLogFont);
					}
					else {
						// update context
						tabContext.UpdateContext(curWidth,
							GetCharSizeWithCache(tabContext.spaceChar, localLogFont));
						return tabContext.GetTabCharSize();
					}
				};
				
				pCharSizeArr[pChar] = getCharSize();
				auto charSz = &pCharSizeArr[pChar];

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
					const bool bTooNarrow = charSz->width > rcWidth;

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

					auto previousCharSz = &pCharSizeArr[pChar - 1];

					auto CanRewindWidth = [&] () {
						return curWidth - (previousCharSz->width + nColSpace + charSz->width) > 0;
					};
					auto RewindWidth = [&] () {
						curWidth -= charSz->width;
						curWidth -= nColSpace;
					};
					auto Punc_Backward = [&]() {
						// unable to get back
						if (pChar == pCharStart) {
							return;
						}

						if (pRegexCache->NotCJK(PreviousChar)) {
							if (WB_AbleToNextLine()) {
								WB_Backword();
							}

							return;
						}

						if (!CanRewindWidth()) {
							return;
						}

						pChar--;

						RewindWidth();
						curWidth -= previousCharSz->width;
					};

					// only one, merge to current line
					if (NotAtStart(curChar)) {
						notAtStartCharPos = pChar;

						// more than one, both move to next line
						if (NotAtStart(nextChar)) {
							Punc_Backward();

							break;
						}
					}
					else {
						if (NotAtEnd(PreviousChar)) {
							Punc_Backward();
						}
						else {
							RewindWidth();
						}

						break;
					}
				}

				if (IsNewLine(curChar, nextChar)) {
					HandleNewLine();

					break;
				}

				pChar++;

				if ((pText + pChar)[0] == L'\0') {
					break;
				}

				if (pChar > pTextLen) {
					break;
				}

				curWidth += nColSpace;
			}

			if (!skipLine) {
#ifdef _DEBUG
				std::wstring strLeft(pText + pChar);
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

		// fix empty (e.g., \r\n only) string crash
		if (strPos.empty()) {
			return CharPos{};
		}

		const auto& lastStrPos = strPos.back();
		const auto lastCharSize = &pCharSizeArr [lastStrPos.start + lastStrPos.length - 1];

		this->startY = GetStartPosY(totalHeight - nRowSpace, rcHeight);

		// last char pos
		previousCharPos = CharPos{
			GetStartPosX(lastStrPos.width, rcWidth) - lastCharSize->width / 4
				+ lastStrPos.width + (lastCharSize->width / 2),
			startY + lastStrPos.y + (lastCharSize->height / 2),
			// if render by this size, space is not enough
			maxWidth + nColSpace,
			totalHeight,
			// col / row space removed
			//maxWidth,
			//totalHeight - nRowSpace,
			ShakeControl()
		};

		return previousCharPos;
	}

	struct RenderOptions {
		// ------------
		// visable ratio
		// ------------

		// let character be displayed gradually
		// displayed char / total char
		// 1.0 -> display all
		// should between 0.0 ~ 1.0
		double visibleRatio = 1.0;

		// if alpha is included, E.g., 10 char, display 5
		// not included: 5 / 10 = 0.5, display 5 chars
		// included: (4 * 255 + 125) / 10 * 255 = 0.45, display 4 char
		// last with 50% extra transparency
		bool bIncludeAlpha = true;

		// Characters that need to render
		size_t textLen = 0;
		size_t renderCharCount = 0;

		constexpr static UCHAR MaxAlpha = 255;

		inline size_t GetRenderCharCount(size_t textLen) {
			this->textLen = textLen;
			renderCharCount = static_cast<size_t>(std::ceil(visibleRatio * textLen));

			return renderCharCount;
		}

		inline UCHAR GetRenderCharAlpha(size_t curChar) const {
			if (!bIncludeAlpha) { return MaxAlpha; }

			const auto opaqueRatio = static_cast<double>(curChar + 1) / textLen;

			if (opaqueRatio < visibleRatio) { return MaxAlpha; }

			const auto alpha = MaxAlpha * (1 - (opaqueRatio - visibleRatio) * textLen);

			return static_cast<UCHAR>(alpha);
		}

		// ------------
		// clip
		// ------------

		// clip: don't render character that out of screen
		bool bClip = false;
		// clip to object: don't render character that out of object
		bool bClipToObject = false;
		// char clipped during rendering
		bool bCharClipped = false;

		// render size: the frame size
		RECT renderRect = { 0,0,65535,65535 };

		// clip: don't render character that out of screen
		inline void SetClip(const bool bClip, const int renderWidth, const int renderHeight) {
			this->bClip = bClip;
			this->renderRect = { 0,0,renderWidth,renderHeight };
		}
		// clip to object: don't render character that out of object
		inline void SetClipToObject(const bool bClip) {
			this->bClipToObject = bClip;
		}

		// rect has part inside render rect
		inline bool RectInside(const RECT& rect) const {
			return (rect.left < renderRect.right
				&& rect.right > renderRect.left
				&& rect.top < renderRect.bottom
				&& rect.bottom > renderRect.top);
		}

		inline bool ClipChar(int objectX, int objectY,
			const int charX, const int charY, const CharSize* charSz) {
			if (!this->bClip) { return false; }
			if (this->bClipToObject) { objectX = 0; objectY = 0; }

			const auto bRet = !RectInside({ objectX + charX,
				objectY + charY,
				objectX + charX + charSz->width,
				objectY + charY + charSz->height });

			this->bCharClipped |= bRet;

			return bRet;
		}

		// ------------
		// render callbacks
		// ------------
	private:
		using PositionCallback = std::function<bool(const CharSize* pCharSize,
			CharPos* pCharPos,
			float& x, float& y)>;
		using GraphicCallback = std::function<Graphics* ()>;

	public:
		// used to override position
		PositionCallback positionCallback = nullptr;
		// used to override graphic object
		GraphicCallback graphicCallback = nullptr;

		inline void UpdateRenderCallback(const PositionCallback& positionCallback,
			const GraphicCallback& graphicCallback) {
			this->positionCallback = positionCallback;
			this->graphicCallback = graphicCallback;
		}

		// ------------
		// tag callbacks
		// ------------
	private:
		// trigger custom tag callback when rendering
		using TagCallback = std::function<void(const std::wstring&, const std::vector<std::wstring>&)>;

	public:
		// only index lager than this will be triggered
		size_t tagCallbackIndex = static_cast<size_t>(-1);
		TagCallback tagCallback = nullptr;

		inline void UpdateTagCallback(const TagCallback& tagCallback) {
			this->tagCallback = tagCallback;
		}

		// update to current raw text length
		inline bool UpdateTagCallbackIndex(const NeoStr* pNeoStr) {
			if (pNeoStr == nullptr) { return false; }

			const auto rawTextLen = [&] ()->size_t {
				const auto pRawText = pNeoStr->GetRawText();

				if (pRawText == nullptr) { return 0; }

				// index start from 0
				const auto len = wcslen(pRawText) - 1;	

				return len;
			}();

			const bool bChanged = this->tagCallbackIndex != rawTextLen;
			this->tagCallbackIndex = rawTextLen;

			return bChanged;
		}
	};

// use render target -> slower
//#define USE_RTT
// do not re alloc HWA each time -> no difference 
//#define REUSE_HWA
// call tag callback after render, in case mid-tag references later char pos
#define CALL_TAGCALLBACK_AFTERRENDER

	// pRc: object rect
	inline void RenderPerChar(LPRECT pRc, RenderOptions& opt) {
#ifdef COUNT_GDI_OBJECT
		GDIObjectCounter objectCounter;

		objectCounter.UpdateObjectCount();
#endif
		// ------------
		// check validity & alloc
		// ------------
		if (!this->bTextValid) {
			return;
		}

		delete[] this->pCharPosArr;
		this->pCharPosArr = nullptr;

		pCharPosArr = new CharPos[pTextLen + 1];

		rcWidth = pRc->right - pRc->left;
		rcHeight = pRc->bottom - pRc->top;

		opt.GetRenderCharCount(pTextLen);

		// ------------
		// render override
		//		used for remark rendering, render to parent directly
		//		so child object doesn't need to alloc and render to surface
		// ------------
		bool bRenderOverride = false;
		Graphics* pGraphic = nullptr;

		if (opt.graphicCallback != nullptr) {
			auto pGraphicOverride = opt.graphicCallback();

			if (pGraphicOverride != nullptr) {
				pGraphic = pGraphicOverride;
				bRenderOverride = true;
			}
		}

		// ------------
		// update offscreen surface
		// ------------
		if (!bRenderOverride) {
			auto width = rcWidth + this->borderOffsetX * 2;
			auto height = totalHeight + this->borderOffsetY * 2;

			opt.bCharClipped = false;
			if (opt.bClipToObject) {
				opt.SetClip(true, rcWidth, rcHeight);
			}

			width = (std::min)(D3D11_TEXTURE_SIZE, width);
			height = (std::min)(D3D11_TEXTURE_SIZE, height);
#ifdef _DEBUG
			if(width == D3D11_TEXTURE_SIZE|| height == D3D11_TEXTURE_SIZE) {
				OutputDebugStringW(L"Texture Size Invalid, Shrink");
			}
#endif

			if (this->pMemSf == nullptr
				|| this->pMemSf->GetWidth() < width
				|| this->pMemSf->GetHeight() < height) {
				delete this->pMemSf;
				this->pMemSf = nullptr;

				pMemSf = CreateSurface(32, width, height);
				pMemSf->CreateAlpha();

				delete this->pBitmap;
				this->pBitmap = nullptr;

				// Use PixelFormat32bppPARGB or manually premul alpha of font color
				// call fusion premul is still needed, as alpha channel is used for
				// anti-aliasing, it's not involved to font color or pixel format.
				this->pBitmap = new Bitmap(width, height, PixelFormat32bppARGB);

#ifdef REUSE_HWA
				delete pHwaSf;
				pHwaSf = nullptr;

				pHwaSf = CreateHWASurface(32, width, height, ST_HWA_ROMTEXTURE, this->hwaDriver);
#endif

#ifdef USE_RTT
				delete pHwaSf;
				pHwaSf = nullptr;

				pHwaSf = CreateHWASurface(32, width, height, ST_HWA_RTTEXTURE, this->hwaDriver);
				pHwaSf->CreateAlpha();
#endif
			}

			// don't need to reset surface, as it wll be overwritten later
			//pMemSf->Fill(BLACK);
			//_ForceAddAlpha(pMemSf, 0);

#ifdef _DEBUG
			auto type = pMemSf->GetType();
#endif
		}

		// ------------
		// begin rendering
		// ------------
		if (!bRenderOverride) {
			pGraphic = new Graphics(pBitmap);

			pGraphic->Clear(Color(0, 0, 0, 0));

			pGraphic->SetTextRenderingHint(this->textRenderingHint);
			pGraphic->SetSmoothingMode(this->smoothingMode);
			pGraphic->SetPixelOffsetMode(this->pixelOffsetMode);
		}			

		//Color fontColor(255, 50, 150, 250);
		// Color fontColor(255, GetRValue(this->dwTextColor), GetGValue(this->dwTextColor), GetBValue(this->dwTextColor));
		Color fontColor(this->dwTextColor);
		SolidBrush solidBrush(fontColor);

		size_t totalChar = 0;

		// non-stack based
		auto triggerItHandler = IteratorHandler(this->triggerFormat);

		auto tagCallbackHandler = [&] (auto tagIt) {
			if (opt.tagCallback == nullptr) { return; }
			if (tagIt->rawStart <= opt.tagCallbackIndex) { return; }

			opt.tagCallback(tagIt->callbackName, tagIt->callbackParams);
		};

#ifndef CALL_TAGCALLBACK_AFTERRENDER
		auto tagItHandler = IteratorHandler(this->tagFormat);
#endif
		auto remarkItHandler = IteratorHandler(this->remarkFormat);
		auto iConItHandler = IteratorHandler(this->iConFormat);

		// stack based
		struct AlignHandler {
			DWORD* pOldAlign = nullptr;
			DWORD oldAlign = 0;

			AlignHandler(DWORD* pOldAlign, DWORD newAlign) {
				if (*pOldAlign == newAlign) { return; }

				this->pOldAlign = pOldAlign;
				this->oldAlign = *pOldAlign;

				*this->pOldAlign = newAlign;
			}

			~AlignHandler() {
				if (this->pOldAlign == nullptr) { return; }

				*this->pOldAlign = this->oldAlign;
			}
		};

		auto charOffsetDisplay = this->charOffsetDisplay;
		auto charOffsetItHandler = IteratorHandler(this->charOffsetFormat);
		auto formatAlign = this->dwDTFlags;
		auto alignItHandler = IteratorHandler(this->alignFormat);
		auto colorItHandler = IteratorHandler(this->colorFormat);
		auto fontItHandler = IteratorHandler(this->fontFormat);

		auto shakeItHandler = IteratorHandler(this->shakeFormat);
		auto localShakeFormat = this->bShake
			? *shakeItHandler.GetInit()
			: FormatShake();

		// update timer if shake or child shake
		if (this->GetShakeUpdateState()) { shakeTimer++; }

		// ------------
		// render & calculate position of each char
		// ------------
		// Exceptions used to terminate render, should be catched and do nothing
		struct NeoStrRenderException final :std::exception {
		private:
			int flag = 0;

		public:
			NeoStrRenderException(char const* const pMsg, int flag = 0) noexcept
				:std::exception(pMsg) {
				this->flag = flag;
			}

			inline int GetFlag() const noexcept { return this->flag; }
		};

		try {
			for (auto& curStrPos : this->strPos) {
#ifdef _DEBUG
				std::wstring str(pText + curStrPos.start, curStrPos.length);
#endif // _DEBUG

				CharSize* charSz = nullptr;

#define NoGDIPlusOffset

				auto GetGDIPlusOffset = [&] () {
#ifdef NoGDIPlusOffset
					return 0;
#else
					// https://blog.csdn.net/stevenkoh/article/details/22777295
					const auto firstChar = (pText + curStrPos.start)[0];
					const auto coef = pRegexCache->NotCJK(firstChar) ? 2.75 : 8;

					return static_cast<int>(pCharSizeArr[curStrPos.start].width / coef);
#endif
					};

				// ReSharper disable once CppVariableCanBeMadeConstexpr
				const auto GDIPlusOffset = GetGDIPlusOffset();

				// ---------
				// update iterator
				// ---------

				// stack based
				alignItHandler.Forward(totalChar, [&] (auto alignIt) {
					formatAlign = alignIt->dwDTFlags;
				});

				// RAII: align only has effect for current line
				auto alignHandler = AlignHandler(&this->dwDTFlags, formatAlign);
				int x = GetStartPosX(curStrPos.width, rcWidth);
				x -= GDIPlusOffset;

				for (size_t curChar = 0; curChar < curStrPos.length; curChar++, totalChar++) {
					auto offset = curStrPos.start + curChar;
					if (offset >= opt.renderCharCount) {
						throw NeoStrRenderException("Exceed visable ratio");
					}

					// get char size
					auto pCurChar = pText + offset;
					charSz = &pCharSizeArr[offset];


					// ---------
					// update iterator
					// ---------

					// stack based
					charOffsetItHandler.Forward(totalChar, [&] (auto charPosIt) {
						charOffsetDisplay = charPosIt->charOffsetDisplay;
					});

					const double charOffsetX = charOffsetDisplay.charOffsetX * static_cast<double>(charSz->width);
					const double charOffsetY = charOffsetDisplay.charOffsetY * static_cast<double>(charSz->height);

					// corrected position (no border offset)
					const double displayX = x + charOffsetX;
					const double displayY = curStrPos.y + charOffsetY;

					// position relative to object left top
					pCharPosArr[offset] = CharPos{ static_cast<long>(displayX + GDIPlusOffset),
													static_cast<long>(this->startY + displayY),
													0,0,
													ShakeControl() };

					const auto& curCharPos = pCharPosArr[offset];

#pragma region FORMAT_IT
					// position relative to bitmap for rendering
					auto positionX = static_cast<float>(displayX + this->borderOffsetX);
					auto positionY = static_cast<float>(displayY + this->borderOffsetY);

					// update position
					if (opt.positionCallback != nullptr) {
						if (!opt.positionCallback(charSz, &pCharPosArr[offset], positionX, positionY)) {
							throw NeoStrRenderException("Exceed remark base length");
						}
					}

					// ---------
					// update iterator
					// ---------

					// stack based
					colorItHandler.Forward(totalChar, [&] (auto colorIt) {
						solidBrush.SetColor(colorIt->color);
					});
					fontItHandler.Forward(totalChar, [&] (auto fontIt) {
						this->pFont = GetFontPointerWithCache(fontIt->logFont);
					});
					shakeItHandler.Forward(totalChar, [&] (auto shakeIt) {
						localShakeFormat = *shakeIt;
					});

					if (this->bShake) {
						double shakeOffset = GetShakeOffset(localShakeFormat, totalChar);
						GetShakePosition(localShakeFormat.shakeControl, shakeOffset, positionX, positionY, charSz);
					}

					// non-stack based
					triggerItHandler.Forward(totalChar, [&] (auto triggerIt) {
						triggerIt->pCharSizeArr = &pCharSizeArr[offset];
						triggerIt->pCharPosArr = &pCharPosArr[offset];
					});
#ifndef CALL_TAGCALLBACK_AFTERRENDER
					tagItHandler.Forward(totalChar, [&] (auto tagIt) {
						tagCallbackHandler(tagIt);
					});
#endif
					remarkItHandler.Forward(totalChar, [&] (auto remarkIt) {
						remarkIt->pCharSizeArr = &pCharSizeArr[offset];
						remarkIt->pCharPosArr = &pCharPosArr[offset];
						//remarkIt->validLength = (std::min)(pTextLen - offset, remarkIt->baseLength);
						remarkIt->validLength = (std::min)(opt.renderCharCount - offset, remarkIt->baseLength);
					});
					iConItHandler.Forward(totalChar, [&] (auto iConIt) {
						// use updated position
						iConIt->x = static_cast<size_t>(positionX);
						iConIt->y = static_cast<size_t>(positionY);
					});
#pragma endregion

					if (!opt.ClipChar(pRc->left, pRc->top,
						curCharPos.x, curCharPos.y, charSz)) {
						struct ColorUpdater {  // NOLINT(cppcoreguidelines-special-member-functions)
							SolidBrush* pBrush = nullptr;
							SolidBrush oldBrush = SolidBrush(Color());

							ColorUpdater(SolidBrush* pBrush, const UCHAR alpha) {
								if (alpha == RenderOptions::MaxAlpha) {
									return;
								}

								// Update alpha
								Color c;
								pBrush->GetColor(&c);
								this->pBrush = pBrush;
								this->oldBrush.SetColor(c);

								const auto oldAlpha = c.GetAlpha();
								const auto newAlpha = static_cast<UCHAR>(static_cast<double>(oldAlpha * alpha) / RenderOptions::MaxAlpha);

								const auto oldColor = c.GetValue();
								const auto newColor = (newAlpha << 24)
									| (oldColor & 0x00FFFFFF);

								c.SetValue(newColor);
								pBrush->SetColor(c);
							}
							~ColorUpdater() {
								if (pBrush == nullptr) { return; }

								Color c;
								oldBrush.GetColor(&c);
								pBrush->SetColor(c);
							}
						};

						// RAII
						ColorUpdater colorUpdater(&solidBrush,
							opt.GetRenderCharAlpha(offset));

						//Gdiplus::FontStyle style = (Gdiplus::FontStyle)this->pFont->GetStyle();
						auto status = pGraphic->DrawString(pCurChar, 1, pFont,
							PointF(positionX, positionY),
#ifdef NoGDIPlusOffset
							StringFormat::GenericTypographic(),
#endif
							&solidBrush);

						if (bDrawRectangle) {
							auto blackPen = Pen(&solidBrush, 1);
							status = pGraphic->DrawRectangle(&blackPen,
								positionX, positionY,
								static_cast<float>(charSz->width),
								static_cast<float>(charSz->height));
						}
						//assert(status == Status::Ok);
					}

					x += (charSz->width + nColSpace);
				}
			}
		} catch([[maybe_unused]] NeoStrRenderException& e) {  // NOLINT(bugprone-empty-catch)
			// use exception to jump out of the loop if exceeds the
			// render char count, so nothing to handle here
		}

		this->GetTriggerRect();
#ifdef CALL_TAGCALLBACK_AFTERRENDER
		for (auto it = this->tagFormat.begin(); it != this->tagFormat.end();++it) {
			tagCallbackHandler(it);
		}
#endif

		// ------------
		// handle remark
		// ------------
		fontItHandler.Reset();
		colorItHandler.Reset();

		auto remarkDisplayItHandler = IteratorHandler(this->remarkDisplayFormat);

		// copy one option here
		RenderOptions remarkOpt = opt;
		// do not clip
		remarkOpt.SetClip(false, 65535, 65535);
		remarkOpt.SetClipToObject(false);
		// skip custom tag callback
		remarkOpt.UpdateTagCallback(nullptr);

		for (auto& it : this->remarkFormat) {
			// no enough place
			if (it.validLength == 0) { continue; }
			// ReSharper disable once GrammarMistakeInComment
			// don't need to handle this as it's granted by validLength
			// if (it.start > opt.renderCharCount) { continue; }

			// update font & color
			fontItHandler.UpdateIt(it.start);
			colorItHandler.UpdateIt(it.start);
			remarkDisplayItHandler.UpdateIt(it.start);

			const auto& remarkDisplay = !remarkDisplayItHandler.End()
				? remarkDisplayItHandler.it->remarkDisplay
				: this->remarkDisplay;

			// create new object
			auto remarkLogFont = fontItHandler.it->logFont;
			remarkLogFont.lfHeight /= 2;

			auto remarkHFont = CreateFontIndirect(&remarkLogFont);

			if (it.pNeoStr == nullptr
				|| LogFontHasher(it.pNeoStr->logFont) != LogFontHasher(remarkLogFont)) {
				delete it.pNeoStr;
				it.pNeoStr = new NeoStr(this->dwDTFlags, this->dwTextColor, remarkHFont, this);
			}

			auto pRemarkNeoStr = it.pNeoStr;
			pRemarkNeoStr->CopyProperties(this);
			pRemarkNeoStr->SetColor(GetDWORD(colorItHandler.it->color), true);
			pRemarkNeoStr->SetBorderOffset(this->borderOffsetX, this->borderOffsetY);
			//remarkNeoStr->SetDrawRectangle(true);

			pRemarkNeoStr->GetFormat(it.remark.c_str(),
				this->previousFlag | FORMAT_IGNORE_AllowEmptyCharStringAfterFormatParsing,
				// if parent is forced, then child is forced (original, operation is not handled)
				this->bPreviousForced);

			// render in infinite rect
			RECT rc = {0,0,65535,65535};
			auto cPos = pRemarkNeoStr->CalculateRange(&rc);

			// calculate render coords
			const auto renderRatio = static_cast<double>(it.validLength) / it.baseLength;
			//const auto remarkCharCount = it.remark.length();
			const auto remarkCharCount = wcslen(pRemarkNeoStr->GetText());
			const auto renderCharCount = static_cast<size_t>(std::ceil(renderRatio * remarkCharCount));

			struct Position {
				float x = 0;
				float y = 0;
			};

			std::vector<Position> mainPositions(it.validLength);
			std::vector<Position> remarkPositions(renderCharCount);

			// get valid old positions
			for (size_t idx = 0; idx < mainPositions.size(); idx++) {
				auto pCharPos = &it.pCharPosArr[idx];
				auto pPos = &mainPositions[idx];

				pPos->x = static_cast<float>(pCharPos->x);
				pPos->y = static_cast<float>(pCharPos->y - this->startY);
			}
					
			// Todo handle line change, it's ignored in current version
			// in different lines -> not equal
			if (std::abs(mainPositions.rbegin()->y - mainPositions.begin()->y) > 1e-6) {
				continue;
			}

			// correct base char size
			// which is the mean value of all usable characters
			auto CorrectCharSize = [&] (const CharSize* pCharSizeSrc, size_t sz) {
				CharSize charSizeCorrect = { 0,0 };

				for (size_t idx = 0; idx < sz; idx++) {
					auto pCharSize = &pCharSizeSrc[idx];

					if (pCharSize == nullptr) {
						pCharSize = &pRemarkNeoStr->defaultCharSz;
					}

					charSizeCorrect.width += pCharSize->width;
					charSizeCorrect.height += pCharSize->height;
				}

				charSizeCorrect.width = static_cast<long>(static_cast<float>(charSizeCorrect.width) / static_cast<float>(sz));
				charSizeCorrect.height = static_cast<long>(static_cast<float>(charSizeCorrect.height) / static_cast<float>(sz));

				return charSizeCorrect;
			};

			// esitmate char size
			auto estimateBaseCharSize = mainPositions.empty()
				? GetCharSizeWithCache(DEFAULT_CHARACTER, fontItHandler.it->logFont)
				: CorrectCharSize(it.pCharSizeArr, mainPositions.size());
			auto estimateRemarkCharSize = remarkPositions.empty()
				? GetCharSizeWithCache(DEFAULT_CHARACTER, remarkLogFont)
				: CorrectCharSize(pRemarkNeoStr->pCharSizeArr, remarkPositions.size());

			// calculate position
			const auto baseWidth = estimateBaseCharSize.width * it.baseLength;
			const auto remarkWidth = (std::max)(baseWidth,
				estimateRemarkCharSize.width * remarkCharCount);
			const auto remarkCenterPos = mainPositions.begin()->x + baseWidth / 2.0;

			const auto remarkSize = static_cast<size_t>(std::ceil(static_cast<double>(remarkWidth) / remarkCharCount));

			// odd & even affects 0.5
			const auto remarkCenterIdx = static_cast<double>(remarkCharCount / 2) - 0.5 * (remarkCharCount % 2 == 0);  // NOLINT(bugprone-integer-division)

			for (size_t idx = 0; idx < remarkPositions.size(); idx++) {
				const auto distance = remarkCenterIdx - static_cast<long>(idx);
				auto& curPos = remarkPositions[idx];

				curPos.x = static_cast<float>(remarkCenterPos - static_cast<double>(distance) * remarkSize - estimateRemarkCharSize.width / 2.0);
				curPos.y = mainPositions.begin()->y;
			}
			
			auto remarkPosIt = remarkPositions.begin();

			// update ratio if not display all
			if (opt.visibleRatio < 1.0) {
				const auto ratio = static_cast<double>(opt.renderCharCount - it.start) / it.baseLength;
				remarkOpt.visibleRatio = Range(ratio, 0.0, 1.0);
			}

			// overrider render, directly to parent
			remarkOpt.UpdateRenderCallback(
				[&](const CharSize* pCharSize,
				CharPos* pCharPos,
				float& x, float& y) {
					if (remarkPosIt == remarkPositions.end()) { return false; }
					// get char pos offsets
					const auto remarkX = x - static_cast<float>(pRemarkNeoStr->borderOffsetX);
					const auto remarkY = y - static_cast<float>(pRemarkNeoStr->borderOffsetY);

					const auto remarkGDIPlusOffset = static_cast<float>(pCharPos->x) - remarkX;
					const auto remarkstartY = pRemarkNeoStr->startY;

					// render position
					x = remarkPosIt->x;
					y = remarkPosIt->y;

					// make enough space
					y -= static_cast<float>(pCharSize->height);

					// remark offset
					x += remarkDisplay.remarkOffsetX * static_cast<float>(estimateBaseCharSize.width);
					y += remarkDisplay.remarkOffsetY * static_cast<float>(estimateBaseCharSize.height);

					// update char pos
					pCharPos->x = static_cast<long>(x + remarkGDIPlusOffset);
					pCharPos->y = static_cast<long>(y) + remarkstartY;


					// add offset
					x += static_cast<float>(this->borderOffsetX);
					y += static_cast<float>(this->borderOffsetY);

					++remarkPosIt;

					return true;
				},			
				[&] () {
					return pGraphic;
				});

			// shake timer need to be updated
			pRemarkNeoStr->SetShakeTimer(this->shakeTimer);
			pRemarkNeoStr->RenderPerChar(&rc, remarkOpt);

			// update shake state here, as child's child may shake
			this->bChildShake |= pRemarkNeoStr->GetShakeUpdateState();

			DeleteObject(remarkHFont);
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
		//const auto fileName = std::wstring(this->pRawText).substr(0, 4);
		//const auto path = std::format(L"D:\\{}.png", fileName);
		//pBitmap->Save(path.c_str(), &pngClsid, NULL);
#endif // _DEBUG

		if (!bRenderOverride) {
			delete pGraphic; pGraphic = nullptr;
		}
		else { return; }

		// ------------
		// copy to surface
		// ------------
		const auto copyWidth = static_cast<int>(pBitmap->GetWidth());
		const auto copyHeight = static_cast<int>(pBitmap->GetHeight());

		BitmapData bitmapData;
		auto bitmapRect = Rect(0, 0, copyWidth, copyHeight);
		auto lockBitsRet = pBitmap->LockBits(&bitmapRect, ImageLockMode::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
		//auto lockBitsRet = pBitmap->LockBits(&bitmapRect, ImageLockMode::ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);
		if (lockBitsRet != Gdiplus::Status::Ok) { return; }

		auto pRawBitmap = (unsigned int*)bitmapData.Scan0;   // for easy access and indexing

		auto sfCoef = GetSfCoef(pMemSf);
		if (sfCoef.pData == nullptr || sfCoef.pAlphaData == nullptr) {
			return;
		}

		auto lineSz = sfCoef.pitch;

		for (int y = 0; y < copyHeight; y++) {
			const auto line = (copyHeight - 1 - y);
			const auto pBmpOffset = line * bitmapData.Stride / 4;

			auto pData = sfCoef.pData + y * sfCoef.pitch;
			auto pBmp = pRawBitmap + pBmpOffset;

			memcpy(pData, pBmp, lineSz);

			const auto pSfAlphaOffset = sfCoef.pAlphaData + line * sfCoef.alphaPitch;
			const auto pBitmapAlphaOffset = pRawBitmap + pBmpOffset;

			for (int x = 0; x < copyWidth; x++) {
				auto pAlphaData = pSfAlphaOffset + x * sfCoef.alphaByte;
				auto curAlpha = pBitmapAlphaOffset + x;
				pAlphaData [0] = (*curAlpha & 0xff000000) >> 24;
			}
		}

		ReleaseSfCoef(pMemSf, sfCoef);
		pBitmap->UnlockBits(&bitmapData);

		// ------------
		// handle icon
		// ------------
		auto RenderICon = [] (this auto&& self, NeoStr* pFormat, NeoStr* pRender)->void {
			auto stretchFlags = pFormat->iConDisplay.iConResample
				? STRF_RESAMPLE | STRF_RESAMPLE_TRANSP
				: 0;

			stretchFlags |= STRF_COPYALPHA;

			auto fontItHandler = IteratorHandler(pFormat->fontFormat);
			auto iConDisplayItHandler = IteratorHandler(pFormat->iConDisplayFormat);

			for (auto& it : pFormat->iConFormat) {
				fontItHandler.UpdateIt(it.start);
				iConDisplayItHandler.UpdateIt(it.start);

				const bool bEnd = fontItHandler.End();

				const auto& charSize = !bEnd
					? pFormat->GetCharSizeWithCache(DEFAULT_CHARACTER, fontItHandler.it->logFont)
					: pFormat->defaultCharSz;
				const auto& tm = !bEnd
					? pFormat->GetCharSizeCacheIt(fontItHandler.it->logFont)->second.tm
					: pFormat->tm;

				const auto& iConDisplay = !iConDisplayItHandler.End()
					? iConDisplayItHandler.it->iConDisplay
					: pFormat->iConDisplay;

				CharSize iConSize = charSize;

				iConSize.width = int(iConSize.width * iConDisplay.iConScale);
				iConSize.height = int(iConSize.height * iConDisplay.iConScale);

				const auto iConXOffset = int(iConDisplay.iConOffsetX * charSize.width
					+ (static_cast<double>(charSize.width) - iConSize.width) / 2
					+ charSize.width / 6.0);
				const auto iConYOffset = int(iConDisplay.iConOffsetY * charSize.width
					+ (static_cast<double>(charSize.height) - iConSize.height) / 2
					+ (charSize.height - charSize.width)
					- tm.tmDescent /*- tm.tmExternalLeading*/);

				LPSURFACE pSf = nullptr;
				auto bFound = it.hImage != FORMAT_INVALID_ICON;

				if (bFound) {
					const auto IConLibIt = pFormat->pIConData->pIConLib->find(it.hImage);
					pSf = IConLibIt->second;
				}
				else {
					pFormat->pIConData->GetDefaultICon();
					pSf = pFormat->pIConData->pDefaultICon;
				}

				LPSURFACE pStrecthSf = nullptr;

				auto key = IConData::ResizeCacheKey{ pSf , iConSize.width, iConSize.width }.GetHash();
				auto resizeCacheIt = pFormat->pIConData->resizeCache.find(key);

				if (resizeCacheIt == pFormat->pIConData->resizeCache.end()) {
					pStrecthSf = CreateSurface(32, iConSize.width, iConSize.width);
					pStrecthSf->CreateAlpha();

					auto ret = pSf->Stretch(*pStrecthSf, stretchFlags);

					pFormat->pIConData->resizeCache[key] = pStrecthSf;
				}
				else {
					pStrecthSf = resizeCacheIt->second;
				}

				const auto blitFlags = pFormat->iConDisplay.iConResample
										   ? BLTF_ANTIA
										   : 0;

				pStrecthSf->Blit(*pRender->pMemSf
					, it.x + iConXOffset, it.y + iConYOffset
					, BMODE_TRANSP, BOP_COPY, 0, blitFlags);

				MixAlpha(pStrecthSf, pRender->pMemSf,
					it.x + iConXOffset, it.y + iConYOffset);

				//auto ret = pSf->Stretch(*pMemSf
				//	, it.x + iConXOffset, it.y + iConYOffset
				//	, iConSize.width, iConSize.width
				//	, BMODE_OPAQUE, BOP_COPY, 0, flags);

#ifdef _DEBUG
			//_SavetoClipBoard(pSf, false);
			//_SavetoClipBoard(pRender->pMemSf, false);
#endif // _DEBUG		
			}

			for (auto& it : pFormat->remarkFormat) {
				const auto pNeoStr = it.pNeoStr;
				if (pNeoStr == nullptr) { continue; }

				self(it.pNeoStr, pRender);
			}
		};

		RenderICon(this, this);

		// ------------
		// handle display: premul, HWA
		// ------------
		if (this->preMulAlpha) {
			pMemSf->PremultiplyAlpha();		// only needed in DX11 premultiplied mode
		}

#ifndef USE_RTT
#ifndef REUSE_HWA
		// reuse HWA: barely has no difference
		// seems the major cost is in blit function
		delete pHwaSf;
		pHwaSf = nullptr;

		pHwaSf = CreateHWASurface(32, pMemSf->GetWidth(), pMemSf->GetHeight(), ST_HWA_ROMTEXTURE, this->hwaDriver);
		//pHwaSf->CreateAlpha();
#endif
		// use render target here is even slower
		// this method costs about 70% time of render target way
		const auto bBlitResult = pMemSf->Blit(*pHwaSf);
#else
		pHwaSf->BeginRendering(TRUE, 0);
		//const auto pRTT = pMemSf->GetRenderTargetSurface();
		pMemSf->Blit(*pHwaSf, 0, 0, BMODE_TRANSP);
		//pMemSf->ReleaseRenderTargetSurface(pRTT);
		pHwaSf->EndRendering();
#endif
#ifdef COUNT_GDI_OBJECT
		const auto count = objectCounter.objectCount;

		objectCounter.UpdateObjectCount();
		if (count < objectCounter.objectCount) {
			const auto info = std::format(L"Add when Render to surface\n");
			OutputDebugStringW(info.c_str());
		}
#endif
	}

	inline CharPos GetCharPos(const size_t pos) const {
		constexpr auto invalid = CharPos { -1, -1, -1, -1, ShakeControl() };

		if (pCharPosArr == nullptr) {
			return invalid;
		}

		const size_t pTextLen = wcslen(pText);

		if (pTextLen == 0) {
			return invalid;
		}

		if (pTextLen <= pos) {
			return invalid;
		}

		return pCharPosArr[pos];
	}

	struct BlitOptions {
		// ------------
		// scroll
		// ------------
		bool bScroll = false;

		float scrollCoefX = 0.0f;
		float scrollCoefY = 0.0f;

		int scrollOffsetX = 0;
		int scrollOffsetY = 0;

		// ------------
		// fusion params
		// ------------
		bool bNoBlit = false;

		BlitMode bm = BMODE_TRANSP;
		BlitOp bo = BOP_COPY;
		LPARAM boParam = 0;
		int bAntiA = 0;
	};

	// pRc: object rect
	inline void BlitResult(const LPSURFACE pDst,
		const LPRECT pRc,
		BlitOptions& opt) const {
		if (!this->bTextValid) {
			return;
		}

		const bool bAllSurfaceValid = pDst != nullptr && pHwaSf != nullptr && pMemSf != nullptr;
		if (!opt.bNoBlit && !bAllSurfaceValid) { return; }

		if (!opt.bScroll) {
			const auto pSf = pHwaSf;

			const int xOffset = -this->borderOffsetX;
			const int yOffset = -this->borderOffsetY + this->startY;

			POINT hotSpot = { this->hotSpotX - xOffset, this->hotSpotY - yOffset };

			const float xPos = static_cast<float>(pRc->left + this->hotSpotX);
			const float yPos = static_cast<float>(pRc->top + this->hotSpotY);

			if (!opt.bNoBlit) {
				const auto bRet = pSf->BlitEx(*pDst, xPos, yPos,
					this->xScale, this->yScale,
					0, 0, pSf->GetWidth(), pSf->GetHeight(),
					&hotSpot, this->angle,
					opt.bm, opt.bo, opt.boParam, opt.bAntiA);
			}

			// update coef based on display
			opt.scrollCoefX = 0.0f;
			opt.scrollCoefY = -1.0f * this->startY / pSf->GetHeight();
		}
		else {
			const auto pSf = pMemSf;

			auto startX = static_cast<int>(pSf->GetWidth() * opt.scrollCoefX) + this->borderOffsetX;
			auto startY = static_cast<int>(pSf->GetHeight() * opt.scrollCoefY) + this->borderOffsetY;
			auto objectWidth = (std::min)(pRc->right - pRc->left, static_cast<long>(pSf->GetWidth() - startX));
			auto objectHeight = (std::min)(pRc->bottom - pRc->top, static_cast<long>(pSf->GetHeight() - startY));

			if (startX >= pSf->GetWidth()) { return; }
			if (startY >= pSf->GetHeight()) { return; }

			// hotsopt is relative to the source size, so kept it not changed here
			POINT hotSpot = { this->hotSpotX, this->hotSpotY };

			float xPos = static_cast<float>(pRc->left + this->hotSpotX);
			float yPos = static_cast<float>(pRc->top + this->hotSpotY);

			if (startX < 0) { xPos -= startX; objectWidth += startX; startX = 0; }
			if (startY < 0) { yPos -= startY; objectHeight += startY; startY = 0; }

			if (objectWidth <= 0) { return; }
			if (objectHeight <= 0) { return; }

			if (!opt.bNoBlit) {
				// blit HWA version directly will mess the alpha channel
				const auto bRet = pSf->BlitEx(*pDst, xPos, yPos,
					this->xScale, this->yScale,
					startX, startY, objectWidth, objectHeight,
					&hotSpot, this->angle,
					opt.bm, opt.bo, opt.boParam, opt.bAntiA);
			}

			// update offset based on coef
			opt.scrollOffsetX = startX - this->borderOffsetX;
			opt.scrollOffsetY = startY - this->borderOffsetY + this->startY;
		}
	}
};

// compatible with MMF
#pragma pop_macro("Font")
#pragma pop_macro("fpFont")
