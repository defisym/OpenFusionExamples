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
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_ACH_UL, M_ACTION_ACH_UL, ACT_ACTION_ACH_UL,	0, 1, PARAM_EXPSTRING, M_ACH_NAME,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_LOGINSUCCESS, M_EXPRESSION_LOGINSUCCESS, EXP_EXPRESSION_LOGINSUCCESS, 0, 0,
		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Condition_OnLogin(LPRDATA rdPtr, long param1, long param2) {
	return true;
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


// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Expression_LoginSuccess(LPRDATA rdPtr,long param1) {
	return rdPtr->bLoginSuccess;
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

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action_Achievement_Unlock,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression_LoginSuccess,

			0
			};