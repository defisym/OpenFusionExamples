#pragma once

#include "SteamInclude.h"
#include "SteamRefresh.h"

//------------
// Concepts
//------------

template <typename T>
concept STAT = std::is_same_v<std::remove_cv_t<T>, int32 > || std::is_same_v<std::remove_cv_t<T>, float >;

//------------
// Class
//------------

class SteamAchAndStat :public SteamCallbackClass, public SteamRefreshClass {
private:
	static constexpr size_t UserStatsReceived = 0;
	static constexpr size_t UserStatsStored = 1;
	static constexpr size_t UserAchievementStored = 2;

	inline void InitCallback() override {
		AddCallback(GetCallBack<UserStatsReceived_t>([&] (const UserStatsReceived_t* pCallback) {
			return pCallback->m_eResult == k_EResultOK
				&& pCallback->m_nGameID == appID;
			}));
		AddCallback(GetCallBack<UserStatsStored_t>([&] (const UserStatsStored_t* pCallback) {
			return pCallback->m_eResult == k_EResultOK
				&& pCallback->m_nGameID == appID;
			}));
		AddCallback(GetCallBack<UserAchievementStored_t>([&] (const UserAchievementStored_t* pCallback) {
			return pCallback->m_nGameID == appID;
			}));
	}

public:
	explicit SteamAchAndStat(RefreshTasks* pTasks)
		: SteamRefreshClass(pTasks) {
		SteamAchAndStat::InitCallback();
		CallCallback([]() {
			bool bSuccess = SteamUserStats()->RequestCurrentStats();
		});
	}
	~SteamAchAndStat() override = default;

	//------------
	// Achievements
	//------------

	static inline void IterateAchievements(const std::function<void(uint32)>& callback) {
		auto achiNum = SteamUserStats()->GetNumAchievements();
		for (decltype(achiNum) i = 0; i < achiNum; i++) {
#ifdef _DEBUG
			auto pName = SteamUserStats()->GetAchievementName(i);
#endif
			callback(i);
		}
	}

	template <STR Name>
	inline void ResetAchievement(const Name pAchName = nullptr) {
		if constexpr (WSTR<Name>) {
			ResetAchievement(pAchName != nullptr
			? ConvertWStrToStr(pAchName).c_str()
			: nullptr);
		}
		else {
			if(!GetCallbackStat()) {
				return;
			}

			if (pAchName != nullptr) {
				SteamUserStats()->ClearAchievement(pAchName);
			}

			IterateAchievements([] (const uint32 id) {
				const auto pName = SteamUserStats()->GetAchievementName(id);
				SteamUserStats()->ClearAchievement(pName);
				});
		}
	}

	template <STR Name>
	inline void UnlockAchievement(const Name pAchName) {
		if constexpr (WSTR<Name>) {
			UnlockAchievement(ConvertWStrToStr(pAchName).c_str());
		}
		else {
			if (!GetCallbackStat()) {
				return;
			}

			bool bUnlock = false;
			auto bState = SteamUserStats()->GetAchievement(pAchName, &bUnlock);

			if (!bUnlock) {
				SteamUserStats()->SetAchievement(pAchName);
				AddToRefresh(RefreshType::AchievementAndStat);
			}
		}
	}

	//------------
	// Stat
	//------------

	template <STR Name, STAT Stat>
	inline void GetStat(const Name pStatName, Stat* pData) {
		if constexpr (WSTR<Name>) {
			GetStat(ConvertWStrToStr(pStatName).c_str(), pData);
		}
		else {
			if (!GetCallbackStat()) {
				return;
			}

			SteamUserStats()->GetStat(pStatName, pData);
		}
	}

	template <STR Name, STAT Stat>
	inline void SetStat(const Name pStatName, const Stat data) {
		if constexpr (WSTR<Name>) {
			SetStat(ConvertWStrToStr(pStatName).c_str(), data);
		}
		else {
			if (!GetCallbackStat()) {
				return;
			}

			SteamUserStats()->SetStat(pStatName, data);
			AddToRefresh(RefreshType::AchievementAndStat);
		}
	}

	template <STR Name, STAT Stat>
	inline void AddStat(const Name pStatName, const Stat data) {
		if constexpr (WSTR<Name>) {
			AddStat(ConvertWStrToStr(pStatName).c_str(), data);
		}
		else {
			if (!GetCallbackStat()) {
				return;
			}

			Stat pData = 0;
			GetStat(pStatName, &pData);

			SteamUserStats()->SetStat(pStatName, pData + data);
			AddToRefresh(RefreshType::AchievementAndStat);
		}
	}


	// the unit of dSessionLength should be the same as Window prop
	// you set in Steamworks
	template <STR Name>
	inline void SetAvgRateStat(const Name pStatName, const float flCountThisSession, const double dSessionLength) {
		if constexpr (WSTR<Name>) {
			SetAvgRateStat(ConvertWStrToStr(pStatName).c_str(), flCountThisSession, dSessionLength);
		}
		else {
			if (!GetCallbackStat()) {
				return;
			}

			SteamUserStats()->UpdateAvgRateStat(pStatName, flCountThisSession, dSessionLength);
			AddToRefresh(RefreshType::AchievementAndStat);
		}
	}

	template <STR Name>
	inline void IndicateAchievementProgress(const Name pStatName, const uint32 nCurProgress, const uint32 nMaxProgress) {
		if constexpr (WSTR<Name>) {
			IndicateAchievementProgress(ConvertWStrToStr(pStatName).c_str(), nCurProgress, nMaxProgress);
		}
		else {
			if (!GetCallbackStat()) {
				return;
			}

			SteamUserStats()->IndicateAchievementProgress(pStatName, nCurProgress, nMaxProgress);
		}
	}
};