// ------------------------
// Edittime & Runtime General
// ------------------------

#pragma once

#include "GeneralDefinition.h"
#include "StrNum.h"

// Object identifier
// obj->rHo.hoIdentifier
constexpr auto IDENTIFIER_ACTIVE = 1230131283;
constexpr auto IDENTIFIER_STRING = 1415071060;
constexpr auto IDENTIFIER_COUNTER = 1381256771;
constexpr auto IDENTIFIER_LIVES = 1163282764;
constexpr auto IDENTIFIER_QUESTION = IDENTIFIER_STRING;
constexpr auto IDENTIFIER_SCORE = 1163019091;

inline bool LPROValid(LPRO object, unsigned long identifier = 0) {
	if (object == nullptr) {
		return false;
	}

	if (identifier) {
		return object->roHo.hoIdentifier == identifier;
	}

	return true;		// need not to check identifier
}

//headerObject 	roHo;		  		// Common structure 
//
//rCom			roc;				// Anim/movement structure
//rMvt			rom;				// Movement structure 
//rAni			roa;				// Animation structure 
//rSpr			ros;				// Sprite handling structure
//rVal			rov;				// Values structure

enum class HeaderName {
	rCom,
	rMvt,
	rAni,
	rSpr,
	rVal,
};

inline size_t GetStructOffset(LPRDATA rdPtr, LPRO object, HeaderName headerName, bool allowRVforCS = false) {
	if (!LPROValid(object)) {
		return -1;
	}

	size_t offset = sizeof(headerObject);

	// Internal objects
	auto identifier = object->roHo.hoIdentifier;

	if (identifier == IDENTIFIER_LIVES || identifier == IDENTIFIER_SCORE) {
		return -1;
	}

	if (identifier == IDENTIFIER_COUNTER || identifier == IDENTIFIER_STRING) {
		if (allowRVforCS) {
			offset += sizeof(rCom) + sizeof(rMvt) + sizeof(rSpr);

			return offset;
		}
		else {
			return -1;
		}
	}

	if (identifier == IDENTIFIER_ACTIVE) {
		offset += sizeof(rCom) + sizeof(rMvt) + sizeof(rSpr);

		return offset;
	}

	// General extensions
	auto flags = object->roHo.hoOEFlags;

	switch (headerName) {
	case HeaderName::rCom: {
		if (!((flags & OEFLAG_MOVEMENTS) || (flags & OEFLAG_ANIMATIONS) || (flags & OEFLAG_SPRITES))) {
			return -1;
		}

		break;
	}
	case HeaderName::rMvt: {
		if (!(flags & OEFLAG_MOVEMENTS)) {
			return -1;
		}

		break;
	}
	case HeaderName::rAni: {
		if (!(flags & OEFLAG_ANIMATIONS)) {
			return -1;
		}

		break;
	}
	case HeaderName::rSpr: {
		if (!(flags & OEFLAG_SPRITES)) {
			return -1;
		}

		break;
	}
	case HeaderName::rVal: {
		if (!(flags & OEFLAG_VALUES)) {
			return -1;
		}

		break;
	}
	}

	offset += (flags & OEFLAG_MOVEMENTS) || (flags & OEFLAG_ANIMATIONS) || (flags & OEFLAG_SPRITES)
		? sizeof(rCom) : 0;

	if (headerName == HeaderName::rCom) {
		return offset - sizeof(rCom);
	}

	offset += (flags & OEFLAG_MOVEMENTS) ? sizeof(rMvt) : 0;
	offset += (flags & OEFLAG_ANIMATIONS) ? sizeof(rAni) : 0;
	offset += (flags & OEFLAG_SPRITES) ? sizeof(rSpr) : 0;

	return offset;
}

// Update edit data

// Usage: in General.cpp

// HGLOBAL WINAPI DLLExport UpdateEditStructure(mv __far *mV, void __far * OldEdPtr) {
// 	HGLOBAL hgNew = NULL;

