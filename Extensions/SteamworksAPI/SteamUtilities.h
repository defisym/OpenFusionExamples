#pragma once

#include <any>
#include <string>
#include <functional>

#include "SteamInclude.h"
#include "SteamAchAndStat.h"
#include "SteamRemote.h"

#ifdef WIN32
#include "WindowsException.h"
#endif

class SteamUtilities {
private:
	//std::vector<SteamCallbackClass*> pCallbackClasses;
	SteamAchAndStat* pAchAndStat = nullptr;
	SteamRemote* pSteamRemote = nullptr;

	Refresh::RefreshTasks refreshTasks;

public:
	const CSteamID playerID;
	const uint64 appID;
	const int buildID;

	SteamUtilities() :playerID(SteamUser()->GetSteamID()),
		appID(SteamUtils()->GetAppID()),
		buildID(SteamApps()->GetAppBuildId()) {
		pAchAndStat = new SteamAchAndStat(&refreshTasks);
		pSteamRemote = new SteamRemote();
	}
	~SteamUtilities() {
		delete pAchAndStat;
		delete pSteamRemote;
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

	inline SteamRemote* GetRemote() const {
		return pSteamRemote;
	}
	
	//------------
	// Achievements & Stat
	//------------

	inline SteamAchAndStat* GetAchAndStat() const {
		return pAchAndStat;
	}

	//------------
	// Refresh
	//------------

	inline void Refresh() {
		// handle tasks
		Refresh::Refresh(&refreshTasks);

		// run callback
		SteamAPI_RunCallbacks();		
	}
};