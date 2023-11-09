#pragma once

#include "Unit.h"
#include "UnitList.h"

struct ArtificialIntelligence {
	inline void GetTarget(Unit& unit, UnitList& unitList) {
		unit.itemSlot.IterateWeapon([&] (Weapon*) {
			unitList.IterateUnitList([] (const Unit&) {

			},
			[] (const Unit&) {
				return true;
			});
		});
	}
};