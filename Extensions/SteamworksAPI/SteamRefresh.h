#pragma once

#include <vector>

class SteamRefreshClass {
public:
	enum class RefreshType :unsigned int {
		None = 0,
		Custom,
		AchievementAndStat,
	};

	struct RefreshTask {
		RefreshType type = RefreshType::None;
		SteamRefreshClass* pClass = nullptr;

		bool operator==(const RefreshTask& task) const {
			return this->type == task.type && this->pClass == task.pClass;
		}
	};
	using RefreshTasks = std::vector<RefreshTask>;

private:
	// copy only, do not release
	RefreshTasks* pTasks = nullptr;

	static inline bool UniquePush(RefreshTasks* pTasks, const RefreshTask task) {
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
	virtual ~SteamRefreshClass() = default;

	// Add task then refresh it in handle rountine
	inline void AddToRefresh(const RefreshType type) {
		if (pTasks == nullptr) { return; }
		if (type == RefreshType::None) { return; }

		UniquePush(pTasks, { type,this });
	}

	// refresh class itself
	virtual inline void Refresh() {}
	// handle refresh
	// tasks added will be removed once refresh is done
	static inline void Refresh(RefreshTasks* pTasks) {
		for (const auto& [type, pChild] : *pTasks) {
			switch (type) {
			case RefreshType::None:
				continue;
			case RefreshType::Custom:
			{
				pChild->Refresh();
				break;
			}
			case RefreshType::AchievementAndStat:
			{
				SteamUserStats()->StoreStats();
				break;
			}
			}
		}

		pTasks->clear();
	}
};
