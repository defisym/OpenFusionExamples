#pragma once

#include "SteamInclude.h"

class SteamInv :public SteamCallbackClass, public SteamRefreshClass {
	using OnInventoryResultReady = std::function<void(bool)>;
	OnInventoryResultReady onInventoryResultReadyCallback = nullptr;
	using OnInventoryFullUpdate = std::function<void()>;
	OnInventoryFullUpdate onInventoryFullUpdateCallback = nullptr;

	using ItemDetails = std::vector<SteamItemDetails_t>;
	// item details get in callback
	ItemDetails itemDetails;
	// user item details
	ItemDetails playerItems;

	inline void InitCallback() override {
		AddCallback(GetCallBack<SteamInventoryResultReady_t>([this] (const SteamInventoryResultReady_t* pCallback) {
			const auto bCallbackSuccess = pCallback->m_result == k_EResultOK;

			do {
				if (!bCallbackSuccess) { break; }
				if (!CheckResultSteamID(pCallback)) { break; }
				if (!GetResultItems(pCallback)) { break; }

				// if user has it, update
				const auto playerItemDetailIt = std::ranges::remove_if(playerItems, [&] (SteamItemDetails_t& playerItemDetail) {
					const auto itemDetailIt = std::ranges::find_if(itemDetails, [&] (const SteamItemDetails_t& itemDetail) {
						return playerItemDetail.m_itemId == itemDetail.m_itemId;
					});
					if (itemDetailIt == itemDetails.end()) { return false; }
					if (itemDetailIt->m_unFlags & k_ESteamItemRemoved) { return true; }

					playerItemDetail = *itemDetailIt;
					itemDetails.erase(itemDetailIt);

					return false;
				}).begin();
				playerItems.erase(playerItemDetailIt, playerItems.end());

				// if user doesn't have it, add
				playerItems.insert(playerItems.end(), itemDetails.begin(), itemDetails.end());

				if (!onInventoryResultReadyCallback) { break; }
				onInventoryResultReadyCallback(bCallbackSuccess);
			} while (false);

			SteamInventory()->DestroyResult(pCallback->m_handle);
			return bCallbackSuccess;
			}));
		AddCallback(GetCallBack<SteamInventoryFullUpdate_t>([this] (const SteamInventoryFullUpdate_t* pCallback) {
			do {
				if (!CheckResultSteamID(pCallback)) { break; }
				if (!GetResultItems(pCallback)) { break; }

				if (!onInventoryFullUpdateCallback) { break; }

				onInventoryFullUpdateCallback();
			} while (false);
			
			return true;
			}));
	}
public:
	explicit SteamInv(RefreshTasks* pTasks)
		: SteamRefreshClass(pTasks) {
		SteamInv::InitCallback();
	}
	~SteamInv() override = default;

	inline void SetCallback(const OnInventoryResultReady& callback) {
		onInventoryResultReadyCallback = callback;
	}
	inline void SetCallback(const OnInventoryFullUpdate& callback) {
		onInventoryFullUpdateCallback = callback;
	}

#ifdef _DEBUG
private:
	SteamItemDef_t newItems[2] = { 110,111 };
	uint32 quantities[2] = { 1,1 };
public:
	inline void GenerateTestItems() const {
		SteamInventory()->GenerateItems(nullptr, newItems, quantities, std::size(newItems));
	}
#endif

	template<typename T>
	static inline bool CheckResultSteamID(T pCallback) {
		// calling SerializeResult/DeserializeResult, but it is better to be safe.
		return SteamInventory()->CheckResultSteamID(pCallback->m_handle, SteamUser()->GetSteamID());
	}

	template<typename T>
	inline bool GetResultItems(T pCallback) {
		do {
			// get items		
			uint32 count = 0;
			if (!SteamInventory()->GetResultItems(pCallback->m_handle, nullptr, &count)) { break; }
			itemDetails.resize(count);
			if (!SteamInventory()->GetResultItems(pCallback->m_handle, itemDetails.data(), &count)) { break; }

			return true;
		} while (false);

		return false;
	}

	static inline void GetAllItems() {
		SteamInventoryResult_t handle = k_SteamInventoryResultInvalid;
		SteamInventory()->GetAllItems(&handle);
	}

	static inline void TriggerItemDrop(SteamItemDef_t dropListDefinition) {
		SteamInventoryResult_t handle = k_SteamInventoryResultInvalid;
		SteamInventory()->TriggerItemDrop(&handle, dropListDefinition);		
	}

	static inline void ConsumeItem(SteamItemInstanceID_t itemConsume, uint32 unQuantity) {
		SteamInventoryResult_t handle = k_SteamInventoryResultInvalid;
		SteamInventory()->ConsumeItem(&handle, itemConsume, unQuantity);
	}
};