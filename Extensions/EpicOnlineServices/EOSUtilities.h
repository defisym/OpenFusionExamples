#pragma once

#include "EOSInclude.h"

#include "eos_auth.h"

#include "eos_achievements.h"
#include "eos_userinfo.h"

enum EOSState {
	Invalid = -1,
	TryInit,
	Init,
	TryAuth,
	Auth,
	TryConnect,
	Connect,
};

class EOSUtilities {
private:
	bool bInit = false;
	EOSState state = EOSState::Invalid;

	EOS_HPlatform platformHandle = nullptr;

	EOS_EpicAccountId accountId = nullptr;
	EOS_ProductUserId productUserId = nullptr;

	EOS_UserInfo* pUserInfo = nullptr;
	EOS_Auth_Token* pAuthToken = nullptr;
	EOS_Auth_IdToken* pAuthIDToken = nullptr;

public:
	EOSUtilities(const EOS_InitializeOptions& initOpt, const EOS_Platform_Options& platOpt) {
		if (bInit) {
			return;
		}

		state = EOSState::TryInit;

		// sdk
		const auto result = EOS_Initialize(&initOpt);
		bInit = EOSOK(result);

		if (!bInit) {
			return;
		}

		// platform
		platformHandle = EOS_Platform_Create(&platOpt);

		if (platformHandle == nullptr) {
			return;
		}

		state = EOSState::Init;

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

	inline bool State() const { return bInit; }

	inline void Auth() {		
		if(state != EOSState::Init) {
			return;
		}

		if (state == EOSState::TryAuth) {
			return;
		}

		state = EOSState::TryAuth;

		EOS_Auth_Credentials authCredentials{};
		authCredentials.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;

#define DEVLOGIN

#ifdef DEVLOGIN
		authCredentials.Type = EOS_ELoginCredentialType::EOS_LCT_Developer;
		authCredentials.Id = "localhost:6547";
		authCredentials.Token = "EOS";
#else
		authCredentials.Type = EOS_ELoginCredentialType::EOS_LCT_Password;
		//authCredentials.Type = EOS_ELoginCredentialType::EOS_LCT_AccountPortal;
		//authCredentials.Type = EOS_ELoginCredentialType::EOS_LCT_ExchangeCode;
		//authCredentials.Type = EOS_ELoginCredentialType::EOS_LCT_PersistentAuth;
#endif

		EOS_Auth_LoginOptions LoginOptions{};
		LoginOptions.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
		LoginOptions.ScopeFlags = EOS_EAuthScopeFlags::EOS_AS_BasicProfile;
		LoginOptions.Credentials = &authCredentials;

		const auto authHandle = EOS_Platform_GetAuthInterface(platformHandle);
		EOS_Auth_Login(authHandle, &LoginOptions, this,
			[] (const EOS_Auth_LoginCallbackInfo* Data) {
				if (!EOSOK(Data->ResultCode)) { return; }

				const auto pEU = static_cast<EOSUtilities*>(Data->ClientData);
				pEU->accountId = Data->LocalUserId;

				const auto userHandle = EOS_Platform_GetUserInfoInterface(pEU->platformHandle);

				EOS_UserInfo_CopyUserInfoOptions userInfoOpt{};
				userInfoOpt.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
				userInfoOpt.LocalUserId = pEU->accountId;
				userInfoOpt.TargetUserId = pEU->accountId;

				if (!EOSOK(EOS_UserInfo_CopyUserInfo(userHandle, &userInfoOpt, &pEU->pUserInfo))) {
					return;
				};
				
				const auto authHandle = EOS_Platform_GetAuthInterface(pEU->platformHandle);

				EOS_Auth_CopyUserAuthTokenOptions copyAuthTokenOptions{};
				copyAuthTokenOptions.ApiVersion = EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST;

				if (!EOSOK(EOS_Auth_CopyUserAuthToken(authHandle, &copyAuthTokenOptions, pEU->accountId, &pEU->pAuthToken))) {
					return;
				}

				EOS_Auth_CopyIdTokenOptions copyIDTokenOptions{};
				copyIDTokenOptions.ApiVersion = EOS_AUTH_COPYIDTOKEN_API_LATEST;
				copyIDTokenOptions.AccountId = pEU->accountId;

				if (!EOSOK(EOS_Auth_CopyIdToken(authHandle, &copyIDTokenOptions, &pEU->pAuthIDToken))) {
					return;
				}

				pEU->state = EOSState::Auth;				
		});
	}

	inline void Connect() {
		if (state != EOSState::Auth) {
			return;
		}

		if (state == EOSState::TryConnect) {
			return;
		}

		state = EOSState::TryConnect;
		
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
			const auto pEU = static_cast<EOSUtilities*>(Data->ClientData);

			if (EOSOK(Data->ResultCode)) {
				pEU->productUserId = Data->LocalUserId;
				pEU->state = EOSState::Connect;
			}
			else if (Data->ResultCode == EOS_EResult::EOS_InvalidUser) {
				const auto connectHandle = EOS_Platform_GetConnectInterface(pEU->platformHandle);

				EOS_Connect_CreateUserOptions createUserOptions = {};
				createUserOptions.ApiVersion = EOS_CONNECT_CREATEUSER_API_LATEST;

				if (Data->ContinuanceToken != nullptr) {
					createUserOptions.ContinuanceToken = Data->ContinuanceToken;
				}

				// NOTE: We're not deleting the received context because we're passing it down to another SDK call
				EOS_Connect_CreateUser(connectHandle, &createUserOptions, pEU, 
					[](const EOS_Connect_CreateUserCallbackInfo* Data) {
						if (!EOSOK(Data->ResultCode)) { return; }

						const auto pEU = static_cast<EOSUtilities*>(Data->ClientData);

						pEU->productUserId = Data->LocalUserId;
						pEU->state = EOSState::Connect;
					});
			}

		});
	}

	inline void Achievement() {
		if (state != EOSState::Connect) {
			return;
		}

		const auto achHandle = EOS_Platform_GetAchievementsInterface(platformHandle);

		EOS_Achievements_QueryDefinitionsOptions opt{};
		opt.ApiVersion = EOS_ACHIEVEMENTS_QUERYDEFINITIONS_API_LATEST;
		opt.LocalUserId = productUserId;

		EOS_Achievements_QueryDefinitions(achHandle, &opt, this, [] (const EOS_Achievements_OnQueryDefinitionsCompleteCallbackInfo* Data) {
			if (!EOSOK(Data->ResultCode)) {
				return;
			}

			const auto pEU = static_cast<EOSUtilities*>(Data->ClientData);
			const auto achHandle = EOS_Platform_GetAchievementsInterface(pEU->platformHandle);

			EOS_Achievements_GetAchievementDefinitionCountOptions opt{};
			opt.ApiVersion = EOS_ACHIEVEMENTS_GETACHIEVEMENTDEFINITIONCOUNT_API_LATEST;
			auto count = EOS_Achievements_GetAchievementDefinitionCount(achHandle, &opt);

			for (decltype(count) i = 0; i < count; i++) {
				EOS_Achievements_CopyAchievementDefinitionV2ByIndexOptions opt{};
				opt.ApiVersion = EOS_ACHIEVEMENTS_COPYDEFINITIONV2BYINDEX_API_LATEST;
				opt.AchievementIndex = i;

				EOS_Achievements_DefinitionV2* p = nullptr;

				const auto ret = EOS_Achievements_CopyAchievementDefinitionV2ByIndex(achHandle, &opt, &p);

				EOS_Achievements_DefinitionV2_Release(p);
			}
		});
	}
};