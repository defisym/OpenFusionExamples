#pragma once

#include "SteamInclude.h"

class SteamScreenshot :public SteamCallbackClass {
public:
	using OnScreenshotCallback = std::function<void()>;
	OnScreenshotCallback onScreenshotCallback = nullptr;

private:
	ScreenshotHandle handle = NULL;

	inline void CallCallback(void* udata = nullptr) override {
		bCallbackSuccess = false;
		pCallback = GetCallBack<ScreenshotReady_t>([&] (const ScreenshotReady_t* pCallback) {
			bCallbackSuccess = pCallback->m_eResult == k_EResultOK;
			handle = pCallback->m_hLocal;

			if (bCallbackSuccess) {
				onScreenshotCallback();
			}

			bCallbackSuccess = false;
		});
	}
public:
	SteamScreenshot()
		:SteamCallbackClass() {
		SteamScreenshot::CallCallback();
	}
	~SteamScreenshot() override {

	}

	inline void SetCallback(const OnScreenshotCallback& callback) {
		onScreenshotCallback = callback;
	}

	template <STR Name>
	inline bool SetLocation(const Name pchLocation) {
		if(!bCallbackSuccess) {
			return false;
		}

		if constexpr (WSTR<Name>) {
			return SetLocation(ConvertWStrToStr(pchLocation).c_str());
		}
		else {
			return SteamScreenshots()->SetLocation(handle, pchLocation);
		}
	}

	inline bool TagPublishedFile(const PublishedFileId_t unPublishedFileID) const {
		if (!bCallbackSuccess) {
			return false;
		}

		return SteamScreenshots()->TagPublishedFile(handle, unPublishedFileID);
	}
	
	inline bool TagUser(const CSteamID steamID) const {
		if (!bCallbackSuccess) {
			return false;
		}

		return SteamScreenshots()->TagUser(handle, steamID);
	}

	static inline void Trigger() {
		SteamScreenshots()->TriggerScreenshot();
	}
};