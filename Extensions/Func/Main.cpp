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
		
		IDMN_CONDITION_FHPA, M_CONDITION_FHPA, CND_CONDITION_FHPA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPRESSION, M_EXP_GP,
		IDMN_CONDITION_FHRA, M_CONDITION_FHRA, CND_CONDITION_FHRA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPRESSION, M_EXP_GP,
		
		IDMN_CONDITION_FHTP, M_CONDITION_FHTP, CND_CONDITION_FHTP, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_CND_FUNCNAME, M_ACT_PARAMNAME,
		IDMN_CONDITION_CFHTP, M_CONDITION_CFHTP, CND_CONDITION_CFHTP, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPSTRING, M_CND_FUNCNAME,

		IDMN_CONDITION_FPAIN, M_CONDITION_FPAIN, CND_CONDITION_FPAIN, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPRESSION, M_EXP_GP,
		IDMN_CONDITION_FRAIN, M_CONDITION_FRAIN, CND_CONDITION_FRAIN, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPRESSION, M_EXP_GP,

		IDMN_CONDITION_FTPIN, M_CONDITION_FTPIN, CND_CONDITION_FTPIN, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_CND_FUNCNAME, M_ACT_PARAMNAME,
		IDMN_CONDITION_CFTPIN, M_CONDITION_CFTPIN, CND_CONDITION_CFTPIN, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPSTRING, M_CND_FUNCNAME,

		IDMN_CONDITION_OITO, M_CONDITION_OITO, CND_CONDITION_OITO, 0, 2, PARAM_OBJECT, PARAM_EXPSTRING, M_OBJECT, M_ITNAME,
		IDMN_CONDITION_SA, M_CONDITION_SA, CND_CONDITION_SA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_OBJECT, M_OBJECT,

		IDMN_CONDITION_IE, M_CONDITION_IE, CND_CONDITION_IE, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		IDMN_CONDITION_IS, M_CONDITION_IS, CND_CONDITION_IS, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,

		IDMN_CONDITION_OILHO, M_CONDITION_OILHO, CND_CONDITION_OILHO, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPSTRING, M_CND_OBJNAME,
		
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_SR, M_ACTION_SR, ACT_ACTION_SR,	0, 1,PARAM_EXPSTRING, M_ACT_RET,
		IDMN_ACTION_PR, M_ACTION_PR, ACT_ACTION_PR,	0, 1,PARAM_EXPSTRING, M_ACT_RET,

		IDMN_ACTION_CF, M_ACTION_CF, ACT_ACTION_CF,	0, 3,PARAM_EXPSTRING,PARAM_EXPSTRING, PARAM_EXPRESSION, M_CND_FUNCNAME, M_EXP_PARAM, M_ACT_LOOP,
		
		IDMN_ACTION_T, M_ACTION_T, ACT_ACTION_T,	0, 5,PARAM_EXPRESSION,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPSTRING,M_EXP_BOOL, M_CND_FUNCNAME, M_EXP_PARAM, M_CND_FUNCNAME, M_EXP_PARAM,

		IDMN_ACTION_STV, M_ACTION_STV, ACT_ACTION_STV,	0, 3,PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPSTRING, M_CND_FUNCNAME, M_ACT_PARAMNAME, M_ACT_PARAM,
		IDMN_ACTION_SCTV, M_ACTION_SCTV, ACT_ACTION_SCTV,	0, 2,PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACT_PARAMNAME, M_ACT_PARAM,

		IDMN_ACTION_SAR, M_ACTION_SAR, ACT_ACTION_SAR,	0, 1,PARAM_EXPSTRING, M_ACT_RETA,

		IDMN_ACTION_SLI, M_ACTION_SLI, ACT_ACTION_SLI,	0, 2,PARAM_EXPSTRING, PARAM_EXPRESSION, M_CND_FUNCNAME, M_ACT_LOOPINDEX,
		IDMN_ACTION_SL, M_ACTION_SL, ACT_ACTION_SL,	0, 1, PARAM_EXPSTRING, M_CND_FUNCNAME,

		IDMN_ACTION_PASSRET, M_ACTION_PASSRET, ACT_ACTION_PASSRET,	0, 1,PARAM_EXPSTRING, M_ACT_RET,
		IDMN_ACTION_PASSRETV, M_ACTION_PASSRETV, ACT_ACTION_PASSRETV,	0, 1,PARAM_EXPRESSION, M_ACT_RET,

		IDMN_ACTION_STVV, M_ACTION_STVV, ACT_ACTION_STVV,	0, 3,PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPRESSION, M_CND_FUNCNAME, M_ACT_PARAMNAME, M_ACT_PARAM,
		IDMN_ACTION_SCTVV, M_ACTION_SCTVV, ACT_ACTION_SCTVV,	0, 2,PARAM_EXPSTRING, PARAM_EXPRESSION, M_ACT_PARAMNAME, M_ACT_PARAM,

		IDMN_ACTION_SRV, M_ACTION_SRV, ACT_ACTION_SRV,	0, 1,PARAM_EXPRESSION, M_ACT_RET,
		IDMN_ACTION_PRV, M_ACTION_PRV, ACT_ACTION_PRV,	0, 1,PARAM_EXPRESSION, M_ACT_RET,

		IDMN_ACTION_ITO, M_ACTION_ITO, ACT_ACTION_ITO, 0, 2, PARAM_OBJECT, PARAM_EXPSTRING, M_OBJECT, M_ITNAME,

		IDMN_ACTION_SSGP, M_ACTION_SSGP, ACT_ACTION_SSGP, 0, 2,PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACT_PARAMNAME, M_ACT_PARAM,
		IDMN_ACTION_SVGP, M_ACTION_SVGP, ACT_ACTION_SVGP, 0, 2,PARAM_EXPSTRING, PARAM_EXPRESSION, M_ACT_PARAMNAME, M_ACT_PARAM,
		
		IDMN_ACTION_ASSERT, M_ACTION_ASSERT, ACT_ACTION_ASSERT, 0, 2,PARAM_EXPRESSION, PARAM_EXPSTRING, M_EXP_CTB, M_ACT_MSG,
		IDMN_ACTION_MSGBOX, M_ACTION_MSGBOX, ACT_ACTION_MSGBOX, 0, 2,PARAM_EXPRESSION, PARAM_EXPSTRING, M_EXP_CTB, M_ACT_MSG,
		IDMN_ACTION_TOAST, M_ACTION_TOAST, ACT_ACTION_TOAST, 0, 3,PARAM_EXPRESSION, PARAM_EXPSTRING, PARAM_EXPSTRING, M_EXP_CTB, M_ACT_TITLE, M_ACT_MSG,
		IDMN_ACTION_TOASTFLAG, M_ACTION_TOASTFLAG, ACT_ACTION_TOASTFLAG, 0, 1, PARAM_EXPRESSION, M_ACT_TOASTFLAG,

		IDMN_ACTION_SOA, M_ACTION_SOA, ACT_ACTION_SOA, 0, 2, PARAM_OBJECT, PARAM_EXPRESSION, M_OBJECT, M_ACT_ALPHA,
		IDMN_ACTION_SOABF, M_ACTION_SOABF, ACT_ACTION_SOABF, 0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_OBJECT, M_ACT_ALPHA,

		IDMN_ACTION_SORC, M_ACTION_SORC, ACT_ACTION_SORC, 0, 2, PARAM_OBJECT, PARAM_EXPRESSION, M_OBJECT, M_ACT_RGBCOEF,
		IDMN_ACTION_SORCBF, M_ACTION_SORCBF, ACT_ACTION_SORCBF, 0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_OBJECT, M_ACT_RGBCOEF,

		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		//CallFunc
		IDMN_EXPRESSION_RV, M_EXPRESSION_RV, EXP_EXPRESSION_RV, 0, 2, EXPPARAM_STRING, EXPPARAM_STRING,M_CND_FUNCNAME, M_EXP_PARAM,
		IDMN_EXPRESSION_RS, M_EXPRESSION_RS, EXP_EXPRESSION_RS, EXPFLAG_STRING,2,EXPPARAM_STRING, EXPPARAM_STRING,M_CND_FUNCNAME, M_EXP_PARAM,

		//Get Param
		IDMN_EXPRESSION_GVP, M_EXPRESSION_GVP, EXP_EXPRESSION_GVP, 0, 1, EXPPARAM_LONG, M_EXP_GP,
		IDMN_EXPRESSION_GSP, M_EXPRESSION_GSP, EXP_EXPRESSION_GSP, EXPFLAG_STRING, 1, EXPPARAM_LONG, M_EXP_GP,

		//Get Ret
		IDMN_EXPRESSION_GVR, M_EXPRESSION_GVR, EXP_EXPRESSION_GVR, 0, 1, EXPPARAM_LONG, M_EXP_GP,
		IDMN_EXPRESSION_GSR, M_EXPRESSION_GSR, EXP_EXPRESSION_GSR, EXPFLAG_STRING, 1, EXPPARAM_LONG, M_EXP_GP,

		//GetRecursiveIndex
		IDMN_EXPRESSION_GRI, M_EXPRESSION_GRI, EXP_EXPRESSION_GRI, 0, 1, EXPPARAM_STRING, M_CND_FUNCNAME,

		//GetSize
		IDMN_EXPRESSION_GPS, M_EXPRESSION_GPS, EXP_EXPRESSION_GPS, 0, 0,
		IDMN_EXPRESSION_GRS, M_EXPRESSION_GRS, EXP_EXPRESSION_GRS, 0, 0,
		
		//Ternary
		IDMN_EXPRESSION_TRV, M_EXPRESSION_TRV, EXP_EXPRESSION_TRV, 0, 3,EXPPARAM_LONG,EXPPARAM_LONG,EXPPARAM_LONG,M_EXP_BOOL,M_EXP_RETA,M_EXP_RETB,
		IDMN_EXPRESSION_TRS, M_EXPRESSION_TRS, EXP_EXPRESSION_TRS, EXPFLAG_STRING, 3,EXPPARAM_LONG,EXPPARAM_STRING,EXPPARAM_STRING,M_EXP_BOOL,M_EXP_RETA,M_EXP_RETB,

		//Get Temp Param
		IDMN_EXPRESSION_GVTP, M_EXPRESSION_GVTP, EXP_EXPRESSION_GVTP, 0, 2, EXPPARAM_STRING, EXPPARAM_STRING,M_CND_FUNCNAME, M_EXP_TEMPPARAM,
		IDMN_EXPRESSION_GSTP, M_EXPRESSION_GSTP, EXP_EXPRESSION_GSTP, EXPFLAG_STRING,2,EXPPARAM_STRING, EXPPARAM_STRING,M_CND_FUNCNAME, M_EXP_TEMPPARAM,
		
		//Get Current FuncName
		IDMN_EXPRESSION_GCFN, M_EXPRESSION_GCFN, EXP_EXPRESSION_GCFN, EXPFLAG_STRING, 0,

		//Get Current Temp Param
		IDMN_EXPRESSION_GVCTP, M_EXPRESSION_GVCTP, EXP_EXPRESSION_GVCTP, 0, 1, EXPPARAM_STRING,  M_EXP_TEMPPARAM,
		IDMN_EXPRESSION_GSCTP, M_EXPRESSION_GSCTP, EXP_EXPRESSION_GSCTP, EXPFLAG_STRING,1, EXPPARAM_STRING, M_EXP_TEMPPARAM,

		//GetLoopIndex
		IDMN_EXPRESSION_GLI, M_EXPRESSION_GLI, EXP_EXPRESSION_GLI, 0, 1,EXPPARAM_STRING,M_CND_FUNCNAME,

		//Get Global Param
		IDMN_EXPRESSION_GVGP, M_EXPRESSION_GVGP, EXP_EXPRESSION_GVGP, 0, 1, EXPPARAM_STRING, M_ACT_PARAMNAME,
		IDMN_EXPRESSION_GSGP, M_EXPRESSION_GSGP, EXP_EXPRESSION_GSGP, EXPFLAG_STRING, 1, EXPPARAM_STRING, M_ACT_PARAMNAME,

		//Equ
		IDMN_EXPRESSION_ES, M_EXPRESSION_ES, EXP_EXPRESSION_ES, 0, 2, EXPPARAM_STRING, EXPPARAM_STRING, M_EXP_A, M_EXP_B,
		IDMN_EXPRESSION_EV, M_EXPRESSION_EV, EXP_EXPRESSION_EV, 0, 2, EXPPARAM_LONG, EXPPARAM_LONG, M_EXP_A, M_EXP_B,
		
		//Bool
		IDMN_EXPRESSION_CTB, M_EXPRESSION_CTB, EXP_EXPRESSION_CTB, 0, 1, EXPPARAM_LONG, M_EXP_CTB,
		IDMN_EXPRESSION_NEG, M_EXPRESSION_NEG, EXP_EXPRESSION_NEG, 0, 1, EXPPARAM_LONG, M_EXP_NEG,
		IDMN_EXPRESSION_AND, M_EXPRESSION_AND, EXP_EXPRESSION_AND, 0, 2, EXPPARAM_LONG, EXPPARAM_LONG, M_EXP_A, M_EXP_B,
		IDMN_EXPRESSION_OR, M_EXPRESSION_OR, EXP_EXPRESSION_OR, 0, 2, EXPPARAM_LONG, EXPPARAM_LONG, M_EXP_A, M_EXP_B,
		IDMN_EXPRESSION_XOR, M_EXPRESSION_XOR, EXP_EXPRESSION_XOR, 0, 2, EXPPARAM_LONG, EXPPARAM_LONG, M_EXP_A, M_EXP_B,

		//Equ_2
		IDMN_EXPRESSION_GV, M_EXPRESSION_GV, EXP_EXPRESSION_GV, 0, 2, EXPPARAM_LONG, EXPPARAM_LONG, M_EXP_A, M_EXP_B,
		IDMN_EXPRESSION_GOEV, M_EXPRESSION_GOEV, EXP_EXPRESSION_GOEV, 0, 2, EXPPARAM_LONG, EXPPARAM_LONG, M_EXP_A, M_EXP_B,
		IDMN_EXPRESSION_LV, M_EXPRESSION_LV, EXP_EXPRESSION_LV, 0, 2, EXPPARAM_LONG, EXPPARAM_LONG, M_EXP_A, M_EXP_B,
		IDMN_EXPRESSION_LOEV, M_EXPRESSION_LOEV, EXP_EXPRESSION_LOEV, 0, 2, EXPPARAM_LONG, EXPPARAM_LONG, M_EXP_A, M_EXP_B,
		
		IDMN_EXPRESSION_GOA, M_EXPRESSION_GOA, EXP_EXPRESSION_GOA, 0, 1, EXPPARAM_LONG, M_OBJECT,

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

