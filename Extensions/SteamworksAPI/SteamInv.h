#pragma once

#include "SteamInclude.h"

class SteamInv :public SteamCallbackClass {
	using OnInventoryFullUpdate = std::function<void()>;
	OnInventoryFullUpdate onInventoryFullUpdateCallback = nullptr;
	using OnInventoryResultReady = std::function<void(bool)>;
	OnInventoryResultReady onInventoryResultReadyCallback = nullptr;

	using ItemDetails = std::vector<SteamItemDetails_t>;
	// item details get in callback
	ItemDetails itemDetails;
	// user item details
	ItemDetails playerItems;

	SteamInventoryResult_t hLastFullUpdate = k_SteamInventoryResultInvalid;

	inline void InitCallback() override {
		AddCallback(GetCallBack<SteamInventoryFullUpdate_t>([this] (const SteamInventoryFullUpdate_t* pCallback) {
			// This callback triggers immediately before the ResultReady callback. We shouldn't
			// free the result handle here, as we wil always free it at the end of ResultReady.
			do {
				if (!GetResultItems(pCallback)) { break; }
				playerItems = itemDetails;
				if (!onInventoryFullUpdateCallback) { break; }
				onInventoryFullUpdateCallback();
			} while (false);

			// Remember that we just processed this full update to avoid doing work in ResultReady
			hLastFullUpdate = pCallback->m_handle;
			return true;
			}));
		AddCallback(GetCallBack<SteamInventoryResultReady_t>([this] (const SteamInventoryResultReady_t* pCallback) {
			const auto bCallbackSuccess = pCallback->m_result == k_EResultOK;

			do {
				if (!bCallbackSuccess) { break; }
				if (pCallback->m_handle == hLastFullUpdate) { break; }
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

			if (pCallback->m_handle == hLastFullUpdate) { hLastFullUpdate = k_SteamInventoryResultInvalid; }
			SteamInventory()->DestroyResult(pCallback->m_handle);
			return bCallbackSuccess;
			}));
	}

public:
	explicit SteamInv() {
		SteamInv::InitCallback();
	}
	~SteamInv() override = default;

	// OnInventoryFullUpdate
	inline void SetCallback(const OnInventoryFullUpdate& callback) {
		onInventoryFullUpdateCallback = callback;
	}
	// OnInventoryResultReady
	inline void SetCallback(const OnInventoryResultReady& callback) {
		onInventoryResultReadyCallback = callback;
	}

	static inline void GenerateTestItems(const SteamItemDef_t* pArrayItemDefs, const uint32* punArrayQuantity, const uint32 unArrayLength) {
		SteamInventoryResult_t handle = k_SteamInventoryResultInvalid;
		SteamInventory()->GenerateItems(&handle, pArrayItemDefs, punArrayQuantity, unArrayLength);
	}
	static inline void GenerateTestItems(const wchar_t* pArrayItemDefs, const wchar_t* punArrayQuantity) {
		constexpr auto delimiter = L',';

		const auto def = SplitString<SteamItemDef_t, wchar_t>(pArrayItemDefs, delimiter, [] (const std::wstring_view& item) {
			return _stoi(item);
		});
		const auto qty = SplitString<uint32, wchar_t>(punArrayQuantity, delimiter, [] (const std::wstring_view& item) {
			return static_cast<uint32>(_stoi(item));
		});

		const auto sz = (std::min)(def.size(), qty.size());
		if (sz == 0) { return; }

		GenerateTestItems(def.data(), qty.data(), sz);
	}

#ifdef _DEBUG
	static inline void GenerateTestItems() {
		constexpr SteamItemDef_t newItems[2] = { 110,111 };
		constexpr uint32 quantities[2] = { 1,1 };

		GenerateTestItems(newItems, quantities, std::size(newItems));
	}
#endif

	// return true if the handle's SteamID is current user's SteamID
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

	inline const ItemDetails& GetPlayerItems() {
		return playerItems;
	}

	static inline void GetAllItems() {
		SteamInventoryResult_t handle = k_SteamInventoryResultInvalid;
		SteamInventory()->GetAllItems(&handle);
	}

	static inline void TriggerItemDrop(const SteamItemDef_t dropListDefinition) {
		SteamInventoryResult_t handle = k_SteamInventoryResultInvalid;
		SteamInventory()->TriggerItemDrop(&handle, dropListDefinition);		
	}

	static inline void ConsumeItem(const SteamItemInstanceID_t itemConsume, const uint32 unQuantity) {
		SteamInventoryResult_t handle = k_SteamInventoryResultInvalid;
		SteamInventory()->ConsumeItem(&handle, itemConsume, unQuantity);
	}

	struct ItemHelper {
		// ------------------------
		// General
		// ------------------------
		static std::wstring GetItemDefinitionProperty(const SteamItemDef_t itemDef,
			const wchar_t* pPropName, const wchar_t* pDefault = Empty_Str) {
			uint32 bufSize = 512;
			auto ret = std::string(bufSize, 0);
			if (SteamInventory()->GetItemDefinitionProperty(itemDef, ConvertWStrToStr(pPropName).c_str(), ret.data(), &bufSize)
				&& bufSize <= ret.size()) {
				return ConvertStrToWStr(ret);
			}

			return pDefault;
		}

		// ------------------------
		// Specific
		// ------------------------
		static std::wstring GetLocalizedName(const SteamItemDef_t itemDef) {
			return GetItemDefinitionProperty(itemDef, L"name", L"(unknown)");
		}
		static std::wstring GetLocalizedDescription(const SteamItemDef_t itemDef) {
			return GetItemDefinitionProperty(itemDef, L"description");
		}
		static std::wstring GetIconURL(const SteamItemDef_t itemDef) {
			return GetItemDefinitionProperty(itemDef, L"icon_url");
		}
	};
};