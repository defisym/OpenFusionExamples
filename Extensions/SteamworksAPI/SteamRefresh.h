#pragma once

#include <vector>

class SteamRefreshClass {
public:
	enum class RefreshType :unsigned int {
		None = 0,
		AchievementAndStat,
	};

	using RefreshTasks = std::vector<RefreshType>;

private:
	// copy only, do not release
	RefreshTasks* pTasks = nullptr;

	static inline bool UniquePush(RefreshTasks* pTasks, const RefreshType task) {
		if (std::ranges::find(*pTasks, task) == pTasks->end()) {
			pTasks->push_back(task);

			return true;
		}

		return false;
	}

public:
	explicit SteamRefreshClass(RefreshTasks* pTasks) {
		this->pTasks = pTasks;
	}

	// Add task then refresh it in handle rountine
	inline void AddToRefresh(RefreshType type) const {
		if (pTasks == nullptr) { return; }
		if (type == RefreshType::None) { return; }

		UniquePush(pTasks, type);
	}

	// handle refresh
	// tasks added will be removed once refresh is done
	static inline void Refresh(RefreshTasks* pTasks) {
		for (const auto& it : *pTasks) {
			switch (it) {
			case RefreshType::AchievementAndStat:
			{
				SteamUserStats()->StoreStats();
				break;
			}
			case RefreshType::None:
				continue;
			}
		}

		pTasks->clear();
	}
};
