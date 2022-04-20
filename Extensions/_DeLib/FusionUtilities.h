#pragma once

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

// Update edit data

// Usage: in General.cpp

// HGLOBAL WINAPI DLLExport UpdateEditStructure(mv __far *mV, void __far * OldEdPtr) {
// 	HGLOBAL hgNew = NULL;

// 	// V1->V2
// 	UpdateEditData<tagEDATA_V1, tagEDATA_V2>(OldEdPtr, hgNew, KCX_VERSION_V2, [](tagEDATA_V2* newEdPtr) {
// 		newEdPtr->newData = newDataDefault;
// 		});

// 	// V3->V3
//     UpdateEditData<tagEDATA_V2, tagEDATA_V3>(OldEdPtr, hgNew, KCX_VERSION_V3, [](tagEDATA_V3* newEdPtr) {
// 		newEdPtr->newData = newDataDefault;
// 		});

//     ......

// 	return hgNew;
// }

#include <functional>

template<typename Old, typename New>
inline void UpdateEditData(void __far* OldEdPtr, HGLOBAL& hgNew, DWORD targetVersion, std::function<void(New*)> updateFunc) {
	if (((Old*)OldEdPtr)->eHeader.extVersion < targetVersion) {
		if ((hgNew = GlobalAlloc(GPTR, sizeof(New))) != NULL) {
			New* newEdPtr = (New*)GlobalLock(hgNew);

			if (newEdPtr != nullptr) {
				memcpy(&newEdPtr->eHeader, &((Old*)OldEdPtr)->eHeader, sizeof(extHeader));
				newEdPtr->eHeader.extVersion = targetVersion;			// Update the version number
				newEdPtr->eHeader.extSize = sizeof(New);				// Update the EDITDATA structure size

				updateFunc(newEdPtr);
			}

			GlobalUnlock(hgNew);
		}
	}
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

// MSBOX
inline void MSGBOX(const std::wstring& Content, const std::wstring& title = L"ALERT") {
	MessageBox(NULL, Content.c_str(), title.c_str(), MB_OK);
}

// Build Types
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

constexpr auto INCOMPATIBLE_WINDOWS = { INCOMPATIBLE_BUILDTYPE_WINDOWS_STANDALONE
								, INCOMPATIBLE_BUILDTYPE_WINDOWS_SCREENSAVER
								, INCOMPATIBLE_BUILDTYPE_WINDOWS_SUBAPPLICATION };

constexpr auto INCOMPATIBLE_ANDROID = { INCOMPATIBLE_BUILDTYPE_ANDROID
									, INCOMPATIBLE_BUILDTYPE_ANDROID_AAB
									, INCOMPATIBLE_BUILDTYPE_ANDROID_AAB_EXP };

constexpr auto INCOMPATIBLE_IOS = { INCOMPATIBLE_BUILDTYPE_IOS_APPLICATION
								, INCOMPATIBLE_BUILDTYPE_IOS_XCODEPROJ
								, INCOMPATIBLE_BUILDTYPE_IOS_XCODEPROJ_FINAL };

constexpr auto INCOMPATIBLE_HTML5 = { INCOMPATIBLE_BUILDTYPE_HTML5_DEV
									, INCOMPATIBLE_BUILDTYPE_HTML5_FINAL };

constexpr auto INCOMPATIBLE = { INCOMPATIBLE_WINDOWS
									, INCOMPATIBLE_ANDROID
									, INCOMPATIBLE_IOS
									, INCOMPATIBLE_HTML5 };

using disableItem = std::remove_const_t<decltype(INCOMPATIBLE_BUILDTYPE_WINDOWS_STANDALONE)>;
using disableList = std::remove_const_t<decltype(INCOMPATIBLE_WINDOWS)>;
using disableArray = std::remove_const_t<decltype(INCOMPATIBLE)>;

HMENU GetPopupMenu(short mn);

constexpr auto DISABLEALL = -1;
constexpr disableList disableMenus_Empty = {};

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
	std::vector<std::vector<disableItem>> ic_V(INCOMPATIBLE.begin(), INCOMPATIBLE.end());
	std::vector<std::vector<disableItem>> dM_V(disableMenus.begin(), disableMenus.end());

	auto hMenu = GetPopupMenu(mn);

	auto pValue = (CPropIntValue*)mvGetAppPropValue(mV, edPtr, PROPID_APP_BUILDTYPE);

	if (pValue != nullptr) {
		auto buildType = pValue->m_nValue;

		// Display BuildType if needed
		// MSGBOX(_itos(buildType), L"BuildType");

		auto buildForPlatform = [buildType](std::vector<disableItem>& lst) {
			return std::find(lst.begin(), lst.end(), buildType) != lst.end();
		};

		auto iterateMenu = [](HMENU hMenu, std::function<void(HMENU, int)> f) {
			constexpr auto SUBMENU = -1;
			constexpr auto SEPARATOR = 0;

			std::function<void(HMENU)> iterateMenu_Func;

			iterateMenu_Func = [&](HMENU hMenu) {
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

		for (size_t platform = 0; platform < ic_V.size(); platform++) {
			if (buildForPlatform(ic_V[platform])) {
				if (dM_V[platform][0] == DISABLEALL) {
					iterateMenu(hMenu, [&](HMENU hMenu, int id) {
						if (dM_V[platform].size() == 1
							|| std::find(dM_V[platform].begin() + 1, dM_V[platform].end(), id) == dM_V[platform].end()) {
							EnableMenuItem(hMenu, id, MF_DISABLED | MF_GRAYED);
						}
						});

					break;
				}
				else {
					for (auto& it : dM_V[platform]) {
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