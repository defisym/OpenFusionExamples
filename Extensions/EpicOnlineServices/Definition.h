#pragma once

#include <functional>

//immediate conditon ID
constexpr auto ON_LoginComplete = 0;

// size of edit data
constexpr auto EOS_IDSZ = 36;

enum class AuthTypeComboListEnum {
	ExchangeCode,
	PersistentAuth,
	AccountPortal,
};

inline auto AuthTypeComboListEnumToLoginCredentialType(AuthTypeComboListEnum combo) {
	switch (combo) {
		case AuthTypeComboListEnum::ExchangeCode:
			return EOS_ELoginCredentialType::EOS_LCT_ExchangeCode;
		case AuthTypeComboListEnum::PersistentAuth:
			return EOS_ELoginCredentialType::EOS_LCT_PersistentAuth;
		case  AuthTypeComboListEnum::AccountPortal:
			return EOS_ELoginCredentialType::EOS_LCT_AccountPortal;
	}

	return EOS_ELoginCredentialType::EOS_LCT_ExchangeCode;
}

class EOSUtilities;

struct tagEDATA_V1;
typedef tagEDATA_V1 EDITDATA;
typedef EDITDATA* LPEDATA;

struct tagRDATA;
typedef tagRDATA RUNDATA;
typedef RUNDATA* LPRDATA;

struct GlobalData {
	LPRDATA rdPtr = nullptr;

	EOSUtilities* pEOSUtilities = nullptr;

	std::string productName;
	std::string productVersion;

	std::string productId;
	std::string sandboxId;
	std::string deploymentId;

	std::string clientId;
	std::string clientSecret;

	GlobalData() = default;
	~GlobalData() {
		delete pEOSUtilities;
	}

	inline bool EOSInit(LPEDATA edPtr);

	inline void EOSLogin(const std::function<void(bool)>& callback) const {
		callback(false);

		if (!pEOSUtilities && pEOSUtilities->State() != EOSState::Init) {
			callback(false);

			return;
		}

		pEOSUtilities->Auth([&callback](EOSUtilities* pEU) {
			const auto state = pEU->State();

			if(state == EOSState::AuthFailed) {
				callback(false);
			}
			if (state == EOSState::Auth) {
				pEU->Connect([&callback] (EOSUtilities* pEU) {
					const auto state = pEU->State();

					if (state == EOSState::ConnectFailed) {
						callback(false);
					}

					if (state == EOSState::Connect) {
						callback(true);
					}
				});
			}
		});		
	}
};