long WINAPI DLLExport FuncHasParamAt(LPRDATA rdPtr, long param1, long param2) {
	size_t Pos = (size_t)CNC_GetStringParameter(rdPtr);

	return !rdPtr->FuncParamStack->back().empty()
		&& (Pos == max(Pos, min(Pos, rdPtr->FuncParamStack->back().size() - 1))) ? TRUE : FALSE;
}
long WINAPI DLLExport FuncHasReturnAt(LPRDATA rdPtr, long param1, long param2) {
	size_t Pos = (size_t)CNC_GetStringParameter(rdPtr);

	return !rdPtr->FuncReturn->empty()
		&& (Pos == max(Pos, min(Pos, rdPtr->FuncReturn->size() - 1))) ? TRUE : FALSE;
}

long WINAPI DLLExport FuncHasTempParam(LPRDATA rdPtr, long param1, long param2) {
	std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return HasTempParam(FuncName, ParamName) ? TRUE : FALSE;
}
long WINAPI DLLExport CurerntFuncHasTempParam(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return !rdPtr->FuncNameStack->empty() && HasTempParam(rdPtr->FuncNameStack->back(), ParamName) ? TRUE : FALSE;
}

long WINAPI DLLExport FuncParamAtIsNum(LPRDATA rdPtr, long param1, long param2) {
	size_t Pos = (size_t)CNC_GetStringParameter(rdPtr);

	return DataIsNum(rdPtr->FuncParamStack->back().at(Pos)) ? TRUE : FALSE;
}
long WINAPI DLLExport FuncReturnAtIsNum(LPRDATA rdPtr, long param1, long param2) {
	size_t Pos = (size_t)CNC_GetStringParameter(rdPtr);

	return DataIsNum(rdPtr->FuncReturn->at(Pos)) ? TRUE : FALSE;
}

