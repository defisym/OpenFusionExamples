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

		IDMN_CONDITION_GHTP, M_CONDITION_GHTP, CND_CONDITION_GHTP, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPSTRING, M_ACT_PARAMNAME,
		
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

		IDMN_ACTION_SS, M_ACTION_SS, ACT_ACTION_SS, 0, 1, PARAM_EXPSTRING, M_SCOPENAME,
		IDMN_ACTION_RS, M_ACTION_RS, ACT_ACTION_RS, 0, 1, PARAM_EXPSTRING, M_SCOPENAME,

		IDMN_ACTION_ITOF, M_ACTION_ITOF, ACT_ACTION_ITOF, 0, 3, PARAM_OBJECT, PARAM_EXPSTRING, PARAM_EXPSTRING, M_OBJECT, M_ITNAME, M_EXP_PARAM,

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

		IDMN_EXPRESSION_GRP, M_EXPRESSION_GRP, EXP_EXPRESSION_GRP, EXPFLAG_STRING, 0,

		};

// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Condition_OnFunc(LPRDATA rdPtr, long param1, long param2) {
	const auto FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return StrEqu(FuncName, rdPtr->FuncNameStack->back().c_str());
}

long WINAPI DLLExport Condition_FuncHasParamAt(LPRDATA rdPtr, long param1, long param2) {
	const auto Pos = (size_t)CNC_GetStringParameter(rdPtr);

    return HasParam(rdPtr, Pos);
}
long WINAPI DLLExport Condition_FuncHasReturnAt(LPRDATA rdPtr, long param1, long param2) {
	const auto Pos = (size_t)CNC_GetStringParameter(rdPtr);

	return HasReturn(rdPtr, Pos);
}

long WINAPI DLLExport Condition_GlobalHasTempParam(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return (*rdPtr->GlobalTempParam).contains(ParamName);
}
long WINAPI DLLExport Condition_FuncHasTempParam(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return HasTempParam(rdPtr, FuncName, ParamName) ;
}
long WINAPI DLLExport Condition_CurerntFuncHasTempParam(LPRDATA rdPtr, long param1, long param2) {
	const auto ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return !rdPtr->FuncNameStack->empty() && HasTempParam(rdPtr, rdPtr->FuncNameStack->back(), ParamName);
}

long WINAPI DLLExport Condition_FuncParamAtIsNum(LPRDATA rdPtr, long param1, long param2) {
	const auto Pos = (size_t)CNC_GetStringParameter(rdPtr);

	return DataIsNum(rdPtr->FuncParamStack->back().at(Pos));
}
long WINAPI DLLExport Condition_FuncReturnAtIsNum(LPRDATA rdPtr, long param1, long param2) {
	const auto Pos = (size_t)CNC_GetStringParameter(rdPtr);

	return DataIsNum(rdPtr->FuncReturn->at(Pos));
}

long WINAPI DLLExport Condition_FuncTempParamIsNum(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return DataIsNum(TempParam(rdPtr, FuncName, ParamName));
}
long WINAPI DLLExport Condition_CurerntFuncTempParamIsNum(LPRDATA rdPtr, long param1, long param2) {
	const auto ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return DataIsNum(TempParam(rdPtr, rdPtr->FuncNameStack->back(), ParamName));
}

