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
// Helper class, to use lambda
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
// base class
// ------------
class SteamCallbackClass {
public:
	SteamCallback* pCallback = nullptr;
	bool bCallbackSuccess = false;

	// copy only
	Refresh::RefreshTasks* pTasks = nullptr;
	Refresh::RefreshType type = Refresh::RefreshType::None;

	// child classes init pCallback with GetCallBack
	// then call then async operation
	explicit SteamCallbackClass(Refresh::RefreshTasks* pTasks,
		Refresh::RefreshType type = Refresh::RefreshType::None) {
		this->pTasks = pTasks;
		this->type = type;
	}
	virtual ~SteamCallbackClass() {
		delete pCallback;
		pCallback = nullptr;
	}

	inline void AddToRefresh() const {
		Refresh::UniquePush(pTasks, type);
	}
};