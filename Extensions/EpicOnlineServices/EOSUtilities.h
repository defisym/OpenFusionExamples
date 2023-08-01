#pragma once

#include "EOSInclude.h"

#include "EOSCommandLine.h"

// Doc
// https://dev.epicgames.com/docs/zh-Hans/epic-account-services/auth/auth-interface
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

struct EOSUtilities_RuntimeOptions {
	EOS_ELoginCredentialType authCredentialsType = EOS_ELoginCredentialType::EOS_LCT_ExchangeCode;

	bool bRequireLauncher = true;
	bool bRequireBootstrap = true;
};

class EOSUtilities {
private:
	friend class PlatformBase;
	friend class EOSAchievement;
	friend class EOSStat;

	using CallbackType = std::function<void(EOSUtilities*)>;
	inline const static CallbackType defaultCb = [] (EOSUtilities*) {};

	bool bInit = false;
	EOSState state = EOSState::Invalid;

	EOSUtilities_RuntimeOptions runtimeOpt;
	const EOSCommandLine cmdLine;

	EOS_HPlatform platformHandle = nullptr;

	EOS_EpicAccountId accountId = nullptr;
	EOS_ProductUserId productUserId = nullptr;

	EOS_UserInfo* pUserInfo = nullptr;
	EOS_Auth_Token* pAuthToken = nullptr;
	EOS_Auth_IdToken* pAuthIDToken = nullptr;

	CallbackType authCb = nullptr;
	CallbackType connectCb = nullptr;

public:
	EOSUtilities(const EOSUtilities_RuntimeOptions& runtimeOpt,
		const EOS_InitializeOptions& initOpt, const EOS_Platform_Options& platOpt) {
		if (bInit) {
			return;
		}

		state = EOSState::TryInit;

		// runtime
		this->runtimeOpt = runtimeOpt;

		// sdk
		const auto result = EOS_Initialize(&initOpt);
		bInit = EOSOK(result);

		if (!bInit) {
			state = EOSState::InitFailed;

			return;
		}

		// platform
		platformHandle = EOS_Platform_Create(&platOpt);

		if (platformHandle == nullptr) {
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

	inline auto State() const { return state; }
	
	inline void Auth(const CallbackType& cb = defaultCb) {
		if (state != EOSState::InitSuccess) {
			return;
		}

		if (state == EOSState::TryAuth || state == EOSState::AuthFailed) {
			return;
		}

		state = EOSState::TryAuth;
		authCb = cb;

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
			if (!cmdLine.bValid) {
				state = EOSState::AuthFailed;
				authCb(this);

				return;
			}

			authCredentials.Token = cmdLine.authType.c_str();

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
				state = EOSState::AuthFailed;
				authCb(this);

				return;
			}

			if (desktopCrossplayStatusInfo.Status != EOS_EDesktopCrossplayStatus::EOS_DCS_OK) {
				if (!runtimeOpt.bRequireBootstrap
					&& desktopCrossplayStatusInfo.Status == EOS_EDesktopCrossplayStatus::EOS_DCS_ApplicationNotBootstrapped) {
					// continue
				}
				else {
					state = EOSState::AuthFailed;
					authCb(this);

					return;
				}
			}

			break;
		}
		default:
			state = EOSState::AuthFailed;
			authCb(this);

			return;
		}

		EOS_Auth_LoginOptions LoginOptions{};
		LoginOptions.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
		LoginOptions.ScopeFlags = EOS_EAuthScopeFlags::EOS_AS_BasicProfile;
		LoginOptions.Credentials = &authCredentials;

		const auto authHandle = EOS_Platform_GetAuthInterface(platformHandle);
		EOS_Auth_Login(authHandle, &LoginOptions, this,
			[] (const EOS_Auth_LoginCallbackInfo* Data) {
				const auto pEU = static_cast<decltype(this)>(Data->ClientData);

				if (!EOSOK(Data->ResultCode)) {
					pEU->state = EOSState::AuthFailed;
					pEU->authCb(pEU);

					return;
				}

				pEU->accountId = Data->LocalUserId;

				const auto userHandle = EOS_Platform_GetUserInfoInterface(pEU->platformHandle);

				EOS_UserInfo_CopyUserInfoOptions userInfoOpt{};
				userInfoOpt.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
				userInfoOpt.LocalUserId = pEU->accountId;
				userInfoOpt.TargetUserId = pEU->accountId;

				if (!EOSOK(EOS_UserInfo_CopyUserInfo(userHandle, &userInfoOpt, &pEU->pUserInfo))) {
					pEU->state = EOSState::AuthFailed;
					pEU->authCb(pEU);

					return;
				};

				const auto authHandle = EOS_Platform_GetAuthInterface(pEU->platformHandle);

				EOS_Auth_CopyUserAuthTokenOptions copyAuthTokenOptions{};
				copyAuthTokenOptions.ApiVersion = EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST;

				if (!EOSOK(EOS_Auth_CopyUserAuthToken(authHandle, &copyAuthTokenOptions, pEU->accountId, &pEU->pAuthToken))) {
					pEU->state = EOSState::AuthFailed;
					pEU->authCb(pEU);

					return;
				}

				EOS_Auth_CopyIdTokenOptions copyIDTokenOptions{};
				copyIDTokenOptions.ApiVersion = EOS_AUTH_COPYIDTOKEN_API_LATEST;
				copyIDTokenOptions.AccountId = pEU->accountId;

				if (!EOSOK(EOS_Auth_CopyIdToken(authHandle, &copyIDTokenOptions, &pEU->pAuthIDToken))) {
					pEU->state = EOSState::AuthFailed;
					pEU->authCb(pEU);

					return;
				}

				pEU->state = EOSState::AuthSuccess;
				pEU->authCb(pEU);
		});
	}

	inline void Connect(const CallbackType& cb = defaultCb) {
		if (state != EOSState::AuthSuccess) {
			return;
		}


		if (state == EOSState::TryConnect || state == EOSState::ConnectFailed) {
			return;
		}

		state = EOSState::TryConnect;
		connectCb = cb;
		
		EOS_Connect_Credentials connectCredentials{};
		connectCredentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
		connectCredentials.Token = pAuthToken->AccessToken;
		connectCredentials.Type = EOS_EExternalCredentialType::EOS_ECT_EPIC;

		//EOS_Connect_UserLoginInfo userLoginInfo{};
		//userLoginInfo.ApiVersion = EOS_CONNECT_USERLOGININFO_API_LATEST;
		//userLoginInfo.DisplayName = pUserInfo->DisplayName;

		EOS_Connect_LoginOptions connectOptions{};
		connectOptions.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
		connectOptions.Credentials = &connectCredentials;
		connectOptions.UserLoginInfo = nullptr;

		const auto connectHandle = EOS_Platform_GetConnectInterface(platformHandle);
		EOS_Connect_Login(connectHandle, &connectOptions, this, [] (const EOS_Connect_LoginCallbackInfo* Data) {
			const auto pEU = static_cast<decltype(this)>(Data->ClientData);

			if (EOSOK(Data->ResultCode)) {
				pEU->productUserId = Data->LocalUserId;
				pEU->state = EOSState::ConnectSuccess;
				pEU->connectCb(pEU);

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
				EOS_Connect_CreateUser(connectHandle, &createUserOptions, pEU, 
					[](const EOS_Connect_CreateUserCallbackInfo* Data) {
						const auto pEU = static_cast<decltype(this)>(Data->ClientData);

						if (!EOSOK(Data->ResultCode)) {
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
			pEU->state = EOSState::ConnectFailed;
			pEU->connectCb(pEU);
		});
	}
};