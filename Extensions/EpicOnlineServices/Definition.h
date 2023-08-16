#pragma once

#include <functional>

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

inline auto GetAuthPremissions(LPEDATA rdPtr);

struct GlobalData {
	LPRDATA rdPtr = nullptr;

	EOSUtilities* pEOSUtilities = nullptr;
	EOSAchievement* pEOSAchievement = nullptr;
	EOSStat* pEOSStat = nullptr;
	EOSPresence* pEOSPresence = nullptr;

	std::string productName;
	std::string productVersion;

	std::string productId;
	std::string sandboxId;
	std::string deploymentId;

	std::string clientId;
	std::string clientSecret;
	
	GlobalData() = default;
	~GlobalData();

	inline void SetRundata(LPRDATA rdPtr) {
		this->rdPtr = rdPtr;
	}

	inline void EOSUpdate() const {
		//EOSUpdatePlatform();
		pEOSUtilities->Update();
	}

	inline bool EOSInit(LPEDATA edPtr);

	inline void EOSInitPlatform() {
		pEOSAchievement = new EOSAchievement(pEOSUtilities);
		pEOSStat = new EOSStat(pEOSUtilities);
		pEOSPresence = new EOSPresence(pEOSUtilities);
	}

	inline void EOSReleasePlatform() const {
		delete pEOSAchievement;
		delete pEOSStat;
		delete pEOSPresence;
	}
	
	inline void EOSUpdatePlatform() const {
		pEOSAchievement->PlatformUpdate();
		pEOSStat->PlatformUpdate();
		pEOSPresence->PlatformUpdate();
	}

	// init platform here
	inline void EOSLoginSuccess() const {
		pEOSAchievement->PlatformInit();
		pEOSStat->PlatformInit();
		pEOSPresence->PlatformInit();
	}

	inline void EOSLogin(const std::function<void(bool)>& callback) const {
		if (!pEOSUtilities && pEOSUtilities->State() != EOSState::InitSuccess) {
			callback(false);

			return;
		}

		// from different thread, must capture by value
		pEOSUtilities->AuthLogin([this, callback](EOSUtilities* pEU) {
			const auto state = pEU->State();

			if(state == EOSState::AuthFailed) {
				callback(false);
			}
			if (state == EOSState::AuthSuccess) {
				// from different thread, must capture by value
				pEU->Connect([this, callback] (const EOSUtilities* pEU) {
					const auto state = pEU->State();

					if (state == EOSState::ConnectFailed) {
						callback(false);
					}

					if (state == EOSState::ConnectSuccess) {
						EOSLoginSuccess();
						callback(true);
					}
				});
			}
		});		
	}

	inline void EOSLogout(const std::function<void(bool)>& callback) const {
		if (!pEOSUtilities && pEOSUtilities->State() < EOSState::AuthSuccess) {
			callback(false);

			return;
		}

		// from different thread, must capture by value
		pEOSUtilities->AuthLogout([this, callback] (const EOSUtilities* pEU) {
			callback(pEU->State() == EOSState::InitSuccess);
		});
	}
};