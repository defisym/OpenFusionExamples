#pragma once

#include "SteamInclude.h"

class SteamGameRecord :public SteamCallbackClass {
private:
	bool bEnable = false;

	inline void InitCallback() override {
	//AddCallback(GetCallBack<CallbackType>([&] (const CallbackType* pCallback) {
	//	return pCallback->m_eResult == k_EResultOK
	//		&& pCallback->m_nGameID == appID;
	//	}));
	}
public:
	SteamGameRecord() {
		SteamGameRecord::InitCallback();
		bEnable = SteamTimeline() != nullptr;
	}
	~SteamGameRecord() override = default;

	inline void SetTimelineStateDescription(const char* pchDescription, const float flTimeDelta) const {
		if (!bEnable) { return; }
		SteamTimeline()->SetTimelineStateDescription(pchDescription, flTimeDelta);
	}

	inline void ClearTimelineStateDescription(const float flTimeDelta) const {
		if (!bEnable) { return; }
		SteamTimeline()->ClearTimelineStateDescription(flTimeDelta);
	}

	void SetTimelineGameMode(const ETimelineGameMode eMode) const {
		if (!bEnable) { return; }
		SteamTimeline()->SetTimelineGameMode(eMode);
	}

	inline void AddTimelineEvent(const char* pchIcon, const char* pchTitle,
		const char* pchDescription, const uint32 unPriority,
		const float flStartOffsetSeconds, const float flDurationSeconds,
		const ETimelineEventClipPriority ePossibleClip) const {
		if (!bEnable) { return; }
		SteamTimeline()->AddTimelineEvent(pchIcon,pchTitle,
			pchDescription, (std::min)(unPriority, k_unMaxTimelinePriority),
			flStartOffsetSeconds,flDurationSeconds,
			ePossibleClip);
	}
};
