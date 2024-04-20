#pragma once

#include "eos_stats.h"

#include "EOSPlatformBase.h"

// Doc
// https://dev.epicgames.com/docs/zh-Hans/game-services/eos-stats-interface
// https://dev.epicgames.com/zh-CN/news/manage-player-statistics-with-epic-online-services

class EOSStat :public PlatformBase {
private:
	using CallbackType = std::function<void(EOSStat*)>;
	inline const static CallbackType defaultCb = [] (EOSStat*) {};

	bool bStatQuery = false;
	CallbackType statQueryCb;
	CallbackType statIngestCb;

	bool bNeedIngest = false;

public:
	explicit EOSStat(EOSUtilities* pEU) : PlatformBase(pEU) {}
	~EOSStat() override = default;
	inline void PlatformInit() override {
		PlatformQuery();
	}
	inline void PlatformQuery() override {
		QueryStat();
	}

	inline bool QueryComplete() const { return bStatQuery; }

	inline void QueryStat(const CallbackType& cb = defaultCb) {
		if (!PlatformOK()) { return; }

		bStatQuery = false;
		statQueryCb = cb;

		const auto statHandle = EOS_Platform_GetStatsInterface(pEU->platformHandle);

		EOS_Stats_QueryStatsOptions queryStatOptions{};
		queryStatOptions.ApiVersion = EOS_STATS_INGESTSTAT_API_LATEST;
		queryStatOptions.LocalUserId = pEU->productUserId;
		queryStatOptions.TargetUserId = pEU->productUserId;

		queryStatOptions.StartTime = EOS_STATS_TIME_UNDEFINED;
		queryStatOptions.EndTime = EOS_STATS_TIME_UNDEFINED;
		queryStatOptions.StatNames = nullptr;
		queryStatOptions.StatNamesCount = 0;

		callbackCounter.CallCallback();
		EOS_Stats_QueryStats(statHandle, &queryStatOptions, this, [] (const EOS_Stats_OnQueryStatsCompleteCallbackInfo* Data) {
			const auto pES = static_cast<decltype(this)>(Data->ClientData);
			CallbackCounterHelper callbackCounterHelper(pES->callbackCounter);

			if (!EOSUtilities::EOSOK(Data->ResultCode)) {
				pES->pEU->SetLastError("Stat", "Failed to query stat", Data->ResultCode);
				return;
			}

			pES->bStatQuery = true;
			pES->statQueryCb(pES);
		});
	}

	inline void IngestStat(const std::vector<std::pair<const std::string, const int32_t>>& toIngest, const CallbackType& cb = defaultCb) {
		if (!PlatformOK()) { return; }

		statIngestCb = cb;

		// generate array
		const auto sz = toIngest.size();
		const auto pArray = new EOS_Stats_IngestData[sz];

		for (auto i = 0u; i < sz; i++) {
			pArray[i].ApiVersion = EOS_STATS_INGESTDATA_API_LATEST;
			pArray[i].StatName = toIngest[i].first.c_str();
			pArray[i].IngestAmount = toIngest[i].second;
		}

		const auto statHandle = EOS_Platform_GetStatsInterface(pEU->platformHandle);

		EOS_Stats_IngestStatOptions ingestStatOptions{};
		ingestStatOptions.ApiVersion = EOS_STATS_INGESTSTAT_API_LATEST;
		ingestStatOptions.LocalUserId = pEU->productUserId;
		ingestStatOptions.TargetUserId = pEU->productUserId;
		ingestStatOptions.Stats = pArray;
		ingestStatOptions.StatsCount = sz;

		callbackCounter.CallCallback();
		EOS_Stats_IngestStat(statHandle, &ingestStatOptions,this,[](const EOS_Stats_IngestStatCompleteCallbackInfo* Data) {
			const auto pES = static_cast<decltype(this)>(Data->ClientData);
			CallbackCounterHelper callbackCounterHelper(pES->callbackCounter);

			if (!EOSUtilities::EOSOK(Data->ResultCode)) {
				pES->pEU->SetLastError("Stat", "Failed to ingest stat", Data->ResultCode);
				return;
			}

			pES->statIngestCb(pES);
		});

		delete[] pArray;
	}

	inline bool GetStatByName(const std::string& name, const std::function<void(EOS_Stats_Stat*)>& cb) const {
		if (!PlatformOK()) { return false; }

		const auto statHandle = EOS_Platform_GetStatsInterface(pEU->platformHandle);

		EOS_Stats_CopyStatByNameOptions copyStatByNameOptions{};
		copyStatByNameOptions.ApiVersion = EOS_STATS_COPYSTATBYNAME_API_LATEST;
		copyStatByNameOptions.TargetUserId = pEU->productUserId;
		copyStatByNameOptions.Name = name.c_str();

		EOS_Stats_Stat* pStat = nullptr;
		if (EOSUtilities::EOSOK(EOS_Stats_CopyStatByName(statHandle, &copyStatByNameOptions, &pStat))) {
			cb(pStat);
			EOS_Stats_Stat_Release(pStat);

			return true;
		}

		pEU->SetLastError("Stat", "Failed to get stat " + name);
		return false;
	}

	inline uint32_t GetStatCount() const {
		if (!PlatformOK()) { return static_cast<uint32_t>(-1); }

		const auto statHandle = EOS_Platform_GetStatsInterface(pEU->platformHandle);

		EOS_Stats_GetStatCountOptions getStatCountOptions{};
		getStatCountOptions.ApiVersion = EOS_STATS_GETSTATSCOUNT_API_LATEST;
		getStatCountOptions.TargetUserId = pEU->productUserId;

		const auto count = EOS_Stats_GetStatsCount(statHandle, &getStatCountOptions);

		return count;
	}

	inline bool GetStatByIndex(const size_t idx, const std::function<void(EOS_Stats_Stat*)>& cb) const {
		if (!PlatformOK()) { return false; }

		const auto statHandle = EOS_Platform_GetStatsInterface(pEU->platformHandle);

		const auto count = GetStatCount();

		if (idx >= count) {
			return false;
		}

		EOS_Stats_CopyStatByIndexOptions copyStatByIndexOptions{};
		copyStatByIndexOptions.ApiVersion = EOS_STATS_COPYSTATBYNAME_API_LATEST;
		copyStatByIndexOptions.TargetUserId = pEU->productUserId;
		copyStatByIndexOptions.StatIndex = idx;

		EOS_Stats_Stat* pStat = nullptr;
		if (EOSUtilities::EOSOK(EOS_Stats_CopyStatByIndex(statHandle, &copyStatByIndexOptions, &pStat))) {
			cb(pStat);
			EOS_Stats_Stat_Release(pStat);

			return true;
		}

		pEU->SetLastError("Stat", "Failed to get stat");
		return false;
	}

	inline void IterateStats(const std::function<void(EOS_Stats_Stat*)>& cb) {
		if (!QueryComplete()) {
			QueryStat([this, cb] (decltype(this)) {
				IterateStats(cb);
			});

			return;
		}

		auto count = GetStatCount();

		for (decltype(count) i = 0; i < count; i++) {
			auto bRet = GetStatByIndex(i, cb);
		}
	}
};
