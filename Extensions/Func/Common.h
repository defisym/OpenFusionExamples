
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

// STL
#include    <map>
#include	<unordered_map>

#include	<vector>
#include	<string>

#if !defined(RUN_ONLY)
#include	<stdexcept>
#endif

// My lib
class ObjectSelection;

// disable for windows 7 compatibility
//#define _ENABLE_TOAST

#ifdef _ENABLE_TOAST
class WinToastHelper;
#endif

// My headers
#include	"GeneralDefinition.h"
#include	"Definition.h"
#include	"StrNum.h"

// Specific to this cox
#include	"resource.h"
#include	"main.h"
#include	"TigsExt.hpp"

// My headers
#include	"Fusion.h"
#include	"FusionUtilities.h"
#include	"ObjectSelection.h"
#include	"EffectUtilities.h"

#include	"Func.h"

#ifdef _ENABLE_TOAST
#include	"Toast.h"
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
