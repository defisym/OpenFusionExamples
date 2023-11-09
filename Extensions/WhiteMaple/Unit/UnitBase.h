#pragma once

#include <vector>
#include <string>

#include "InfoBase.h"
#include "ItemBase.h"
#include "Property.h"
#include "StateHandler.h"

#include "UnitAttribute.h"

using CharID = long;

struct UnitBase {
	// Basic
	InfoBase info;
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