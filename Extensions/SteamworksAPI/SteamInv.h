#pragma once

#include "SteamInclude.h"

class SteamInv :public SteamCallbackClass, public SteamRefreshClass {
	using OnInventoryResultReady = std::function<void(bool)>;
	OnInventoryResultReady onInventoryResultReadyCallback = nullptr;

	inline void InitCallback() override {
		AddCallback(GetCallBack<SteamInventoryResultReady_t>([this] (const SteamInventoryResultReady_t* pCallback) {
			const auto bCallbackSuccess = pCallback->m_result == k_EResultOK;

			if (onInventoryResultReadyCallback != nullptr) {
				onInventoryResultReadyCallback(bCallbackSuccess);
			}

			SteamInventory()->DestroyResult(pCallback->m_handle);
			return bCallbackSuccess;
			}));
		AddCallback(GetCallBack<SteamInventoryFullUpdate_t>([this] (const SteamInventoryFullUpdate_t* pCallback) {
			const auto vecDetails = GetResultItems(pCallback);
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
	static inline auto GetResultItems(T pCallback) {
		std::vector<SteamItemDetails_t> vecDetails;

		do {
			// Ignore results that belong to some other SteamID - this normally won't happen, unless you start
			// calling SerializeResult/DeserializeResult, but it is better to be safe.
			if (!SteamInventory()->CheckResultSteamID(pCallback->m_handle, SteamUser()->GetSteamID())) { break; }

			// get items		
			uint32 count = 0;
			if (!SteamInventory()->GetResultItems(pCallback->m_handle, nullptr, &count)) { break; }
			vecDetails.resize(count);
			if (!SteamInventory()->GetResultItems(pCallback->m_handle, vecDetails.data(), &count)) { break; }
		} while (false);

		return vecDetails;
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