long WINAPI DLLExport FuncTempParamIsNum(LPRDATA rdPtr, long param1, long param2) {
	std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return DataIsNum(TempParam(FuncName, ParamName)) ? TRUE : FALSE;
}
long WINAPI DLLExport CurerntFuncTempParamIsNum(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return DataIsNum(TempParam(rdPtr->FuncNameStack->back(), ParamName)) ? TRUE : FALSE;
}

long WINAPI DLLExport OnIterateObject(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);
	std::wstring iterateName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	
	if (iterateName == *rdPtr->pOnItObjName
		&& rdPtr->pSelect->ObjectIsOfType(rdPtr->pObject, oil)) {
		rdPtr->pSelect->SelectOneObject(rdPtr->pObject);
		return true;
	}

	return false;

	//return iterateName == *rdPtr->pOnItObjName
	//	&& rdPtr->pSelect->FilterObjects(rdPtr, oil, negated
	//		, [&](LPRDATA rdPtr, LPRO object)->bool { return object == rdPtr->pObject; });
}

long WINAPI DLLExport SelectAll(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);

	rdPtr->pSelect->SelectAll(oil);

	return TRUE;
}

long WINAPI DLLExport InEditor(LPRDATA rdPtr, long param1, long param2) {	
#ifndef RUN_ONLY
	return TRUE;
#else
	return FALSE;	
#endif // !RUN_ONLY
}

