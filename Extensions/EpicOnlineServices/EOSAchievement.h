#pragma once

#include "eos_achievements.h"

#include "EOSPlatformBase.h"

// Doc
// https://dev.epicgames.com/docs/zh-Hans/game-services/achievements
// https://dev.epicgames.com/zh-CN/news/adding-achievements-to-your-game

class EOSAchievement :public PlatformBase {
private:
	using CallbackType = std::function<void(EOSAchievement*)>;
	inline const static CallbackType defaultCb = [] (EOSAchievement*) {};

	bool bAchievementQuery = false;
	CallbackType achievementQueryCb = nullptr;
	CallbackType achievementUnlockCb = nullptr;

public:
	explicit EOSAchievement(EOSUtilities* pEU) : PlatformBase(pEU) {}
	~EOSAchievement() override = default;
	inline void PlatformInit() override {
		PlatformQuery();
	}
	inline void PlatformQuery() override {
		QueryAchievementDefinitions();
	}

	inline bool QueryComplete() const { return bAchievementQuery; }

	inline void QueryAchievementDefinitions(const CallbackType& cb = defaultCb) {
		if (!PlatformOK()) { return; }

		bAchievementQuery = false;
		achievementQueryCb = cb;

		const auto achHandle = EOS_Platform_GetAchievementsInterface(pEU->platformHandle);

		EOS_Achievements_QueryDefinitionsOptions opt{};
		opt.ApiVersion = EOS_ACHIEVEMENTS_QUERYDEFINITIONS_API_LATEST;
		opt.LocalUserId = pEU->productUserId;

		callbackCounter.CallCallback();
		EOS_Achievements_QueryDefinitions(achHandle, &opt, this, [] (const EOS_Achievements_OnQueryDefinitionsCompleteCallbackInfo* Data) {
			const auto pEA = static_cast<decltype(this)>(Data->ClientData);
			CallbackCounterHelper callbackCounterHelper(pEA->callbackCounter);

			if (!EOSUtilities::EOSOK(Data->ResultCode)) {
				pEA->pEU->SetLastError("Achievement", "Failed to query definition", Data->ResultCode);
				return;
			}

			pEA->bAchievementQuery = true;
			pEA->achievementQueryCb(pEA);
		});
	}

	inline void UnlockAchievements(const std::vector<std::string>& toUnlock, const CallbackType& cb = defaultCb) {
		if (!PlatformOK()) { return; }

		achievementUnlockCb = cb;

		// generate array
		const auto sz = toUnlock.size();
		const auto pArray = new const char* [sz];

		for (auto i = 0u; i < sz; i++) {
			pArray[i] = toUnlock[i].c_str();
		}

		// unlock
		const auto achHandle = EOS_Platform_GetAchievementsInterface(pEU->platformHandle);
		EOS_Achievements_UnlockAchievementsOptions unlockAchievementsOptions{};
		unlockAchievementsOptions.ApiVersion = EOS_ACHIEVEMENTS_UNLOCKACHIEVEMENTS_API_LATEST;
		unlockAchievementsOptions.UserId = pEU->productUserId;
		unlockAchievementsOptions.AchievementIds = pArray;
		unlockAchievementsOptions.AchievementsCount = sz;

		callbackCounter.CallCallback();
		EOS_Achievements_UnlockAchievements(achHandle, &unlockAchievementsOptions, this,
			[] (const EOS_Achievements_OnUnlockAchievementsCompleteCallbackInfo* Data) {
				const auto pEA = static_cast<decltype(this)>(Data->ClientData);
				CallbackCounterHelper callbackCounterHelper(pEA->callbackCounter);

				if (!EOSUtilities::EOSOK(Data->ResultCode)) {
					pEA->pEU->SetLastError("Achievement", "Failed to unlock achievement", Data->ResultCode);
					return;
				}

				pEA->achievementUnlockCb(pEA);
			});

		delete[] pArray;
	}

	inline bool GetAchievementByName(const std::string& name, const std::function<void(EOS_Achievements_DefinitionV2*)>& cb) const {
		if (!PlatformOK()) { return false; }

		const auto achHandle = EOS_Platform_GetAchievementsInterface(pEU->platformHandle);

		EOS_Achievements_CopyAchievementDefinitionV2ByAchievementIdOptions copyOpt{};
		copyOpt.ApiVersion = EOS_ACHIEVEMENTS_COPYDEFINITIONV2BYINDEX_API_LATEST;
		copyOpt.AchievementId = name.c_str();

		EOS_Achievements_DefinitionV2* pDefinition = nullptr;

		if (EOSUtilities::EOSOK(EOS_Achievements_CopyAchievementDefinitionV2ByAchievementId(achHandle, &copyOpt, &pDefinition))) {
			cb(pDefinition);

			EOS_Achievements_DefinitionV2_Release(pDefinition);
			return true;
		}

		pEU->SetLastError("Achievement", "Failed to get achievement " + name);
		return false;
	}

	inline uint32_t GetAchievementCount() const {
		if (!PlatformOK()) { return static_cast<uint32_t>(-1); }

		const auto achHandle = EOS_Platform_GetAchievementsInterface(pEU->platformHandle);

		EOS_Achievements_GetAchievementDefinitionCountOptions getOpt{};
		getOpt.ApiVersion = EOS_ACHIEVEMENTS_GETACHIEVEMENTDEFINITIONCOUNT_API_LATEST;
		const auto count = EOS_Achievements_GetAchievementDefinitionCount(achHandle, &getOpt);

		return count;
	}

	inline bool GetAchievementByIndex(const size_t idx, const std::function<void(EOS_Achievements_DefinitionV2*)>& cb) const {
		if (!PlatformOK()) { return false; }

		const auto achHandle = EOS_Platform_GetAchievementsInterface(pEU->platformHandle);

		const auto count = GetAchievementCount();

		if (idx >= count) {
			return false;
		}

		EOS_Achievements_CopyAchievementDefinitionV2ByIndexOptions copyOpt{};
		copyOpt.ApiVersion = EOS_ACHIEVEMENTS_COPYDEFINITIONV2BYINDEX_API_LATEST;
		copyOpt.AchievementIndex = idx;

		EOS_Achievements_DefinitionV2* pDefinition = nullptr;

		if (EOSUtilities::EOSOK(EOS_Achievements_CopyAchievementDefinitionV2ByIndex(achHandle, &copyOpt, &pDefinition))) {
			cb(pDefinition);

			EOS_Achievements_DefinitionV2_Release(pDefinition);
			return true;
		}

		pEU->SetLastError("Achievement", "Failed to get achievement");
		return false;
	}

	inline void IterateAchievements(const std::function<void(EOS_Achievements_DefinitionV2*)>& cb) {
		if (!QueryComplete()) {
			QueryAchievementDefinitions([this, cb] (decltype(this)) {
				IterateAchievements(cb);
			});

			return;
		}

		auto count = GetAchievementCount();

		for (decltype(count) i = 0; i < count; i++) {			
			auto bRet = GetAchievementByIndex(i, cb);
		}
	}
};