// 	// V1->V2
// 	UpdateEditData<tagEDATA_V1, tagEDATA_V2>(OldEdPtr, hgNew, KCX_VERSION_V2, [](tagEDATA_V2* newEdPtr) {
// 		newEdPtr->newData = newDataDefault;
// 		});

// 	// V3->V3
//  UpdateEditData<tagEDATA_V2, tagEDATA_V3>(OldEdPtr, hgNew, KCX_VERSION_V3, [](tagEDATA_V3* newEdPtr) {
// 		newEdPtr->newData = newDataDefault;
// 		});

//     ......

// 	return hgNew;
// }

#include <functional>

// Update Core
template<typename EDATA>
inline void UpdateEditData(extHeader* pEHeader, HGLOBAL& hgNew, std::function<void(EDATA*)> updateFunc) {
	if ((hgNew = GlobalAlloc(GPTR, sizeof(EDATA))) != NULL) {
		auto newEdPtr = (EDATA*)GlobalLock(hgNew);

		if (newEdPtr != nullptr) {
			memcpy(&newEdPtr->eHeader, pEHeader, sizeof(extHeader));
			newEdPtr->eHeader.extSize = sizeof(EDATA);				// Update the EDITDATA structure size			

			updateFunc(newEdPtr);
		}

		GlobalUnlock(hgNew);
	}
}

// Update during development, doesn't check & update version
template<typename EDATA>
inline void UpdateEditData(EDATA* OldEdPtr, HGLOBAL& hgNew, std::function<void(EDATA*)> updateFunc) {
	UpdateEditData<EDATA>(&OldEdPtr->eHeader, hgNew, updateFunc);
}

// General update, check & update version
template<typename Old, typename New>
inline void UpdateEditData(Old* OldEdPtr, HGLOBAL& hgNew, DWORD targetVersion, std::function<void(Old*, New*)> updateFunc) {
	if (OldEdPtr->eHeader.extVersion < targetVersion) {
		UpdateEditData<New>(&OldEdPtr->eHeader, hgNew, [&] (New* pNew) {
			pNew->eHeader.extVersion = targetVersion;			// Update the version number

			updateFunc(OldEdPtr, pNew);
			});
	}
}

template<typename T>
inline void UpdateEditFlag(T& flags, const T& flag, bool bCheck) {
	if (bCheck) {
		flags |= flag;
	}
	else {
		flags &= ~flag;
	}
}

// Update App props for SDL
inline void SDL_UpdateAppProp(mv _far* mV, LPEDATA edPtr) {
	const auto bRWM = mvGetAppPropCheck(mV, edPtr, PROPID_APP_RUNWHENMINIMIZED);
	const auto bRWR = mvGetAppPropCheck(mV, edPtr, PROPID_APP_RUNWHILERESIZING);

	if (bRWM && bRWR) {
		return;
	}
	
	MSGBOX(L"This object needs to enable \"Run when minimized\" & \"Run while resizing\".\nClick \"OK\" to set props automatically");

	mvSetAppPropCheck(mV, edPtr, PROPID_APP_RUNWHENMINIMIZED, TRUE);
	mvSetAppPropCheck(mV, edPtr, PROPID_APP_RUNWHILERESIZING, TRUE);
}

// Cast anytype to fusion's expression return

template<typename T>
constexpr inline long ConvertToLong(T t) {
	return *((long*)&t);
}

template<typename T>
constexpr inline T ConvertToType(long l) {
	return *((T*)&l);
}

// Cast anytype

template<typename Input, typename Output>
constexpr inline Output ConvertToType(Input l) {
	return *((Output*)&l);
}

// Build Types
#include <initializer_list>

constexpr auto PLATFORM_WINDOWS = 0;
constexpr auto PLATFORM_ANDROID = 1;
constexpr auto PLATFORM_IOS = 2;
constexpr auto PLATFORM_HTML5 = 3;

constexpr auto INCOMPATIBLE_BUILDTYPE_WINDOWS_STANDALONE = 1;
constexpr auto INCOMPATIBLE_BUILDTYPE_WINDOWS_SCREENSAVER = 2;
constexpr auto INCOMPATIBLE_BUILDTYPE_WINDOWS_SUBAPPLICATION = 3;