long WINAPI DLLExport InSubApp(LPRDATA rdPtr, long param1, long param2) {
	auto bSub = rdPtr->rHo.hoAdRunHeader->rhApp->m_pParentApp != nullptr;

	return bSub;
}

long WINAPI DLLExport OILHasObject(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR pObjName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return rdPtr->pSelect->OILHasObject(pObjName);
}

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport SetGlobalParamStr(LPRDATA rdPtr, long param1, long param2) {
	std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring Param = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	(*rdPtr->GlobalTempParam)[ParamName] = Data_Str(Param);

	return 0;
}
short WINAPI DLLExport SetGlobalParamVal(LPRDATA rdPtr, long param1, long param2) {
	std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	float Param = GetFloatParam(rdPtr);
	
	(*rdPtr->GlobalTempParam)[ParamName] = Data_Val(Param);

	return 0;
}

short WINAPI DLLExport SetTempParamStr(LPRDATA rdPtr, long param1, long param2) {
	std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring Param = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	TempParam(FuncName, ParamName) = Data_Str(Param);

	return 0;
}
short WINAPI DLLExport SetTempParamVal(LPRDATA rdPtr, long param1, long param2) {
	std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	long p1 = CNC_GetFloatParameter(rdPtr);
	float Param = *(float*)&p1;

	TempParam(FuncName, ParamName) = Data_Val(Param);
	return 0;
}

