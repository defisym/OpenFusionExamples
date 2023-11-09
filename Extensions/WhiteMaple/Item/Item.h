#pragma once

#include "ItemBase.h"

struct ItemCorrection :Correction {
	long teamSlot;

	long mood;
	long hp;
	long defSlot;
};

struct Item :ItemBase {
	ItemCorrection correction;

	bool bAttachState = false;
	size_t attachStateID;
	double attachStatePossibility;

	bool bRemoveState = false;
	size_t removeStateID;
	double removeStatePossibility;

	bool bInstantEffect = false;

	bool bAnotherMove = false;
	bool bAnotherAttack = false;

	Item() :ItemBase() {

	}
};