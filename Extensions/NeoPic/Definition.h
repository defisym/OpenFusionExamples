#pragma once

typedef std::map<std::wstring, LPSURFACE> SurfaceLib;

#define SetExtUserData(pData) rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvSetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib,pData)
#define GetExtUserData() rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvGetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib)