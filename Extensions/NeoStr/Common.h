
#ifndef COMMON_H
#define COMMON_H

#if defined(_DEBUG) && defined(WIN32)
	#define	_CRTDBG_MAP_ALLOC	1
#endif

#define	IN_EXT_VERSION2
#define	COXSDK

// ----------------------------------------------
// VERSION AND BUILD YOUR EXTENSION CAN WORK WITH
// ----------------------------------------------
//#define TGFEXT		// TGF, MMF Standard and MMF Pro
#define	MMFEXT			// MMF Standard and MMF Pro
//#define PROEXT		// MMF Pro only

// Build number of the minimum required version of MMF
#ifdef _UNICODE
#define	MINBUILD	249
#else
#define	MINBUILD	243
#endif

// General includes
#include	"ccxhdr.h"
#include	"Surface.h"

// My lib
#include	<string>

class NeoStr;

struct CharPos {
	long x;
	long y;
	long maxWidth;
	long totalHeight;
};

enum class HotSpotPos;
struct GlobalData;

// Specific to this cox
#include	"resource.h"
#include	"main.h"
#include	"TigsExt.hpp"

// My header
#include	"GeneralDefinition.h"
#include	"GetFontName.h"
#include	"WindowsCommon.h"
#include	"ObjectSelection.h"

#include	"Fusion.h"
#include	"FusionUtilities.h"
#include	"Encryption.h"
#include	"NeoStr.h"

struct GlobalData {
	bool gdiInitialized = false;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	
	NeoStr::FontCache* pFontCache;
	NeoStr::CharSizeCacheWithFont* pCharSzCacheWithFont;
	NeoStr::RegexHandler* pRegexHandler;

	PrivateFontCollection* pFontCollection;
	NeoStr::IConData* pIConData;

	GlobalData(){
		auto state = Gdiplus::GdiplusStartup(&gdiplusToken
			, &gdiplusStartupInput
			, NULL);
		gdiInitialized = true;

		NeoStr::Alloc(pFontCache);
		NeoStr::Alloc(pCharSzCacheWithFont);
		NeoStr::Alloc(pRegexHandler);

		pFontCollection = new PrivateFontCollection;
		pIConData = new NeoStr::IConData;
	}

	~GlobalData() {
		delete pIConData;
		delete pFontCollection;

		NeoStr::Release(pFontCache);
		NeoStr::Release(pCharSzCacheWithFont);
		NeoStr::Release(pRegexHandler);

		Gdiplus::GdiplusShutdown(gdiplusToken);
	}
};

#include	"Func.h"

#ifdef _CONSOLE
#include <iostream>
#endif

// Globals and Prototypes
extern HINSTANCE hInstLib;
extern short conditionsInfos[];
extern short actionsInfos[];
extern short expressionsInfos[];

long (WINAPI * ConditionJumps[])(LPRDATA rdPtr, long param1, long param2);
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2);
long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param);

// Used to ensure the MMF version is 1.5, you can safely ignore this
#if defined(MMFEXT)
#define	IS_COMPATIBLE(v) (v->mvGetVersion != NULL && (v->mvGetVersion() & MMFBUILD_MASK) >= MINBUILD && (v->mvGetVersion() & MMFVERSION_MASK) >= MMFVERSION_20 && ((v->mvGetVersion() & MMFVERFLAG_MASK) & MMFVERFLAG_HOME) == 0)
#elif defined(PROEXT)
#define IS_COMPATIBLE(v) (v->mvGetVersion != NULL && (v->mvGetVersion() & MMFBUILD_MASK) >= MINBUILD && (v->mvGetVersion() & MMFVERSION_MASK) >= MMFVERSION_20 && ((v->mvGetVersion() & MMFVERFLAG_MASK) & MMFVERFLAG_PRO) != 0)
#else
#define	IS_COMPATIBLE(v) (v->mvGetVersion != NULL && (v->mvGetVersion() & MMFBUILD_MASK) >= MINBUILD && (v->mvGetVersion() & MMFVERSION_MASK) >= MMFVERSION_20)
#endif

#endif
