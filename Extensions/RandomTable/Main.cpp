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
		//IDMN_CONDITION, M_CONDITION, CND_CONDITION, EVFLAGS_ALWAYS, 3, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_CND_P1, M_CND_P2, M_CND_P3,
		0,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{		
		IDMN_ACTION_GRT, M_ACTION_GRT,ACT_ACTION_GRT,0, 0,
		IDMN_ACTION_GFB64, M_ACTION_GFB64,ACT_ACTION_GFB64,	0, 1,PARAM_EXPSTRING,M_ACTION_GFB64_P1,
		IDMN_ACTION_CRN, M_ACTION_CRN,ACT_ACTION_CRN,0, 0,
		IDMN_ACTION_CNRN, M_ACTION_CNRN,ACT_ACTION_CNRN,0, 1,PARAM_EXPRESSION,M_ACTION_CNRN_P1,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		//IDMN_EXPRESSION, M_EXPRESSION, EXP_EXPRESSION, 0, 3, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, 0, 0, 0,
		//
		////Note in the following.  If you are returning a string, you set the EXPFLAG_STRING.	
		//IDMN_EXPRESSION2, M_EXPRESSION2, EXP_EXPRESSION2, EXPFLAG_STRING, 1, EXPPARAM_STRING, 0,
		//
		////Note in the following.  If you are returning a float, you set the EXPFLAG_DOUBLE
		//IDMN_EXPRESSION3, M_EXPRESSION3, EXP_EXPRESSION3, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG, 0,

		IDMN_EXPRESSION_GRN, M_EXPRESSION_GRN, EXP_EXPRESSION_GRN, 0,0,
		IDMN_EXPRESSION_SRN, M_EXPRESSION_SRN, EXP_EXPRESSION_SRN, 0,1,EXPPARAM_LONG,M_EXPRESSION_SRN_P1,
		IDMN_EXPRESSION_S2B64, M_EXPRESSION_S2B64, EXP_EXPRESSION_S2B64, EXPFLAG_STRING,0,
		IDMN_EXPRESSION_GS, M_EXPRESSION_GS, EXP_EXPRESSION_GS, 0,0,
		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

// -----------------
// Sample Condition
// -----------------
// Returns TRUE when the two values are equal!
// 

//long WINAPI DLLExport Condition(LPRDATA rdPtr, long param1, long param2)
//{
//
////  **** Still use this method for 1 or 2 parameters ****	
////	if (param1==param2)	
////		return TRUE;
//
//	long p1 = CNC_GetParameter(rdPtr);
//	long p2 = CNC_GetParameter(rdPtr);
//	long p3 = CNC_GetParameter(rdPtr);
//
//	if ((p1 + p2)==p3)
//		return TRUE;
//		 
//	return FALSE;
//}


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
//short WINAPI DLLExport Action(LPRDATA rdPtr, long param1, long param2)
//{
//	// Actions work just like Conditions
//
//	// Use directly param1 and/or param2 if this action has 1 or 2 parameters
//
//	// Use this if this action has 3 parameters or more
////	long p1 = CNC_GetParameter(rdPtr);
////	long p2 = CNC_GetParameter(rdPtr);
////	long p3 = CNC_GetParameter(rdPtr);
////	etc.
//
//	return 0;
//}

// Generate Random Table
short WINAPI DLLExport GenerateRandomTable(LPRDATA rdPtr, long param1, long param2)
{
	for (auto t = RandomTable.begin(); t != RandomTable.end(); t++) {
		*t = GenerateRandom();
	}

	return 0;	
}

// Generate Random Table From Base64
short WINAPI DLLExport GenerateFromBase64(LPRDATA rdPtr, long param1, long param2)
{
	LPWSTR Input = (LPWSTR)param1;
	DWORD dwNeed = rdPtr->MaxSize;

	CryptStringToBinary(Input, 0, CRYPT_STRING_BASE64, Buffer, &dwNeed, NULL, NULL);

	for (auto t = 0; t != rdPtr->MaxSize; t++) {
		RandomTable[t] = Buffer[t];
	}

	return 0;
}

// Consume a Random Number
short WINAPI DLLExport ConsumeRandomNumber(LPRDATA rdPtr, long param1, long param2)
{
	_GetRandomNumber();
	return 0;
}

// Consume N Random Number
short WINAPI DLLExport ConsumeNRandomNumber(LPRDATA rdPtr, long param1, long param2)
{
	int t = (int)param1;
	while (t != 0) {
		_GetRandomNumber();
		t--;
	}
	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

// -----------------
// Sample expression
// -----------------
// Add three values
// 
//long WINAPI DLLExport Expression(LPRDATA rdPtr,long param1)
//{
//
//	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
//	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
//	long p3 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
//
//	// Performs the wonderfull calculation
//	return p1+p2+p3;
//}
//
//
////Reverse the string passed in.
//long WINAPI DLLExport Expression2(LPRDATA rdPtr,long param1)
//{
//	char *temp;
//
//	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
//
//	//I'm storing the string pointer returned into a char *
//	temp = (LPSTR)p1;
//
//	//Reversing the string.
//	_strrev(temp);
//	
//	//Setting the HOF_STRING flag lets MMF know that you are a string.
//	rdPtr->rHo.hoFlags |= HOF_STRING;
//	
//	//This returns a pointer to the string for MMF.
//	return (long)temp;
//}
//
////Divide the float by 2.
//long WINAPI DLLExport Expression3(LPRDATA rdPtr,long param1)
//{
//	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
//
//	//Floats are tricky.  If you want to pass in a float, you must do the
//	//following to convert the long to a true float, but only when you use
//	//TYPE_FLOAT.
//	float fp1 = *(float *)&p1;
//
//	//Just doing simple math now.
//	fp1 /=2;
//
//	//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
//	rdPtr->rHo.hoFlags |= HOF_FLOAT;
//
//	//Return the float without conversion
//	return *((int*)&fp1);
//}

// Get Random Number
long WINAPI DLLExport GetRandomNumber(LPRDATA rdPtr,long param1)
{
	return _GetRandomNumber();
}

long WINAPI DLLExport ShowRandomNumber(LPRDATA rdPtr, long param1)
{	
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	int Pos = min(max(0, p1), (int)(rdPtr->MaxSize)-1);
	return (int)RandomTable[Pos];
}

// Save to Base64
long WINAPI DLLExport SavetoBase64(LPRDATA rdPtr, long param1)
{
	for (auto t = 0; t != rdPtr->MaxSize; t++) {
		Buffer[t] = RandomTable[t];
	}

	DWORD Size= StrLength;
	CryptBinaryToString(Buffer, rdPtr->MaxSize, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, lpBase64Str, &Size);
	
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.	
	return (long)lpBase64Str;
}

long WINAPI DLLExport GetRandomTableSize(LPRDATA rdPtr, long param1)
{	
	return rdPtr->MaxSize;
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