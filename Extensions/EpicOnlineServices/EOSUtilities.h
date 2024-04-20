#pragma once

#include <cassert>

#include "EOSCallbackCounter.h"
#include "EOSInclude.h"

#include "EOSCommandLine.h"

// Doc
// https://dev.epicgames.com/zh-CN/news/introduction-to-epic-online-services-eos#series-reference
// 
// https://dev.epicgames.com/docs/zh-Hans/epic-account-services/auth/auth-interface
// https://dev.epicgames.com/docs/zh-Hans/game-services/eos-connect-interface

// https://epicsupport.force.com/devportal/s/question/0D54z00008akVjUCAU/how-should-i-use-authentication-while-developing-and-testing-eos-achievements-and-leaderboards?language=en_US


// To debug with DevAuthTool
// disable VPN to avoid 502 error
// portal: 6547
// name: EOS

enum class EOSState {
	Invalid = -1,

	TryInit,
	InitFailed,
	InitSuccess,

	TryAuth,
	AuthFailed,
	AuthSuccess,

	TryConnect,
	ConnectFailed,
	ConnectSuccess,
};

struct EOSQueryType {
	inline static const wchar_t* Achievement = L"Achievement";
	inline static const wchar_t* Stat = L"Stat";
	inline static const wchar_t* Presence = L"Presence";
};

struct EOSUtilities_RuntimeOptions {
	EOS_ELoginCredentialType authCredentialsType = EOS_ELoginCredentialType::EOS_LCT_ExchangeCode;
	EOS_EAuthScopeFlags authPremissions = EOS_EAuthScopeFlags::EOS_AS_NoFlags;

	bool bRequireLauncher = true;
	bool bRequireBootstrap = true;
};

class EOSUtilities {
private:
	friend class PlatformBase;
	friend class EOSAchievement;
	friend class EOSStat;
	friend class EOSPresence;

	using CallbackType = std::function<void(EOSUtilities*)>;
	inline const static CallbackType defaultCb = [] (EOSUtilities*) {};

	bool bInit = false;
	EOSState state = EOSState::Invalid;

	using ErrorCallbackType = std::function<void(const std::string&)>;
	ErrorCallbackType errorCallback = nullptr;

	std::string lastErrorType;
	std::string lastErrorInfo;

	// Get from global, don't need to release
	const EOSCommandLine* pCmdLine = nullptr;
	EOSUtilities_RuntimeOptions runtimeOpt;

	EOS_HPlatform platformHandle = nullptr;

	EOS_EpicAccountId accountId = nullptr;
	EOS_ProductUserId productUserId = nullptr;

	EOS_UserInfo* pUserInfo = nullptr;
	EOS_Auth_Token* pAuthToken = nullptr;
	EOS_Auth_IdToken* pAuthIDToken = nullptr;

	CallbackType authLoginCb = nullptr;
	CallbackType authLogoutCb = nullptr;
	CallbackType deletePersistentAuthCb = nullptr;

	CallbackType connectCb = nullptr;
	CallbackType authExpirationCb = nullptr;
	EOS_NotificationId notificationId = 0;

	constexpr static auto InvalidID = "InvalidID";

	CallbackCounter callbackCounter;
public:
	inline bool AllCallbackComplete() const {
		return callbackCounter.AllCallbackComplete();
	}

public:
	EOSUtilities(const EOSCommandLine* pCmdLine,
		const EOSUtilities_RuntimeOptions& runtimeOpt,
		const EOS_InitializeOptions& initOpt,
		const EOS_Platform_Options& platOpt) {
		state = EOSState::TryInit;

		// runtime
		this->pCmdLine = pCmdLine;
		this->runtimeOpt = runtimeOpt;

		// sdk
		const auto result = EOS_Initialize(&initOpt);
		bInit = EOSOK(result);

		if (!bInit) {
			SetLastError("Init", result);
			state = EOSState::InitFailed;

			return;
		}

		// platform
		platformHandle = EOS_Platform_Create(&platOpt);

		if (platformHandle == nullptr) {
			SetLastError("Create platform", result);
			state = EOSState::InitFailed;

			return;
		}

		if (runtimeOpt.bRequireLauncher) {
			const auto ret = EOS_Platform_CheckForLauncherAndRestart(platformHandle);

			if (ret == EOS_EResult::EOS_Success) {
				exit(0);
			}

			if (ret == EOS_EResult::EOS_NoChange) {

			}

			if (ret == EOS_EResult::EOS_UnexpectedError) {

			}
		}

		state = EOSState::InitSuccess;

#ifdef _DEBUG
		const auto setLogCallbackResult = EOS_Logging_SetCallback([] (const EOS_LogMessage* Message) {
			OutputDebugStringA(std::format("Category: {}, Message: {}\r\n",
			Message->Category, Message->Message).c_str());
		});
#endif
	}

