#pragma once

//immediate conditon ID
//constexpr auto ON_FINISH = 4;

class SteamUtilities;

struct GlobalData {
	bool bInit = false;
	SteamUtilities* pSteamUtil = nullptr;

	GlobalData() {
		bInit = SteamAPI_Init();
		pSteamUtil = new SteamUtilities;
	}
	~GlobalData() {
		delete pSteamUtil;
		pSteamUtil = nullptr;

		if (bInit) {
			SteamAPI_Shutdown();
		}
	}
};