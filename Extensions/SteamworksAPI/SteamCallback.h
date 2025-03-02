#pragma once

#include <functional>
#include <utility>

// ------------
// Official Callback example
// ------------
// 
// https://partner.steamgames.com/doc/sdk/api#callbacks
// 
//class SteamCallbackHandler {
//public:
//	STEAM_CALLBACK(SteamCallbackHandler, CallbackManager, GameOverlayActivated_t);
//};
//
//inline void SteamCallbackHandler::CallbackManager(GameOverlayActivated_t* pCallback) {
//	return;
//}

// ------------
// Usage
// ------------
//SteamCallback* pCallback = GetCallBack<CallBackType>([&](CallBackType* pCallback) {
//	// Do somethings
//	});
//
//delete pCallback;
//pCallback = nullptr;

// ------------
// Base class
// ------------
class SteamCallback {
public:
	// callback result
	bool bCallbackSuccess = false;
	// reset callback result, for next callback
	void ResetCallbackResult() { bCallbackSuccess = false; }

	virtual ~SteamCallback() = default;
};

// ------------
// Callback helper class, to use lambda
// ------------
template<typename CallBackType>
class SteamCallbackHandler :public SteamCallback {
private:
	using Handler = std::function<bool(CallBackType*)>;
	Handler handler = nullptr;

	STEAM_CALLBACK(SteamCallbackHandler, CallbackManager, CallBackType);

public:
	explicit SteamCallbackHandler(Handler handler) {
		this->handler = std::move(handler);
    }

	~SteamCallbackHandler() override = default;
};

template<typename CallBackType>
inline void SteamCallbackHandler<CallBackType>::CallbackManager(CallBackType* pCallback) {
	bCallbackSuccess = handler(pCallback);
}

// ------------
// Function to get new instance
// ------------
// return true: callback success
template<typename CallBackType>
inline auto GetCallBack(std::function<bool(CallBackType*)> callback) {
	return new SteamCallbackHandler<CallBackType>(std::move(callback));
}

// ------------
// Call result helper class, to use lambda
// ------------
template<typename CallBackType>
class SteamCallResultHandler :public SteamCallback {
private:
	using Handler = std::function<bool(CallBackType*, bool)>;
	Handler handler = nullptr;

	SteamAPICall_t hSteamAPICall = 0;
	CCallResult<SteamCallResultHandler, CallBackType> callResult;

	inline void OnCallbackResult(CallBackType* pCallback, bool bIOFailure);

public:
	explicit SteamCallResultHandler(SteamAPICall_t hSteamAPICall, Handler handler) {
		this->handler = std::move(handler);
        this->hSteamAPICall = hSteamAPICall;
		callResult.Set(hSteamAPICall, this, &SteamCallResultHandler::OnCallbackResult);
	}

	~SteamCallResultHandler() override = default;
};

template<typename CallBackType>
inline void SteamCallResultHandler<CallBackType>::OnCallbackResult(CallBackType* pCallback, bool bIOFailure) {
	bCallbackSuccess = handler(pCallback, bIOFailure);
}

// ------------
// Function to get new instance
// ------------
// return true: callback success
template<typename CallBackType>
inline auto GetCallBack(SteamAPICall_t hSteamAPICall, std::function<bool(CallBackType*, bool)> callback) {
	return new SteamCallResultHandler<CallBackType>(hSteamAPICall, std::move(callback));
}

// ------------
// Usage
// ------------
//
//#pragma once
//
//#include "SteamInclude.h"
//
//class SteamFunction :public SteamCallbackClass {
//private:
//	inline void InitCallback() override {
//	AddCallback(GetCallBack<CallbackType>([&] (const CallbackType* pCallback) {
//		return pCallback->m_eResult == k_EResultOK
//			&& pCallback->m_nGameID == appID;
//		}));
//
//  if api has STEAM_CALL_RESULT(CallbackType) above the definition
//  or returns a SteamAPICall_t
//  the following callback result should be used instead of callback
//	AddCallback(GetCallBack<CallbackType>([&] (const CallbackType* pCallback) {
//		return pCallback->m_eResult == k_EResultOK
//			&& pCallback->m_nGameID == appID;
//		}));
//	}
//public:
//	SteamFunction() {
//		SteamFunction::InitCallback();
//	}
//	~SteamFunction() override = default;
//};

// ------------
// Base class
// ------------
constexpr auto DefaultCallbackCount = 1;

class SteamCallbackClass {
protected:
	AppId_t appID = 0;
	std::vector<SteamCallback*> SteamCallbacks;

	inline bool CallbackIndexValid(size_t index) const {
		return SteamCallbacks.size() > index;
	}

public:
	// child classes init pCallback with GetCallBack
	// then call the async operation
	explicit SteamCallbackClass() {
		this->appID = SteamUtils()->GetAppID();
		SteamCallbacks.reserve(DefaultCallbackCount);
	}
	virtual ~SteamCallbackClass() {
		for (const auto& it : SteamCallbacks) {
			delete it;
		}
	}

	inline bool GetCallbackStat(size_t index = 0) const {
		if (!CallbackIndexValid(index)) { return false; }

		return SteamCallbacks[index]->bCallbackSuccess;
	}
	inline SteamCallback* GetCallback(size_t index = 0) const {
		if (!CallbackIndexValid(index)) { return nullptr; }

		return SteamCallbacks[index];
	}

	// add callbacks, must be implemented
	virtual inline void InitCallback() = 0;

	inline void AddCallback(SteamCallback* pCallback) {
		SteamCallbacks.emplace_back(pCallback);
	}
	// reset callback result then call worker, keep worker nullptr if callback
	// will be triggered by Steam
	// call the first callback by default
	inline void CallCallback(const std::function<void()>& worker = nullptr) const {
		CallCallback(0, worker);
	}
	// reset callback result then call worker, keep worker nullptr if callback
	// will be triggered by Steam
	// call the given callback
	inline void CallCallback(size_t index, const std::function<void()>& worker = nullptr) const {
		if (!CallbackIndexValid(index)) { return; }
		SteamCallbacks[index]->ResetCallbackResult();

		if (worker != nullptr) { worker(); }
	}
};