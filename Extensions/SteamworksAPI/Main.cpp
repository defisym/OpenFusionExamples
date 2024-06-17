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
		IDMN_CONDITION_RPO, M_CONDITION_RPO, CND_CONDITION_RPO, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPSTRING, M_CND_RPO,
		
		IDMN_CONDITION_OMTE, M_CONDITION_OMTE, CND_CONDITION_OMTE, 0, 2, PARAM_EXPSTRING, PARAM_EXPRESSION, M_MT_NAME, M_MT_STEP,
		IDMN_CONDITION_OMTF, M_CONDITION_OMTF, CND_CONDITION_OMTF, 0, 2, PARAM_EXPSTRING, PARAM_EXPRESSION, M_MT_NAME, M_MT_STEP,

		IDMN_CONDITION_ROSD, M_CONDITION_ROSD, CND_CONDITION_ROSD, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,

		IDMN_CONDITION_OSS, M_CONDITION_OSS, CND_CONDITION_OSS, 0, 0,

		IDMN_CONDITION_OID, M_CONDITION_OID, CND_CONDITION_OID, 0, 0,
		IDMN_CONDITION_SUBMITTED, M_CONDITION_SUBMITTED, CND_CONDITION_SUBMITTED, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,

		IDMN_CONDITION_ODLCIC, M_CONDITION_ODLCIC, CND_CONDITION_ODLCIC, 0, 1, PARAM_EXPRESSION, M_APPID,

		IDMN_CONDITION_PE, M_CONDITION_PE, CND_CONDITION_PE, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_UA, M_ACTION_UA,	ACT_ACTION_UA,	0, 1, PARAM_EXPSTRING, M_ACH_NAME,
		IDMN_ACTION_AS, M_ACTION_AS,	ACT_ACTION_AS,	0, 2, PARAM_EXPSTRING, PARAM_EXPRESSION, M_STAT_NAME, M_STAT_DATA,

		IDMN_ACTION_SRP, M_ACTION_SRP,	ACT_ACTION_SRP,	0, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_STAT_RPK, M_STAT_RPV,
		IDMN_ACTION_CRP, M_ACTION_CRP,	ACT_ACTION_CRP,	0, 0,

		IDMN_ACTION_MT_SI, M_ACTION_MT_SI,	ACT_ACTION_MT_SI,	0, 4, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPSTRING, M_MT_NAME, M_MT_KEY, M_MT_ORDERID, M_MT_USERID,
		IDMN_ACTION_MT_GUI, M_ACTION_MT_GUI,	ACT_ACTION_MT_GUI,	0, 0,
		IDMN_ACTION_MT_SR, M_ACTION_MT_SR,	ACT_ACTION_MT_SR,	0, 6, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPSTRING, M_MT_ITEMCOUNT, M_MT_ITEMID, M_MT_ITEMNUM, M_MT_AMOUNT, M_MT_DESC, M_MT_OTHERS,
		IDMN_ACTION_MT_F, M_ACTION_MT_F,	ACT_ACTION_MT_F,	0, 0,

		IDMN_ACTION_SSSL, M_ACTION_SSSL,	ACT_ACTION_SSSL,	0, 1, PARAM_EXPSTRING, M_LOACTION,
		IDMN_ACTION_TSSU, M_ACTION_TSSU,	ACT_ACTION_TSSU,	0, 1, PARAM_EXPSTRING, M_MT_USERID,
		IDMN_ACTION_TSSPF, M_ACTION_TSSPF,	ACT_ACTION_TSSPF,	0, 1, PARAM_EXPSTRING, M_PFID,
		IDMN_ACTION_TSS, M_ACTION_TSS,	ACT_ACTION_TSS,	0, 0,

		IDMN_ACTION_SGTI, M_ACTION_SGTI,	ACT_ACTION_SGTI,	0, 5, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPSTRING, PARAM_EXPRESSION, PARAM_EXPSTRING, M_INPUTMODE, M_LINEMODE, M_DESCRIPTION, M_MAXCHAR, M_EXISTINGTEXT,

		IDMN_ACTION_IAP, M_ACTION_IAP,	ACT_ACTION_IAP, 0, 3, PARAM_EXPSTRING, PARAM_EXPRESSION, PARAM_EXPRESSION, M_ACH_NAME, M_CURPROGRESS, M_MAXPROGRESS,
		IDMN_ACTION_SARS, M_ACTION_SARS,	ACT_ACTION_SARS, 0, 3, PARAM_EXPSTRING, PARAM_EXPRESSION, PARAM_EXPRESSION, M_STAT_NAME, M_COUNTTHISSESSION, M_SESSIONLENGTH,

		IDMN_ACTION_AGOTS, M_ACTION_AGOTS,	ACT_ACTION_AGOTS, 0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_APPID, M_GOTSFLAG, 
		IDMN_ACTION_ID, M_ACTION_ID, ACT_ACTION_ID, 0, 1, PARAM_EXPRESSION, M_APPID, 

		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GCGL, M_EXPRESSION_GCGL, EXP_EXPRESSION_GCGL, EXPFLAG_STRING, 0,		
		IDMN_EXPRESSION_GSCL, M_EXPRESSION_GSCL, EXP_EXPRESSION_GSCL, EXPFLAG_STRING, 0,
		
		IDMN_EXPRESSION_MT_GS, M_EXPRESSION_MT_GS, EXP_EXPRESSION_MT_GS, 0, 0,
		IDMN_EXPRESSION_MT_GHTMLEC, M_EXPRESSION_MT_GHTMLEC, EXP_EXPRESSION_MT_GHTMLEC, 0, 0,
		IDMN_EXPRESSION_MT_GED, M_EXPRESSION_MT_GED, EXP_EXPRESSION_MT_GED, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_MT_GTID, M_EXPRESSION_MT_GTID, EXP_EXPRESSION_MT_GTID, EXPFLAG_STRING, 0,

		IDMN_EXPRESSION_GCBP, M_EXPRESSION_GCBP, EXP_EXPRESSION_GCBP, 0, 0,

		IDMN_EXPRESSION_GGIT, M_EXPRESSION_GGIT, EXP_EXPRESSION_GGIT, EXPFLAG_STRING, 0,

		IDMN_EXPRESSION_GDDPP, M_EXPRESSION_GDDPP, EXP_EXPRESSION_GDDPP, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG, M_APPID,

		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Condition_PlatformEnabled(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->pData->SteamUtilitiesValid();
}

