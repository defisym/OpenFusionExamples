#pragma once

#pragma warning( disable : 4996)
#pragma warning( disable : 4819)

// if disable this marco, then it's not compatible with legacy steamworks object
#define CompatibleWithSteamworksObject

#include "steam/steam_api.h"
#ifdef CompatibleWithSteamworksObject
#pragma comment(lib,"SteamworksAPI.lib")
#else
#pragma comment(lib,"steam_api.lib")
#endif

#include "SteamRefresh.h"

#include "SteamCallback.h"
#include "SteamConcepts.h"
