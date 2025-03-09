#pragma once

#include "SteamInclude.h"
#include "WindowsCommon.h"

struct SteamInit {
	bool bInit = false;
	SteamErrMsg errMsg;

	inline void Init() {
		if (!bInit) {
			const auto initResult = SteamAPI_InitEx(&errMsg);
			bInit = initResult == k_ESteamAPIInitResult_OK;
		}
	}

	inline void Shutdown() {
		if (bInit) {
			SteamAPI_Shutdown();
			bInit = false;
		}
	}

	inline const char* GetInitErrorInfo() const {
		if(bInit) {
			return "No Init Error";
		}

		return errMsg;
	}
};

// https://partner.steamgames.com/doc/features/overlay#requirements
// To use overlay, steam must be init before init OpenGL/D3D devices
// so should be called in General.cpp -> Initialize
// or RAII as a global value
extern SteamInit steamInit;