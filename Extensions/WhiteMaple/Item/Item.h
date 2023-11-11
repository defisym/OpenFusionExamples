#pragma once

#include "ItemBase.h"
#include "Correction.h"

struct Item :ItemBase {
	Correction correction;

	// usability
	bool bUnusable = false;

	// state
	bool bAttachState = false;
	size_t attachStateID;
	double attachStatePossibility;

	bool bRemoveState = false;
	size_t removeStateID;
	double removeStatePossibility;

	// instant effect
	bool bInstantEffect = false;

	bool bAnotherMove = false;
	bool bAnotherAttack = false;

	Item() = default;
	Item(IniInterface* pIni) :ItemBase(pIni) {

	}
};