long WINAPI DLLExport Condition_RemotePlayOn(LPRDATA rdPtr, long param1, long param2) {
	const auto pPlatform = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto factorToSearch = SteamRemote::DeviceNameToFactor(pPlatform);

	if (!rdPtr->pData->SteamUtilitiesValid()) {
		return false;
	}

	bool bResult = false;
	bool bLocal = true;	

	SteamRemote::IterateRemoteSessions([&](RemotePlaySessionID_t unSessionID) {
		// check current player
		if (SteamRemote::GetSessionSteamID(unSessionID) != rdPtr->pData->pSteamUtil->playerID) {
			return;
		}

		bLocal = false;

		if (SteamRemote::GetSessionClientFormFactor(unSessionID) == factorToSearch) {
			bResult = true;
		}
		});

	// no session, playing local on PC
	if (bLocal && StrIEqu(pPlatform, L"Local")) {
		bResult = true;
	}

	return bResult;
}

long WINAPI DLLExport Condition_OnMixroTxnError(LPRDATA rdPtr, long param1, long param2) {
	const auto pName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto step = (SteamMicroTxn::Step)CNC_GetIntParameter(rdPtr);
	
	if (!rdPtr->pData->SteamUtilitiesValid()) {
		return false;
	}

	//MSGBOX(std::format(L"Error Step {}", (int)rdPtr->pData->pSteamUtil->GetMicroTxn()->step));

	return 	step == rdPtr->pData->pSteamUtil->GetMicroTxn()->step
		&& StrEqu(pName, ConvertStrToWStr(rdPtr->pData->pSteamUtil->GetMicroTxn()->name).c_str());
}

long WINAPI DLLExport Condition_OnMixroTxnFinish(LPRDATA rdPtr, long param1, long param2) {
	const auto pName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto step = (SteamMicroTxn::Step)CNC_GetIntParameter(rdPtr);
	
	if (!rdPtr->pData->SteamUtilitiesValid()) {
		return false;
	}

	//MSGBOX(std::format(L"Finish Step {}", (int)rdPtr->pData->pSteamUtil->GetMicroTxn()->step));

	return 	step == rdPtr->pData->pSteamUtil->GetMicroTxn()->step
		&& StrEqu(pName, ConvertStrToWStr(rdPtr->pData->pSteamUtil->GetMicroTxn()->name).c_str());
}

