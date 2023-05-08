#pragma once

#include "SteamInclude.h"

//------------
// Concepts
//------------

template <typename T>
concept STAT = std::is_same_v<std::remove_cv_t<T>, int32 > || std::is_same_v<std::remove_cv_t<T>, float >;

//------------
// Class
//------------

class SteamAchAndStat :public SteamCallbackClass {
private:
	inline void CallCallback() override {
		bCallbackSuccess = false;
		pCallback = GetCallBack<UserStatsReceived_t>([&] (const UserStatsReceived_t* pCallback) {
			bCallbackSuccess = pCallback->m_eResult == k_EResultOK
				&& pCallback->m_nGameID == appID;
		});
		bool bSuccess = SteamUserStats()->RequestCurrentStats();
	}
public:
	SteamAchAndStat(Refresh::RefreshTasks* pTasks)
		:SteamCallbackClass(pTasks, Refresh::RefreshType::AchievementAndStat) {
		SteamAchAndStat::CallCallback();
	}
	~SteamAchAndStat() override {

	}
	
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
			if(!bCallbackSuccess) {
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
			if (!bCallbackSuccess) {
				return;
			}

			bool bUnlock = false;
			auto bState = SteamUserStats()->GetAchievement(pAchName, &bUnlock);

			if (!bUnlock) {
				SteamUserStats()->SetAchievement(pAchName);
				AddToRefresh();
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
			if (!bCallbackSuccess) {
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
			if (!bCallbackSuccess) {
				return;
			}

			SteamUserStats()->SetStat(pStatName, data);
			AddToRefresh();
		}
	}

	template <STR Name, STAT Stat>
	inline void AddStat(const Name pStatName, const Stat data) {
		if constexpr (WSTR<Name>) {
			AddStat(ConvertWStrToStr(pStatName).c_str(), data);
		}
		else {
			if (!bCallbackSuccess) {
				return;
			}

			Stat pData = 0;
			GetStat(pStatName, &pData);

			SteamUserStats()->SetStat(pStatName, pData + data);
			AddToRefresh();
		}
	}
};