	~EOSUtilities() {
		if (!bInit) {
			return;
		}

		if (pUserInfo != nullptr) {
			EOS_UserInfo_Release(pUserInfo);
			pUserInfo = nullptr;
		}

		if (pAuthToken != nullptr) {
			EOS_Auth_Token_Release(pAuthToken);
			pAuthToken = nullptr;
		}

		if (pAuthIDToken != nullptr) {
			EOS_Auth_IdToken_Release(pAuthIDToken);
			pAuthIDToken = nullptr;
		}

		EOS_Platform_Release(platformHandle);
		EOS_Shutdown();
	}

	static inline bool EOSOK(EOS_EResult result) {
		return result == EOS_EResult::EOS_Success;
	}

	inline void Update() const {
		EOS_Platform_Tick(platformHandle);
	}

	inline auto Init() const { return bInit; }
	inline auto State() const { return state; }

public:
	inline void SetErrorCallback(const ErrorCallbackType& cb) {
		errorCallback = cb;
	}

private:
	inline void CallErrorCallback() {
		if (errorCallback != nullptr) {
			errorCallback(GetLastError());
		}
	}

public:
	inline void SetLastError(const std::string& errType, const std::string& errInfo) {
		lastErrorType = errType;
		lastErrorInfo = errInfo;

		CallErrorCallback();
	}

	inline void SetLastError(const std::string& errType, EOS_EResult result) {
		lastErrorType = errType;
		lastErrorInfo = EOS_EResult_ToString(result);

		CallErrorCallback();
	}

	inline void SetLastError(const std::string& errType, const std::string& errInfo, EOS_EResult result) {
		lastErrorType = errType;
		lastErrorInfo = std::format("{}: {}", errInfo, EOS_EResult_ToString(result));

		CallErrorCallback();
	}

