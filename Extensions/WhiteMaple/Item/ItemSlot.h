#pragma once

#include <vector>
#include <functional>

#include "ItemBase.h"
#include "Item.h"
#include "Weapon.h"

struct ItemSlot {
	std::vector<ItemBase*> itemSlot;
	size_t weaponEquipment;

	~ItemSlot() {
		for (auto& item : itemSlot) {
			delete item;
		}
	}

	inline bool ExchangeItem(ItemSlot* pTargetItemSlot, size_t targetIdx, size_t idx) {

	}

	inline bool DropItem(size_t idx) {
		
	}

	inline void IterateWeapon(const std::function<void(Weapon*)>& cb) {
		for (auto& item : itemSlot) {
			if (item == nullptr) {
				continue;
			}

			if (item->itemType == ItemType::Weapon) {
				cb(dynamic_cast<Weapon*>(item));
			}
		}
	}
};