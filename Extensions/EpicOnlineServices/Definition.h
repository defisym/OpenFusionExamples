#pragma once

#include <functional>

#include "EOSCommandLine.h"

//immediate conditon ID
constexpr auto ON_LoginComplete = 0;
constexpr auto ON_LogoutComplete = 4;

constexpr auto ON_Error = 3;

// size of edit data
constexpr auto EOS_IDSZ = 36;

struct tagEDATA_V1;
typedef tagEDATA_V1 EDITDATA;
typedef EDITDATA* LPEDATA;

struct tagRDATA;
typedef tagRDATA RUNDATA;
typedef RUNDATA* LPRDATA;

enum class SandboxComboListEnum {
	Dev,
	Stage,
	Live,
};

enum class AuthTypeComboListEnum {
	Developer,
	ExchangeCode,
	PersistentAuth,
	AccountPortal,
};

inline auto AuthTypeComboListEnumToLoginCredentialType(AuthTypeComboListEnum combo) {
	switch (combo) {
	case AuthTypeComboListEnum::Developer:
		return EOS_ELoginCredentialType::EOS_LCT_Developer;
	case AuthTypeComboListEnum::ExchangeCode:
		return EOS_ELoginCredentialType::EOS_LCT_ExchangeCode;
	case AuthTypeComboListEnum::PersistentAuth:
		return EOS_ELoginCredentialType::EOS_LCT_PersistentAuth;
	case  AuthTypeComboListEnum::AccountPortal:
		return EOS_ELoginCredentialType::EOS_LCT_AccountPortal;
	}

	return EOS_ELoginCredentialType::EOS_LCT_ExchangeCode;
}

struct LogOpt {
	bool bAutoLogin = false;
	bool bAutoLogout = false;

	bool bLoginCalled = false;
	bool bUserLogin = false;
};

struct GlobalData {
	LPRDATA rdPtr = nullptr;
	bool bEnable = false;

	LogOpt logOpt;

	EOSUtilities* pEOSUtilities = nullptr;
	EOSAchievement* pEOSAchievement = nullptr;
	EOSStat* pEOSStat = nullptr;
	EOSPresence* pEOSPresence = nullptr;

	std::string productName;
	std::string productVersion;

	std::string productId;

	std::string clientId;
	std::string clientSecret;

	SandboxComboListEnum sandboxType = SandboxComboListEnum::Dev;

	std::string devSandboxId;
	std::string devDeploymentId;
	std::string stageSandboxId;
	std::string stageDeploymentId;
	std::string liveSandboxId;
	std::string liveDeploymentId;

	EOSCommandLine cmdLine;
	
	GlobalData() = default;
	~GlobalData();

	inline void SetRundata(LPRDATA rdPtr) {
		this->rdPtr = rdPtr;
	}

	inline void EOSUpdate() const {
		EOSUpdatePlatform();
		pEOSUtilities->Update();
	}
	
	// Sandbox can be overridden by Epic Launcher command line
	// Sandbox ID: This ID can be obtained from the Epic Games Launcher at launch time in the form of the epicsandboxid launch argument provided to all games.
	// Deployment ID: This ID can be set within your build in a custom if/switch statement block targeting the expected deployment for the sandbox in question.
	// https://dev.epicgames.com/docs/epic-games-store/testing-guide#sandboxdeployment-id-handling-epic-games-store-publishing-tools-only
	inline auto GetSandboxInfo() {
		if (!cmdLine.epicSandboxID.empty()) {
			const auto& sandboxId = cmdLine.epicSandboxID;

			if (sandboxId == devSandboxId) {
				sandboxType = SandboxComboListEnum::Dev;
				return std::make_tuple(devSandboxId, devDeploymentId);
			}

			if (sandboxId == stageSandboxId) {
				sandboxType = SandboxComboListEnum::Stage;
				return std::make_tuple(stageSandboxId, stageDeploymentId);
			}

			if (sandboxId == liveSandboxId) {
				sandboxType = SandboxComboListEnum::Live;
				return std::make_tuple(liveSandboxId, liveDeploymentId);
			}
		}

		switch (sandboxType) {
		case SandboxComboListEnum::Dev:
			return std::make_tuple(devSandboxId, devDeploymentId);
		case SandboxComboListEnum::Stage:
			return std::make_tuple(stageSandboxId, stageDeploymentId);
		case SandboxComboListEnum::Live:
			return std::make_tuple(liveSandboxId, liveDeploymentId);
		}

		// default value
		// this line shouldn't be reached, just make clang-tidy happy
		return std::make_tuple(devSandboxId, devDeploymentId);
	}

