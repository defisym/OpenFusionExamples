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
		0,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_EN, M_ACTION_EN, ACT_ACTION_EN,	0, 3,PARAM_FILENAME2,PARAM_FILENAME2,PARAM_EXPSTRING,M_ACT_SRC,M_ACT_DES,M_ACT_KEY,
		IDMN_ACTION_DE, M_ACTION_DE, ACT_ACTION_DE,	0, 3,PARAM_FILENAME2,PARAM_FILENAME2,PARAM_EXPSTRING,M_ACT_SRC,M_ACT_DES,M_ACT_KEY,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		0,
		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport Encrypt(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR pSrcFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR pDesFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR pKey = (LPCTSTR)CNC_GetStringParameter(rdPtr);
    
	EncryptCore(pSrcFilePath, pDesFilePath, pKey, ENCRY);

	return 0;
}

short WINAPI DLLExport Decrypt(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR pSrcFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR pDesFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR pKey = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	EncryptCore(pSrcFilePath, pDesFilePath, pKey, DECRY);

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

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
			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Encrypt,
			Decrypt,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			0
			};