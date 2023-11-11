#pragma once

#include <vector>
#include <string>

#include "InfoBase.h"
#include "ItemBase.h"
#include "Property.h"
#include "StateHandler.h"

using CharID = long;

struct UnitAttribute {
	size_t init;
	size_t cost;
	size_t max;
};

struct UnitBase :InfoBase {
	// Basic
	CharID charID;
	Property property;

	std::vector<State> abilities;

	UnitAttribute hp;
	UnitAttribute defSlot;

	UnitAttribute str;
	UnitAttribute def;
	UnitAttribute mag;
	UnitAttribute res;
	UnitAttribute spe;
	UnitAttribute ski;

	// Roster
	std::wstring gender;
	unsigned char age;
	unsigned char height;

	std::wstring experience;
	std::wstring interest;
	std::wstring like;
	std::wstring dislike;

	UnitBase() = default;
	UnitBase(IniInterface* pIni) :InfoBase(pIni) {

	}
	virtual ~UnitBase() {}
};