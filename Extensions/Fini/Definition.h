#pragma once

typedef CSimpleIni INI;
typedef CSimpleIni* LPINI;

typedef CSimpleIni::TNamesDepend INILIST;
typedef CSimpleIni::TNamesDepend* LPINILIST;

typedef CSimpleIni::TNamesDepend::const_iterator INIIT;

#define Fini rdPtr->ini
#define OStr rdPtr->Str

#define FLOAT_MAX 50+1

#define Default_Val	_T("0")

#define ONIT_SEC	0
#define ONIT_ITEM	1

#define invalid(X) if (!valid(Fini)) { return X; }

#define release_str() release_arr(rdPtr->SecLoopName);release_arr(rdPtr->ItemLoopName);release_arr(rdPtr->CurrentSec);release_arr(rdPtr->CurrentItem);release_arr(rdPtr->AutoSaveFilePath);release_arr(rdPtr->AutoSaveKey);

#define release_ini() release_ptr(Fini);release_str();rdPtr->Modified = false;
#define init_ini() release_ini();Fini = new INI;Fini->SetUnicode();

#define InvalidSec(X) if (StrEmpty(Section)) { return X; }
#define InvalidSecItem(X) if (StrEmpty(Section) || StrEmpty(Item)) { return X; }

//// 2.5+ unlimited values and strings
//typedef struct tagRV25P {
//	CValue* rvpValues;
//	int		rvNumberOfValues;
//	int		rvFree1[VALUES_NUMBEROF_ALTERABLE - 2];		// for compatiblity with old extensions
//	int		rvValueFlags;
//	BYTE	rvFree2[VALUES_NUMBEROF_ALTERABLE];			// for compatiblity with old extensions
//	LPTSTR* rvpStrings;
//	int		rvNumberOfStrings;
//	LPTSTR	rvFree3[STRINGS_NUMBEROF_ALTERABLE - 2];		// for compatiblity with old extensions
//} rVal25P;
//
//// 2.5 unlimited values
//typedef struct tagRV25 {
//	CValue* rvpValues;
//	long	rvNumberOfValues;
//	long	rvFree1[VALUES_NUMBEROF_ALTERABLE - 2];
//	long	rvValueFlags;
//	BYTE	rvFree2[VALUES_NUMBEROF_ALTERABLE];
//	LPTSTR	rvStrings[STRINGS_NUMBEROF_ALTERABLE];
//} rVal25;
//
//// 2.0 Unicode
//typedef struct tagRV20U {
//	CValue* rvpValues;
//	long	rvFree1[VALUES_NUMBEROF_ALTERABLE - 1];
//	long	rvValueFlags;
//	BYTE	rvFree2[VALUES_NUMBEROF_ALTERABLE];
//	LPTSTR	rvStrings[STRINGS_NUMBEROF_ALTERABLE];
//} tagRV20U;
