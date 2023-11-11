#pragma once

#include <vector>

#include "Unit.h"
#include "UnitList.h"

struct TargetWeight {
	int weight;
};

struct ArtificialIntelligence {
	std::vector<TargetWeight> targetWeight;

	inline void GetTarget(Unit& unit, UnitList& unitList) {
		targetWeight.clear();

		unit.itemSlot.IterateWeapon([&] (Weapon*) {
			unitList.IterateUnitList([] (const Unit&) {

			},
			[] (const Unit&) {
				return true;
			});
		});

		// std::sort
	}
};