long WINAPI DLLExport Condition_OnIterateObject(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	const auto oil = (short)OIL_GetParameter(rdPtr);
	const std::wstring iterateName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	
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

long WINAPI DLLExport Condition_SelectAll(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	const auto oil = (short)OIL_GetParameter(rdPtr);

	rdPtr->pSelect->SelectAll(oil);

	return TRUE;
}

long WINAPI DLLExport Condition_InEditor(LPRDATA rdPtr, long param1, long param2) {	
#ifndef RUN_ONLY
	return TRUE;
#else
	return FALSE;	
#endif // !RUN_ONLY
}

long WINAPI DLLExport Condition_InSubApp(LPRDATA rdPtr, long param1, long param2) {
	const auto bSub = rdPtr->rHo.hoAdRunHeader->rhApp->m_pParentApp != nullptr;

	return bSub;
}

long WINAPI DLLExport Condition_OILHasObject(LPRDATA rdPtr, long param1, long param2) {
	const auto pObjName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return rdPtr->pSelect->OILHasObject(pObjName);
}

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport Action_SetGlobalParamStr(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring Param = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	(*rdPtr->GlobalTempParam)[ParamName] = Data(Param);

	return 0;
}
short WINAPI DLLExport Action_SetGlobalParamVal(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const float Param = GetFloatParam(rdPtr);
	
	(*rdPtr->GlobalTempParam)[ParamName] = Data(Param);

	return 0;
}

short WINAPI DLLExport Action_SetTempParamStr(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring Param = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	TempParam(rdPtr, FuncName, ParamName) = Data(Param);

	return 0;
}
short WINAPI DLLExport Action_SetTempParamVal(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	long p1 = CNC_GetFloatParameter(rdPtr);
	const float Param = *(float*)&p1;

	TempParam(rdPtr, FuncName, ParamName) = Data(Param);

	return 0;
}

short WINAPI DLLExport Action_SetCurrentTempParamStr(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring Param = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	TempParam(rdPtr, rdPtr->FuncNameStack->back(), ParamName) = Data(Param);

	return 0;
}
short WINAPI DLLExport Action_SetCurrentTempParamVal(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring ParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	long p1 = CNC_GetFloatParameter(rdPtr);
	const float Param = *(float*)&p1;

	TempParam(rdPtr, rdPtr->FuncNameStack->back(), ParamName) = Data(Param);

	return 0;
}

short WINAPI DLLExport Action_PassReturnStr(LPRDATA rdPtr, long param1, long param2) {
	std::wstring Return = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return 0;
}
short WINAPI DLLExport Action_PassReturnVal(LPRDATA rdPtr, long param1, long param2) {
	long p1 = CNC_GetFloatParameter(rdPtr);
	float Return = *(float*)&p1;

	return 0;
}

short WINAPI DLLExport Action_SetAllReturn(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring Return = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	rdPtr->FuncReturn->clear();
	UpdateReturn(rdPtr, Return);

	return 0;
}

short WINAPI DLLExport Action_SetReturnValueStr(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring Return = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->FuncReturn->clear();
	rdPtr->FuncReturn->reserve(DefaultVecSize);
	rdPtr->FuncReturn->emplace_back(Data(Return));

	return 0;
}
short WINAPI DLLExport Action_SetReturnValueVal(LPRDATA rdPtr, long param1, long param2) {
	long p1 = CNC_GetFloatParameter(rdPtr);
	const float Return = *(float*)&p1;

	rdPtr->FuncReturn->clear();
	rdPtr->FuncReturn->reserve(DefaultVecSize);
	rdPtr->FuncReturn->emplace_back(Data(Return));

	return 0;
}

short WINAPI DLLExport Action_PushReturnValueStr(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring Return = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	rdPtr->FuncReturn->emplace_back(Data(Return));
	
	return 0;
}
short WINAPI DLLExport Action_PushReturnValueVal(LPRDATA rdPtr, long param1, long param2) {
	long p1 = CNC_GetFloatParameter(rdPtr);
	const float Return = *(float*)&p1;

	rdPtr->FuncReturn->emplace_back(Data(Return));

	return 0;
}

short WINAPI DLLExport Action_CallFunc(LPRDATA rdPtr, long param1, long param2) {
	std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring Param = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const auto LoopIndex = (size_t)CNC_GetIntParameter(rdPtr);

	CallFuncCore(rdPtr, FuncName, Param, LoopIndex);

	return 0;
}

short WINAPI DLLExport Action_SetLoopIndex(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const auto LoopIndex = (size_t)CNC_GetIntParameter(rdPtr);

	const auto& name = GetFuncNameWithRecursiveID(rdPtr, FuncName);

	if (rdPtr->FuncLoopIndex->contains(name)) {
		(*rdPtr->FuncCurLoopIndex)[name] = LoopIndex;
	}

	return 0;
}

short WINAPI DLLExport Action_StopLoop(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring FuncName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	const auto& name = GetFuncNameWithRecursiveID(rdPtr, FuncName);

	if (rdPtr->FuncLoopIndex->contains(name)) {
		(*rdPtr->FuncCurLoopIndex)[name] = (*rdPtr->FuncLoopIndex)[name];
	}

	return 0;
}

short WINAPI DLLExport Action_Ternary(LPRDATA rdPtr, long param1, long param2) {
	const bool Result = (bool)CNC_GetIntParameter(rdPtr);
	
	std::wstring FuncNameA = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring ParamA = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	std::wstring FuncNameB = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring ParamB = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	CallFuncCore(rdPtr,
		Result ? FuncNameA : FuncNameB,
		Result ? ParamA : ParamB);

	return 0;
}

short WINAPI DLLExport Action_IterateObject(LPRDATA rdPtr, long param1, long param2) {
	const auto oil = (short)OIL_GetParameter(rdPtr);
	*rdPtr->pOnItObjName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	IterateObjectCore(rdPtr, oil, [&] (const ObjectSelection::SelObj& toIterate) {
		rdPtr->pSelect->KeepScopeCall(rdPtr->bKeepScope, [&] () {
			for (const auto& object : toIterate) {
				rdPtr->pObject = object;

				CallEvent(ONITOBJ);

				rdPtr->pObject = nullptr;
			}
		});
	});

	return 0;
}

short WINAPI DLLExport Action_IterateObjectFunc(LPRDATA rdPtr, long param1, long param2) {
	const auto oil = (short)OIL_GetParameter(rdPtr);
	*rdPtr->pOnItObjName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	std::wstring Param = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	IterateObjectCore(rdPtr, oil, [&] (const ObjectSelection::SelObj& toIterate) {
		const FuncInfoObject funcObj(rdPtr, *rdPtr->pOnItObjName, Param);

		rdPtr->pSelect->KeepScopeCall(rdPtr->bKeepScope, [&] () {
			for (const auto& object : toIterate) {
				rdPtr->pObject = object;

				CallEvent(ONITOBJ);

				rdPtr->pObject = nullptr;
			}
		});
	});

	return 0;
}

short WINAPI DLLExport Action_Assert(LPRDATA rdPtr, long param1, long param2) {
	const auto value = CNC_GetParameter(rdPtr);
	const std::wstring msg = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	Assert(value, msg);

	return 0;
}

short WINAPI DLLExport Action_MsgBox(LPRDATA rdPtr, long param1, long param2) {
	const auto value = CNC_GetParameter(rdPtr);
	const std::wstring msg = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (value) {
		auto ret = MessageBox(NULL, StrEqu(msg.c_str(), Empty_Str)
			? L"Message"
			: msg.c_str()
			, L"Alert"
			, MB_OK);
	}

	return 0;
}

short WINAPI DLLExport Action_Toast(LPRDATA rdPtr, long param1, long param2) {
	auto value = CNC_GetParameter(rdPtr);
	std::wstring title = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	std::wstring content = (LPCWSTR)CNC_GetStringParameter(rdPtr);

#ifdef _ENABLE_TOAST
	if (value) {
		rdPtr->pToast->ShowToast(std::forward<std::wstring>(title), std::forward<std::wstring>(content));
	}
#endif

	return 0;
}

short WINAPI DLLExport Action_ToastFlags(LPRDATA rdPtr, long param1, long param2) {
	auto flags = CNC_GetParameter(rdPtr);

#ifdef _ENABLE_TOAST
	rdPtr->pToast->SetFlag(flags);
#endif

	return 0;
}

short WINAPI DLLExport Action_SetObjectAlpha(LPRDATA rdPtr, long param1, long param2) {
	const LPRO pObject = LPRO(CNC_GetParameter(rdPtr));
	const auto alpha = UCHAR(CNC_GetIntParameter(rdPtr));

	EffectUtilities::SetAlpha(pObject, alpha);

	return 0;
}

short WINAPI DLLExport Action_SetObjectAlphaByFixed(LPRDATA rdPtr, long param1, long param2) {
	const long fixed = long(CNC_GetParameter(rdPtr));
	const auto alpha = UCHAR(CNC_GetIntParameter(rdPtr));

	EffectUtilities::SetAlpha(LproFromFixed(rdPtr, fixed), alpha);

	return 0;
}

short WINAPI DLLExport Action_SetObjectRGBCoef(LPRDATA rdPtr, long param1, long param2) {
	const LPRO pObject = LPRO(CNC_GetParameter(rdPtr));
	const auto dwRGBCoef = DWORD(CNC_GetIntParameter(rdPtr));

	EffectUtilities::SetRGBCoef(pObject, EffectUtilities::BGRToRGB(dwRGBCoef));

	return 0;
}

short WINAPI DLLExport Action_SetObjectRGBCoefByFixed(LPRDATA rdPtr, long param1, long param2) {
	const long fixed = long(CNC_GetParameter(rdPtr));
	const auto dwRGBCoef = DWORD(CNC_GetIntParameter(rdPtr));

	EffectUtilities::SetRGBCoef(LproFromFixed(rdPtr, fixed), EffectUtilities::BGRToRGB(dwRGBCoef));

	return 0;
}

short WINAPI DLLExport Action_SaveScope(LPRDATA rdPtr, long param1, long param2) {
	const auto pName = (LPCWSTR)CNC_GetIntParameter(rdPtr);

	Assert(rdPtr->bScope, L"You must enable scope feature in properties, or this action will have not effect");

	if(!rdPtr->bScope) {
		return 0;
	}

	const auto pScope = new ObjectSelection::Scope(rdPtr);
	rdPtr->pSelect->SaveScope(pScope);

	(*rdPtr->pScope)[pName] = pScope;

	return 0;
}

short WINAPI DLLExport Action_RestoreScope(LPRDATA rdPtr, long param1, long param2) {
	const auto pName = (LPCWSTR)CNC_GetIntParameter(rdPtr);

	Assert(rdPtr->bScope, L"You must enable scope feature in properties, or this action will have not effect");

	if (!rdPtr->bScope) {
		return 0;
	}

	const auto it = rdPtr->pScope->find(pName);

	if (it == rdPtr->pScope->end()) {
		return 0;
	}

	const auto pScope = static_cast<ObjectSelection::Scope*>(it->second);

	pScope->RestoreActionState(rdPtr);
	rdPtr->pSelect->RestoreScope(*pScope);

	delete pScope;

	rdPtr->pScope->erase(it);
	
	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Expression_CallFuncRV(LPRDATA rdPtr,long param1) {
	std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	std::wstring Param = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);	

	if (!CheckCompatibility(rdPtr)) {
#ifndef RUN_ONLY
		MSGBOX(L"Expression Func is not supported in 295 or later");
#endif
		return 0;
	}

	CallFuncCore(rdPtr, FuncName, Param);

	auto& ret = GetReturn(rdPtr, 0);
	Data_StoV(ret);

	return ReturnFloat(ret.Val);
}

long WINAPI DLLExport Expression_CallFuncRS(LPRDATA rdPtr,long param1) {
	std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	std::wstring Param = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	if (!CheckCompatibility(rdPtr)) {
#ifndef RUN_ONLY
		MSGBOX(L"Expression Func is not supported in 295 or later");
#endif
		rdPtr->rHo.hoFlags |= HOF_STRING;
		return (long)Default_Str;
	}

	CallFuncCore(rdPtr, FuncName, Param);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;
	
	//This returns a pointer to the string for MMF.
	auto& ret = GetReturn(rdPtr, 0);
	Data_VtoS(ret);

	return (long)ret.Str.c_str();
}

long WINAPI DLLExport Expression_GetParamRV(LPRDATA rdPtr, long param1) {
	const auto Pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

#ifndef RUN_ONLY
	if (rdPtr->FuncParamStack->empty()) {
		NotInFuncError<long>();
	}
#endif

	if (HasParam(rdPtr, Pos)) {
		auto& ret = GetParam(rdPtr, Pos);
		Data_StoV(ret);

		return ReturnFloat(ret.Val);
	}
	else {
		return 0;
	}
}

long WINAPI DLLExport Expression_GetParamRS(LPRDATA rdPtr, long param1) {
	const auto Pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

#ifndef RUN_ONLY
	if (rdPtr->FuncParamStack->empty()) {
		NotInFuncError<std::wstring>();
	}
#endif

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	if (HasParam(rdPtr, Pos)) {
		auto& ret = GetParam(rdPtr, Pos);
		Data_VtoS(ret);

		return (long)ret.Str.c_str();
	}
	else {
		return (long)Default_Str;
	}	
}

long WINAPI DLLExport Expression_GetRawParam(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return	!rdPtr->FuncRawParamStack->empty()
		? (long)rdPtr->FuncRawParamStack->back().c_str()
		: NotInFuncError<std::wstring>();
}

long WINAPI DLLExport Expression_GetGlobalParamRV(LPRDATA rdPtr, long param1) {
	const std::wstring ParamName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	const auto it = rdPtr->GlobalTempParam->find(ParamName);

	if (it != rdPtr->GlobalTempParam->end()) {
		Data_StoV(it->second);

		return ReturnFloat(it->second.Val);
	}
	else {
		return 0;
	}
}

long WINAPI DLLExport Expression_GetGlobalParamRS(LPRDATA rdPtr, long param1) {
	const std::wstring ParamName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	const auto it = rdPtr->GlobalTempParam->find(ParamName);

	if (it != rdPtr->GlobalTempParam->end()) {
		Data_VtoS(it->second);

		return (long)it->second.Str.c_str();
	}
	else {
		return (long)Default_Str;
	}
}

long WINAPI DLLExport Expression_GetTempParamRV(LPRDATA rdPtr, long param1) {
	const std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	const std::wstring ParamName = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
		
#ifndef RUN_ONLY
	if (!(*rdPtr->FuncTempParam).contains(GetFuncNameWithRecursiveID(rdPtr, FuncName))) {
		NotInFuncError<long>();
	}
#endif

	auto& ret = TempParam(rdPtr, FuncName, ParamName);

	if (ret.bDefault) {
		return 0;
	}

	Data_StoV(ret);

	return ReturnFloat(ret.Val);
}

long WINAPI DLLExport Expression_GetTempParamRS(LPRDATA rdPtr, long param1) {
	const std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	const std::wstring ParamName = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

#ifndef RUN_ONLY
	if (!(*rdPtr->FuncTempParam).contains(GetFuncNameWithRecursiveID(rdPtr, FuncName))) {
		NotInFuncError<std::wstring>();
	}
#endif

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	auto& ret = TempParam(rdPtr, rdPtr->FuncNameStack->back(), ParamName);

	//This returns a pointer to the string for MMF.
	if (ret.bDefault) {
		return (long)Default_Str;
	}

	Data_VtoS(ret);

	return (long)ret.Str.c_str();
}

long WINAPI DLLExport Expression_GetCurrentTempParamRV(LPRDATA rdPtr, long param1) {
	const std::wstring ParamName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	auto& ret = TempParam(rdPtr, rdPtr->FuncNameStack->back(), ParamName);

	if (ret.bDefault) {
		return 0;
	}

	Data_StoV(ret);

	return ReturnFloat(ret.Val);
}

long WINAPI DLLExport Expression_GetCurrentTempParamRS(LPRDATA rdPtr, long param1) {
	const std::wstring ParamName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	auto& ret = TempParam(rdPtr, rdPtr->FuncNameStack->back(), ParamName);

	//This returns a pointer to the string for MMF.
	if (ret.bDefault) {
		return (long)Default_Str;
	}

	Data_VtoS(ret);

	return (long)ret.Str.c_str();	
}

long WINAPI DLLExport Expression_GetRetRV(LPRDATA rdPtr, long param1) {
	const auto Pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	
	if (HasReturn(rdPtr, Pos)) {
		auto& ret = GetReturn(rdPtr, Pos);
		Data_StoV(ret);

		return ReturnFloat(ret.Val);
	}
	else {
		return 0;
	}
}

long WINAPI DLLExport Expression_GetRetRS(LPRDATA rdPtr, long param1) {
	const auto Pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	if (HasReturn(rdPtr, Pos)) {
		auto& ret = GetReturn(rdPtr, Pos);
		Data_VtoS(ret);

		return (long)ret.Str.c_str();
	}
	else {
		return (long)Default_Str;
	}
}

long WINAPI DLLExport Expression_GetRecursiveIndex(LPRDATA rdPtr, long param1) {
	const std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	if (rdPtr->RecursiveIndex->contains(FuncName)) {
		return (long)(*rdPtr->RecursiveIndex)[FuncName];
	}
	else {
		return -1;
	}
}

long WINAPI DLLExport Expression_GetParamSize(LPRDATA rdPtr, long param1) {	
	return !rdPtr->FuncParamStack->empty()
		? (long)rdPtr->FuncParamStack->back().size()
		: NotInFuncError<long>();
}

long WINAPI DLLExport Expression_GetRetSize(LPRDATA rdPtr, long param1) {
	return (long)rdPtr->FuncReturn->size();
}

long WINAPI DLLExport Expression_TernaryRV(LPRDATA rdPtr,long param1) {
	const bool Result = (bool)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	long p1 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	const float ReturnA = *(float*)&p1;

	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	const float ReturnB = *(float*)&p2;
		
	return ReturnFloat(Result ? ReturnA : ReturnB);
}

long WINAPI DLLExport Expression_TernaryRS(LPRDATA rdPtr, long param1) {
	const bool Result = (bool)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	const std::wstring ReturnA = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
	const std::wstring ReturnB = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	NewStr(rdPtr->OutPut, Result ? ReturnA : ReturnB);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->OutPut;
}

long WINAPI DLLExport Expression_GetCurrentFuncName(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return	!rdPtr->FuncNameStack->empty()
		? (long)rdPtr->FuncNameStack->back().c_str()
		: NotInFuncError<std::wstring>();
}

long WINAPI DLLExport Expression_GetLoopIndex(LPRDATA rdPtr, long param1) {
	const std::wstring FuncName = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	const auto& name = GetFuncNameWithRecursiveID(rdPtr, FuncName);

	if (rdPtr->FuncLoopIndex->contains(name)) {
		return (long)(*rdPtr->FuncCurLoopIndex)[name];
	}
	else {
		return -1;
	}
}

long WINAPI DLLExport Expression_EquStr(LPRDATA rdPtr, long param1) {
	const std::wstring a = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	const std::wstring b = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	return a == b;
}

long WINAPI DLLExport Expression_EquVal(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	const float a = *(float*)&p1;

	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	const float b = *(float*)&p2;

	return abs(a - b) <= 1e-6;
}

long WINAPI DLLExport Expression_GreaterVal(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	const float a = *(float*)&p1;

	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	const float b = *(float*)&p2;

	return a > b;
}

long WINAPI DLLExport Expression_GreaterOrEqualVal(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	const float a = *(float*)&p1;

	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	const float b = *(float*)&p2;

	return a >= b;
}

long WINAPI DLLExport Expression_LowerVal(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	const float a = *(float*)&p1;

	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	const float b = *(float*)&p2;

	return a < b;
}

long WINAPI DLLExport Expression_LowerOrEqualVal(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	const float a = *(float*)&p1;

	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	const float b = *(float*)&p2;

	return a <= b;
}

long WINAPI DLLExport Expression_CastToBool(LPRDATA rdPtr, long param1) {
	const auto bRet = (bool(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_LONG)));

	return bRet;
}