short WINAPI DLLExport SetCurrentTempParamStr(LPRDATA rdPtr, long param1, long param2) {
	std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring Param = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	TempParam(rdPtr->FuncNameStack->back(), ParamName) = Data_Str(Param);

	return 0;
}
short WINAPI DLLExport SetCurrentTempParamVal(LPRDATA rdPtr, long param1, long param2) {
	std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	long p1 = CNC_GetFloatParameter(rdPtr);
	float Param = *(float*)&p1;

	TempParam(rdPtr->FuncNameStack->back(), ParamName) = Data_Val(Param);

	return 0;
}

short WINAPI DLLExport PassReturnStr(LPRDATA rdPtr, long param1, long param2) {
	std::wstring Return = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return 0;
}
short WINAPI DLLExport PassReturnVal(LPRDATA rdPtr, long param1, long param2) {
	long p1 = CNC_GetFloatParameter(rdPtr);
	float Return = *(float*)&p1;

	return 0;
}

short WINAPI DLLExport SetAllReturn(LPRDATA rdPtr, long param1, long param2) {
	std::wstring Return = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	rdPtr->FuncReturn->clear();
	UpdateReturn(rdPtr, Return);

	return 0;
}

short WINAPI DLLExport SetReturnValueStr(LPRDATA rdPtr, long param1, long param2) {
	std::wstring Return = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->FuncReturn->clear();
	rdPtr->FuncReturn->reserve(DefaultVecSize);
	rdPtr->FuncReturn->emplace_back(Data_Str(Return));

	return 0;
}
short WINAPI DLLExport SetReturnValueVal(LPRDATA rdPtr, long param1, long param2) {
	long p1 = CNC_GetFloatParameter(rdPtr);
	float Return = *(float*)&p1;

	rdPtr->FuncReturn->clear();
	rdPtr->FuncReturn->reserve(DefaultVecSize);
	rdPtr->FuncReturn->emplace_back(Data_Val(Return));

	return 0;
}

short WINAPI DLLExport PushReturnValueStr(LPRDATA rdPtr, long param1, long param2) {
	std::wstring Return = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	rdPtr->FuncReturn->emplace_back(Data_Str(Return));
	
	return 0;
}
short WINAPI DLLExport PushReturnValueVal(LPRDATA rdPtr, long param1, long param2) {
	long p1 = CNC_GetFloatParameter(rdPtr);
	float Return = *(float*)&p1;

	rdPtr->FuncReturn->emplace_back(Data_Val(Return));

	return 0;
}

short WINAPI DLLExport CallFunc(LPRDATA rdPtr, long param1, long param2) {
	std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring Param = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	size_t LoopIndex = (size_t)CNC_GetIntParameter(rdPtr);

	(*rdPtr->FuncLoopIndex)[FuncName] = LoopIndex;

	for ((*rdPtr->FuncCurLoopIndex)[FuncName] = 0; 
		(*rdPtr->FuncCurLoopIndex)[FuncName] < LoopIndex; 
		(*rdPtr->FuncCurLoopIndex)[FuncName]++) {
		CallFuncCore(FuncName, Param);
	}

	rdPtr->FuncLoopIndex->erase(FuncName);
	rdPtr->FuncCurLoopIndex->erase(FuncName);

	return 0;
}

short WINAPI DLLExport SetLoopIndex(LPRDATA rdPtr, long param1, long param2) {
	std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	size_t LoopIndex = (size_t)CNC_GetIntParameter(rdPtr);

	if (rdPtr->FuncLoopIndex->count(FuncName) != 0) {
		(*rdPtr->FuncCurLoopIndex)[FuncName] = LoopIndex;
	}

	return 0;
}

short WINAPI DLLExport StopLoop(LPRDATA rdPtr, long param1, long param2) {
	std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	if (rdPtr->FuncLoopIndex->count(FuncName) != 0) {
		(*rdPtr->FuncCurLoopIndex)[FuncName] = (*rdPtr->FuncLoopIndex)[FuncName];
	}

	return 0;
}

short WINAPI DLLExport Ternary(LPRDATA rdPtr, long param1, long param2) {
	bool Result = (bool)CNC_GetIntParameter(rdPtr);
	
	std::wstring FuncNameA = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring ParamA = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	std::wstring FuncNameB = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring ParamB = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	CallFuncCore(Result ? FuncNameA : FuncNameB, Result ? ParamA : ParamB);	

	return 0;
}

