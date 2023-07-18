#pragma once

#include "SteamInclude.h"

struct SteamInit {
	bool bInit = false;

	SteamInit() {
		bInit = SteamAPI_Init();
	}

	~SteamInit() {
		if (bInit) {
			SteamAPI_Shutdown();
		}
	}
};

// https://partner.steamgames.com/doc/features/overlay#requirements
// To use overlay, steam must be init before init OpenGL/D3D devices
// so should be called in General.cpp -> Initialize
// or RAII as a global value
extern SteamInit steamInit;