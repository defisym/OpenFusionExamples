#pragma once

#pragma warning( disable : 4996)
#pragma warning( disable : 4819)

#define CompatibleWithSteamworksObject

#include "steam/steam_api.h"
#ifdef CompatibleWithSteamworksObject
#pragma comment(lib,"neoSteam_api.lib")
#else
#pragma comment(lib,"steam_api.lib")
#endif

#include "SteamRefresh.h"

#include "SteamCallback.h"
#include "SteamConcepts.h"