short WINAPI DLLExport IterateObject(LPRDATA rdPtr, long param1, long param2) {
	short oil = (short)OIL_GetParameter(rdPtr);
	*rdPtr->pOnItObjName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	
	std::vector<LPRO> toIterate;
	char flag = rdPtr->pSelect->ObjectIsSelected(oil) ? 0b00000010 : 0;

	// usually fusion will do a internal for-each for all selected objects
	// but when you call immediate events, this for-each will be terminated
	// so here need a manual for-each to force it
	rdPtr->pSelect->ForEach(rdPtr, oil, [&](LPRO object) {
		toIterate.emplace_back(object);
		}, flag);

	for (auto& object : toIterate) {
		rdPtr->pObject = object;
		
		CallEvent(ONITOBJ);

		rdPtr->pObject = nullptr;
	}

	return 0;
}

short WINAPI DLLExport Assert(LPRDATA rdPtr, long param1, long param2) {
	auto value = CNC_GetParameter(rdPtr);
	std::wstring msg = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	Assert(value, msg);

	return 0;
}

short WINAPI DLLExport MsgBox(LPRDATA rdPtr, long param1, long param2) {
	auto value = CNC_GetParameter(rdPtr);
	std::wstring msg = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (value) {
		auto ret = MessageBox(NULL, StrEqu(msg.c_str(), Empty_Str)
			? L"Message"
			: msg.c_str()
			, L"Alert"
			, MB_OK);
	}

	return 0;
}

short WINAPI DLLExport Toast(LPRDATA rdPtr, long param1, long param2) {
	auto value = CNC_GetParameter(rdPtr);
	std::wstring title = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	std::wstring content = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (value) {
		rdPtr->pToast->ShowToast(std::forward<std::wstring>(title), std::forward<std::wstring>(content));
	}

	return 0;
}

short WINAPI DLLExport ToastFlags(LPRDATA rdPtr, long param1, long param2) {
	auto flags = CNC_GetParameter(rdPtr);

	rdPtr->pToast->SetFlag(flags);

	return 0;
}

short WINAPI DLLExport SetObjectAlpha(LPRDATA rdPtr, long param1, long param2) {
	LPRO pObject = LPRO(CNC_GetParameter(rdPtr));
	UCHAR alpha = UCHAR(CNC_GetIntParameter(rdPtr));

	EffectUtilities::SetAlpha(pObject, alpha);

	return 0;
}

short WINAPI DLLExport SetObjectAlphaByFixed(LPRDATA rdPtr, long param1, long param2) {
	long fixed = long(CNC_GetParameter(rdPtr));
	UCHAR alpha = UCHAR(CNC_GetIntParameter(rdPtr));

	EffectUtilities::SetAlpha(LproFromFixed(rdPtr, fixed), alpha);

	return 0;
}

short WINAPI DLLExport SetObjectRGBCoef(LPRDATA rdPtr, long param1, long param2) {
	LPRO pObject = LPRO(CNC_GetParameter(rdPtr));
	DWORD dwRGBCoef = DWORD(CNC_GetIntParameter(rdPtr));

	EffectUtilities::SetRGBCoef(pObject, EffectUtilities::BGRToRGB(dwRGBCoef));

	return 0;
}

short WINAPI DLLExport SetObjectRGBCoefByFixed(LPRDATA rdPtr, long param1, long param2) {
	long fixed = long(CNC_GetParameter(rdPtr));
	DWORD dwRGBCoef = DWORD(CNC_GetIntParameter(rdPtr));

	EffectUtilities::SetRGBCoef(LproFromFixed(rdPtr, fixed), EffectUtilities::BGRToRGB(dwRGBCoef));

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport CallFuncRV(LPRDATA rdPtr,long param1) {
	std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	std::wstring Param = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);	
	
	CallFuncCore(FuncName, Param);	

	auto& ret = Return(0);

	Data_StoV(ret);
	return ReturnFloat(ret.Val);
}

long WINAPI DLLExport CallFuncRS(LPRDATA rdPtr,long param1) {
	std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	std::wstring Param = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	CallFuncCore(FuncName, Param);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;
	
	//This returns a pointer to the string for MMF.
	auto& ret = Return(0);

	Data_VtoS(ret);
	return (long)ret.Str.c_str();
}

long WINAPI DLLExport GetParamRV(LPRDATA rdPtr, long param1) {
	size_t Pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

#ifndef RUN_ONLY
	if (rdPtr->FuncParamStack->empty()) {
		NotInFuncError<long>();
	}
#endif

	if (!rdPtr->FuncParamStack->empty()
		&& !rdPtr->FuncParamStack->back().empty()
		&& (Pos == max(Pos, min(Pos, rdPtr->FuncParamStack->back().size() - 1)))) {
		Data_StoV(GetParam(Pos));
		return ReturnFloat(GetParam(Pos).Val);
	}
	else {
		return 0;
	}
}

