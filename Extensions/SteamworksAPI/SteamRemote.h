#pragma once

#include <string>
#include "GeneralDefinition.h"

#include "SteamInclude.h"

class SteamRemote {
public:
	static std::wstring DeviceFactorToName(const ESteamDeviceFormFactor factor) {
		switch (factor) {
		case k_ESteamDeviceFormFactorPhone:
			return L"Phone";
		case k_ESteamDeviceFormFactorTablet:
			return L"Tablet";
		case k_ESteamDeviceFormFactorComputer:
			return L"Computer";
		case k_ESteamDeviceFormFactorTV:
			return L"TV";
		case k_ESteamDeviceFormFactorUnknown:
			return L"Unknown";
		}

		return L"Unknown";
	}

	static ESteamDeviceFormFactor DeviceNameToFactor(const LPCWSTR pPlatform) {
		if (StrIEqu(pPlatform, L"Phone")) {
			return k_ESteamDeviceFormFactorPhone;
		}

		if (StrIEqu(pPlatform, L"Tablet")) {
			return k_ESteamDeviceFormFactorTablet;
		}

		if (StrIEqu(pPlatform, L"Computer")) {
			return k_ESteamDeviceFormFactorComputer;
		}

		if (StrIEqu(pPlatform, L"TV")) {
			return k_ESteamDeviceFormFactorTV;
		}

		if (StrIEqu(pPlatform, L"Unknown")) {
			return k_ESteamDeviceFormFactorUnknown;
		}

		return k_ESteamDeviceFormFactorUnknown;
	}

	static inline ESteamDeviceFormFactor GetSessionClientFormFactor(RemotePlaySessionID_t unSessionID) {
		return SteamRemotePlay()->GetSessionClientFormFactor(unSessionID);
	}

	static inline CSteamID GetSessionSteamID(RemotePlaySessionID_t unSessionID) {
		return SteamRemotePlay()->GetSessionSteamID(unSessionID);
	}

	static inline void IterateRemoteSessions(const std::function<void(RemotePlaySessionID_t)>& callBack) {
		const uint32 unSessionCount = SteamRemotePlay()->GetSessionCount();
		for (uint32 iIndex = 0; iIndex < unSessionCount; iIndex++) {
			const RemotePlaySessionID_t unSessionID = SteamRemotePlay()->GetSessionID(static_cast<int>(iIndex));
			if (!unSessionID) {
				continue;
			}

			callBack(unSessionID);
		}
	}
};