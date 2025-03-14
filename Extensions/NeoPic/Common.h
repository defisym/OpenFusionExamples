
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
#include	"ImageFlt.h"
#include	"ImgFlt.h"

#include	"Macro.h"

#include	<map>
#include	<vector>
#include	<string>
#include	<thread>
#include    <format>

enum class HotSpotPos;
struct ZoomScale;

struct FrameCapture;
struct AnimationInterface;
struct NineSliceInterface;

#ifdef QUERY_VRAM
#include	"D3DUtilities/VRAMUtilities.h"
#endif

#include	"GeneralDefinition.h"
#include	"Definition.h"

// Specific to this cox
#include	"resource.h"
#include	"main.h"
#include	"TigsExt.hpp"

#include	"Encryption.h"

#include	"WindowsCommon.h"
#include	"EffectUtilities.h"
#include	"FusionUtilities.h"
#include	"Fusion.h"

#include	"Func.h"
#include	"FrameCapture.h"

#include	"AnimationInterface.h"
#include	"NineSliceInterface.h"

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