long WINAPI DLLExport GetParamRS(LPRDATA rdPtr, long param1) {
	size_t Pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

#ifndef RUN_ONLY
	if (rdPtr->FuncParamStack->empty()) {
		NotInFuncError<std::wstring>();
	}
#endif

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	if (!rdPtr->FuncParamStack->empty()
		&& !rdPtr->FuncParamStack->back().empty()
		&& (Pos == max(Pos, min(Pos, rdPtr->FuncParamStack->back().size() - 1)))) {
		Data_VtoS(GetParam(Pos));
		return (long)GetParam(Pos).Str.c_str();
	}
	else {
		return (long)Default_Str;
	}	
}

long WINAPI DLLExport GetGlobalParamRV(LPRDATA rdPtr, long param1) {
	std::wstring ParamName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	auto it = rdPtr->GlobalTempParam->find(ParamName);

	if (it != rdPtr->GlobalTempParam->end()) {
		Data_StoV(it->second);
		return ReturnFloat(it->second.Val);
	}
	else {
		return 0;
	}
}

long WINAPI DLLExport GetGlobalParamRS(LPRDATA rdPtr, long param1) {
	std::wstring ParamName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	auto it = rdPtr->GlobalTempParam->find(ParamName);

	if (it != rdPtr->GlobalTempParam->end()) {
		Data_VtoS(it->second);
		return (long)it->second.Str.c_str();
	}
	else {
		return (long)Default_Str;
	}
}

long WINAPI DLLExport GetTempParamRV(LPRDATA rdPtr, long param1) {
	std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	std::wstring ParamName = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
		
#ifndef RUN_ONLY
	if ((*rdPtr->FuncTempParam).count(GetFuncNameWithRecursiveID(FuncName)) == 0) {
		NotInFuncError<long>();
	}
#endif

	if (HasTempParam(FuncName, ParamName)) {
		Data_StoV(TempParam(FuncName, ParamName));
		return ReturnFloat(TempParam(FuncName, ParamName).Val);
	}
	else {
		return 0;
	}
}

long WINAPI DLLExport GetTempParamRS(LPRDATA rdPtr, long param1) {
	std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	std::wstring ParamName = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

#ifndef RUN_ONLY
	if ((*rdPtr->FuncTempParam).count(GetFuncNameWithRecursiveID(FuncName)) == 0) {
		NotInFuncError<std::wstring>();
	}
#endif

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	if (HasTempParam(FuncName, ParamName)) {
		Data_VtoS(TempParam(FuncName, ParamName));
		return (long)TempParam(FuncName, ParamName).Str.c_str();
	}
	else {
		return (long)Default_Str;
	}
}

long WINAPI DLLExport GetCurrentTempParamRV(LPRDATA rdPtr, long param1) {
	std::wstring ParamName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	if (HasTempParam(rdPtr->FuncNameStack->back(), ParamName)) {
		Data_StoV(TempParam(rdPtr->FuncNameStack->back(), ParamName));
		return ReturnFloat(TempParam(rdPtr->FuncNameStack->back(), ParamName).Val);
	}
	else {
		return 0;
	}
}

long WINAPI DLLExport GetCurrentTempParamRS(LPRDATA rdPtr, long param1) {
	std::wstring ParamName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	if (HasTempParam(rdPtr->FuncNameStack->back(), ParamName)) {
		Data_VtoS(TempParam(rdPtr->FuncNameStack->back(), ParamName));
		return (long)TempParam(rdPtr->FuncNameStack->back(), ParamName).Str.c_str();
	}
	else {
		return (long)Default_Str;
	}
}

long WINAPI DLLExport GetRetRV(LPRDATA rdPtr, long param1) {
	size_t Pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	
	if (!rdPtr->FuncReturn->empty() && (Pos == max(Pos, min(Pos, rdPtr->FuncReturn->size() - 1)))) {
		auto& ret = Return(Pos);

		Data_StoV(ret);
		return ReturnFloat(ret.Val);
	}
	else {
		return 0;
	}
}

long WINAPI DLLExport GetRetRS(LPRDATA rdPtr, long param1) {
	size_t Pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	if (!rdPtr->FuncReturn->empty() && (Pos == max(Pos, min(Pos, rdPtr->FuncReturn->size() - 1)))) {
		auto& ret = Return(Pos);

		Data_VtoS(ret);
		return (long)ret.Str.c_str();
	}
	else {
		return (long)Default_Str;
	}
}

long WINAPI DLLExport GetRecursiveIndex(LPRDATA rdPtr, long param1) {
	std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	if (rdPtr->RecursiveIndex->count(FuncName) != 0) {
		return (*rdPtr->RecursiveIndex)[FuncName];
	}
	else {
		return -1;
	}
}

long WINAPI DLLExport GetParamSize(LPRDATA rdPtr, long param1) {	
	return !rdPtr->FuncParamStack->empty()
		? rdPtr->FuncParamStack->back().size()
		: NotInFuncError<long>();
}

long WINAPI DLLExport GetRetSize(LPRDATA rdPtr, long param1) {
	return rdPtr->FuncReturn->size();
}