	inline std::string GetLastError() {
		if (lastErrorType.empty() && lastErrorInfo.empty()) {
			return "No error";
		}

		//if(!PlatformOK()) {
		//	return "Platform not initialized";
		//}

		return std::format("Type: {}, Info: {}", lastErrorType, lastErrorInfo);
	}

private:
	inline void DeletePersistentAuth(const CallbackType& cb = defaultCb) {
		deletePersistentAuthCb = cb;

		const auto authHandle = EOS_Platform_GetAuthInterface(platformHandle);

		EOS_Auth_DeletePersistentAuthOptions deletePersistentAuthOptions = {};
		deletePersistentAuthOptions.ApiVersion = EOS_AUTH_DELETEPERSISTENTAUTH_API_LATEST;

		callbackCounter.CallCallback();
		EOS_Auth_DeletePersistentAuth(authHandle, &deletePersistentAuthOptions, this,
			[] (const EOS_Auth_DeletePersistentAuthCallbackInfo* Data) {
				const auto pEU = static_cast<decltype(this)>(Data->ClientData);
				CallbackCounterHelper callbackCounterHelper(pEU->callbackCounter);

				if(!EOSOK(Data->ResultCode)) {
					pEU->SetLastError("Auth", "Failed to delete persistent auth", Data->ResultCode);
				}

				pEU->deletePersistentAuthCb(pEU);
			});
	}

public:
	inline void AuthLogin(const CallbackType& cb = defaultCb) {
		if(!(state == EOSState::InitSuccess || state == EOSState::AuthFailed)) {
			return;
		}

		state = EOSState::TryAuth;
		authLoginCb = cb;

		EOS_Auth_Credentials authCredentials{};
		authCredentials.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;

		switch (runtimeOpt.authCredentialsType) {  // NOLINT(clang-diagnostic-switch-enum)
		//EOS_LCT_Developer - ID is the host (e.g. localhost:6547), and Token is the credential name registered in the EOS Developer Authentication Tool.
		case EOS_ELoginCredentialType::EOS_LCT_Developer:
		{
			authCredentials.Type = EOS_ELoginCredentialType::EOS_LCT_Developer;
			authCredentials.Id = "localhost:6547";
			authCredentials.Token = "EOS";

			break;
		}
		//EOS_LCT_ExchangeCode - Token is the exchange code.
		case EOS_ELoginCredentialType::EOS_LCT_ExchangeCode:
		{
			authCredentials.Type = EOS_ELoginCredentialType::EOS_LCT_ExchangeCode;
			if (pCmdLine->authPassword.empty()) {
				SetLastError("Auth", "Invalid exchange code");
				state = EOSState::AuthFailed;
				authLoginCb(this);

				return;
			}

			authCredentials.Token = pCmdLine->authPassword.c_str();

			break;
		}
		//EOS_LCT_PersistentAuth - If targeting console platforms, Token is the long lived refresh token.Otherwise N / A.
		case EOS_ELoginCredentialType::EOS_LCT_PersistentAuth:
		{
			authCredentials.Type = EOS_ELoginCredentialType::EOS_LCT_PersistentAuth;

			break;
		}
		//EOS_LCT_AccountPortal - SystemAuthCredentialsOptions may be required if targeting mobile platforms.Otherwise N / A.
		case EOS_ELoginCredentialType::EOS_LCT_AccountPortal:
		{
			authCredentials.Type = EOS_ELoginCredentialType::EOS_LCT_AccountPortal;
			EOS_Platform_GetDesktopCrossplayStatusOptions desktopCrossplayStatusOptions{};
			desktopCrossplayStatusOptions.ApiVersion = EOS_PLATFORM_GETDESKTOPCROSSPLAYSTATUS_API_LATEST;

			EOS_Platform_GetDesktopCrossplayStatusInfo desktopCrossplayStatusInfo{};
			if (!EOSOK(EOS_Platform_GetDesktopCrossplayStatus(platformHandle, &desktopCrossplayStatusOptions, &desktopCrossplayStatusInfo))) {
				SetLastError("Auth", "Crossplay not installed");
				state = EOSState::AuthFailed;
				authLoginCb(this);

				return;
			}

			if (desktopCrossplayStatusInfo.Status != EOS_EDesktopCrossplayStatus::EOS_DCS_OK) {
				if (!runtimeOpt.bRequireBootstrap
					&& desktopCrossplayStatusInfo.Status == EOS_EDesktopCrossplayStatus::EOS_DCS_ApplicationNotBootstrapped) {
					// continue
				}
				else {
					SetLastError("Auth", "Application not bootstrapped");
					state = EOSState::AuthFailed;
					authLoginCb(this);

					return;
				}
			}

			break;
		}
		default:
			SetLastError("Auth", "Invalid type");
			state = EOSState::AuthFailed;
			authLoginCb(this);

			return;
		}

		const auto authHandle = EOS_Platform_GetAuthInterface(platformHandle);

		EOS_Auth_LoginOptions loginOptions{};
		loginOptions.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
		loginOptions.ScopeFlags = runtimeOpt.authPremissions;
		loginOptions.Credentials = &authCredentials;

		callbackCounter.CallCallback();
		EOS_Auth_Login(authHandle, &loginOptions, this,
			[] (const EOS_Auth_LoginCallbackInfo* Data) {
				const auto pEU = static_cast<decltype(this)>(Data->ClientData);
				CallbackCounterHelper callbackCounterHelper(pEU->callbackCounter);

				if (!EOSOK(Data->ResultCode)) {
					pEU->SetLastError("Auth", Data->ResultCode);

					// https://dev.epicgames.com/docs/en-US/epic-account-services/auth/auth-interface#persisting-user-login-to-epic-account-outside-epic-games-launcher
					if(pEU->runtimeOpt.authCredentialsType == EOS_ELoginCredentialType::EOS_LCT_PersistentAuth) {
						// Applications are expected to attempt automatic login using the EOS_LCT_PersistentAuth login method
						// and fall back to EOS_LCT_AccountPortal to prompt users for manual login.
						auto persistentAuthFallback = [=]() {
							// save
							auto typeSave = pEU->runtimeOpt.authCredentialsType;
							auto cbSave = pEU->authLoginCb;

							// re-login
							pEU->state = EOSState::InitSuccess;
							pEU->runtimeOpt.authCredentialsType = EOS_ELoginCredentialType::EOS_LCT_AccountPortal;

							pEU->AuthLogin([typeSave, cbSave] (EOSUtilities* pEU) {
								pEU->runtimeOpt.authCredentialsType = typeSave;
								cbSave(pEU);
							});
						};

						// Delete saved persistent auth token if token has expired or auth is invalid
						// Don't delete for other errors (e.g. EOS_EResult::EOS_NoConnection), the auth token may still be valid in these cases
						if (Data->ResultCode == EOS_EResult::EOS_Auth_Expired ||
							Data->ResultCode == EOS_EResult::EOS_InvalidAuth) {
							pEU->SetLastError("Auth", "Persistent auth token has expired or auth is invalid");
							pEU->DeletePersistentAuth([=](EOSUtilities*) {
								persistentAuthFallback();
							});

							return;
						}

						persistentAuthFallback();
						return;
					}

					pEU->state = EOSState::AuthFailed;
					pEU->authLoginCb(pEU);

					return;
				}

				pEU->accountId = Data->LocalUserId;

				const auto userHandle = EOS_Platform_GetUserInfoInterface(pEU->platformHandle);

				EOS_UserInfo_CopyUserInfoOptions userInfoOpt{};
				userInfoOpt.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
				userInfoOpt.LocalUserId = pEU->accountId;
				userInfoOpt.TargetUserId = pEU->accountId;

				if (!EOSOK(EOS_UserInfo_CopyUserInfo(userHandle, &userInfoOpt, &pEU->pUserInfo))) {
					pEU->SetLastError("Auth", "Failed to copy user info");
					pEU->state = EOSState::AuthFailed;
					pEU->authLoginCb(pEU);

					return;
				}

				const auto authHandle = EOS_Platform_GetAuthInterface(pEU->platformHandle);

				EOS_Auth_CopyUserAuthTokenOptions copyAuthTokenOptions{};
				copyAuthTokenOptions.ApiVersion = EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST;

				if (!EOSOK(EOS_Auth_CopyUserAuthToken(authHandle, &copyAuthTokenOptions, pEU->accountId, &pEU->pAuthToken))) {
					pEU->SetLastError("Auth", "Failed to copy user auth token");
					pEU->state = EOSState::AuthFailed;
					pEU->authLoginCb(pEU);

					return;
				}

				EOS_Auth_CopyIdTokenOptions copyIDTokenOptions{};
				copyIDTokenOptions.ApiVersion = EOS_AUTH_COPYIDTOKEN_API_LATEST;
				copyIDTokenOptions.AccountId = pEU->accountId;

				if (!EOSOK(EOS_Auth_CopyIdToken(authHandle, &copyIDTokenOptions, &pEU->pAuthIDToken))) {
					pEU->SetLastError("Auth", "Failed to copy ID token");
					pEU->state = EOSState::AuthFailed;
					pEU->authLoginCb(pEU);

					return;
				}

				pEU->state = EOSState::AuthSuccess;
				pEU->authLoginCb(pEU);
		});
	}

