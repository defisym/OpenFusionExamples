#pragma once

#include "EOSInclude.h"

#include "EOSCommandLine.h"

// define this macro to debug with DevAuthTool
// disable VPN to avoid 502 error
// portal: 6547
// name: EOS

#define DEVLOGIN

enum class EOSState {
	Invalid = -1,

	TryInit,
	InitFailed,
	Init,

	TryAuth,
	AuthFailed,
	Auth,

	TryConnect,
	ConnectFailed,
	Connect,
};

struct EOSUtilities_RuntimeOptions {
	EOS_ELoginCredentialType authCredentialsType = EOS_ELoginCredentialType::EOS_LCT_ExchangeCode;

	bool bRequireLauncher = true;
	bool bRequireBootstrap = true;
};

class EOSUtilities {
private:
	friend class EOSAchievement;

	using CallbackType = std::function<void(EOSUtilities*)>;
	#define DefaultCb [](EOSUtilities*) {}

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

	bool bAchievementQuery = false;
	CallbackType achievementQueryCb = nullptr;
	CallbackType achievementUnlockCb = nullptr;

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

	inline auto State() const { return state; }
	
	inline void Auth(const CallbackType& cb = DefaultCb) {
		if (state != EOSState::Init) {
			return;
		}

		if (state == EOSState::TryAuth || state == EOSState::AuthFailed) {
			return;
		}

		state = EOSState::TryAuth;
		authCb = cb;

		EOS_Auth_Credentials authCredentials{};
		authCredentials.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;

#ifdef DEVLOGIN
		//EOS_LCT_Developer - ID is the host (e.g. localhost:6547), and Token is the credential name registered in the EOS Developer Authentication Tool.
		authCredentials.Type = EOS_ELoginCredentialType::EOS_LCT_Developer;
		authCredentials.Id = "localhost:6547";
		authCredentials.Token = "EOS";
#else
		switch (runtimeOpt.authCredentialsType) {  // NOLINT(clang-diagnostic-switch-enum)
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
#endif

		EOS_Auth_LoginOptions LoginOptions{};
		LoginOptions.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
		LoginOptions.ScopeFlags = EOS_EAuthScopeFlags::EOS_AS_BasicProfile;
		LoginOptions.Credentials = &authCredentials;

		const auto authHandle = EOS_Platform_GetAuthInterface(platformHandle);
		EOS_Auth_Login(authHandle, &LoginOptions, this,
			[] (const EOS_Auth_LoginCallbackInfo* Data) {
				const auto pEU = static_cast<EOSUtilities*>(Data->ClientData);

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

				pEU->state = EOSState::Auth;
				pEU->authCb(pEU);
		});
	}

	inline void Connect(const CallbackType& cb = DefaultCb) {
		if (state != EOSState::Auth) {
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
			const auto pEU = static_cast<EOSUtilities*>(Data->ClientData);

			if (EOSOK(Data->ResultCode)) {
				pEU->productUserId = Data->LocalUserId;
				pEU->state = EOSState::Connect;
				pEU->connectCb(pEU);
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
						const auto pEU = static_cast<EOSUtilities*>(Data->ClientData);

						if (!EOSOK(Data->ResultCode)) {
							pEU->state = EOSState::ConnectFailed;
							pEU->connectCb(pEU);

							return;
						}

						pEU->productUserId = Data->LocalUserId;
						pEU->state = EOSState::Connect;
						pEU->connectCb(pEU);
					});
			}

			pEU->state = EOSState::ConnectFailed;
			pEU->connectCb(pEU);
		});
	}

	inline void QueryAchievementDefinitions(const CallbackType& cb = DefaultCb) {
		if (state != EOSState::Connect) {
			return;
		}

		achievementQueryCb = cb;

		const auto achHandle = EOS_Platform_GetAchievementsInterface(platformHandle);

		EOS_Achievements_QueryDefinitionsOptions opt{};
		opt.ApiVersion = EOS_ACHIEVEMENTS_QUERYDEFINITIONS_API_LATEST;
		opt.LocalUserId = productUserId;

		EOS_Achievements_QueryDefinitions(achHandle, &opt, this, [] (const EOS_Achievements_OnQueryDefinitionsCompleteCallbackInfo* Data) {
			if (!EOSOK(Data->ResultCode)) {
				return;
			}

			const auto pEU = static_cast<EOSUtilities*>(Data->ClientData);
			pEU->bAchievementQuery = true;
			pEU->achievementQueryCb(pEU);
		});
	}

	//inline void QueryPlayerAchievement(const CallbackType& cb = DefaultCb) {
	//	
	//}

	inline void IterateAchievements(const std::function<void(EOS_Achievements_DefinitionV2*)>& cb) const {
		if (!bAchievementQuery) {
			return;
		}

		const auto achHandle = EOS_Platform_GetAchievementsInterface(platformHandle);

		EOS_Achievements_GetAchievementDefinitionCountOptions getOpt{};
		getOpt.ApiVersion = EOS_ACHIEVEMENTS_GETACHIEVEMENTDEFINITIONCOUNT_API_LATEST;
		auto count = EOS_Achievements_GetAchievementDefinitionCount(achHandle, &getOpt);

		for (decltype(count) i = 0; i < count; i++) {
			EOS_Achievements_CopyAchievementDefinitionV2ByIndexOptions copyOpt{};
			copyOpt.ApiVersion = EOS_ACHIEVEMENTS_COPYDEFINITIONV2BYINDEX_API_LATEST;
			copyOpt.AchievementIndex = i;

			EOS_Achievements_DefinitionV2* pDefinition = nullptr;

			if(EOSOK(EOS_Achievements_CopyAchievementDefinitionV2ByIndex(achHandle, &copyOpt, &pDefinition))) {
				cb(pDefinition);

				EOS_Achievements_DefinitionV2_Release(pDefinition);
			}
		}
	}

	inline void UnlockAchievements(const std::vector<std::string>& toUnlock, const CallbackType& cb = DefaultCb) {
		if (state != EOSState::Connect) {
			return;
		}

		achievementUnlockCb = cb;

		// generate array
		const auto sz = toUnlock.size();
		const auto pArray = new const char* [sz];

		for (auto i = 0u; i < sz; i++) {
			pArray[i] = toUnlock[i].c_str();
		}

		// unlock
		const auto achHandle = EOS_Platform_GetAchievementsInterface(platformHandle);
		EOS_Achievements_UnlockAchievementsOptions unlockAchievementsOptions{};
		unlockAchievementsOptions.ApiVersion = EOS_ACHIEVEMENTS_UNLOCKACHIEVEMENTS_API_LATEST;
		unlockAchievementsOptions.UserId = productUserId;
		unlockAchievementsOptions.AchievementIds = pArray;
		unlockAchievementsOptions.AchievementsCount = sz;

		EOS_Achievements_UnlockAchievements(achHandle,&unlockAchievementsOptions,this,
			[](const EOS_Achievements_OnUnlockAchievementsCompleteCallbackInfo* Data) {
				if (!EOSOK(Data->ResultCode)) {
					return;
				}

				const auto pEU = static_cast<EOSUtilities*>(Data->ClientData);
				pEU->achievementUnlockCb(pEU);
			});
	}
};