#pragma once

#include <string>
#include <functional>

#include "va_args_iterators/pp_iter.h"

#include "SteamInclude.h"

#ifdef WIN32
#include "WindowsException.h"
#endif

//------------
// Class
//------------
#include "SteamAchAndStat.h"
#include "SteamMicroTxn.h"
#include "SteamRichPresence.h"
#include "SteamScreenshot.h"
#include "SteamGamepadTextInput.h"
#include "SteamDLC.h"
#include "SteamInv.h"
#include "SteamFriend.h"
#include "SteamGameRecord.h"

#include "SteamRemote.h"

//------------
// Callback
//------------
#define REFRESH_CLASS SteamAchAndStat
#define CALLBACK_CLASS SteamMicroTxn, SteamRichPresence, SteamScreenshot, \
	SteamGamepadTextInput, SteamDLC, SteamInv, SteamFriend, SteamGameRecord

//------------
// None Callback
//------------
#define GENERAL_CLASS SteamRemote

//------------
// GENERATE
//------------
#define DECLEAR_CLASS(x) x* p##x = nullptr;
#define DELETE_CLASS(x) delete p##x;
#define CREATE_REFRESH_CLASS(x) p##x = new x(&refreshTasks);
#define CREATE_NONREFRESH_CLASS(x) p##x = new x();
#define GET_CLASS(x) inline x* Get##x() const { return p##x; }

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
	template <CStyleStrConcept Name>
	inline auto GetSteamCommandLine() {
		if constexpr (CStyleWideStrConcept<Name>) {
			return ConvertStrToWStr(cmdLine);
		}
		else {
			return cmdLine;
		}
	}

private:
	SteamRefreshClass::RefreshTasks refreshTasks;
	PP_EACH(DECLEAR_CLASS, REFRESH_CLASS, CALLBACK_CLASS, GENERAL_CLASS);

public:
	CSteamID playerID;
	AppId_t appID;
	int buildID;

	SteamUtilities() :
		playerID(SteamUser()->GetSteamID()),
		appID(SteamUtils()->GetAppID()),
		buildID(SteamApps()->GetAppBuildId()) {
		InitSteamCommandLine();

		PP_EACH(CREATE_REFRESH_CLASS, REFRESH_CLASS);
		PP_EACH(CREATE_NONREFRESH_CLASS, CALLBACK_CLASS, GENERAL_CLASS);
	}
	~SteamUtilities() {
		PP_EACH(DELETE_CLASS, REFRESH_CLASS, CALLBACK_CLASS, GENERAL_CLASS);
	}

	//------------
	// Error
	//------------

	static inline void SetErrorHandler() {
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

	PP_EACH(GET_CLASS, REFRESH_CLASS, CALLBACK_CLASS, GENERAL_CLASS);

	//------------
	// Refresh
	//------------

	inline void Refresh() {
		// handle tasks
		SteamRefreshClass::Refresh(&refreshTasks);

		// run callback
		SteamAPI_RunCallbacks();		
	}
};