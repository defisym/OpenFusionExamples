#pragma once

#include "eos_presence.h"

#include "EOSPlatformBase.h"

// Doc
// https://dev.epicgames.com/docs/zh-Hans/epic-account-services/eos-presence-interface
// https://dev.epicgames.com/zh-CN/news/getting-and-setting-player-presence

class EOSPresence :public PlatformBase {
private:
	using CallbackType = std::function<void(EOSPresence*)>;
	inline const static CallbackType defaultCb = [] (EOSPresence*) {};

	bool bPresenceQuery = false;
	CallbackType presenceQueryCb = nullptr;
	CallbackType presenceSetCb = nullptr;

	constexpr static auto noPresence = "NoPresence";

public:
	explicit EOSPresence(EOSUtilities* pEU) : PlatformBase(pEU) {}
	~EOSPresence() override = default;
	inline void PlatformInit() override {
		PlatformQuery();
	}
	inline void PlatformQuery() override {
		QueryPresence();
	}

	inline bool QueryComplete() const { return bPresenceQuery; }

	inline void QueryPresence(const CallbackType& cb = defaultCb) {
		if (!PlatformOK()) { return; }

		bPresenceQuery = false;
		presenceQueryCb = cb;

		const auto preHandle = EOS_Platform_GetPresenceInterface(pEU->platformHandle);

		EOS_Presence_QueryPresenceOptions queryPresenceOptions{};
		queryPresenceOptions.ApiVersion = EOS_PRESENCE_QUERYPRESENCE_API_LATEST;
		queryPresenceOptions.LocalUserId = pEU->accountId;
		queryPresenceOptions.TargetUserId = pEU->accountId;

		callbackCounter.CallCallback();
		EOS_Presence_QueryPresence(preHandle, &queryPresenceOptions, this, [] (const EOS_Presence_QueryPresenceCallbackInfo* Data) {
			const auto pEP = static_cast<decltype(this)>(Data->ClientData);
			CallbackCounterHelper callbackCounterHelper(pEP->callbackCounter);

			if (!EOSUtilities::EOSOK(Data->ResultCode)) {
				pEP->pEU->SetLastError("Presence", "Failed to query presence", Data->ResultCode);
				return;
			}
			
			pEP->bPresenceQuery = true;
			pEP->presenceQueryCb(pEP);			
		});
	}

	inline bool HasPresence() const {
		if (!PlatformOK()) { return false; }

		const auto preHandle = EOS_Platform_GetPresenceInterface(pEU->platformHandle);

		EOS_Presence_HasPresenceOptions hasPresenceOptions{};
		hasPresenceOptions.ApiVersion = EOS_PRESENCE_HASPRESENCE_API_LATEST;
		hasPresenceOptions.LocalUserId = pEU->accountId;
		hasPresenceOptions.TargetUserId = pEU->accountId;

		return EOS_Presence_HasPresence(preHandle, &hasPresenceOptions);
	}