	inline void AuthLogout(const CallbackType& cb = defaultCb) {
		if (state < EOSState::AuthSuccess) {
			return;
		}

		authLogoutCb = cb;

		const auto authHandle = EOS_Platform_GetAuthInterface(platformHandle);

		EOS_Auth_LogoutOptions logoutOptions{};
		logoutOptions.ApiVersion = EOS_AUTH_LOGOUT_API_LATEST;
		logoutOptions.LocalUserId = accountId;

		callbackCounter.CallCallback();
		EOS_Auth_Logout(authHandle,&logoutOptions,this,[](const EOS_Auth_LogoutCallbackInfo* Data) {
			const auto pEU = static_cast<decltype(this)>(Data->ClientData);
			CallbackCounterHelper callbackCounterHelper(pEU->callbackCounter);

			if (!EOSOK(Data->ResultCode)) {
				pEU->SetLastError("Auth", Data->ResultCode);
				pEU->authLogoutCb(pEU);

				return;
			}

			if(pEU->runtimeOpt.authCredentialsType == EOS_ELoginCredentialType::EOS_LCT_PersistentAuth) {
				pEU->DeletePersistentAuth();
			}

			pEU->state = EOSState::InitSuccess;
			pEU->authLogoutCb(pEU);
		});
	}

	inline void Connect(const CallbackType& cb = defaultCb) {
		if (!(state == EOSState::AuthSuccess || state == EOSState::ConnectFailed)) {
			return;
		}

		state = EOSState::TryConnect;
		connectCb = cb;

		const auto connectHandle = EOS_Platform_GetConnectInterface(platformHandle);

		EOS_Connect_Credentials connectCredentials{};
		connectCredentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
		connectCredentials.Token = pAuthToken->AccessToken;
		connectCredentials.Type = EOS_EExternalCredentialType::EOS_ECT_EPIC;

		EOS_Connect_LoginOptions connectOptions{};
		connectOptions.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
		connectOptions.Credentials = &connectCredentials;
		connectOptions.UserLoginInfo = nullptr;

		callbackCounter.CallCallback();
		EOS_Connect_Login(connectHandle, &connectOptions, this, [] (const EOS_Connect_LoginCallbackInfo* Data) {
			const auto pEU = static_cast<decltype(this)>(Data->ClientData);
			CallbackCounterHelper callbackCounterHelper(pEU->callbackCounter);

			if (EOSOK(Data->ResultCode)) {
				pEU->productUserId = Data->LocalUserId;
				pEU->state = EOSState::ConnectSuccess;
				pEU->connectCb(pEU);

				pEU->RemoveNotifyAuthExpiration();
				pEU->AddNotifyAuthExpiration();

				return;
			}
			if (Data->ResultCode == EOS_EResult::EOS_InvalidUser) {
				const auto connectHandle = EOS_Platform_GetConnectInterface(pEU->platformHandle);

				EOS_Connect_CreateUserOptions createUserOptions = {};
				createUserOptions.ApiVersion = EOS_CONNECT_CREATEUSER_API_LATEST;

				if (Data->ContinuanceToken != nullptr) {
					createUserOptions.ContinuanceToken = Data->ContinuanceToken;
				}

				// NOTE: We're not deleting the received context because we're passing it down to another SDK call
				pEU->callbackCounter.CallCallback();
				EOS_Connect_CreateUser(connectHandle, &createUserOptions, pEU, 
					[](const EOS_Connect_CreateUserCallbackInfo* Data) {
						const auto pEU = static_cast<decltype(this)>(Data->ClientData);
						CallbackCounterHelper callbackCounterHelper(pEU->callbackCounter);

						if (!EOSOK(Data->ResultCode)) {
							pEU->SetLastError("Auth", "Failed to copy ID token", Data->ResultCode);
							pEU->state = EOSState::ConnectFailed;
							pEU->connectCb(pEU);

							return;
						}

						pEU->productUserId = Data->LocalUserId;
						pEU->state = EOSState::ConnectSuccess;
						pEU->connectCb(pEU);
					});

				return;
			}

			// failed
			pEU->SetLastError("Connect", Data->ResultCode);
			pEU->state = EOSState::ConnectFailed;
			pEU->connectCb(pEU);
		});
	}