long WINAPI DLLExport Condition_RunningOnSteamDeck(LPRDATA rdPtr, long param1, long param2) {
	if (!rdPtr->pData->SteamUtilitiesValid()) {
		return false;
	}

	return SteamUtils()->IsSteamRunningOnSteamDeck();
}

long WINAPI DLLExport Condition_OnScreenshot(LPRDATA rdPtr, long param1, long param2) {
	if (!rdPtr->pData->SteamUtilitiesValid()) {
		return false;
	}

	return true;
}

long WINAPI DLLExport Condition_OnGamepadInputDismiss(LPRDATA rdPtr, long param1, long param2) {
	if (!rdPtr->pData->SteamUtilitiesValid()) {
		return false;
	}

	return true;
}

long WINAPI DLLExport Condition_GamepadInputSubmitted(LPRDATA rdPtr, long param1, long param2) {
	if (!rdPtr->pData->SteamUtilitiesValid()) {
		return false;
	}

	return rdPtr->pData->pSteamUtil->GetSteamGamepadTextInput()->GetSubmitted();
}

long WINAPI DLLExport Condition_OnDLCInstallComplete(LPRDATA rdPtr, long param1, long param2) {
	const auto appID = (AppId_t)CNC_GetParameter(rdPtr);

	if (!rdPtr->pData->SteamUtilitiesValid()) {
		return false;
	}

	return rdPtr->callBackAppID = appID;
}

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport Action_UnlockAchievement(LPRDATA rdPtr, long param1, long param2) {
	const auto pAchievementName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pData->GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetAchAndStat()->UnlockAchievement(pAchievementName);
	});

	return 0;
}

short WINAPI DLLExport Action_IndicateAchievementProgress(LPRDATA rdPtr, long param1, long param2) {
	const auto pAchievementName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto nCurProgress = (uint32)CNC_GetParameter(rdPtr);
	const auto nMaxProgress = (uint32)CNC_GetParameter(rdPtr);

	rdPtr->pData->GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetAchAndStat()->IndicateAchievementProgress(pAchievementName, nCurProgress, nMaxProgress);
	});

	return 0;
}

short WINAPI DLLExport Action_AddToStat(LPRDATA rdPtr, long param1, long param2) {
	const auto pStatName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto data = (int)CNC_GetIntParameter(rdPtr);

	
	return 0;
}

short WINAPI DLLExport Action_SetAvgRateStat(LPRDATA rdPtr, long param1, long param2) {
	const auto pStatName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto flCountThisSession = GetFloatParam(rdPtr);
	const auto dSessionLength = GetFloatParam(rdPtr);

	rdPtr->pData->GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetAchAndStat()->SetAvgRateStat(pStatName, flCountThisSession, dSessionLength);
	});

	return 0;
}

short WINAPI DLLExport Action_SetRichPresence(LPRDATA rdPtr, long param1, long param2) {
	const auto pchKey = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto pchValue = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pData->GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		const auto bRet = pSteamUtil->GetRichPresence()->SetRichPresence(pchKey, pchValue);
	});

	return 0;
}

short WINAPI DLLExport Action_ClearRichPresence(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pData->GetSteamUtilities([] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetRichPresence()->ClearRichPresence();
	});

	return 0;
}

short WINAPI DLLExport Action_MixroTxn_SetInfo(LPRDATA rdPtr, long param1, long param2) {
	const auto name = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto key = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto orderID = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto userID = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pData->GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		if (StrEmpty(userID)) {
			pSteamUtil->GetMicroTxn()->SetMicroTxnInfo(
				ConvertWStrToStr(name),
				ConvertWStrToStr(key),
				std::stoull(orderID));
		}else {
			pSteamUtil->GetMicroTxn()->SetMicroTxnInfo(
				ConvertWStrToStr(name),
				ConvertWStrToStr(key),
				std::stoull(orderID),
				std::stoull(userID));
		}
	});

	return 0;
}

short WINAPI DLLExport Action_MixroTxn_GetUserInfo(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pData->GetSteamUtilities([] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetMicroTxn()->GetUserInfo();
	});

	return 0;
}

