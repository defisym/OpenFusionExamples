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
		0
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_NF, M_ACTION_NF,	ACT_ACTION_NF,	0, 0,
		IDMN_ACTION_DF, M_ACTION_DF,	ACT_ACTION_DF,	0, 0,
		IDMN_ACTION_LF, M_ACTION_LF,	ACT_ACTION_LF,	0, 1,PARAM_FILENAME2,ACT_ACTION_F,
		IDMN_ACTION_SF, M_ACTION_SF,	ACT_ACTION_SF,	0, 1,PARAM_FILENAME2,ACT_ACTION_F,
		IDMN_ACTION_SSIV, M_ACTION_SSIV,	ACT_ACTION_SSIV,	0, 3,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPRESSION,ACT_ACTION_SSI_S,ACT_ACTION_SSI_I,ACT_ACTION_SSI_VAL,
		IDMN_ACTION_SSIS, M_ACTION_SSIS,	ACT_ACTION_SSIS,	0, 3,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPSTRING,ACT_ACTION_SSI_S,ACT_ACTION_SSI_I,ACT_ACTION_SSI_STR,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GSIV, M_EXPRESSION_GSIV, EXP_EXPRESSION_GSIV, 0, 2, EXPPARAM_STRING, EXPPARAM_STRING, ACT_ACTION_SSI_S, ACT_ACTION_SSI_I,
		IDMN_EXPRESSION_GSIS, M_EXPRESSION_GSIS, EXP_EXPRESSION_GSIS, EXPFLAG_STRING, 2, EXPPARAM_STRING, EXPPARAM_STRING, ACT_ACTION_SSI_S, ACT_ACTION_SSI_I,
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

short WINAPI DLLExport New(LPRDATA rdPtr, long param1, long param2) {
	Init();

	return 0;
}

short WINAPI DLLExport Release(LPRDATA rdPtr, long param1, long param2) {
	if (!valid()) {
		return 0;
	}
		
	release();

	return 0;
}

short WINAPI DLLExport LoadFromFile(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)param1;
	Init();

	Fini->load(FilePath);

	return 0;
}

short WINAPI DLLExport SaveToFile(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)param1;
	if (!valid()) {
		return 0;
	}
	
	Fini->save(FilePath);

	return 0;
}

short WINAPI DLLExport SaveSecItem_Value(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR Section = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Item = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	
	long p = CNC_GetFloatParameter(rdPtr);
	double Value = *(float*)&p;
	
	if (!valid()) {
		return 0;
	}

	(*Fini)[_B(Section)][_B(Item)] = Value;	

	return 0;
}

short WINAPI DLLExport SaveSecItem_String(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR Section = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Item = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	std::wstring String = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	if (!valid()) {
		return 0;
	}

	(*Fini)[_B(Section)][_B(Item)] = String;

	return 0;
}



short WINAPI DLLExport Action(LPRDATA rdPtr, long param1, long param2) {
	//LPTSTR t = _T("´óÐÍGundam");
	//WTBC c;

	//ini::IniFile* myIni2 = new ini::IniFile;
	//ini::IniFile myIni;
	//std::string a = "abc";

	//myIni.load(t);
	//myIni2->load("qsave.ini");
	//std::wstring myStr = (*myIni2)["string"]["talk"].as<std::wstring>();
	//delete  myIni2;
	//std::wstring a2 = t;
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

long WINAPI DLLExport GeySecItem_Value(LPRDATA rdPtr, long param1) {
	LPCTSTR Section = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	LPCTSTR Item = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	float Val = (*Fini)[_B(Section)][_B(Item)].as<float>();
	//int Val = (*Fini)[_B(Section)][_B(Item)].as<int>();
	
	//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	
	//Return the float without conversion
	return *((int*)&Val);
}

long WINAPI DLLExport GeySecItem_String(LPRDATA rdPtr, long param1) {
	LPCTSTR Section = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	LPCTSTR Item = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
	
	NewStr((*Fini)[_B(Section)][_B(Item)].as<std::wstring>());

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;
	
	//This returns a pointer to the string for MMF.
	return (long)rdPtr->Str;
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
			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			New,
			Release,
			LoadFromFile,
			SaveToFile,
			SaveSecItem_Value,
			SaveSecItem_String,		
			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			GeySecItem_Value,
			GeySecItem_String,
			0
			};