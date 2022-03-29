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
		IDMN_ACTION_GRT, M_ACTION_GRT, ACT_ACTION_GRT, 0, 0,
		IDMN_ACTION_GFB64, M_ACTION_GFB64, ACT_ACTION_GFB64, 0, 1, PARAM_EXPSTRING, M_ACTION_GFB64_P1,
		IDMN_ACTION_CRN, M_ACTION_CRN, ACT_ACTION_CRN, 0, 0,
		IDMN_ACTION_CNRN, M_ACTION_CNRN, ACT_ACTION_CNRN, 0, 1, PARAM_EXPRESSION, M_ACTION_CNRN_P1,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GRN, M_EXPRESSION_GRN, EXP_EXPRESSION_GRN, 0, 0,
		IDMN_EXPRESSION_SRN, M_EXPRESSION_SRN, EXP_EXPRESSION_SRN, 0, 1, EXPPARAM_LONG, M_EXPRESSION_SRN_P1,
		IDMN_EXPRESSION_S2B64, M_EXPRESSION_S2B64, EXP_EXPRESSION_S2B64, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GS, M_EXPRESSION_GS, EXP_EXPRESSION_GS, 0, 0,
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

// Generate Random Table
short WINAPI DLLExport GenerateRandomTable(LPRDATA rdPtr, long param1, long param2) {
	for (auto& it : *rdPtr->pRandomTable) {
		it = rdPtr->pRand->GenerateRandNumber();
	}

	return 0;	
}

// Generate Random Table From Base64
short WINAPI DLLExport GenerateFromBase64(LPRDATA rdPtr, long param1, long param2) {
	LPWSTR Input = (LPWSTR)param1;

	rdPtr->pBase64->base64_decode_to_pointer(Input, rdPtr->pBuffer, rdPtr->maxSize);

	for (size_t i = 0; i < rdPtr->maxSize; i++) {
		(*rdPtr->pRandomTable)[i] = (rdPtr->pBuffer[i]);
	}

	return 0;
}

// Consume a Random Number
short WINAPI DLLExport ConsumeRandomNumber(LPRDATA rdPtr, long param1, long param2) {
	GetRandomNumber(rdPtr);

	return 0;
}

// Consume N Random Number
short WINAPI DLLExport ConsumeNRandomNumber(LPRDATA rdPtr, long param1, long param2) {
	size_t num = (size_t)param1;
	
	for (size_t i = 0; i < num; i++) {
		GetRandomNumber(rdPtr);
	}

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

// Get Random Number
long WINAPI DLLExport GetRandomNumber(LPRDATA rdPtr,long param1) {
	return GetRandomNumber(rdPtr);;
}

long WINAPI DLLExport ShowRandomNumber(LPRDATA rdPtr, long param1) {	
	size_t p1 = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	size_t pos = min(max(0, p1), (size_t)(rdPtr->maxSize-1));

	return (*rdPtr->pRandomTable)[pos];
}

// Save to Base64
long WINAPI DLLExport SavetoBase64(LPRDATA rdPtr, long param1) {
	for (size_t i = 0; i < rdPtr->maxSize; i++) {
		(rdPtr->pBuffer[i]) = (*rdPtr->pRandomTable)[i];
	}

	*rdPtr->pBase64Str = rdPtr->pBase64->base64_encode(rdPtr->pBuffer, rdPtr->maxSize);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.	
	return (long)rdPtr->pBase64Str->c_str();
}

long WINAPI DLLExport GetRandomTableSize(LPRDATA rdPtr, long param1) {	
	return rdPtr->maxSize;
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
			//Condition,
			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			GenerateRandomTable,
			GenerateFromBase64,
			ConsumeRandomNumber,
			ConsumeNRandomNumber,
			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			GetRandomNumber,
			ShowRandomNumber,
			SavetoBase64,
			GetRandomTableSize,
			0
			};