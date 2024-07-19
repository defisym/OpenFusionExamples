#pragma once

#include "SteamInclude.h"

class SteamGameRecord :public SteamCallbackClass {
private:
	inline void InitCallback() override {
	//AddCallback(GetCallBack<CallbackType>([&] (const CallbackType* pCallback) {
	//	return pCallback->m_eResult == k_EResultOK
	//		&& pCallback->m_nGameID == appID;
	//	}));
	}
public:
	SteamGameRecord() {
		SteamGameRecord::InitCallback();
	}
	~SteamGameRecord() override = default;

	static inline void SetTimelineStateDescription(const char* pchDescription, const float flTimeDelta) {
		SteamTimeline()->SetTimelineStateDescription(pchDescription, flTimeDelta);
	}

	static inline void ClearTimelineStateDescription(const float flTimeDelta) {
		SteamTimeline()->ClearTimelineStateDescription(flTimeDelta);
	}

	static inline void AddTimelineEvent(const char* pchIcon, const char* pchTitle,
		const char* pchDescription, const uint32 unPriority,
		const float flStartOffsetSeconds, const float flDurationSeconds,
		const ETimelineEventClipPriority ePossibleClip) {
		SteamTimeline()->AddTimelineEvent(pchIcon,pchTitle,
			pchDescription,unPriority,
			flStartOffsetSeconds,flDurationSeconds,
			ePossibleClip);
	}

	static void SetTimelineGameMode(const ETimelineGameMode eMode) {
		SteamTimeline()->SetTimelineGameMode(eMode);
	}
};
