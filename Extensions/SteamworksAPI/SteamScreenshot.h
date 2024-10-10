#pragma once

#include "SteamInclude.h"

class SteamScreenshot :public SteamCallbackClass {
public:
	using OnScreenshotCallback = std::function<void()>;
	OnScreenshotCallback onScreenshotCallback = nullptr;

private:
	ScreenshotHandle handle = NULL;

	inline void InitCallback() override {
		AddCallback(GetCallBack<ScreenshotReady_t>([&] (const ScreenshotReady_t* pCallback) {
			const auto bSuccess = pCallback->m_eResult == k_EResultOK;
			handle = pCallback->m_hLocal;

			if (bSuccess && onScreenshotCallback != nullptr) {
				onScreenshotCallback();
			}

			return bSuccess;
			}));
	}

public:
	SteamScreenshot() { SteamScreenshot::InitCallback(); }
	~SteamScreenshot() override = default;

	inline void SetCallback(const OnScreenshotCallback& callback) {
		onScreenshotCallback = callback;
	}

	template <CStyleStrConcept Name>
	inline bool SetLocation(const Name pchLocation) {
		if(!GetCallbackStat()) {
			return false;
		}

		if constexpr (CStyleWideStrConcept<Name>) {
			return SetLocation(ConvertWStrToStr(pchLocation).c_str());
		}
		else {
			return SteamScreenshots()->SetLocation(handle, pchLocation);
		}
	}

	inline bool TagPublishedFile(const PublishedFileId_t unPublishedFileID) const {
		if (!GetCallbackStat()) {
			return false;
		}

		return SteamScreenshots()->TagPublishedFile(handle, unPublishedFileID);
	}
	
	inline bool TagUser(const CSteamID steamID) const {
		if (!GetCallbackStat()) {
			return false;
		}

		return SteamScreenshots()->TagUser(handle, steamID);
	}

	static inline void Trigger() {
		SteamScreenshots()->TriggerScreenshot();
	}
};