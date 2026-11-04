#pragma once

// ------------------------------------------
// Macro Helper
// ------------------------------------------
#if defined(MMFEXT) || defined(PROEXT)
#define Empty_Str	_T("")
#define Default_Str	_T("")

//You need "\\+" to escape +
#define RegStr_IsNum _T("(\\+|\\-)?[0-9]+(\\.[0-9]+)?")

#define CallEvent(X) callRunTimeFunction(rdPtr, RFUNCTION_GENERATEEVENT, X, 0)
#define AddEvent(X) callRunTimeFunction(rdPtr, RFUNCTION_PUSHEVENT , X, 0)

#define RAD(_DEG) ((PI*(_DEG))/180)
#define DEG(_RAD) ((180*(_RAD))/PI)

#define MV rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv

#define SetExtUserData(pData) MV->mvSetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib, pData)
#define GetExtUserData() MV->mvGetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib)

#define TRUNCATE_SHORT(v) ((short)(v) & 0x7FFF)
#endif

#define CONNECT_STR(l, r) l##r
#define STRINGIFY(s) #s
