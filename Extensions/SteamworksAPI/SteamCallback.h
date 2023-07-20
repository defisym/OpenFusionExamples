#pragma once

#include <functional>

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
	virtual ~SteamCallback() = default;
};

// ------------
// Callback helper class, to use lambda
// ------------
template<typename CallBackType>
class SteamCallbackHandler :public SteamCallback {
private:
	using Handler = std::function<void(CallBackType*)>;
	Handler handler = nullptr;

	STEAM_CALLBACK(SteamCallbackHandler, CallbackManager, CallBackType);

public:
	explicit SteamCallbackHandler(Handler handler) {
		this->handler = handler;
	}

	~SteamCallbackHandler() override = default;
};

template<typename CallBackType>
inline void SteamCallbackHandler<CallBackType>::CallbackManager(CallBackType* pCallback) {
	handler(pCallback);
}

// ------------
// Function to get new instance
// ------------
template<typename CallBackType>
inline auto GetCallBack(std::function<void(CallBackType*)> callback) {
	return new SteamCallbackHandler<CallBackType>(callback);
}

// ------------
// Call result helper class, to use lambda
// ------------
template<typename CallBackType>
class SteamCallResultHandler :public SteamCallback {
private:
	using Handler = std::function<void(CallBackType*, bool)>;
	Handler handler = nullptr;

	SteamAPICall_t hSteamAPICall = 0;
	CCallResult<SteamCallResultHandler, CallBackType> callResult;

	inline void OnCallbackResult(CallBackType* pCallback, bool bIOFailure);

public:
	explicit SteamCallResultHandler(SteamAPICall_t hSteamAPICall, Handler handler) {
		this->handler = handler;
		this->hSteamAPICall = hSteamAPICall;
		callResult.Set(hSteamAPICall, this, &SteamCallResultHandler::OnCallbackResult);
	}

	~SteamCallResultHandler() override = default;
};

template<typename CallBackType>
inline void SteamCallResultHandler<CallBackType>::OnCallbackResult(CallBackType* pCallback, bool bIOFailure) {
	handler(pCallback, bIOFailure);
}

// ------------
// Function to get new instance
// ------------
template<typename CallBackType>
inline auto GetCallBack(SteamAPICall_t hSteamAPICall, std::function<void(CallBackType*, bool)> callback) {
	return new SteamCallResultHandler<CallBackType>(hSteamAPICall, callback);
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
//	inline void CallCallback(void* udata = nullptr) override {
//		bCallbackSuccess = false;
//		pCallback = GetCallBack<CallbackType>([&] (const CallbackType* pCallback) {
//			bCallbackSuccess = pCallback->m_eResult == k_EResultOK
//				&& pCallback->m_nGameID == appID;
//		});
//		bool bSuccess = SteamUserStats()->RequestCurrentStats();
//	}
//public:
//	SteamFunction(Refresh::RefreshTasks* pTasks)
//		:SteamCallbackClass(pTasks, Refresh::RefreshType::AchievementAndStat) {
//		SteamFunction::CallCallback();
//	}
//	~SteamFunction() override {
//
//	}
//};

// ------------
// Base class
// ------------
class SteamCallbackClass {
private:
	// copy only, do not release
	Refresh::RefreshTasks* pTasks = nullptr;
	Refresh::RefreshType type = Refresh::RefreshType::None;

protected:
	uint64 appID = 0;

	SteamCallback* pCallback = nullptr;
	bool bCallbackSuccess = false;

public:
	// child classes init pCallback with GetCallBack
	// then call then async operation
	explicit SteamCallbackClass(Refresh::RefreshTasks* pTasks = nullptr,
		Refresh::RefreshType type = Refresh::RefreshType::None) {
		this->appID = SteamUtils()->GetAppID();

		this->pTasks = pTasks;
		this->type = type;
	}
	virtual ~SteamCallbackClass() {
		delete pCallback;
		pCallback = nullptr;
	}

	// Add task then refresh it in handle rountine
	inline void AddToRefresh() const {
		if (pTasks != nullptr) {
			Refresh::UniquePush(pTasks, type);
		}
	}

	inline bool GetCallbackStat() const {
		return bCallbackSuccess;
	}

	virtual inline void CallCallback(void* udata = nullptr) = 0;
};