	inline void RemoveNotifyAuthExpiration() {
		const auto connectHandle = EOS_Platform_GetConnectInterface(platformHandle);
		EOS_Connect_RemoveNotifyAuthExpiration(connectHandle, notificationId);
		notificationId = 0;
	}

	inline void AddNotifyAuthExpiration(const CallbackType& cb = defaultCb) {
		authExpirationCb = cb;

		EOS_Connect_AddNotifyAuthExpirationOptions opt{};
		opt.ApiVersion = EOS_CONNECT_ADDNOTIFYAUTHEXPIRATION_API_LATEST;

		const auto connectHandle = EOS_Platform_GetConnectInterface(platformHandle);
		notificationId = EOS_Connect_AddNotifyAuthExpiration(connectHandle, &opt, this, [] (const EOS_Connect_AuthExpirationCallbackInfo* Data) {
			const auto pEU = static_cast<decltype(this)>(Data->ClientData);

			// reconnect
			pEU->state = EOSState::AuthSuccess;
			pEU->Connect();
		});
	}

	inline bool PlatformOK() const {
		return state == EOSState::ConnectSuccess;
	}

	inline std::string GetAccountID() {
		if (!PlatformOK()) {
			return InvalidID;
		}

		int32_t size = EOS_EPICACCOUNTID_MAX_LENGTH + 1;
		std::string accountIDStr(size, 0);
		if (EOSOK(EOS_EpicAccountId_ToString(accountId, accountIDStr.data(), &size))) {
			return accountIDStr;
		}

		SetLastError("GetAccountID", "Failed to get account ID");
		return InvalidID;
	}

	inline std::string GetProductUserID() {
		if (!PlatformOK()) {
			return InvalidID;
		}

		int32_t size = EOS_PRODUCTUSERID_MAX_LENGTH + 1;
		std::string productUserIDStr(size, 0);
		if (EOSOK(EOS_ProductUserId_ToString(productUserId, productUserIDStr.data(), &size))) {
			return productUserIDStr;
		}

		SetLastError("GetAccountID", "Failed to get product user ID");
		return InvalidID;
	}
};