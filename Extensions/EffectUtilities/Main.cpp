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
		IDMN_CONDITION, M_CONDITION, CND_CONDITION, EVFLAGS_ALWAYS, 3, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_CND_P1, M_CND_P2, M_CND_P3,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
IDMN_ACTION_SE, M_ACTION_SE, ACT_ACTION_SE,	0, 2, PARAM_OBJECT, PARAM_EFFECT, M_OBJECT, M_EFFECT,
		IDMN_ACTION_SEP, M_ACTION_SEP, ACT_ACTION_SEP, 0, 3, PARAM_OBJECT, PARAM_EXPSTRING, PARAM_EXPRESSION, M_OBJECT, M_PARAMNAME, M_PARAM,

		IDMN_ACTION_SBE, M_ACTION_SBE, ACT_ACTION_SBE,	0, 2, PARAM_EXPSTRING, PARAM_EFFECT, M_BACKDROP, M_EFFECT,
		IDMN_ACTION_SBEP, M_ACTION_SBEP, ACT_ACTION_SBEP, 0, 3, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPRESSION, M_BACKDROP, M_PARAMNAME, M_PARAM,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION, M_EXPRESSION, EXP_EXPRESSION, 0, 3, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, 0, 0, 0,
		
		//Note in the following.  If you are returning a string, you set the EXPFLAG_STRING.	
		IDMN_EXPRESSION2, M_EXPRESSION2, EXP_EXPRESSION2, EXPFLAG_STRING, 1, EXPPARAM_STRING, 0,
		
		//Note in the following.  If you are returning a float, you set the EXPFLAG_DOUBLE
		IDMN_EXPRESSION3, M_EXPRESSION3, EXP_EXPRESSION3, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG, 0,
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

long WINAPI DLLExport Condition(LPRDATA rdPtr, long param1, long param2) {

//  **** Still use this method for 1 or 2 parameters ****	
//	if (param1==param2)	
//		return TRUE;

const long p1 = CNC_GetParameter(rdPtr);
const long p2 = CNC_GetParameter(rdPtr);
const long p3 = CNC_GetParameter(rdPtr);

	if ((p1 + p2)==p3)
		return TRUE;
		 
	return FALSE;
}


// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport Action_SetEffect(LPRDATA rdPtr, long param1, long param2) {
	const auto pObject = (LPRO)CNC_GetParameter(rdPtr);
	const auto pEffectName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (!LPROValid(pObject, IDENTIFIER_ACTIVE)) {
		return 0;
	}

	rdPtr->pEffectUtilities->SetEffect(pObject, pEffectName);

	return 0;
}

short WINAPI DLLExport Action_SetEffectParam(LPRDATA rdPtr, long param1, long param2) {
	const auto pObject = (LPRO)CNC_GetParameter(rdPtr);
	const auto pParamName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	long param = (long)CNC_GetParameter(rdPtr);

	if (!LPROValid(pObject, IDENTIFIER_ACTIVE)) {
		return 0;
	}

	const auto pEffect = (CEffectEx*)pObject->ros.rsEffectParam;
	const auto paramType = EffectUtilities::GetParamType(pEffect, pParamName);

	rdPtr->pEffectUtilities->SetParamEx(pEffect, pParamName, paramType,
		paramType == EFFECTPARAM_SURFACE
		? ConvertToType<long, LPSURFACE>(param)
		: static_cast<void*>(&param));

	rdPtr->pEffectUtilities->ModifSpriteEffect(pObject);

	return 0;
}

short WINAPI DLLExport Action_SetBackDropEffect(LPRDATA rdPtr, long param1, long param2) {
	auto pBackDropName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	auto pEffectName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

#ifdef ENABLE_TEST_FEATURE
	rdPtr->pEffectUtilities->SetEffect(pBackDropName, pEffectName);
#endif

	return 0;
}

short WINAPI DLLExport Action_SetBackDropEffectParam(LPRDATA rdPtr, long param1, long param2) {
	const auto pBackDropName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto pParamName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	long param = (long)CNC_GetParameter(rdPtr);

	const auto pEffectVec = rdPtr->pEffectUtilities->GetEffect(pBackDropName);

	for (const auto& it : pEffectVec) {
		const auto pEffect = it.pEffect;
		const auto paramType = EffectUtilities::GetParamType(pEffect, pParamName);

		rdPtr->pEffectUtilities->SetParamEx(pEffect, pParamName, paramType,
			paramType == EFFECTPARAM_SURFACE
			? ConvertToType<long, LPSURFACE>(param)
			: static_cast<void*>(&param));

		rdPtr->pEffectUtilities->ModifSpriteEffect(it.pSpr);
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
long WINAPI DLLExport Expression(LPRDATA rdPtr,long param1) {
	const long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	const long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	const long p3 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	// Performs the wonderfull calculation
	return p1+p2+p3;
}


//Reverse the string passed in.
long WINAPI DLLExport Expression2(LPRDATA rdPtr,long param1) {
	char *temp;

	const long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	//I'm storing the string pointer returned into a char *
	temp = (LPSTR)p1;

	//Reversing the string.
	_strrev(temp);
	
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;
	
	//This returns a pointer to the string for MMF.
	return (long)temp;
}

//Divide the float by 2.
long WINAPI DLLExport Expression3(LPRDATA rdPtr,long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);

	//Floats are tricky.  If you want to pass in a float, you must do the
	//following to convert the long to a true float, but only when you use
	//TYPE_FLOAT.
	float fp1 = *(float *)&p1;

	//Just doing simple math now.
	fp1 /=2;

	//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
	rdPtr->rHo.hoFlags |= HOF_FLOAT;

	//Return the float without conversion
	return *((int*)&fp1);
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
			Condition,

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action_SetEffect,
			Action_SetEffectParam,

			Action_SetBackDropEffect,
			Action_SetBackDropEffectParam,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression,
			Expression2,
			Expression3,

			0
			};