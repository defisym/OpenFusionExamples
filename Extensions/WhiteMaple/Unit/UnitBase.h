#pragma once

#include <vector>
#include <string>

#include "Item/ItemBase.h"
#include "Property/Property.h"
#include "State/StateHandler.h"

#include "UnitAttribute.h"

using InternalID = long;
using CharID = long;

struct UnitBase {
	// Basic
	InternalID ID;
	std::wstring name;
	std::wstring headFileName;
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

	virtual ~UnitBase() {}
};