#pragma once

#include <string>
#include <functional>

#include "SteamCallback.h"

class SteamUtilities {
private:
	SteamCallback* pCallback = nullptr;

	bool bReceived = false;;
	bool bToRefresh_UpdateAchAndStat = false;

public:
	SteamUtilities() {
		bool bSuccess = SteamUserStats()->RequestCurrentStats();

		//pCallback = GetCallBack<UserStatsReceived_t>([&](UserStatsReceived_t* pCallback) {
		//	bReceived = true;
		//	});

		//SteamUtils()->SetOverlayNotificationPosition(k_EPositionTopLeft);
	}
	~SteamUtilities() {
		delete pCallback;
		pCallback = nullptr;
	}

	std::wstring DeviceFactorToName(ESteamDeviceFormFactor factor) {
		std::wstring name = L"Unknown";

		switch (factor)	{
		case k_ESteamDeviceFormFactorPhone:
			name = L"Phone";

			break;
		case k_ESteamDeviceFormFactorTablet:
			name = L"Tablet";

			break;
		case k_ESteamDeviceFormFactorComputer:
			name = L"Computer";

			break;
		case k_ESteamDeviceFormFactorTV:
			name = L"TV";

			break;
		}

		return name;
	}

	ESteamDeviceFormFactor DeviceNameToFactor(LPCWSTR pPlatform) {
		ESteamDeviceFormFactor factorToSearch = k_ESteamDeviceFormFactorUnknown;

		do {
			if (StrIEqu(pPlatform, L"Phone")) {
				factorToSearch = k_ESteamDeviceFormFactorPhone;

				break;
			}

			if (StrIEqu(pPlatform, L"Tablet")) {
				factorToSearch = k_ESteamDeviceFormFactorTablet;

				break;
			}

			if (StrIEqu(pPlatform, L"Computer")) {
				factorToSearch = k_ESteamDeviceFormFactorComputer;

				break;
			}

			if (StrIEqu(pPlatform, L"TV")) {
				factorToSearch = k_ESteamDeviceFormFactorTV;

				break;
			}
		} while (0);

		return factorToSearch;
	}

	inline void IterateRemoteSessions(std::function<void(RemotePlaySessionID_t)> callBack) {
		uint32 unSessionCount = SteamRemotePlay()->GetSessionCount();
		for (uint32 iIndex = 0; iIndex < unSessionCount; iIndex++) {
			RemotePlaySessionID_t unSessionID = SteamRemotePlay()->GetSessionID(iIndex);
			if (!unSessionID) {
				continue;
			}

			callBack(unSessionID);
		}
	}

	inline void IterateAchievements(std::function<void(uint32)> callback) {
		auto achiNum = SteamUserStats()->GetNumAchievements();
		for (decltype(achiNum) i = 0; i < achiNum; i++) {
			callback(i);
		}
	}

	inline void ResetAchievement(LPCWSTR pAchName = nullptr) {
		ResetAchievement(pAchName != nullptr
			? ConvertWStrToStr(pAchName).c_str()
			: nullptr);
	}

	inline void ResetAchievement(LPCSTR pAchName = nullptr) {
		if (pAchName != nullptr) {
			SteamUserStats()->ClearAchievement(pAchName);
		}

		IterateAchievements([](uint32 id) {
			auto pName = SteamUserStats()->GetAchievementName(id);
			SteamUserStats()->ClearAchievement(pName);
			});
	}

	inline void UnlockAchievement(LPCWSTR pAchName) {
		UnlockAchievement(ConvertWStrToStr(pAchName).c_str());
	}

	inline void UnlockAchievement(LPCSTR pAchName) {
		bool bUnlock = false;
		auto bState = SteamUserStats()->GetAchievement(pAchName, &bUnlock);

		if (!bUnlock) {
			SteamUserStats()->SetAchievement(pAchName);
			ToRefresh_UpdateAchAndStat();
		}
	}

	// ToRefresh series: set flag then refresh it later
	inline void ToRefresh_UpdateAchAndStat() {
		this->bToRefresh_UpdateAchAndStat = true;
	}

	inline void ToRefresh_Refresh() {
		if (this->bToRefresh_UpdateAchAndStat) {
			this->bToRefresh_UpdateAchAndStat = false;
			SteamUserStats()->StoreStats();
		}
	}

	// actual refresh
	inline void Refresh() {
		ToRefresh_Refresh();

		SteamAPI_RunCallbacks();		
	}
};