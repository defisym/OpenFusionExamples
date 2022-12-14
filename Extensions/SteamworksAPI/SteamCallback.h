#pragma once

#include <functional>

// ------------
// Official Callback example
// ------------
// 
// https://partner.steamgames.com/doc/sdk/api#callbacks
// 
//class SteamCallbackHandlera {
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
	virtual ~SteamCallback() {};
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
	SteamCallbackHandler(Handler handler) {
		this->handler = handler;
	}

	~SteamCallbackHandler() {};
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
	return new SteamCallbackHandler<UserStatsReceived_t>(callback);
}
