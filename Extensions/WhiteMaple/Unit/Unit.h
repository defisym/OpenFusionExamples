#pragma once

#include "Class.h"
#include "ItemSlot.h"
#include "UnitBase.h"

struct Unit :UnitBase {
	bool bAI;
	// to check if is ally or enemy
	size_t identifier;

	Class unitClass;
	ItemSlot itemSlot;
	StateHandler stateHandler;

	Unit() = default;
	Unit(IniInterface* pIni) :UnitBase(pIni) {

	}
};