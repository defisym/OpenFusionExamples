#pragma once

#include <vector>

namespace Refresh {
	enum class RefreshType {
		None,
		AchievementAndStat,
	};

	using RefreshTasks = std::vector<RefreshType>;

	inline bool UniquePush(RefreshTasks* pTasks, RefreshType task) {
		if (std::ranges::find(*pTasks, task) == pTasks->end()) {
			pTasks->push_back(task);

			return true;
		}

		return false;
	}

	inline void Refresh(RefreshTasks* pTasks) {
		for (const auto& it : *pTasks) {
			switch (it) {
			case Refresh::RefreshType::AchievementAndStat:
				SteamUserStats()->StoreStats();
			}
		}

		pTasks->clear();
	}
}