	inline bool EOSInit(LPEDATA edPtr);

	// same as platform functions, but do them together
	inline void EOSAllocPlatform() {
		pEOSAchievement = new EOSAchievement(pEOSUtilities);
		pEOSStat = new EOSStat(pEOSUtilities);
		pEOSPresence = new EOSPresence(pEOSUtilities);
	}

	inline void EOSReleasePlatform() const {
		delete pEOSAchievement;
		delete pEOSStat;
		delete pEOSPresence;
	}

	inline void EOSInitPlatform() const {
		pEOSAchievement->PlatformInit();
		pEOSStat->PlatformInit();
		pEOSPresence->PlatformInit();
	}

	inline void EOSQueryPlatform() const {
		pEOSAchievement->PlatformQuery();
		pEOSStat->PlatformQuery();
		pEOSPresence->PlatformQuery();
	}

	inline void EOSUpdatePlatform() const {
		pEOSAchievement->PlatformUpdate();
		pEOSStat->PlatformUpdate();
		pEOSPresence->PlatformUpdate();
	}

	inline bool EOSCallbackComplete()const {
		bool bFinish = true;

		auto checkCallback = [] (auto* pEOS) {
			return pEOS != nullptr
				? pEOS->AllCallbackComplete()
				: true;
			};

		bFinish = bFinish && checkCallback(pEOSAchievement);
		bFinish = bFinish && checkCallback(pEOSStat);
		bFinish = bFinish && checkCallback(pEOSPresence);
		bFinish = bFinish && checkCallback(pEOSUtilities);

		return bFinish;
	}

	inline void EOSWaitForCallbackComplete() const {
		while (!EOSCallbackComplete()) {
			//OutputDebugStringA("Wait...\n");
			EOSUpdate();
		}

		//OutputDebugStringA("Complete!\n");
	}

private:
	using LogResultCallback = std::function<void(GlobalData*, bool)>;
	inline const static LogResultCallback defaultCb = [] (GlobalData*, bool) {};

public:
	inline void EOSLogin(const LogResultCallback& callback = defaultCb) {
		if (!pEOSUtilities || !pEOSUtilities->Init()) {
			callback(this, false);

			return;
		}

		// from different thread, must capture by value
		pEOSUtilities->AuthLogin([this, callback](EOSUtilities* pEU) {
			const auto state = pEU->State();

			if(state == EOSState::AuthFailed) {
				callback(this, false);
			}
			if (state == EOSState::AuthSuccess) {
				// from different thread, must capture by value
				pEU->Connect([this, callback] (const EOSUtilities* pEU) {
					const auto state = pEU->State();

					if (state == EOSState::ConnectFailed) {
						callback(this, false);
					}

					if (state == EOSState::ConnectSuccess) {
						EOSInitPlatform();
						callback(this, true);
					}
				});
			}
		});		
	}

	inline void EOSAutoLogin(const LogResultCallback& callback = defaultCb) {
		if (logOpt.bAutoLogin && !logOpt.bLoginCalled) {
			logOpt.bLoginCalled = true;
			// login will add callback count internally
			EOSLogin(callback);
		}
	}

	inline void EOSLogout(const LogResultCallback& callback = defaultCb) {
		if (!pEOSUtilities && pEOSUtilities->State() < EOSState::AuthSuccess) {
			callback(this, false);

			return;
		}

		// from different thread, must capture by value
		pEOSUtilities->AuthLogout([this, callback] (const EOSUtilities* pEU) {
			callback(this, pEU->State() == EOSState::InitSuccess);
		});
	}

	inline void EOSAutoLogout(const LogResultCallback& callback = defaultCb) {
		if (logOpt.bAutoLogout && logOpt.bUserLogin) {
			// logout will add callback count internally
			EOSLogout(callback);
		}
	}
};