short WINAPI DLLExport Action_MixroTxn_SendRequest(LPRDATA rdPtr, long param1, long param2) {
	const auto itemcount = (uint32)CNC_GetParameter(rdPtr);
	const auto itemid = (uint32)CNC_GetParameter(rdPtr);
	const auto qty = (int16)CNC_GetParameter(rdPtr);
	const auto amount = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto description = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto otherParams = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pData->GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetMicroTxn()->SendRequest(itemcount, itemid, qty,
		std::stoll(amount),
		ConvertWStrToStr(description),
		ConvertWStrToStr(otherParams));
	});

	return 0;
}

short WINAPI DLLExport Action_MixroTxn_Finalize(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pData->GetSteamUtilities([] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetMicroTxn()->Finalize();
	});

	return 0;
}

short WINAPI DLLExport Action_Screenshot_SetLocation(LPRDATA rdPtr, long param1, long param2) {
	const auto pLocation = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pData->GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetSteamScreenshot()->SetLocation(pLocation);
	});
	return 0;
}

short WINAPI DLLExport Action_Screenshot_TagPublishedFile(LPRDATA rdPtr, long param1, long param2) {
	const auto publishedFileID = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pData->GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetSteamScreenshot()->TagPublishedFile(std::stoull(publishedFileID));
	});

	return 0;
}

short WINAPI DLLExport Action_Screenshot_TagUser(LPRDATA rdPtr, long param1, long param2) {
	const auto steamID = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pData->GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetSteamScreenshot()->TagUser(std::stoull(steamID));
	});

	return 0;
}

short WINAPI DLLExport Action_Screenshot_Trigger(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pData->GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetSteamScreenshot()->Trigger();
	});

	return 0;
}

short WINAPI DLLExport Action_ShowGamepadTextInput(LPRDATA rdPtr, long param1, long param2) {
	const auto inputMode = (EGamepadTextInputMode)CNC_GetIntParameter(rdPtr);
	const auto lineMode = (EGamepadTextInputLineMode)CNC_GetIntParameter(rdPtr);
	const auto description = ConvertWStrToStr((LPCWSTR)CNC_GetStringParameter(rdPtr));
	const auto maxChar = (uint32)CNC_GetIntParameter(rdPtr);
	const auto existingText = ConvertWStrToStr((LPCWSTR)CNC_GetStringParameter(rdPtr));

	rdPtr->pData->GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetSteamGamepadTextInput()->ShowGamepadTextInput(inputMode,lineMode,description.c_str(),maxChar,existingText.c_str());
	});

	return 0;
}

short WINAPI DLLExport Action_ActivateGameOverlayToStore(LPRDATA rdPtr, long param1, long param2) {
	const auto nAppID = (AppId_t)CNC_GetParameter(rdPtr);
	const auto eFlag = (EOverlayToStoreFlag)CNC_GetParameter(rdPtr);

	rdPtr->pData->GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		SteamDLC::ActivateGameOverlayToStore(nAppID, eFlag);
	});

	return 0;
}

short WINAPI DLLExport Action_InstallDLC(LPRDATA rdPtr, long param1, long param2) {
	const auto nAppID = (AppId_t)CNC_GetParameter(rdPtr);

	rdPtr->pData->GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		if (SteamDLC::DLCInstalled(nAppID)) { return; }
		SteamDLC::InstallDLC(nAppID);
	});

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Expression_GetCurrentGameLanguage(LPRDATA rdPtr,long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return rdPtr->pData->GetSteamUtilities<long>((long)Empty_Str,
		[&] (SteamUtilities* pSteamUtil) {
		const auto pLanguage = SteamApps()->GetCurrentGameLanguage();

#ifdef _DEBUG
		auto pAvailableLanguage = SteamApps()->GetAvailableGameLanguages();
		auto pUILanguage = SteamUtils()->GetSteamUILanguage();
#endif // _DEBUG

		*rdPtr->pRet = ConvertStrToWStr(pLanguage);

		return (long)rdPtr->pRet->c_str();
	});
}

long WINAPI DLLExport Expression_GetSteamCommandLine(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return rdPtr->pData->GetSteamUtilities<long>((long)Empty_Str,
		[&] (SteamUtilities* pSteamUtil) {
		*rdPtr->pRet = pSteamUtil->GetSteamCommandLine<LPWSTR>();
		return (long)rdPtr->pRet->c_str();
	});
}

