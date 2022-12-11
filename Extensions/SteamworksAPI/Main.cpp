// ============================================================================
//
// This file are where the Conditions/Actions/Expressions are defined.
// You can manually enter these, or use CICK (recommended)
// See the Extension FAQ in this SDK for more info and where to download it
//
// ============================================================================

// Common Include
#include	"common.h"

// Quick memo: content of the eventInformations arrays
// ---------------------------------------------------
// Menu ID
// String ID
// Code
// Flags
// Number_of_parameters
// Parameter_type [Number_of_parameters]
// Parameter_TitleString [Number_of_parameters]

// Definitions of parameters for each condition
short conditionsInfos[]=
		{
		IDMN_CONDITION_RPO, M_CONDITION_RPO, CND_CONDITION_RPO, EVFLAGS_ALWAYS, 1, PARAM_EXPSTRING, M_CND_RPO,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION, M_ACTION,	ACT_ACTION,	0, 0,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GCGL, M_EXPRESSION_GCGL, EXP_EXPRESSION_GCGL, EXPFLAG_STRING, 0,		
		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Condition_RemotePlayOn(LPRDATA rdPtr, long param1, long param2) {
	auto pPlatform = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	auto factorToSearch = rdPtr->pSteamUtil->DeviceNameToFactor(pPlatform);

	bool bResult = false;
	bool bLocal = true;
	rdPtr->pSteamUtil->IterateRemoteSessions([&](RemotePlaySessionID_t unSessionID) {
		// check current player
		auto steamID = SteamRemotePlay()->GetSessionSteamID(unSessionID);
		if (steamID != rdPtr->pData->playerID) {
			return;
		}

		bLocal = false;
		
		ESteamDeviceFormFactor eFormFactor = SteamRemotePlay()->GetSessionClientFormFactor(unSessionID);
		if (eFormFactor == factorToSearch) {
			bResult = true;
		}
		});

	// no session, playing local on PC
	if (bLocal && StrIEqu(pPlatform, L"Local")) {
		bResult = true;
	}

	return bResult;
}


// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

// -----------------
// Sample Action
// -----------------
// Does nothing!
// 
short WINAPI DLLExport Action(LPRDATA rdPtr, long param1, long param2)
{
	// Actions work just like Conditions

	// Use directly param1 and/or param2 if this action has 1 or 2 parameters

	// Use this if this action has 3 parameters or more
//	long p1 = CNC_GetParameter(rdPtr);
//	long p2 = CNC_GetParameter(rdPtr);
//	long p3 = CNC_GetParameter(rdPtr);
//	etc.

	return 0;
}


// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Expression_GetCurrentGameLanguage(LPRDATA rdPtr,long param1) {
	auto pLanguage = SteamApps()->GetCurrentGameLanguage();

#ifdef _DEBUG
	auto pAvailableLanguage = SteamApps()->GetAvailableGameLanguages();
	auto pUILanguage = SteamUtils()->GetSteamUILanguage();
#endif // _DEBUG

	*rdPtr->pRet = ConvertStrToWStr(pLanguage);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->pRet->c_str();
}


// ----------------------------------------------------------
// Condition / Action / Expression jump table
// ----------------------------------------------------------
// Contains the address inside the extension of the different
// routines that handle the action, conditions and expressions.
// Located at the end of the source for convinience
// Must finish with a 0
//
long (WINAPI * ConditionJumps[])(LPRDATA rdPtr, long param1, long param2) = 
			{ 
			Condition_RemotePlayOn,

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression_GetCurrentGameLanguage,

			0
			};