constexpr auto INCOMPATIBLE_BUILDTYPE_ANDROID = 5;
constexpr auto INCOMPATIBLE_BUILDTYPE_ANDROID_AAB = 6;
constexpr auto INCOMPATIBLE_BUILDTYPE_ANDROID_AAB_EXP = 7;

constexpr auto INCOMPATIBLE_BUILDTYPE_IOS_APPLICATION = 9;
constexpr auto INCOMPATIBLE_BUILDTYPE_IOS_XCODEPROJ = 10;
constexpr auto INCOMPATIBLE_BUILDTYPE_IOS_XCODEPROJ_FINAL = 11;

constexpr auto INCOMPATIBLE_BUILDTYPE_HTML5_DEV = 13;
constexpr auto INCOMPATIBLE_BUILDTYPE_HTML5_FINAL = 14;

#define INCOMPATIBLE_WINDOWS { INCOMPATIBLE_BUILDTYPE_WINDOWS_STANDALONE, INCOMPATIBLE_BUILDTYPE_WINDOWS_SCREENSAVER, INCOMPATIBLE_BUILDTYPE_WINDOWS_SUBAPPLICATION }

#define INCOMPATIBLE_ANDROID { INCOMPATIBLE_BUILDTYPE_ANDROID, INCOMPATIBLE_BUILDTYPE_ANDROID_AAB, INCOMPATIBLE_BUILDTYPE_ANDROID_AAB_EXP }

#define INCOMPATIBLE_IOS { INCOMPATIBLE_BUILDTYPE_IOS_APPLICATION, INCOMPATIBLE_BUILDTYPE_IOS_XCODEPROJ, INCOMPATIBLE_BUILDTYPE_IOS_XCODEPROJ_FINAL }

#define INCOMPATIBLE_HTML5 { INCOMPATIBLE_BUILDTYPE_HTML5_DEV, INCOMPATIBLE_BUILDTYPE_HTML5_FINAL }

#define INCOMPATIBLE { INCOMPATIBLE_WINDOWS, INCOMPATIBLE_ANDROID, INCOMPATIBLE_IOS, INCOMPATIBLE_HTML5 }

using disableItem = std::remove_const_t<decltype(INCOMPATIBLE_BUILDTYPE_WINDOWS_STANDALONE)>;
using disableArray = std::vector<std::vector<disableItem>>;

// Declaration in Edittime.cpp
HMENU GetPopupMenu(short mn);

inline void IterateMenu (HMENU hMenu, std::function<void(HMENU, int)> f) {
	constexpr auto SUBMENU = -1;
	constexpr auto SEPARATOR = 0;

	std::function<void(HMENU)> iterateMenu_Func;

	iterateMenu_Func = [&] (HMENU hMenu) {
		auto nCount = GetMenuItemCount(hMenu);

		for (int item = 0; item < nCount; item++) {
			auto id = GetMenuItemID(hMenu, item);

			if (id == SUBMENU) {
				auto subMenu = GetSubMenu(hMenu, item);
				iterateMenu_Func(subMenu);
			}
			else if (id != SEPARATOR) {
				f(hMenu, id);
			}
		}
	};

	iterateMenu_Func(hMenu);
};

#define DISABLEALL  -1
#define disableMenus_Empty {}
#define disableMenus_DisableAll  { DISABLEALL }

// Usage
// 
// if disableList is started with DISABLEALL, and it's the only element, all menu items will be disabled
// if disableList is started with DISABLEALL, and it's not the only element, menu items except the defined ones will be disabled
// 
// // Predefine
// constexpr disableList disableMenus_Windows = {};
// constexpr disableList disableMenus_Android = { IDMN_CONDITION_SMBP,IDMN_CONDITION_SMBSF };
// constexpr disableList disableMenus_iOS = {};
// constexpr disableList disableMenus_HTML5 = {};

