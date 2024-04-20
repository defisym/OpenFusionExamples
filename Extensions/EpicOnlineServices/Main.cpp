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
		IDMN_CONDITION_ONLOGIN, M_CONDITION_ONLOGIN, CND_CONDITION_ONLOGIN, 0, 0,
		IDMN_CONDITION_USERLOGIN, M_CONDITION_USERLOGIN, CND_CONDITION_USERLOGIN, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		IDMN_CONDITION_QUEARYCOMPLETE, M_CONDITION_QUEARYCOMPLETE, CND_CONDITION_QUEARYCOMPLETE, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPSTRING, M_QUERYTYPE,
		IDMN_CONDITION_ONERROR, M_CONDITION_ONERROR, CND_CONDITION_ONERROR, 0, 0,
		IDMN_CONDITION_ONLOGOUT, M_CONDITION_ONLOGOUT, CND_CONDITION_ONLOGOUT, 0, 0,

		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_ACH_UL, M_ACTION_ACH_UL, ACT_ACTION_ACH_UL,	0, 1, PARAM_EXPSTRING, M_ACH_NAME,
		IDMN_ACTION_STAT_I, M_ACTION_STAT_I, ACT_ACTION_STAT_I,	0, 2, PARAM_EXPSTRING, PARAM_EXPRESSION, M_STAT_NAME, M_STAT_VALUE,
		IDMN_ACTION_QUERY, M_ACTION_QUERY, ACT_ACTION_QUERY,	0, 1, PARAM_EXPSTRING, M_QUERYTYPE,
		IDMN_ACTION_PRE_SRT, M_ACTION_PRE_SRT, ACT_ACTION_PRE_SRT,	0, 1, PARAM_EXPSTRING, M_RICHTEXT,
		IDMN_ACTION_LI, M_ACTION_LI, ACT_ACTION_LI,	0, 0,
		IDMN_ACTION_LO, M_ACTION_LO, ACT_ACTION_LO,	0, 0,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_STATVALUE, M_EXPRESSION_STATVALUE, EXP_EXPRESSION_STATVALUE, 0, 1, EXPPARAM_STRING, M_STAT_NAME,
		IDMN_EXPRESSION_ACCOUNTID, M_EXPRESSION_ACCOUNTID, EXP_EXPRESSION_ACCOUNTID, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_PRODUCTUSERID, M_EXPRESSION_PRODUCTUSERID, EXP_EXPRESSION_PRODUCTUSERID, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_PRE_GRT, M_EXPRESSION_PRE_GRT, EXP_EXPRESSION_PRE_GRT, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GLE, M_EXPRESSION_GLE, EXP_EXPRESSION_GLE, EXPFLAG_STRING, 0,
		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Condition_OnLogin(LPRDATA rdPtr, long param1, long param2) {
	return true;
}

long WINAPI DLLExport Condition_OnLogout(LPRDATA rdPtr, long param1, long param2) {
	return true;
}

long WINAPI DLLExport Condition_LoginSuccess(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->pData->logOpt.bUserLogin;
}

long WINAPI DLLExport Condition_QueryComplete(LPRDATA rdPtr, long param1, long param2) {
	const auto pQueryType = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	do {
		if (StrEmpty(pQueryType)) {
			bool bComplete = true;

			bComplete = bComplete && rdPtr->pData->pEOSAchievement->QueryComplete();
			bComplete = bComplete && rdPtr->pData->pEOSStat->QueryComplete();
			bComplete = bComplete && rdPtr->pData->pEOSPresence->QueryComplete();

			return bComplete;
		}

		if (StrIEqu(pQueryType, EOSQueryType::Achievement)) {
			return rdPtr->pData->pEOSAchievement->QueryComplete();
		}
		if (StrIEqu(pQueryType, EOSQueryType::Stat)) {
			return rdPtr->pData->pEOSStat->QueryComplete();
		}
		if (StrIEqu(pQueryType, EOSQueryType::Presence)) {
			return rdPtr->pData->pEOSPresence->QueryComplete();
		}
	} while (false);

	return false;
}

long WINAPI DLLExport Condition_OnError(LPRDATA rdPtr, long param1, long param2) {
	return true;
}

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport Action_Login(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pData->EOSLogin([=] (bool bSuccess) {
		rdPtr->pData->logOpt.bUserLogin = bSuccess;
		AddEvent(ON_LoginComplete);
		});

	return 0;
}