long WINAPI DLLExport Expression_Negate(LPRDATA rdPtr, long param1) {
	const auto bRet= !(bool(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_LONG)));

	return bRet;
}

long WINAPI DLLExport Expression_And(LPRDATA rdPtr, long param1) {
	const auto a = bool(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_LONG));
	const auto b = bool(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_LONG));

	const auto bRet = a && b;

	return bRet;
}

long WINAPI DLLExport Expression_Or(LPRDATA rdPtr, long param1) {
	const auto a = bool(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_LONG));
	const auto b = bool(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_LONG));

	const auto bRet = a || b;

	return bRet;
}

long WINAPI DLLExport Expression_Xor(LPRDATA rdPtr, long param1) {
	const auto a = bool(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_LONG));
	const auto b = bool(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_LONG));

	const auto bRet = a ^ b;

	return bRet;
}

long WINAPI DLLExport Expression_GetObjectAlpha(LPRDATA rdPtr, long param1) {
	const long fixed = long(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_LONG));

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
			Condition_OnFunc,

			Condition_FuncHasParamAt,
			Condition_FuncHasReturnAt,
			
			Condition_FuncHasTempParam,
			Condition_CurerntFuncHasTempParam,

			Condition_FuncParamAtIsNum,
			Condition_FuncReturnAtIsNum,

			Condition_FuncTempParamIsNum,
			Condition_CurerntFuncTempParamIsNum,

			Condition_OnIterateObject,
			Condition_SelectAll,

			Condition_InEditor,
			Condition_InSubApp,

			Condition_OILHasObject,

			Condition_GlobalHasTempParam,

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action_SetReturnValueStr,
			Action_PushReturnValueStr,

			Action_CallFunc,

			Action_Ternary,

			Action_SetTempParamStr,
			Action_SetCurrentTempParamStr,

			Action_SetAllReturn,

			Action_SetLoopIndex,
			Action_StopLoop,

			Action_PassReturnStr,
			Action_PassReturnVal,

			Action_SetTempParamVal,
			Action_SetCurrentTempParamVal,

			Action_SetReturnValueVal,
			Action_PushReturnValueVal,

			Action_IterateObject,

			Action_SetGlobalParamStr,
			Action_SetGlobalParamVal,

			Action_Assert,
			Action_MsgBox,
			Action_Toast,
			Action_ToastFlags,

			Action_SetObjectAlpha,
			Action_SetObjectAlphaByFixed,

			Action_SetObjectRGBCoef,
			Action_SetObjectRGBCoefByFixed,

			Action_SaveScope,
			Action_RestoreScope,

			Action_IterateObjectFunc,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression_CallFuncRV,
			Expression_CallFuncRS,

			Expression_GetParamRV,
			Expression_GetParamRS,

			Expression_GetRetRV,
			Expression_GetRetRS,

			Expression_GetRecursiveIndex,

			Expression_GetParamSize,
			Expression_GetRetSize,

			Expression_TernaryRV,
			Expression_TernaryRS,

			Expression_GetTempParamRV,
			Expression_GetTempParamRS,

			Expression_GetCurrentFuncName,

			Expression_GetCurrentTempParamRV,
			Expression_GetCurrentTempParamRS,

			Expression_GetLoopIndex,

			Expression_GetGlobalParamRV,
			Expression_GetGlobalParamRS,

			Expression_EquStr,
			Expression_EquVal,

			Expression_CastToBool,
			Expression_Negate,
			Expression_And,
			Expression_Or,
			Expression_Xor,

			Expression_GreaterVal,
			Expression_GreaterOrEqualVal,
			Expression_LowerVal,
			Expression_LowerOrEqualVal,

			Expression_GetObjectAlpha,
			
			Expression_GetRawParam,

			0
			};