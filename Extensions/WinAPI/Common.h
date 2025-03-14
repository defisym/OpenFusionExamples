
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
//#define	MINBUILD	249
#define	MINBUILD	292
#else
#define	MINBUILD	292
#endif

// must be here to avoid conflict of min/max macro
#include	"CLI11/include/CLI/CLI.hpp"

// General includes
#include	"ccxhdr.h"
#include	"Surface.h"

#include	"Macro.h"

//STL
#include	<deque>
#include	<vector>
#include	<thread>
#include	<mutex>
#include	<memory>
#include	<format>
#include	<chrono>

//WinAPI

//FileList
#include	<shlwapi.h>
#pragma	comment(lib,"shlwapi.lib") 

//IME
#pragma comment(lib,"Imm32.lib")

//Definitions
#include	"GeneralDefinition.h"
#include	"Definition.h"

class ObjectSelection;

// Specific to this cox
#include	"resource.h"
#include	"main.h"
#include	"TigsExt.hpp"

//MMF
#include	"ImageFlt.h"
#include	"ImgFlt.h"
#include	"CfcFile.h"

//My headers
#include	"Fusion.h"
#include	"FusionUtilities.h"
#include	"Encryption.h"
#include	"StrNum.h"
#include	"WindowsCommon.h"
#include	"WindowsException.h"
#include	"ObjectSelection.h"
#include	"Base64.h"

#ifdef _FFMPEG
#include	"GetVideoFrame.h"
#endif

//Global Func
#include	"Func.h"
#include	"WindowResizing.h"
#include	"MouseHandler.h"

// Globals and Prototypes
extern HINSTANCE hInstLib;
extern WindowResizing windowResizing;
extern MouseHandler mouseHandler;

extern short conditionsInfos[];
extern short actionsInfos[];
extern short expressionsInfos[];

long (WINAPI* ConditionJumps[])(LPRDATA rdPtr, long param1, long param2);
short (WINAPI* ActionJumps[])(LPRDATA rdPtr, long param1, long param2);
long (WINAPI* ExpressionJumps[])(LPRDATA rdPtr, long param);

// Used to ensure the MMF version is 1.5, you can safely ignore this
#if defined(MMFEXT)
#define	IS_COMPATIBLE(v) (v->mvGetVersion != NULL && (v->mvGetVersion() & MMFBUILD_MASK) >= MINBUILD && (v->mvGetVersion() & MMFVERSION_MASK) >= MMFVERSION_20 && ((v->mvGetVersion() & MMFVERFLAG_MASK) & MMFVERFLAG_HOME) == 0)
#elif defined(PROEXT)
#define IS_COMPATIBLE(v) (v->mvGetVersion != NULL && (v->mvGetVersion() & MMFBUILD_MASK) >= MINBUILD && (v->mvGetVersion() & MMFVERSION_MASK) >= MMFVERSION_20 && ((v->mvGetVersion() & MMFVERFLAG_MASK) & MMFVERFLAG_PRO) != 0)
#else
#define	IS_COMPATIBLE(v) (v->mvGetVersion != NULL && (v->mvGetVersion() & MMFBUILD_MASK) >= MINBUILD && (v->mvGetVersion() & MMFVERSION_MASK) >= MMFVERSION_20)
#endif

#endif
