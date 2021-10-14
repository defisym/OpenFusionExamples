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
		IDMN_CONDITION_OF, M_CONDITION_OF, CND_CONDITION_OF, 0, 1, PARAM_EXPSTRING, M_CND_FUNCNAME,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_SR, M_ACTION_SR, ACT_ACTION_SR,	0, 1,PARAM_EXPSTRING, M_ACT_RET,
		IDMN_ACTION_PR, M_ACTION_PR, ACT_ACTION_PR,	0, 1,PARAM_EXPSTRING, M_ACT_RET,

		IDMN_ACTION_CF, M_ACTION_CF, ACT_ACTION_CF,	0, 2,PARAM_EXPSTRING,PARAM_EXPSTRING, M_CND_FUNCNAME, M_EXP_PARAM,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_RV, M_EXPRESSION_RV, EXP_EXPRESSION_RV, 0, 2, EXPPARAM_STRING, EXPPARAM_STRING,M_CND_FUNCNAME, M_EXP_PARAM,
		IDMN_EXPRESSION_RS, M_EXPRESSION_RS, EXP_EXPRESSION_RS, EXPFLAG_STRING,2,EXPPARAM_STRING, EXPPARAM_STRING,M_CND_FUNCNAME, M_EXP_PARAM,

		IDMN_EXPRESSION_GVP, M_EXPRESSION_GVP, EXP_EXPRESSION_GVP, 0, 1, EXPPARAM_LONG,M_EXP_GP,
		IDMN_EXPRESSION_GSP, M_EXPRESSION_GSP, EXP_EXPRESSION_GSP, EXPFLAG_STRING, 1, EXPPARAM_LONG,M_EXP_GP,

		IDMN_EXPRESSION_GVR, M_EXPRESSION_GVR, EXP_EXPRESSION_GVR, 0, 1, EXPPARAM_LONG,M_EXP_GP,
		IDMN_EXPRESSION_GSR, M_EXPRESSION_GSR, EXP_EXPRESSION_GSR, EXPFLAG_STRING, 1, EXPPARAM_LONG,M_EXP_GP,

		IDMN_EXPRESSION_GRI, M_EXPRESSION_GRI, EXP_EXPRESSION_GRI, 0, 0,

		IDMN_EXPRESSION_GPS, M_EXPRESSION_GPS, EXP_EXPRESSION_GPS, 0, 0,
		IDMN_EXPRESSION_GRS, M_EXPRESSION_GRS, EXP_EXPRESSION_GRS, 0, 0,

		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport OnFunc(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	return StrEqu(FuncName, rdPtr->FuncNameStack->back().c_str()) ? TRUE : FALSE;
}


// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport SetReturnValue(LPRDATA rdPtr, long param1, long param2) {
	std::wstring Return = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	rdPtr->FuncReturn->clear();
	rdPtr->FuncReturn->emplace_back(Return);

	return 0;
}

short WINAPI DLLExport PushReturnValue(LPRDATA rdPtr, long param1, long param2) {
	std::wstring Return = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	rdPtr->FuncReturn->emplace_back(Return);
	
	return 0;
}

short WINAPI DLLExport CallFunc(LPRDATA rdPtr, long param1, long param2) {
	std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	rdPtr->FuncNameStack->emplace_back(FuncName);

	std::wstring Param = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	rdPtr->FuncParamStack->emplace_back();
	UpdateParam(rdPtr, Param);

	rdPtr->FuncReturn->clear();

	//Call Func;
	CallEvent(ONFUNC);

	rdPtr->FuncNameStack->pop_back();
	rdPtr->FuncParamStack->pop_back();

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport CallFuncRV(LPRDATA rdPtr,long param1) {
	std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	rdPtr->FuncNameStack->emplace_back(FuncName);

	std::wstring Param = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);	
	rdPtr->FuncParamStack->emplace_back();	
	UpdateParam(rdPtr, Param);

	rdPtr->FuncReturn->clear();

	//Call Func;
	CallEvent(ONFUNC);

	rdPtr->FuncNameStack->pop_back();
	rdPtr->FuncParamStack->pop_back();

	if (StrIsNum(Return(0))) {
		float Val = std::stof(Return(0));

		if (Val == (int)Val) {			
			return (int)Val;
		}
		else {
			//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
			rdPtr->rHo.hoFlags |= HOF_FLOAT;

			//Return the float without conversion
			return *((long*)&Val);
		}
	}
	else {
		return 0;
	}
}

long WINAPI DLLExport CallFuncRS(LPRDATA rdPtr,long param1) {
	std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	rdPtr->FuncNameStack->emplace_back(FuncName);

	std::wstring Param = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
	rdPtr->FuncParamStack->emplace_back();
	UpdateParam(rdPtr, Param);

	rdPtr->FuncReturn->clear();

	//Call Func;
	CallEvent(ONFUNC);

	rdPtr->FuncNameStack->pop_back();
	rdPtr->FuncParamStack->pop_back();	
	
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;
	
	//This returns a pointer to the string for MMF.
	return (long)Return(0).c_str();	
}

long WINAPI DLLExport GetParamRV(LPRDATA rdPtr, long param1) {
	size_t Pos = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	Pos = max(Pos, min(Pos, rdPtr->FuncParamStack->back().size() - 1));
		
	if (StrIsNum(Param(Pos))) {
		float  Val = std::stof(Param(Pos));
		if (Val == (int)Val) {
			return (int)Val;
		}
		else {
			//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
			rdPtr->rHo.hoFlags |= HOF_FLOAT;

			//Return the float without conversion
			return *((long*)&Val);
		}
	}
	else {
		return 0;
	}
}

long WINAPI DLLExport GetParamRS(LPRDATA rdPtr, long param1) {
	size_t Pos = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	Pos = max(Pos, min(Pos, rdPtr->FuncParamStack->back().size() - 1));

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)Param(Pos).c_str();
}

long WINAPI DLLExport GetRetRV(LPRDATA rdPtr, long param1) {
	size_t Pos = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	Pos = max(Pos, min(Pos, rdPtr->FuncReturn->size() - 1));

	if (StrIsNum(Return(Pos))) {
		float  Val = std::stof(Return(Pos));
		if (Val == (int)Val) {
			return (int)Val;
		}
		else {
			//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
			rdPtr->rHo.hoFlags |= HOF_FLOAT;

			//Return the float without conversion
			return *((long*)&Val);
		}
	}
	else {
		return 0;
	}
}

long WINAPI DLLExport GetRetRS(LPRDATA rdPtr, long param1) {
	size_t Pos = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	Pos = max(Pos, min(Pos, rdPtr->FuncReturn->size() - 1));

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)Return(Pos).c_str();
}

long WINAPI DLLExport GetRecursiveIndex(LPRDATA rdPtr, long param1) {
	size_t RecursiveIndex = 0;

	for (auto& it : *(rdPtr->FuncNameStack)) {
		if (it == rdPtr->FuncNameStack->back()) {
			RecursiveIndex++;
		}
	}
	
	return RecursiveIndex;
}

long WINAPI DLLExport GetParamSize(LPRDATA rdPtr, long param1) {	
	return rdPtr->FuncParamStack->back().size();
}

long WINAPI DLLExport GetRetSize(LPRDATA rdPtr, long param1) {
	return rdPtr->FuncReturn->size();
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
			OnFunc,

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			SetReturnValue,
			PushReturnValue,

			CallFunc,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			CallFuncRV,
			CallFuncRS,

			GetParamRV,
			GetParamRS,

			GetRetRV,
			GetRetRS,

			GetRecursiveIndex,

			GetParamSize,
			GetRetSize,

			0
			};