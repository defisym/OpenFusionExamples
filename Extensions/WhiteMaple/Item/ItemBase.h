#pragma once

#include <string>

#include "Class.h"

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
};

struct ItemBase {
	std::wstring name;
	std::wstring explanation;
	std::wstring IConFileName;
	ItemType itemType;
	int weight;
	int price;
	double discount;
	Durability durability;
	ItemEigen eigen;

	bool bDropAble = false;
	bool bUnExchangeAble = false;
	SpecificHandler specificHandler;

	virtual ~ItemBase() {}
};

struct Correction {
	long str;
	long def;
	long mag;
	long res;
	long spe;
	long ski;

	long moveRange;
	long attackRange;
	long attackInterval;
	long repelRange;

	long hit;
	long critRate;
	long defSlot;
};