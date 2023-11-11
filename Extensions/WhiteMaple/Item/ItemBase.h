#pragma once

#include <string>

#include "Class.h"
#include "InfoBase.h"

enum class ItemType {
	Item,
	Weapon,
	Card,
};

enum class ItemEigen {
	Heal,
	PowerUp,
	RemoveState,
};

struct Durability {
	bool bAutoRecover = false;
	size_t durability;
};

struct SpecificHandler {
	bool bSpecific = false;
	long unitID;
	Class unitClass;

	inline bool Usable() {
		if (!bSpecific) { return true; }
	}

	SpecificHandler() = default;
};

struct ItemBase :InfoBase {
	ItemType itemType;
	int weight;
	int price;
	double discount;
	Durability durability;
	ItemEigen eigen;

	bool bDropAble = false;
	bool bUnExchangeAble = false;
	SpecificHandler specificHandler;

	ItemBase() = default;
	ItemBase(IniInterface* pIni) :InfoBase(pIni) {

	}

	virtual ~ItemBase() {}
};