	inline bool CopyPresence(const std::function<void(EOS_Presence_Info*)>& cb) const {
		if (!PlatformOK()) { return false; }

		const auto preHandle = EOS_Platform_GetPresenceInterface(pEU->platformHandle);

		EOS_Presence_CopyPresenceOptions copyPresenceOptions{};
		copyPresenceOptions.ApiVersion = EOS_PRESENCE_COPYPRESENCE_API_LATEST;
		copyPresenceOptions.LocalUserId = pEU->accountId;
		copyPresenceOptions.TargetUserId = pEU->accountId;

		EOS_Presence_Info* pPresenceInfo = nullptr;

		if(EOSUtilities::EOSOK(EOS_Presence_CopyPresence(preHandle,&copyPresenceOptions, &pPresenceInfo))) {
			cb(pPresenceInfo);
			EOS_Presence_Info_Release(pPresenceInfo);

			return true;
		}

		pEU->SetLastError("Presence", "Failed to copy presence");
		return false;
	}

private:
	inline bool SetPresence(const std::function<EOS_EResult(EOS_HPresenceModification)>& typeCb,
		const CallbackType& cb = defaultCb) {
		if (!PlatformOK()) { return false; }
		
		const auto preHandle = EOS_Platform_GetPresenceInterface(pEU->platformHandle);

		EOS_Presence_CreatePresenceModificationOptions createPresenceModificationOptions{};
		createPresenceModificationOptions.ApiVersion = EOS_PRESENCE_CREATEPRESENCEMODIFICATION_API_LATEST;
		createPresenceModificationOptions.LocalUserId = pEU->accountId;

		EOS_HPresenceModification presenceModificationHandle = nullptr;

		if(EOSUtilities::EOSOK(EOS_Presence_CreatePresenceModification(preHandle, &createPresenceModificationOptions, &presenceModificationHandle))) {
			const auto result = typeCb(presenceModificationHandle);
			const auto bSuccess = EOSUtilities::EOSOK(result);

			if(bSuccess) {
				presenceSetCb = cb;
				
				EOS_Presence_SetPresenceOptions setPresenceOptions{};
				setPresenceOptions.ApiVersion = EOS_PRESENCE_SETPRESENCE_API_LATEST;
				setPresenceOptions.LocalUserId = pEU->accountId;
				setPresenceOptions.PresenceModificationHandle = presenceModificationHandle;

				callbackCounter.CallCallback();
				EOS_Presence_SetPresence(preHandle, &setPresenceOptions, this, [] (const EOS_Presence_SetPresenceCallbackInfo* Data) {
					const auto pEP = static_cast<decltype(this)>(Data->ClientData);
					CallbackCounterHelper callbackCounterHelper(pEP->callbackCounter);

					if (!EOSUtilities::EOSOK(Data->ResultCode)) {
						pEP->pEU->SetLastError("Presence", "Failed to set presence", Data->ResultCode);
						return;
					}

					pEP->presenceSetCb(pEP);
				});
			}
			else {
				pEU->SetLastError("Presence", "Failed to set presence", result);
			}

			EOS_PresenceModification_Release(presenceModificationHandle);
			return bSuccess;
		}

		pEU->SetLastError("Presence", "Failed to create presence modification handle");
		return false;
	}

public:	
	inline bool SetPresenceSetStatus(EOS_Presence_EStatus status) {
		return SetPresence([&] (EOS_HPresenceModification presenceModificationHandle) {
			EOS_PresenceModification_SetStatusOptions setStatusOptions{};
			setStatusOptions.ApiVersion = EOS_PRESENCEMODIFICATION_SETSTATUS_API_LATEST;
			setStatusOptions.Status = status;

			return EOS_PresenceModification_SetStatus(presenceModificationHandle, &setStatusOptions);
		});		
	}

	inline bool SetPresenceSetRawRichText(const std::string& richText) {
		return SetPresence([&] (EOS_HPresenceModification presenceModificationHandle) {
			EOS_PresenceModification_SetRawRichTextOptions setRawRichTextOptions{};
			setRawRichTextOptions.ApiVersion = EOS_PRESENCEMODIFICATION_SETRAWRICHTEXT_API_LATEST;
			setRawRichTextOptions.RichText = richText.c_str();

			return EOS_PresenceModification_SetRawRichText(presenceModificationHandle, &setRawRichTextOptions);
		});
	}

	inline bool SetPresenceSetData(const std::vector<std::pair<std::string, std::string>>& datas) {
		return SetPresence([&] (EOS_HPresenceModification presenceModificationHandle) {
			// generate array
			const auto sz = datas.size();
			const auto pArray = new EOS_Presence_DataRecord[sz];

			for (auto i = 0u; i < sz; i++) {
				pArray[i].ApiVersion = EOS_PRESENCE_DATARECORD_API_LATEST;
				pArray[i].Key = datas[i].first.c_str();
				pArray[i].Value = datas[i].second.c_str();
			}

			EOS_PresenceModification_SetDataOptions setDataOptions{};
			setDataOptions.ApiVersion = EOS_PRESENCEMODIFICATION_SETDATA_API_LATEST;
			setDataOptions.RecordsCount = static_cast<int32_t>(sz);
			setDataOptions.Records = pArray;

			const auto result = EOS_PresenceModification_SetData(presenceModificationHandle, &setDataOptions);

			delete[] pArray;

			return result;
		});
	}
	
	inline bool SetPresenceDeleteData(const std::vector<std::string>& datas) {
		return SetPresence([&] (EOS_HPresenceModification presenceModificationHandle) {
			// generate array
			const auto sz = datas.size();
			const auto pArray = new EOS_PresenceModification_DataRecordId[sz];

			for (auto i = 0u; i < sz; i++) {
				pArray[i].ApiVersion = EOS_PRESENCEMODIFICATION_DATARECORDID_API_LATEST;
				pArray[i].Key = datas[i].c_str();
			}

			EOS_PresenceModification_DeleteDataOptions deleteDataOptions{};
			deleteDataOptions.ApiVersion = EOS_PRESENCEMODIFICATION_DELETEDATA_API_LATEST;
			deleteDataOptions.RecordsCount = static_cast<int32_t>(sz);
			deleteDataOptions.Records = pArray;

			const auto result =EOS_PresenceModification_DeleteData(presenceModificationHandle, &deleteDataOptions);

			delete[] pArray;

			return result;
		});
	}
};