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

	inline void SetTimelineTooltip(const char* pchDescription, const float flTimeDelta) const {
		if (!bEnable) { return; }
		SteamTimeline()->SetTimelineTooltip(pchDescription, flTimeDelta);
	}

	inline void ClearTimelineTooltip(const float flTimeDelta) const {
		if (!bEnable) { return; }
		SteamTimeline()->ClearTimelineTooltip(flTimeDelta);
	}

	void SetTimelineGameMode(const ETimelineGameMode eMode) const {
		if (!bEnable) { return; }
		SteamTimeline()->SetTimelineGameMode(eMode);
	}

private:
    TimelineEventHandle_t hPreviousHandle = 0;
public:
    inline void AddTimelineEvent(const char* pchTitle, const char* pchDescription,
         const char* pchIcon, const uint32 unPriority,
		const float flStartOffsetSeconds, const float flDurationSeconds,
		const ETimelineEventClipPriority ePossibleClip) {
		if (!bEnable) { return; }

        // new in 1.61
        // if duration is 0, then it is an instantaneous event
        if (NearlyEqualFLT(flDurationSeconds, 0.0f)) {
            hPreviousHandle = SteamTimeline()->AddRangeTimelineEvent(pchTitle, pchDescription,
                pchIcon, (std::min)(unPriority, k_unMaxTimelinePriority),
                flStartOffsetSeconds, flDurationSeconds,
                ePossibleClip);
        }
        else {
            hPreviousHandle = SteamTimeline()->AddInstantaneousTimelineEvent(pchTitle, pchDescription,
                pchIcon, (std::min)(unPriority, k_unMaxTimelinePriority),
                flStartOffsetSeconds,
                ePossibleClip);
        }
	}
};
