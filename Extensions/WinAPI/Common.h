
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

// General includes
#include	"ccxhdr.h"
#include	"Surface.h"

// Specific to this cox
#include	"resource.h"
#include	"main.h"
#include	"TigsExt.hpp"

//My headers
//STL
#include	<deque>
//WinAPI
#include	"tlhelp32.h"
#include	"shlwapi.h"
#pragma	comment(lib,"shlwapi.lib") 

// Globals and Prototypes
extern HINSTANCE hInstLib;
extern short conditionsInfos[];
extern short actionsInfos[];
extern short expressionsInfos[];

//My Global
//定义extern后还需要再对应的cpp里初始化

//全局窗口句柄
extern HWND CurrentWindowHandle;

//窗口锁定情况
extern bool Lock;

//当前鼠标锁定的矩形区域
extern RECT CurrentLockRect;

//枚举窗体回调
BOOL CALLBACK WINAPIEXT_EnumWindowsProc(
	HWND hwnd,      // handle to parent window
	LPARAM lParam   // application-defined value
);

void UnlockLockedMouse();
HWND ReturnCurrentWindowHandle();


//所有创建线程的句柄
extern std::deque <LPTSTR> RunApplicationName;

/*----------------------------------------------------------------*/

long (WINAPI * ConditionJumps[])(LPRDATA rdPtr, long param1, long param2);
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2);
long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param);

//My Func
void StopAllApplication();
void DeleteRunApplicationName(LPCTSTR ApplicationName);
void DeleteRunApplicationNameByPos(size_t pos);
void AddNewApplicationName(LPTSTR lpApplicationName);
void DeleteAllApplicationName();
DWORD GetProcessIDByName(LPCTSTR ApplicationName);

// Used to ensure the MMF version is 1.5, you can safely ignore this
#if defined(MMFEXT)
#define	IS_COMPATIBLE(v) (v->mvGetVersion != NULL && (v->mvGetVersion() & MMFBUILD_MASK) >= MINBUILD && (v->mvGetVersion() & MMFVERSION_MASK) >= MMFVERSION_20 && ((v->mvGetVersion() & MMFVERFLAG_MASK) & MMFVERFLAG_HOME) == 0)
#elif defined(PROEXT)
#define IS_COMPATIBLE(v) (v->mvGetVersion != NULL && (v->mvGetVersion() & MMFBUILD_MASK) >= MINBUILD && (v->mvGetVersion() & MMFVERSION_MASK) >= MMFVERSION_20 && ((v->mvGetVersion() & MMFVERFLAG_MASK) & MMFVERFLAG_PRO) != 0)
#else
#define	IS_COMPATIBLE(v) (v->mvGetVersion != NULL && (v->mvGetVersion() & MMFBUILD_MASK) >= MINBUILD && (v->mvGetVersion() & MMFVERSION_MASK) >= MMFVERSION_20)
#endif

#endif
