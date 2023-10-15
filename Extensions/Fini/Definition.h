#pragma once
#include <ranges>

typedef CSimpleIni INI;
typedef CSimpleIni* LPINI;

typedef CSimpleIni::TNamesDepend INILIST;
typedef CSimpleIni::TNamesDepend* LPINILIST;

typedef CSimpleIni::TNamesDepend::const_iterator INIIT;

constexpr auto ONIT_SEC = 0;
constexpr auto ONIT_ITEM = 1;

constexpr auto Default_Val = L"0";

#define InvalidIni(X) if (!valid(rdPtr->ini)) { return X; }

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

struct tagRDATA;
typedef tagRDATA RUNDATA;
typedef RUNDATA* LPRDATA;

struct GlobalData {
	std::map<std::wstring, LPINI> pInis;

	GlobalData() = default;
	~GlobalData() {
		for(const auto& pIni : pInis | std::views::values) {
			delete pIni;
		}
	}

	inline void StashData(LPRDATA rdPtr);
	inline void RetrieveData(LPRDATA rdPtr);
};
