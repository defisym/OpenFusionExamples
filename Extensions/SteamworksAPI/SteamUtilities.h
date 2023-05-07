#pragma once

#include <any>
#include <string>
#include <functional>

#include "SteamInclude.h"
#include "SteamAchAndStat.h"

#ifdef WIN32
#include "WindowsException.h"
#endif

class SteamUtilities {
private:
	Refresh::RefreshTasks refreshTasks;

	//std::vector<SteamCallbackClass*> pCallbackClasses;
	SteamAchAndStat* pAchAndStat = nullptr;

public:
	SteamUtilities() {
		pAchAndStat = new SteamAchAndStat(&refreshTasks);
	}
	~SteamUtilities() {
		delete pAchAndStat;
	}

	//------------
	// Error
	//------------

	inline void SetErrorHandler() const {
#ifdef WIN32
		if(IsDebuggerPresent()) {
			return;
		}

		WindowsException::SetVEHFilter([](EXCEPTION_POINTERS* pExceptionInfo)->LONG {
			const auto code = pExceptionInfo->ExceptionRecord->ExceptionCode;
			const auto msg = ConvertWStrToStr(WindowsException::GetExceptionMessage(code));

			SteamAPI_SetMiniDumpComment(std::format("{:#x}\n{}", code, msg).c_str());
			SteamAPI_WriteMiniDump(code, pExceptionInfo, SteamApps()->GetAppBuildId());

			return EXCEPTION_CONTINUE_SEARCH;
		});
#endif
	}

	//------------
	// Remote Play
	//------------

	static std::wstring DeviceFactorToName(const ESteamDeviceFormFactor factor) {
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

	static ESteamDeviceFormFactor DeviceNameToFactor(const LPCWSTR pPlatform) {
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

	static inline void IterateRemoteSessions(const std::function<void(RemotePlaySessionID_t)>& callBack) {
		const uint32 unSessionCount = SteamRemotePlay()->GetSessionCount();
		for (uint32 iIndex = 0; iIndex < unSessionCount; iIndex++) {
			const RemotePlaySessionID_t unSessionID = SteamRemotePlay()->GetSessionID(iIndex);
			if (!unSessionID) {
				continue;
			}

			callBack(unSessionID);
		}
	}

	//------------
	// Achievements
	//------------

	inline void ResetAchievement(const LPCWSTR pAchName = nullptr) const {
		pAchAndStat->ResetAchievement(pAchName);
	}

	inline void UnlockAchievement(const LPCWSTR pAchName) const {
		pAchAndStat->UnlockAchievement(pAchName);
	}

	//------------
	// Stat
	//------------

	template <STAT Stat>
	inline void GetStat(const LPCWSTR pStatName, Stat* pData) {
		pAchAndStat->GetStat(pStatName, pData);
	}

	template <STAT Stat>
	inline void SetStat(const LPCWSTR pStatName, const Stat data) {
		pAchAndStat->SetStat(pStatName, data);
	}

	template <STAT Stat>
	inline void AddStat(const LPCWSTR pStatName, const Stat data) {
		pAchAndStat->AddStat(pStatName, data);
	}

	//------------
	// Refresh
	//------------

	// ToRefresh series: set flag then refresh it later
	inline void AddToRefresh(Refresh::RefreshType task) {
		Refresh::UniquePush(&refreshTasks, task);
	}

	// do refresh
	inline void Refresh() {
		// handle tasks
		Refresh::Refresh(&refreshTasks);

		// run callback
		SteamAPI_RunCallbacks();		
	}
};