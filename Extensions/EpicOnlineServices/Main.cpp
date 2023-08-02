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
		IDMN_CONDITION_LOGINSUCCESS, M_CONDITION_LOGINSUCCESS, CND_CONDITION_LOGINSUCCESS, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		IDMN_CONDITION_QUEARYCOMPLETE, M_CONDITION_QUEARYCOMPLETE, CND_CONDITION_QUEARYCOMPLETE, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_ACH_UL, M_ACTION_ACH_UL, ACT_ACTION_ACH_UL,	0, 1, PARAM_EXPSTRING, M_ACH_NAME,
		IDMN_ACTION_STAT_I, M_ACTION_STAT_I, ACT_ACTION_STAT_I,	0, 2, PARAM_EXPSTRING, PARAM_EXPRESSION, M_STAT_NAME, M_STAT_VALUE,
		IDMN_ACTION_QUERY, M_ACTION_QUERY, ACT_ACTION_QUERY,	0, 0,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_STATVALUE, M_EXPRESSION_STATVALUE, EXP_EXPRESSION_STATVALUE, 0, 1, EXPPARAM_STRING, M_STAT_NAME,
		IDMN_EXPRESSION_ACCOUNTID, M_EXPRESSION_ACCOUNTID, EXP_EXPRESSION_ACCOUNTID, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_PRODUCTUSERID, M_EXPRESSION_PRODUCTUSERID, EXP_EXPRESSION_PRODUCTUSERID, EXPFLAG_STRING, 0,
		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Condition_OnLogin(LPRDATA rdPtr, long param1, long param2) {
	return true;
}

long WINAPI DLLExport Condition_LoginSuccess(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->bLoginSuccess;
}

long WINAPI DLLExport Condition_QueryComplete(LPRDATA rdPtr, long param1, long param2) {
	bool bComplete = true;

	bComplete = bComplete && rdPtr->pData->pEOSAchievement->QueryComplete();
	bComplete = bComplete && rdPtr->pData->pEOSStat->QueryComplete();
	bComplete = bComplete && rdPtr->pData->pEOSPresence->QueryComplete();

	return bComplete;
}


// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

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
	rdPtr->pData->EOSUpdatePlatform();

	return 0;
}


// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Expression_StatValue(LPRDATA rdPtr, long param1) {
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

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action_Achievement_Unlock,
			Action_Stat_Ingest,
			Action_Query,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression_StatValue,
			Expression_GetAccountID,
			Expression_GetProductUserID,

			0
			};