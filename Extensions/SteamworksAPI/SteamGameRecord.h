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

    // ------------------------
    // TimelineEvent
    // ------------------------

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

    inline void StartRangeTimelineEvent(const char* pchTitle, const char* pchDescription,
        const char* pchIcon, const uint32 unPriority, const float flStartOffsetSeconds,
        const ETimelineEventClipPriority ePossibleClip) {
        if (!bEnable) { return; }
        hPreviousHandle = SteamTimeline()->StartRangeTimelineEvent(pchTitle, pchDescription,
            pchIcon, unPriority, flStartOffsetSeconds,
            ePossibleClip);
    }

    inline void UpdateRangeTimelineEvent(const TimelineEventHandle_t ulEvent,
        const char* pchTitle, const char* pchDescription, 
        const char* pchIcon, const uint32 unPriority,
        const ETimelineEventClipPriority ePossibleClip) const {
        if (!bEnable) { return; }
        SteamTimeline()->UpdateRangeTimelineEvent(ulEvent,
            pchTitle, pchDescription, pchIcon, unPriority,
            ePossibleClip);
    }

    inline void EndRangeTimelineEvent(const TimelineEventHandle_t ulEvent,
        const float flEndOffsetSeconds) const {
        if (!bEnable) { return; }
        SteamTimeline()->EndRangeTimelineEvent(ulEvent, flEndOffsetSeconds);
    }

    inline TimelineEventHandle_t GetPreviousHandle() const { return hPreviousHandle; }

    inline void RemoveTimelineEvent(const TimelineEventHandle_t hEvent) const {
        if (!bEnable) { return; }
        SteamTimeline()->RemoveTimelineEvent(hEvent);
	}

    // will check if handle vaild first then open overlay in callback
    inline void OpenOverlayToTimelineEvent(const TimelineEventHandle_t hEvent) {
        if (!bEnable) { return; }
        const auto hCall = SteamTimeline()->DoesEventRecordingExist(hEvent);
        AddCallback(GetCallBack<SteamTimelineEventRecordingExists_t>(hCall,
            [&] (const SteamTimelineEventRecordingExists_t* pCallback, bool bIOFailure) {
                if (pCallback->m_bRecordingExists) {
                    SteamTimeline()->OpenOverlayToTimelineEvent(pCallback->m_ulEventID);
                }

                return pCallback->m_bRecordingExists;
            }));
    }

    // ------------------------
    // GamePhase
    // ------------------------

    inline void StartGamePhase() const {
        if (!bEnable) { return; }
        SteamTimeline()->StartGamePhase();
    }

    inline void EndGamePhase() const {
        if (!bEnable) { return; }
        SteamTimeline()->EndGamePhase();
    }

    inline void SetGamePhaseID(const char* pchPhaseID) const {
        if (!bEnable) { return; }
        SteamTimeline()->SetGamePhaseID(pchPhaseID);
    }

    inline void AddGamePhaseTag(const char* pchTagName, const char* pchTagIcon,
        const char* pchTagGroup, const uint32 unPriority) const {
        if (!bEnable) { return; }
        SteamTimeline()->AddGamePhaseTag(pchTagName, pchTagIcon, pchTagGroup, unPriority);
    }

    inline void SetGamePhaseAttribute(const char* pchAttributeGroup, const char* pchAttributeValue,
        const uint32 unPriority) const {
        if (!bEnable) { return; }
        SteamTimeline()->SetGamePhaseAttribute(pchAttributeGroup, pchAttributeValue, unPriority);
    }

    inline void OpenOverlayToGamePhase(const char* pchPhaseID) {
        if (!bEnable) { return; }
        const auto hCall = SteamTimeline()->DoesGamePhaseRecordingExist(pchPhaseID);
        AddCallback(GetCallBack<SteamTimelineGamePhaseRecordingExists_t>(hCall,
            [&] (const SteamTimelineGamePhaseRecordingExists_t* pCallback, bool bIOFailure) {
                // seems the call always success
                SteamTimeline()->OpenOverlayToGamePhase(pCallback->m_rgchPhaseID);

                return true;
            }));
    }
};
