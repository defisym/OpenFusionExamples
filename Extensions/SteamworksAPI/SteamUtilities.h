#pragma once

#include <any>
#include <string>
#include <functional>

#include "SteamInclude.h"

#include "SteamAchAndStat.h"
#include "SteamMicroTxn.h"
#include "SteamRichPresence.h"
#include "SteamScreenshot.h"
#include "SteamGamepadTextInput.h"
#include "SteamDLC.h"
#include "SteamFriend.h"
#include "SteamInv.h"

#include "SteamRemote.h"

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

	SteamAchAndStat* pSteamAchAndStat = nullptr;
	SteamInv* pSteamInventory = nullptr;

	SteamMicroTxn* pSteamMicroTxn = nullptr;
	SteamRichPresence* pSteamRichPresence = nullptr;
	SteamScreenshot* pSteamScreenshot = nullptr;
	SteamGamepadTextInput* pSteamGamepadTextInput = nullptr;
	SteamDLC* pSteamDLC = nullptr;
	SteamFriend* pSteamFriend = nullptr;
	
	//------------
	// None Callback
	//------------

	SteamRemote* pSteamRemote = nullptr;

	//------------
	// Tasks
	//------------

	SteamRefreshClass::RefreshTasks refreshTasks;

public:
	CSteamID playerID;
	AppId_t appID;
	int buildID;

	SteamUtilities() :
		playerID(SteamUser()->GetSteamID()),
		appID(SteamUtils()->GetAppID()),
		buildID(SteamApps()->GetAppBuildId()) {
		InitSteamCommandLine();

		pSteamAchAndStat = new SteamAchAndStat(&refreshTasks);

		pSteamMicroTxn = new SteamMicroTxn();
		pSteamRichPresence = new SteamRichPresence();
		pSteamScreenshot = new SteamScreenshot();
		pSteamGamepadTextInput = new SteamGamepadTextInput();
		pSteamDLC = new SteamDLC();
		pSteamInventory = new SteamInv();
		pSteamFriend = new SteamFriend();

		pSteamRemote = new SteamRemote();
	}
	~SteamUtilities() {
		delete pSteamAchAndStat;
		delete pSteamInventory;

		delete pSteamMicroTxn;
		delete pSteamRichPresence;
		delete pSteamScreenshot;
		delete pSteamGamepadTextInput;
		delete pSteamDLC;
		delete pSteamFriend;
		
		delete pSteamRemote;
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

	inline SteamAchAndStat* GetAchAndStat() const { return pSteamAchAndStat; }
	inline SteamInv* GetSteamInventory() const { return pSteamInventory; }

	inline SteamMicroTxn* GetMicroTxn() const { return pSteamMicroTxn; }
	inline SteamRichPresence* GetRichPresence() const { return pSteamRichPresence; }
	inline SteamScreenshot* GetSteamScreenshot() const { return pSteamScreenshot; }
	inline SteamGamepadTextInput* GetSteamGamepadTextInput() const { return pSteamGamepadTextInput; }
	inline SteamDLC* GetSteamDLC() const { return pSteamDLC; }
	inline SteamFriend* GetSteamFriend() const { return pSteamFriend; }

	inline SteamRemote* GetRemote() const { return pSteamRemote; }

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