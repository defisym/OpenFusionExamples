#pragma once

#include <vector>
#include <functional>

#include "Unit.h"

struct UnitList {
	std::vector<Unit> unitList;

	inline void IterateUnitList(const std::function<void(Unit&)>& cb, const std::function<bool(Unit&)>& filter = nullptr) {
		for (auto& unit : unitList) {
			if (filter == nullptr || filter(unit)) {
				cb(unit);
			}
		}
	}
};