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
		IDMN_CONDITION_ISEMBED, M_CONDITION_ISEMBED, CND_CONDITION_ISEMBED, EVFLAGS_ALWAYS, 1, PARAM_EXPSTRING, M_ACTION_EXTRACTBINARYFILE_SRC,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_BINARYFILEEXTRACT_SETTINGS, M_ACTION_BINARYFILEEXTRACT_SETTINGS,	ACT_ACTION_BINARYFILEEXTRACT_SETTINGS,	0, 2, PARAM_EXPSTRING,PARAM_EXPSTRING, M_ACTION_BINARYFILEEXTRACT_DIR, M_ACTION_BINARYFILEEXTRACT_NAME,
		IDMN_ACTION_EXTRACTBINARYFILE, M_ACTION_EXTRACTBINARYFILE,	ACT_ACTION_EXTRACTBINARYFILE,	0, 1, PARAM_EXPSTRING, M_ACTION_EXTRACTBINARYFILE_SRC,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GETBINARYFILEPATH, M_EXPRESSION_GETBINARYFILEPATH, EXP_EXPRESSION_GETBINARYFILEPATH, EXPFLAG_STRING, 1, EXPPARAM_STRING, M_ACTION_EXTRACTBINARYFILE_SRC,
		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Condition_IsEmbed(LPRDATA rdPtr, long param1, long param2) {
	return TRUE;
}


// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport Action_BinaryFileExtractSetings(LPRDATA rdPtr, long param1, long param2) {
	return 0;
}
short WINAPI DLLExport Action_ExtractBinaryFile(LPRDATA rdPtr, long param1, long param2) {
	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Expression_GetBinaryFilePath(LPRDATA rdPtr, long param1) {
	LPCTSTR DataType = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)WindowsDummyStr;
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
			Condition_IsEmbed,
			
			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action_BinaryFileExtractSetings,
			Action_ExtractBinaryFile,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression_GetBinaryFilePath,

			0
			};