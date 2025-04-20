
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

#ifdef _DEBUG
#include	<assert.h>
#endif

//#define _CONSOLE

#ifdef _CONSOLE
#include <iostream>
#endif

//#define _USE_CRTDBG

#ifdef _USE_CRTDBG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

// General includes
#include	"ccxhdr.h"
#include	"Surface.h"

#include	"Macro.h"

#ifdef _OPENMP
#include	<omp.h>
#endif

#include	<map>
#include	<string>
#include	<chrono>

#ifdef COPY_D3D_TEXTURE
#ifdef _DEBUG
#include	"D3D/HookD3D.h"
#endif

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#endif

// My lib
#include	"SDLUtilities.h"

#include	"Encryption.h"
#include	"FFMpeg.h"

#include	"SDL_Interface.h"
#include	"SDL_Mixer_Interface.h"

#include	"GeneralDefinition.h"
#include	"Definition.h"

// Specific to this cox
#include	"resource.h"
#include	"main.h"
#include	"TigsExt.hpp"

// My header
#include	"Fusion.h"
#include	"FusionUtilities.h"

#ifdef COPY_D3D_TEXTURE
#include	"D3D/D3DHandler.h"
#endif

#include	"WindowsCommon.h"
#include	"Func.h"

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
