#pragma once

//immediate conditon ID
//constexpr auto ON_FINISH = 4;

class SteamUtilities;

struct GlobalData {
	bool bInit = false;
	SteamUtilities* pSteamUtil = nullptr;
	CSteamID playerID;

	GlobalData() {
		bInit = SteamAPI_Init();
		pSteamUtil = new SteamUtilities;

		playerID = SteamUser()->GetSteamID();

#ifdef _POPUP_TEST
		if (!bInit) {
			MSGBOX(L"Init Failed !");

			exit(0);
	}

		int result = SteamApps()->GetAppBuildId();
		MSGBOX(L"Build ID: " + _itos(result));

#endif // _POPUP_TEST
	}
	~GlobalData() {
		delete pSteamUtil;
		pSteamUtil = nullptr;

		if (bInit) {
			SteamAPI_Shutdown();
		}
	}
};