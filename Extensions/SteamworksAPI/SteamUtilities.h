#pragma once

#include <any>
#include <string>
#include <functional>

#include "SteamInclude.h"

#include "SteamAchAndStat.h"
#ifndef NOIMPL_MICROTXN
#include "SteamMicroTxn.h"
#endif
#include "SteamHttp.h"

#include "SteamRemote.h"
#include "SteamRichPresence.h"

#ifdef WIN32
#include "WindowsException.h"
#endif

class SteamUtilities {
private:
	//------------
	// Info
	//------------

	std::string cmdLine;

	inline void InitSteamCommandLine() {
		static char commandLine[512];
		SteamApps()->GetLaunchCommandLine(commandLine, 512);
		cmdLine = commandLine;
	}

public:
	template <STR Name>
	inline auto GetSteamCommandLine() {
		if constexpr (WSTR<Name>) {
			return ConvertStrToWStr(cmdLine);
		}
		else {
			return cmdLine;
		}
	}

private:
	//------------
	// Callback
	//------------

	//std::vector<SteamCallbackClass*> pCallbackClasses;
	SteamAchAndStat* pAchAndStat = nullptr;
	SteamRichPresence* pSteamRichPresence = nullptr;
#ifndef NOIMPL_MICROTXN
	SteamMicroTxn* pSteamMicroTxn = nullptr;
#endif
	
	//------------
	// None Callback
	//------------

	SteamRemote* pSteamRemote = nullptr;

	//------------
	// Tasks
	//------------

	Refresh::RefreshTasks refreshTasks;

public:
	const CSteamID playerID;
	const uint64 appID;
	const int buildID;

	SteamUtilities() :playerID(SteamUser()->GetSteamID()),
		appID(SteamUtils()->GetAppID()),
		buildID(SteamApps()->GetAppBuildId()) {		
		InitSteamCommandLine();

		pAchAndStat = new SteamAchAndStat(&refreshTasks);
		pSteamRichPresence = new SteamRichPresence();
#ifndef NOIMPL_MICROTXN
		pSteamMicroTxn = new SteamMicroTxn();
#endif

		pSteamRemote = new SteamRemote();
	}
	~SteamUtilities() {
		delete pAchAndStat;
		delete pSteamRichPresence;
#ifndef NOIMPL_MICROTXN
		delete pSteamMicroTxn;
#endif
		
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

			if (!WindowsException::ExceptionUnknown(code)) {
				const auto msg = WindowsException::GetExceptionMessage(code);
				const auto comment = std::format("{:#x}\n{}", code, ConvertWStrToStr(msg));

				//MSGBOX(ConvertStrToWStr(comment));

				SteamAPI_SetMiniDumpComment(comment.c_str());
				SteamAPI_WriteMiniDump(code, pExceptionInfo, SteamApps()->GetAppBuildId());
			}

			return EXCEPTION_CONTINUE_SEARCH;
		});
#endif
	}

	//------------
	// Impl Class
	//------------

	inline SteamAchAndStat* GetAchAndStat() const { return pAchAndStat; }
	inline SteamRichPresence* GetSteamRichPresence() const { return pSteamRichPresence; }

	inline SteamRemote* GetRemote() const { return pSteamRemote; }
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