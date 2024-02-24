#pragma once

#include <functional>

#include "SteamInclude.h"

class SteamDLC :public SteamCallbackClass {
private:
	using DLCInstalledCallback_t = std::function<void(AppId_t)>;

	DLCInstalledCallback_t DLCInstalledCallback = nullptr;

	inline void InitCallback() override {
		AddCallback(GetCallBack<DlcInstalled_t>([&] (const DlcInstalled_t* pCallback) {
			if (DLCInstalledCallback != nullptr) {
				DLCInstalledCallback(pCallback->m_nAppID);
			}

			return true;
			}));
	}
public:
	SteamDLC() {
		SteamDLC::InitCallback();
	}
	~SteamDLC() override = default;

	static inline bool ActivateGameOverlayToStore(const AppId_t nAppID, const EOverlayToStoreFlag eFlag = k_EOverlayToStoreFlag_None) {
		if (!SteamUtils()->IsOverlayEnabled()) { return false; }

		SteamFriends()->ActivateGameOverlayToStore(nAppID, eFlag);

		return true;
	}

	inline bool InGamePurchase_ShowAllDownloadAbleContent() const {
		return ActivateGameOverlayToStore(this->appID);
	}

	// nAppID = k_uAppIdInvalid = to Steam home page
	static inline bool InGamePurchase_ShowGivenDownloadAbleContent(const AppId_t nAppID = k_uAppIdInvalid, const EOverlayToStoreFlag eFlag = k_EOverlayToStoreFlag_None) {
		return ActivateGameOverlayToStore(nAppID, eFlag);
	}

	static inline bool DLCInstalled(const AppId_t appID) {
		return SteamApps()->BIsDlcInstalled(appID);
	}

	static inline void InstallDLC(const AppId_t appID) {
		SteamApps()->InstallDLC(appID);
	}

	inline void SetDLCInstallCallback(const DLCInstalledCallback_t& cb) {
		this->DLCInstalledCallback = cb;
	}

	static inline void UninstallDLC(const AppId_t appID) {
		SteamApps()->UninstallDLC(appID);
	}

	// 0.00 ~ 100.00
	static inline float GetDlcDownloadProgressPercent(const AppId_t appID) {
		uint64 bytesDownloaded = 0;
		uint64 bytesTotal = 0;
		SteamApps()->GetDlcDownloadProgress(appID, &bytesDownloaded, &bytesTotal);

		float percent = static_cast<float>(static_cast<double>(bytesDownloaded) / static_cast<double>(bytesTotal));
		percent = round(percent * 10000) / 100;

		return percent;
	}
};