// constexpr disableArray disableMenus = { disableMenus_Windows
// 							, disableMenus_Android
// 							, disableMenus_iOS
// 							, disableMenus_HTML5 };

// GetPlatformPopupMenu(MN_CONDITIONS, disableMenus)

// // Define in GetConditionMenu
// disableArray disableMenus = { disableMenus_Empty
// 							, { IDMN_CONDITION_SMBP,IDMN_CONDITION_SMBSF }
// 							, disableMenus_Empty
// 							, disableMenus_Empty };

// return GetPlatformPopupMenu(MN_CONDITIONS, disableMenus);

inline HMENU GetPlatformPopupMenu(mv* mV, fpObjInfo oiPtr, LPEDATA edPtr, short mn, const disableArray& disableMenus) {
	std::vector<std::vector<disableItem>> ic_V = INCOMPATIBLE;
	std::vector<std::vector<disableItem>> dM_V = disableMenus;

	auto hMenu = GetPopupMenu(mn);

	auto pValue = (CPropIntValue*)mvGetAppPropValue(mV, edPtr, PROPID_APP_BUILDTYPE);

	if (pValue != nullptr) {
		auto buildType = pValue->m_nValue;

		// Display BuildType if needed
		// MSGBOX(_itos(buildType), L"BuildType");

		auto buildForPlatform = [buildType] (std::vector<disableItem>& lst) {
			return std::find(lst.begin(), lst.end(), buildType) != lst.end();
		};

		for (size_t platform = 0; platform < ic_V.size(); platform++) {
			if (buildForPlatform(ic_V [platform])) {
				if (!dM_V [platform].empty()
					&& dM_V [platform][0] == DISABLEALL) {
					IterateMenu(hMenu, [&] (HMENU hMenu, int id) {
						if (dM_V [platform].size() == 1
							|| std::find(dM_V [platform].begin() + 1, dM_V [platform].end(), id) == dM_V [platform].end()) {
							EnableMenuItem(hMenu, id, MF_DISABLED | MF_GRAYED);
						}
						});

					break;
				}
				else {
					for (auto& it : dM_V [platform]) {
						EnableMenuItem(hMenu, it, MF_DISABLED | MF_GRAYED);
					}

					break;
				}
			}
		}
	}

	return hMenu;
}

#define GetPlatformPopupMenu(mn,disableMenus) GetPlatformPopupMenu(mV,oiPtr,edPtr,mn,disableMenus)

inline HMENU GetFilteredPopupMenu(mv* mV, fpObjInfo oiPtr, LPEDATA edPtr, short mn, const std::vector<disableItem>& disableItems) {
	auto hMenu = GetPopupMenu(mn);

	for (auto& it : disableItems) {
		EnableMenuItem(hMenu, it, MF_DISABLED | MF_GRAYED);
	}

	return hMenu;
}

#define GetFilteredPopupMenu(mn,disableItems) GetFilteredPopupMenu(mV,oiPtr,edPtr,mn,disableItems)

#define _mvCalloc(size) mvCalloc(rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv, size)

//inline void UpdateEditString(LPMV mV, LPEDATA edPtr, LPCWSTR pStr, LPWSTR pBuf) {
//	// If the length is different
//	if (wcslen(pStr) != wcslen(pBuf))
//	{
//		//MSGBOX(L"Text buffer");
//
//		// Asks MMF to reallocate the structure with the new size
//		auto bfSz = (wcslen(pStr) + 1);
//		DWORD dwNewSize = sizeof(EDITDATA) + bfSz * sizeof(wchar_t);
//		LPEDATA pNewPtr = (LPEDATA)mvReAllocEditData(mV, edPtr, dwNewSize);
//		//MSGBOX(L"Text copy");
//
//		// If reallocation worked
//		if (pNewPtr != NULL)
//		{
//			// Copy the string
//			edPtr = pNewPtr;
//			wcscpy_s(pBuf, bfSz, pStr);
//		}
//	}
//	else
//	{
//		// Same size : simply copy
//		wcscpy_s(pBuf, wcslen(pStr) + 1, pStr);
//	}
//}