long WINAPI DLLExport Expression_MixroTxn_GetStep(LPRDATA rdPtr, long param1) {
	return rdPtr->pData->SteamUtilitiesValid()
		? (long)rdPtr->pData->pSteamUtil->GetMicroTxn()->step
		: (long)-1;
}

long WINAPI DLLExport Expression_MixroTxn_GetHTMLErrorCode(LPRDATA rdPtr, long param1) {
	return rdPtr->pData->SteamUtilitiesValid()
		? (long)rdPtr->pData->pSteamUtil->GetMicroTxn()->code
		: (long)k_EHTTPStatusCodeInvalid;
}

long WINAPI DLLExport Expression_MixroTxn_GetErrorDesc(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return rdPtr->pData->GetSteamUtilities<long>((long)Empty_Str,
	[&] (SteamUtilities* pSteamUtil) {
		*rdPtr->pRet = ConvertStrToWStr(pSteamUtil->GetMicroTxn()->errordesc);
		return (long)rdPtr->pRet->c_str();
	},
	[&] (SteamUtilities* pSteamUtil) {
		return !pSteamUtil->GetMicroTxn()->errordesc.empty();
	});
}

long WINAPI DLLExport Expression_MixroTxn_GetTransID(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return rdPtr->pData->GetSteamUtilities<long>((long)Empty_Str,
		[&] (SteamUtilities* pSteamUtil) {
			*rdPtr->pRet = ConvertStrToWStr(pSteamUtil->GetMicroTxn()->transid);
			return (long)rdPtr->pRet->c_str();
	},
		[&] (SteamUtilities* pSteamUtil) {
			return !pSteamUtil->GetMicroTxn()->transid.empty();
	});
}

long WINAPI DLLExport Expression_GetCurrentBatteryPower(LPRDATA rdPtr, long param1) {
	return rdPtr->pData->GetSteamUtilities<long>(255,
		[&] (SteamUtilities* pSteamUtil) {
			return SteamUtils()->GetCurrentBatteryPower();
	});
}

long WINAPI DLLExport Expression_GetGamepadText(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return rdPtr->pData->GetSteamUtilities<long>((long)Empty_Str,
		[&] (SteamUtilities* pSteamUtil) {
			*rdPtr->pRet = ConvertStrToWStr(pSteamUtil->GetSteamGamepadTextInput()->GetText());
			return (long)rdPtr->pRet->c_str();
	});
}

long WINAPI DLLExport Expression_GetDLCDownloadProgressPercent(LPRDATA rdPtr, long param1) {
	const auto appID = (AppId_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
	rdPtr->rHo.hoFlags |= HOF_FLOAT;

	//Return the float without conversion
	return rdPtr->pData->GetSteamUtilities<long>(ConvertToLong(-1.0f),
		[&] (SteamUtilities* pSteamUtil) {
			const auto progress = SteamDLC::GetDLCDownloadProgressPercent(appID);

			return ConvertToLong(progress);
	});
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
			
			Condition_OnMixroTxnError,
			Condition_OnMixroTxnFinish,

			Condition_RunningOnSteamDeck,

			Condition_OnScreenshot,

			Condition_OnGamepadInputDismiss,
			Condition_GamepadInputSubmitted,

			Condition_OnDLCInstallComplete,

			Condition_PlatformEnabled,

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action_UnlockAchievement,
			Action_AddToStat,

			Action_SetRichPresence,
			Action_ClearRichPresence,

			Action_MixroTxn_SetInfo,
			Action_MixroTxn_GetUserInfo,
			Action_MixroTxn_SendRequest,
			Action_MixroTxn_Finalize,

			Action_Screenshot_SetLocation,
			Action_Screenshot_TagPublishedFile,
			Action_Screenshot_TagUser,
			Action_Screenshot_Trigger,

			Action_ShowGamepadTextInput,

			Action_IndicateAchievementProgress,
			Action_SetAvgRateStat,

			Action_ActivateGameOverlayToStore,
			Action_InstallDLC,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression_GetCurrentGameLanguage,
			Expression_GetSteamCommandLine,
			
			Expression_MixroTxn_GetStep,
			Expression_MixroTxn_GetHTMLErrorCode,
			Expression_MixroTxn_GetErrorDesc,
			Expression_MixroTxn_GetTransID,

			Expression_GetCurrentBatteryPower,

			Expression_GetGamepadText,
	
			Expression_GetDLCDownloadProgressPercent,

			0
			};