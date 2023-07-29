#pragma once

#include "EOSInclude.h"

#include "eos_auth.h"

#include "eos_achievements.h"
#include "eos_userinfo.h"

class EOSUtilities {
private:
	bool bInit = false;

	EOS_HPlatform platformHandle = nullptr;

	EOS_EpicAccountId localUserId = nullptr;
	EOS_ProductUserId productUserId = nullptr;

	EOS_UserInfo* pUserInfo = nullptr;
	EOS_Auth_Token* pAuthToken = nullptr;

public:
	EOSUtilities(const EOS_InitializeOptions& initOpt, const EOS_Platform_Options& platOpt) {
		if (bInit) {
			return;
		}

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

#ifdef _DEBUG
		const auto setLogCallbackResult = EOS_Logging_SetCallback([] (const EOS_LogMessage* Message) {
			OutputDebugStringA(std::format("Category: {}, Message: {}\r\n",
			Message->Category, Message->Message).c_str());
		});
#endif

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
			[](const EOS_Auth_LoginCallbackInfo* Data) {
				if(!EOSOK(Data->ResultCode)) {
					return;
				}

				const auto pEU = static_cast<EOSUtilities*>(Data->ClientData);
				pEU->localUserId = Data->LocalUserId;

				const auto userHandle = EOS_Platform_GetUserInfoInterface(pEU->platformHandle);

				EOS_UserInfo_CopyUserInfoOptions userInfoOpt{};
				userInfoOpt.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
				userInfoOpt.LocalUserId = pEU->localUserId;
				userInfoOpt.TargetUserId = pEU->localUserId;

				if (!EOSOK(EOS_UserInfo_CopyUserInfo(userHandle, &userInfoOpt, &pEU->pUserInfo))) {
					return;
				};

				const auto authHandle = EOS_Platform_GetAuthInterface(pEU->platformHandle);

				EOS_Auth_CopyUserAuthTokenOptions copyTokenOptions{};
				copyTokenOptions.ApiVersion = EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST;

				if (!EOSOK(EOS_Auth_CopyUserAuthToken(authHandle, &copyTokenOptions, pEU->localUserId, &pEU->pAuthToken))) {
					return;
				}

				EOS_Connect_Credentials connectCredentials{};
				connectCredentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
				connectCredentials.Token = pEU->pAuthToken->AccessToken;

				EOS_Connect_LoginOptions connectOptions{};
				connectOptions.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
				connectOptions.Credentials = &connectCredentials;

				const auto connectHandle = EOS_Platform_GetConnectInterface(pEU->platformHandle);
				EOS_Connect_Login(connectHandle, &connectOptions, pEU, [] (const EOS_Connect_LoginCallbackInfo* Data) {
					if (!EOSOK(Data->ResultCode)) {
						return;
					}

					const auto pEU = static_cast<EOSUtilities*>(Data->ClientData);
					pEU->productUserId = Data->LocalUserId;
				});
		});				
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

	inline void Test() {
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