inline bool GetAppOptions(LPRDATA rdPtr, DWORD options) {
	return rdPtr->rHo.hoAdRunHeader->rhApp->m_pHdr2->dwOptions & options;
}

inline bool PreMulAlpha(LPRDATA rdPtr) {
	return GetAppOptions(rdPtr, AH2OPT_PREMULTIPLIEDALPHA);
}

inline bool D3D11(int driver) {
	return driver == SD_D3D11;
}

inline bool D3D11(LPRDATA rdPtr) {
	LPSURFACE wSurf = WinGetSurface((int)rdPtr->rHo.hoAdRunHeader->rhIdEditWin);
	int sfDrv = wSurf->GetDriver();

	return D3D11(sfDrv);
}

//X : L M R
//Y : T M B
enum class HotSpotPos {
	LT,
	LM,
	LB,
	MT,
	MM,
	MB,
	RT,
	RM,
	RB,
	CUSTOM
};

inline HotSpotPos StrToHotSpotPos(const wchar_t* pStr) {
	do {
		if(StrIEqu(pStr,L"LT")) {
			return  HotSpotPos::LT;
		}

		if (StrIEqu(pStr, L"LM")) {
			return  HotSpotPos::LM;
		}

		if (StrIEqu(pStr, L"LB")) {
			return  HotSpotPos::LB;
		}

		if (StrIEqu(pStr, L"MT")) {
			return  HotSpotPos::MT;
		}

		if (StrIEqu(pStr, L"MM")) {
			return  HotSpotPos::MM;
		}

		if (StrIEqu(pStr, L"MB")) {
			return  HotSpotPos::MB;
		}

		if (StrIEqu(pStr, L"RT")) {
			return  HotSpotPos::RT;
		}

		if (StrIEqu(pStr, L"RM")) {
			return  HotSpotPos::RM;
		}

		if (StrIEqu(pStr, L"RB")) {
			return  HotSpotPos::RB;
		}		

		return HotSpotPos::CUSTOM;
	} while (false);
}

// Update X/Y according to width/height
inline void UpdateHotSpot(HotSpotPos Type, size_t width, size_t height, int& X, int& Y) {
	switch (Type) {
	case HotSpotPos::LT:
		X = 0;
		Y = 0;
		break;
	case HotSpotPos::LM:
		X = 0;
		Y = height / 2;
		break;
	case HotSpotPos::LB:
		X = 0;
		Y = height;
		break;
	case HotSpotPos::MT:
		X = width / 2;
		Y = 0;
		break;
	case HotSpotPos::MM:
		X = width / 2;
		Y = height / 2;
		break;
	case HotSpotPos::MB:
		X = width / 2;
		Y = height;
		break;
	case HotSpotPos::RT:
		X = width;
		Y = 0;
		break;
	case HotSpotPos::RM:
		X = width;
		Y = height / 2;
		break;
	case HotSpotPos::RB:
		X = width;
		Y = height;
		break;
	case HotSpotPos::CUSTOM:
		X = X;
		Y = Y;
		break;
	}
}

inline void UpdateRectByHotSpot(HotSpotPos Type, size_t width, size_t height, int X, int Y, LPRECT pRc) {
	int hotSpotX = X;
	int hotSpotY = Y;

	//UpdateHotSpot(hotSpotX == 0 && hotSpotY == 0
	//	? Type
	//	: HotSpotPos::CUSTOM

	UpdateHotSpot(Type
		, width, height
		, hotSpotX, hotSpotY);

	pRc->left -= hotSpotX;
	pRc->top -= hotSpotY;

	pRc->right = pRc->left + width;
	pRc->bottom = pRc->top + height;
}

