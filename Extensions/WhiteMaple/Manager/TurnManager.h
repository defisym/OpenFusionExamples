#pragma once

#include "UnitList.h"

struct TurnManager {
	size_t turn;

	inline void StartTurn(){
		
	}

	inline void EndTurn(){
		UpdateState();
	}

	inline void UpdateState(){
		// unitList.IterateUnitList([](Unit& unit){
		// 	unit.stateHandler.UpdateState();
		// });
	}
};