long WINAPI DLLExport TernaryRV(LPRDATA rdPtr,long param1) {
	bool Result = (bool)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	long p1 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float ReturnA = *(float*)&p1;

	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float ReturnB = *(float*)&p2;
		
	return ReturnFloat(Result ? ReturnA : ReturnB);
}

long WINAPI DLLExport TernaryRS(LPRDATA rdPtr, long param1) {
	bool Result = (bool)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	std::wstring ReturnA = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
	std::wstring ReturnB = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	NewStr(OStr, Result ? ReturnA : ReturnB);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)OStr;
}

long WINAPI DLLExport GetCurrentFuncName(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return	!rdPtr->FuncNameStack->empty()
		? (long)rdPtr->FuncNameStack->back().c_str()
		: NotInFuncError<std::wstring>();
}

long WINAPI DLLExport GetLoopIndex(LPRDATA rdPtr, long param1) {
	std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	if (rdPtr->FuncLoopIndex->count(FuncName) != 0) {
		return (*rdPtr->FuncCurLoopIndex)[FuncName];
	}
	else {
		return -1;
	}
}

long WINAPI DLLExport EquStr(LPRDATA rdPtr, long param1) {
	std::wstring a = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	std::wstring b = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	return a == b;
}

long WINAPI DLLExport EquVal(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float a = *(float*)&p1;

	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float b = *(float*)&p2;

	return abs(a - b) <= 1e-6;
}

long WINAPI DLLExport GreaterVal(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float a = *(float*)&p1;

	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float b = *(float*)&p2;

	return a > b;
}

long WINAPI DLLExport GreaterOrEqualVal(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float a = *(float*)&p1;

	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float b = *(float*)&p2;

	return a >= b;
}

long WINAPI DLLExport LowerVal(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float a = *(float*)&p1;

	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float b = *(float*)&p2;

	return a < b;
}

long WINAPI DLLExport LowerOrEqualVal(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float a = *(float*)&p1;

	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float b = *(float*)&p2;

	return a <= b;
}

long WINAPI DLLExport CastToBool(LPRDATA rdPtr, long param1) {
	auto bRet = (bool(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_LONG)));

	return bRet;
}

long WINAPI DLLExport Negate(LPRDATA rdPtr, long param1) {
	auto bRet= !(bool(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_LONG)));

	return bRet;
}

long WINAPI DLLExport And(LPRDATA rdPtr, long param1) {
	auto a = bool(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_LONG));
	auto b = bool(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_LONG));

	auto bRet = a && b;

	return bRet;
}

long WINAPI DLLExport Or(LPRDATA rdPtr, long param1) {
	auto a = bool(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_LONG));
	auto b = bool(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_LONG));

	auto bRet = a || b;

	return bRet;
}

long WINAPI DLLExport Xor(LPRDATA rdPtr, long param1) {
	auto a = bool(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_LONG));
	auto b = bool(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_LONG));

	auto bRet = a ^ b;

	return bRet;
}

long WINAPI DLLExport GetObjectAlpha(LPRDATA rdPtr, long param1) {
	long fixed = long(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_LONG));

	return EffectUtilities::GetAlpha(LproFromFixed(rdPtr, fixed));
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

			FuncHasParamAt,
			FuncHasReturnAt,
			
			FuncHasTempParam,
			CurerntFuncHasTempParam,

			FuncParamAtIsNum,
			FuncReturnAtIsNum,

			FuncTempParamIsNum,
			CurerntFuncTempParamIsNum,

			OnIterateObject,
			SelectAll,

			InEditor,
			InSubApp,

			OILHasObject,

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			SetReturnValueStr,
			PushReturnValueStr,

			CallFunc,

			Ternary,

			SetTempParamStr,
			SetCurrentTempParamStr,

			SetAllReturn,

			SetLoopIndex,
			StopLoop,

			PassReturnStr,
			PassReturnVal,

			SetTempParamVal,
			SetCurrentTempParamVal,

			SetReturnValueVal,
			PushReturnValueVal,

			IterateObject,

			SetGlobalParamStr,
			SetGlobalParamVal,

			Assert,
			MsgBox,
			Toast,
			ToastFlags,

			SetObjectAlpha,
			SetObjectAlphaByFixed,

			SetObjectRGBCoef,
			SetObjectRGBCoefByFixed,

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

			TernaryRV,
			TernaryRS,

			GetTempParamRV,
			GetTempParamRS,

			GetCurrentFuncName,

			GetCurrentTempParamRV,
			GetCurrentTempParamRS,

			GetLoopIndex,

			GetGlobalParamRV,
			GetGlobalParamRS,

			EquStr,
			EquVal,

			CastToBool,
			Negate,
			And,
			Or,
			Xor,

			GreaterVal,
			GreaterOrEqualVal,
			LowerVal,
			LowerOrEqualVal,

			GetObjectAlpha,

			0
			};