inline void RotatePoint(double angle, int* hotX, int* hotY, int sw, int sh) {
	//Rotate hotspot
	if (angle == 0) {
		return;
	}

	float hx = (float)*hotX;
	float hy = (float)*hotY;

	float si = (float)sin(angle);
	float co = (float)cos(angle);

	int trX = (int)(sw * co);
	int trY = (int)(-sw * si);
	int blX = (int)(sh * si);
	int blY = (int)(sh * co);
	int brX = (int)(sw * co + sh * si);
	int brY = (int)(sh * co - sw * si);

	//Update topleft coordinate
	int dx = (std::min)(0, (std::min)(trX, (std::min)(blX, brX)));
	int dy = (std::min)(0, (std::min)(trY, (std::min)(blY, brY)));

	//Update hotspot
	*hotX = (int)(round(hx * co + hy * si) - dx);
	*hotY = (int)(round(hy * co - hx * si) - dy);
}

inline void RotatePoint(int angle, int* hotX, int* hotY, int sw, int sh) {
	return RotatePoint(RAD(angle), hotX, hotY, sw, sh);
}

#ifndef _NODISPLAY
// should be called when updating
// note: hotspot used in BlitEx shouldn't be changed
inline void UpdateHoImgInfo(LPRDATA rdPtr
	, int srcWidth, int srcHeight
	, float xScale, float yScale
	, HotSpotPos hotSpotPos
	, int hotSpotX, int hotSpotY
	, int angle) {
	//Get scale (absolute since negative will mirror)
	float scaleX = abs(xScale);
	float scaleY = abs(yScale);

	//Get scaled size
	int width = int(srcWidth * scaleX);
	int height = int(srcHeight * scaleY);

	//Get scaled hotspot
	int hotX = hotSpotX;
	int hotY = hotSpotY;

	UpdateHotSpot(hotSpotPos, width, height, hotX, hotY);

	rdPtr->rc.rcAngle = (float)angle;

	rdPtr->rc.rcScaleX = scaleX;
	rdPtr->rc.rcScaleY = scaleY;

	//Rotate hotspot
	if (rdPtr->rc.rcAngle) {
		RotatePoint(angle, &hotX, &hotY, width, height);
		cSurface::GetSizeOfRotatedRect(&width, &height, rdPtr->rc.rcAngle);
	}

	//Update size and scale image
	rdPtr->rHo.hoImgWidth = width; // leave room for rounding errors
	rdPtr->rHo.hoImgHeight = height;

	//Apply hotspot
	rdPtr->rHo.hoImgXSpot = (short)hotX;
	rdPtr->rHo.hoImgYSpot = (short)hotY;
}

inline void UpdateHoImgInfo(LPRDATA rdPtr, LPSURFACE pSrc
	, float xScale, float yScale
	, HotSpotPos hotSpotPos
	, int hotSpotX, int hotSpotY
	, int angle) {
	UpdateHoImgInfo(rdPtr
		, pSrc->GetWidth(), pSrc->GetHeight()
		, xScale, yScale
		, hotSpotPos
		, hotSpotX, hotSpotY
		, angle);
}

#endif

inline void AddBackdrop(LPRDATA rdPtr, cSurface* pSf, int x, int y, DWORD dwInkEffect, DWORD dwInkEffectParam, int nObstacleType, int nLayer) {
	rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvAddBackdrop(pSf, x, y, dwInkEffect, dwInkEffectParam, nObstacleType, nLayer);
}

//IterateBank(rdPtr, BK_FONTS, [] (const void* pAddress) {
//
//});

inline bool IterateBank(LPRDATA rdPtr, int type, const std::function<void(const void*)>& callback) {
	if (type < BK_IMGS || type > BK_MAX) {
		return false;
	}

	const auto pAppli = rdPtr->rHo.hoAdRunHeader->rhIdAppli;

	auto ret = LockBank(pAppli, type) == 0;

	if (!ret) {
		return ret;
	}

	const auto sz = Bank_GetEltCount(pAppli, type);

	if (sz == 0) {
		return false;
	}

	for (int i = 1; i <= sz; i++) {
		const auto pAddress = Bank_GetEltAddr(pAppli, type, i);

		if (pAddress != nullptr) {
			callback(pAddress);
		}
	}

	ret |= UnlockBank(pAppli, type) == 0;

	return ret;
}