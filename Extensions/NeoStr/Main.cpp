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
		IDMN_ACTION_CS, M_ACTION_CS, ACT_ACTION_CS,	0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_WIDTH, M_HEIGHT,
		IDMN_ACTION_CSTR, M_ACTION_CSTR, ACT_ACTION_CSTR,	0, 1, PARAM_EXPSTRING, M_STR,
		IDMN_ACTION_CRS, M_ACTION_CRS, ACT_ACTION_CRS,	0, 1, PARAM_EXPRESSION, M_ROWSPACE,
		IDMN_ACTION_CCS, M_ACTION_CCS, ACT_ACTION_CCS,	0, 1, PARAM_EXPRESSION, M_COLSPACE,

		IDMN_ACTION_CHA, M_ACTION_CHA, ACT_ACTION_CHA,	0, 1, PARAM_EXPRESSION, M_HA,
		IDMN_ACTION_CVA, M_ACTION_CVA, ACT_ACTION_CVA,	0, 1, PARAM_EXPRESSION, M_VA,

		IDMN_ACTION_ASTR, M_ACTION_ASTR, ACT_ACTION_ASTR,	0, 1, PARAM_EXPSTRING, M_STR,
		IDMN_ACTION_ASTRNL, M_ACTION_ASTRNL, ACT_ACTION_ASTRNL,	0, 1, PARAM_EXPSTRING, M_STR,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GSTR, M_EXPRESSION_GSTR, EXP_EXPRESSION_GSTR, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GRS, M_EXPRESSION_GRS, EXP_EXPRESSION_GRS, 0, 0,
		IDMN_EXPRESSION_GCS, M_EXPRESSION_GCS, EXP_EXPRESSION_GCS, 0, 0,
		
		IDMN_EXPRESSION_GLCX, M_EXPRESSION_GLCX, EXP_EXPRESSION_GLCX, 0, 0,
		IDMN_EXPRESSION_GLCY, M_EXPRESSION_GLCY, EXP_EXPRESSION_GLCY, 0, 0,
		IDMN_EXPRESSION_GMW, M_EXPRESSION_GMW, EXP_EXPRESSION_GMW, 0, 0,

		IDMN_EXPRESSION_GHA, M_EXPRESSION_GHA, EXP_EXPRESSION_GHA, 0, 0,
		IDMN_EXPRESSION_GVA, M_EXPRESSION_GVA, EXP_EXPRESSION_GVA, 0, 0,

		IDMN_EXPRESSION_GTH, M_EXPRESSION_GTH, EXP_EXPRESSION_GTH, 0, 0,

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

long WINAPI DLLExport Condition(LPRDATA rdPtr, long param1, long param2)
{

//  **** Still use this method for 1 or 2 parameters ****	
//	if (param1==param2)	
//		return TRUE;

	long p1 = CNC_GetParameter(rdPtr);
	long p2 = CNC_GetParameter(rdPtr);
	long p3 = CNC_GetParameter(rdPtr);

	if ((p1 + p2)==p3)
		return TRUE;
		 
	return FALSE;
}


// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport Action_ChangeSize(LPRDATA rdPtr, long param1, long param2) {
	short width = (short)CNC_GetIntParameter(rdPtr);
	short height = (short)CNC_GetIntParameter(rdPtr);

	rdPtr->swidth = width;
	rdPtr->sheight = height;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_ChangeString(LPRDATA rdPtr, long param1, long param2) {
	LPCWSTR pStr = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	*rdPtr->pStr = pStr;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_AppendString(LPRDATA rdPtr, long param1, long param2) {
	LPCWSTR pStr = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pStr->append(pStr);

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_AppendStringNewLine(LPRDATA rdPtr, long param1, long param2) {
	LPCWSTR pStr = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pStr->append(L"\r\n");
	rdPtr->pStr->append(pStr);

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_ChangeRowSpace(LPRDATA rdPtr, long param1, long param2) {
	short nRowSpace = (short)CNC_GetIntParameter(rdPtr);

	rdPtr->nRowSpace = nRowSpace;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_ChangeColSpace(LPRDATA rdPtr, long param1, long param2) {
	short nColSpace = (short)CNC_GetIntParameter(rdPtr);

	rdPtr->nColSpace = nColSpace;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_ChangeHorizontalAlign(LPRDATA rdPtr, long param1, long param2) {
	short horizontalFlag = (short)CNC_GetIntParameter(rdPtr);
	
	rdPtr->dwAlignFlags = (rdPtr->dwAlignFlags & ~(DT_LEFT | DT_CENTER | DT_RIGHT)) | (horizontalFlag);

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_ChangeVerticalAlign(LPRDATA rdPtr, long param1, long param2) {
	short verticalFlag = (short)CNC_GetIntParameter(rdPtr);

	rdPtr->dwAlignFlags = (rdPtr->dwAlignFlags & ~(DT_TOP | DT_VCENTER | DT_BOTTOM)) | (verticalFlag);

	ReDisplay(rdPtr);

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Expression_GetString(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(rdPtr->pStr->c_str());
}

long WINAPI DLLExport Expression_GetRowSpace(LPRDATA rdPtr,long param1) {
	return rdPtr->nRowSpace;
}

long WINAPI DLLExport Expression_GetColSpace(LPRDATA rdPtr, long param1) {
	return rdPtr->nColSpace;
}

long WINAPI DLLExport Expression_GetLastCharX(LPRDATA rdPtr, long param1) {
	return UpdateLastCharPos(rdPtr).x;
}

long WINAPI DLLExport Expression_GetLastCharY(LPRDATA rdPtr, long param1) {
	return UpdateLastCharPos(rdPtr).y;
}

long WINAPI DLLExport Expression_GetMaxWidth(LPRDATA rdPtr, long param1) {
	return UpdateLastCharPos(rdPtr).maxWidth;
}

long WINAPI DLLExport Expression_GetTotalHeight(LPRDATA rdPtr, long param1) {
	return UpdateLastCharPos(rdPtr).totalHeight;
}

long WINAPI DLLExport Expression_GetHorizontalAlign(LPRDATA rdPtr, long param1) {
	if (rdPtr->dwAlignFlags & DT_CENTER) {
		return DT_CENTER;
	}
	if (rdPtr->dwAlignFlags & DT_RIGHT) {
		return DT_RIGHT;
	}

	return DT_LEFT;
}

long WINAPI DLLExport Expression_GetVerticalAlign(LPRDATA rdPtr, long param1) {
	if (rdPtr->dwAlignFlags & DT_VCENTER) {
		return DT_VCENTER;
	}
	if (rdPtr->dwAlignFlags & DT_BOTTOM) {
		return DT_BOTTOM;
	}

	return DT_TOP;
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
			Action_ChangeSize,
			Action_ChangeString,
			Action_ChangeRowSpace,
			Action_ChangeColSpace,
			
			Action_ChangeHorizontalAlign,
			Action_ChangeVerticalAlign,
			
			Action_AppendString,
			Action_AppendStringNewLine,
			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression_GetString,
			Expression_GetRowSpace,
			Expression_GetColSpace,
			
			Expression_GetLastCharX,
			Expression_GetLastCharY,
			Expression_GetMaxWidth,

			Expression_GetHorizontalAlign,
			Expression_GetVerticalAlign,

			Expression_GetTotalHeight,
			
			0
			};