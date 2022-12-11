#pragma once

#include <string>
#include <functional>

class SteamUtilities {
private:
public:
	SteamUtilities() {

	}
	~SteamUtilities() {

	}

	std::wstring DeviceFactorToName(ESteamDeviceFormFactor factor) {
		std::wstring name = L"Unknown";

		switch (factor)	{
		case k_ESteamDeviceFormFactorPhone:
			name = L"Phone";

			break;
		case k_ESteamDeviceFormFactorTablet:
			name = L"Tablet";

			break;
		case k_ESteamDeviceFormFactorComputer:
			name = L"Computer";

			break;
		case k_ESteamDeviceFormFactorTV:
			name = L"TV";

			break;
		}

		return name;
	}

	ESteamDeviceFormFactor DeviceNameToFactor(LPCWSTR pPlatform) {
		ESteamDeviceFormFactor factorToSearch = k_ESteamDeviceFormFactorUnknown;

		do {
			if (StrIEqu(pPlatform, L"Phone")) {
				factorToSearch = k_ESteamDeviceFormFactorPhone;

				break;
			}

			if (StrIEqu(pPlatform, L"Tablet")) {
				factorToSearch = k_ESteamDeviceFormFactorTablet;

				break;
			}

			if (StrIEqu(pPlatform, L"Computer")) {
				factorToSearch = k_ESteamDeviceFormFactorComputer;

				break;
			}

			if (StrIEqu(pPlatform, L"TV")) {
				factorToSearch = k_ESteamDeviceFormFactorTV;

				break;
			}
		} while (0);

		return factorToSearch;
	}

	inline void IterateRemoteSessions(std::function<void(RemotePlaySessionID_t)> callBack) {
		uint32 unSessionCount = SteamRemotePlay()->GetSessionCount();
		for (uint32 iIndex = 0; iIndex < unSessionCount; iIndex++) {
			RemotePlaySessionID_t unSessionID = SteamRemotePlay()->GetSessionID(iIndex);
			if (!unSessionID) {
				continue;
			}

			callBack(unSessionID);
		}
	}

	inline void Refresh() {
		//SteamUserStats()->RequestCurrentStats();

		SteamAPI_RunCallbacks();		
	}
};