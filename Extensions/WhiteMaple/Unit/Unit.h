#pragma once

#include "Class.h"
#include "UnitBase.h"

struct Unit :UnitBase {
	bool bAI;
	// to check if is ally or enemy
	size_t identifier;

	Class unitClass;
	std::vector<ItemBase*> itemSlot;
	StateHandler stateHandler;
};