short WINAPI DLLExport Action_Logout(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pData->EOSLogout([=] (bool bSuccess) {
		rdPtr->pData->logOpt.bUserLogin = !bSuccess;
		AddEvent(ON_LogoutComplete);
		});

	return 0;
}

short WINAPI DLLExport Action_Achievement_Unlock(LPRDATA rdPtr, long param1, long param2) {
	const std::string achName = ConvertWStrToStr((LPCWSTR)CNC_GetStringParameter(rdPtr));

	rdPtr->pData->pEOSAchievement->UnlockAchievements({ achName });

	return 0;
}

short WINAPI DLLExport Action_Stat_Ingest(LPRDATA rdPtr, long param1, long param2) {
	const std::string statName = ConvertWStrToStr((LPCWSTR)CNC_GetStringParameter(rdPtr));
	const auto value = (int32_t)CNC_GetStringParameter(rdPtr);

	rdPtr->pData->pEOSStat->IngestStat({ {statName,value} }, [=] (EOSStat*) {});

	return 0;
}

short WINAPI DLLExport Action_Query(LPRDATA rdPtr, long param1, long param2) {
	const auto pQueryType = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	do {
		if (StrEmpty(pQueryType)) {
			rdPtr->pData->EOSQueryPlatform();
			break;
		}

		if(StrIEqu(pQueryType, EOSQueryType::Achievement)) {
			rdPtr->pData->pEOSAchievement->PlatformQuery();
			break;
		}
		if (StrIEqu(pQueryType, EOSQueryType::Stat)) {
			rdPtr->pData->pEOSStat->PlatformQuery();
			break;
		}
		if (StrIEqu(pQueryType, EOSQueryType::Presence)) {
			rdPtr->pData->pEOSPresence->PlatformQuery();
			break;
		}
	} while (false);

	return 0;
}

short WINAPI DLLExport Action_Presence_SetRichTest(LPRDATA rdPtr, long param1, long param2) {
	const auto pQueryType = ConvertWStrToStr((LPCWSTR)CNC_GetStringParameter(rdPtr));

	rdPtr->pData->pEOSPresence->SetPresenceSetRawRichText(pQueryType);

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Expression_GetLastError(LPRDATA rdPtr, long param1) {
	*rdPtr->pRet = ConvertStrToWStr(rdPtr->pData->pEOSUtilities->GetLastError());

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->pRet->c_str();
}

long WINAPI DLLExport Expression_GetStatValue(LPRDATA rdPtr, long param1) {
	const std::string statName = ConvertWStrToStr((LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING));

	int32_t val = 0;

	rdPtr->pData->pEOSStat->GetStatByName(statName, [&] (EOS_Stats_Stat* pData) {
		val = pData->Value;
	});

	return val;
}

long WINAPI DLLExport Expression_GetAccountID(LPRDATA rdPtr, long param1) {
	*rdPtr->pRet = ConvertStrToWStr(rdPtr->pData->pEOSUtilities->GetAccountID());

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->pRet->c_str();
}

long WINAPI DLLExport Expression_GetProductUserID(LPRDATA rdPtr, long param1) {
	*rdPtr->pRet = ConvertStrToWStr(rdPtr->pData->pEOSUtilities->GetProductUserID());

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->pRet->c_str();
}

long WINAPI DLLExport Expression_Presence_GetRichTest(LPRDATA rdPtr, long param1) {
	const auto pEP = rdPtr->pData->pEOSPresence;

	rdPtr->pRet->clear();

	if(pEP->HasPresence()) {
		pEP->CopyPresence([&] (const EOS_Presence_Info* pInfo) {
			*rdPtr->pRet = ConvertStrToWStr(pInfo->RichText);
		});
	}
	
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
			Condition_OnLogin,
			Condition_LoginSuccess,
			Condition_QueryComplete,
			Condition_OnError,
			Condition_OnLogout,

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action_Achievement_Unlock,
			Action_Stat_Ingest,
			Action_Query,
			Action_Presence_SetRichTest,
			Action_Login,
			Action_Logout,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression_GetStatValue,
			Expression_GetAccountID,
			Expression_GetProductUserID,
			Expression_Presence_GetRichTest,
			Expression_GetLastError,

			0
			};