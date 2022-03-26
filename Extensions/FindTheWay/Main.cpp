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
		IDMN_CONDITION_SMBS, M_CONDITION_SMBS, CND_CONDITION_SMBS, EVFLAGS_ALWAYS, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_WIDTH, M_HEIGHT,
		IDMN_CONDITION_SMBB, M_CONDITION_SMBB, CND_CONDITION_SMBB, EVFLAGS_ALWAYS, 1, PARAM_EXPSTRING, M_BASE64,
		IDMN_CONDITION_SMBC, M_CONDITION_SMBC, CND_CONDITION_SMBC, EVFLAGS_ALWAYS, 6, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_GIRDSIZE, M_GIRDOFFSETX, M_GIRDOFFSETY, M_EVEIT, M_BASEALYER, M_TYPE,
		IDMN_CONDITION_OSMBC, M_CONDITION_OSMBC, CND_CONDITION_OSMBC, 0, 0,

		IDMN_CONDITION_OPF, M_CONDITION_OPF, CND_CONDITION_OPF, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 7, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPSTRING, M_STARTX, M_STARTY, M_DESTINATIONX, M_DESTINATIONY, M_FORCEFIND, M_USEREALCOORD, M_SAVENAME,

		IDMN_CONDITION_PA, M_CONDITION_PA, CND_CONDITION_PA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPSTRING, M_PATHNAME,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_SM, M_ACTION_SM, ACT_ACTION_SM,	0, 6, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_X, M_Y, M_COST, M_TYPE, M_USEREALCOORD, M_USEITERATECOORD,
		IDMN_ACTION_C, M_ACTION_C, ACT_ACTION_C,	0, 0,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_OSMBC_GX, M_EXPRESSION_OSMBC_GX, EXP_EXPRESSION_OSMBC_GX, 0, 0,
		IDMN_EXPRESSION_OSMBC_GY, M_EXPRESSION_OSMBC_GY, EXP_EXPRESSION_OSMBC_GY, 0, 0,

		IDMN_EXPRESSION_GS, M_EXPRESSION_GS, EXP_EXPRESSION_GS, 0, 7, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_STRING, M_STARTX, M_STARTY, M_DESTINATIONX, M_DESTINATIONY, M_FORCEFIND, M_USEREALCOORD, M_SAVENAME,
		IDMN_EXPRESSION_GSOP, M_EXPRESSION_GSOP, EXP_EXPRESSION_GSOP, 0, 1, EXPPARAM_STRING, M_PATHNAME,
		IDMN_EXPRESSION_GSCOP, M_EXPRESSION_GSCOP, EXP_EXPRESSION_GSCOP, 0, 3, EXPPARAM_STRING, EXPPARAM_LONG, EXPPARAM_LONG, M_PATHNAME, M_STEP, M_COORDTYPE,

		IDMN_EXPRESSION_GGC, M_EXPRESSION_GGC, EXP_EXPRESSION_GGC, 0, 1, EXPPARAM_LONG, M_COORD,
		IDMN_EXPRESSION_GRC, M_EXPRESSION_GRC, EXP_EXPRESSION_GRC, 0, 1, EXPPARAM_LONG, M_COORD,
		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport SetMapBySize(LPRDATA rdPtr, long param1, long param2) {
	delete rdPtr->pFTW;
	rdPtr->pFTW = nullptr;

	size_t width = (size_t)CNC_GetParameter(rdPtr);
	size_t height = (size_t)CNC_GetParameter(rdPtr);

	try {
		rdPtr->pFTW = new FindTheWayClass(width, height);
	}
	catch (Exception) {
		return FALSE;
	}

#ifdef _DEBUG
	auto map = rdPtr->pFTW->OutPutMapStr();
#endif // _DEBUG

	return TRUE;
}

long WINAPI DLLExport SetMapByBase64(LPRDATA rdPtr, long param1, long param2) {
	delete rdPtr->pFTW;
	rdPtr->pFTW = nullptr;

	wstring base64 = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	try {
		rdPtr->pFTW = new FindTheWayClass(base64);
	}
	catch (Exception) {
		return FALSE;
	}

#ifdef _DEBUG
	auto map = rdPtr->pFTW->OutPutMapStr();
#endif // _DEBUG

	return TRUE;
}

long WINAPI DLLExport SetMapByCollision(LPRDATA rdPtr, long param1, long param2) {
	delete rdPtr->pFTW;
	rdPtr->pFTW = nullptr;

	size_t girdSize = (size_t)CNC_GetParameter(rdPtr);
	size_t girdOffsetX = (size_t)CNC_GetParameter(rdPtr);
	size_t girdOffsetY = (size_t)CNC_GetParameter(rdPtr);

	bool eventIterate = (bool)CNC_GetParameter(rdPtr);
	size_t baseLayer = (size_t)CNC_GetParameter(rdPtr) - 1;		// Index start from 0, LAYER_ALL = -1 for All layer
	MapType type = (MapType)CNC_GetParameter(rdPtr);

	const auto& frameRect = rdPtr->rHo.hoAdRunHeader->rhFrame->m_leVirtualRect;

	auto frameWidth = frameRect.right - frameRect.left;
	auto frameHeight = frameRect.bottom - frameRect.top;

	size_t width = frameWidth / girdSize;
	size_t height = frameHeight / girdSize;

	try {
		rdPtr->pFTW = new FindTheWayClass(width, height);
	}
	catch (Exception) {
		return FALSE;
	}
	
	rdPtr->pFTW->SetGirdSize(girdSize, girdOffsetX, girdOffsetY);

	auto hoPtr = &(rdPtr->rHo);
	npWin win = hoPtr->hoAdRunHeader->rh4.rh4Mv->mvIdEditWin;

	int layerOffsetX = hoPtr->hoAdRunHeader->rh3.rh3DisplayX;
	int layerOffsetY = hoPtr->hoAdRunHeader->rh3.rh3DisplayY;

	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			rdPtr->itRealCoord = rdPtr->pFTW->GetRealCoord(Coord{ x,y });

			if (eventIterate) {
				rdPtr->itGirdCoord = Coord{ x,y };
				CallEvent(ONSETMAPBYCOLLISION);
			}
			// Ref: https://community.clickteam.com/threads/59029-Accessing-backdrops-from-extensions
			else if (!callColMaskTestPoint(hoPtr					// colMaskTestPoint in Java/OC/JS
				, rdPtr->itRealCoord.x - layerOffsetX
				, rdPtr->itRealCoord.y - layerOffsetY
				, baseLayer, 0)) {
				rdPtr->pFTW->SetMap(x, y, MAP_OBSTACLE, MapType::TERRAIN);
			}
		}
	}

#ifdef _DEBUG
	auto map = rdPtr->pFTW->OutPutMapStr(MapType::TERRAIN);
#endif // _DEBUG

	return TRUE;
}

long WINAPI DLLExport OnSetMapByCollision(LPRDATA rdPtr, long param1, long param2) {
	return TRUE;
}

long WINAPI DLLExport OnPathFound(LPRDATA rdPtr, long param1, long param2) {
	size_t startX = (size_t)CNC_GetParameter(rdPtr);
	size_t startY = (size_t)CNC_GetParameter(rdPtr);

	size_t destinationX = (size_t)CNC_GetParameter(rdPtr);
	size_t destinationY = (size_t)CNC_GetParameter(rdPtr);

	bool forceFind=(bool)CNC_GetParameter(rdPtr);
	bool useRealCoord = (bool)CNC_GetParameter(rdPtr);

	wstring saveName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	FindPath(rdPtr, Coord{ startX ,startY }, Coord{ destinationX ,destinationY }, forceFind, useRealCoord, saveName);

#ifdef _DEBUG
	auto map = rdPtr->pFTW->OutPutMapStr();
#endif // _DEBUG

	return rdPtr->pFTW->GetPathValidity();
}

long WINAPI DLLExport PathAvailable(LPRDATA rdPtr, long param1, long param2) {
	wstring pathName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	return rdPtr->pFTW->GetPathValidity(&pathName);
}

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport SetMap(LPRDATA rdPtr, long param1, long param2) {
	size_t x = (size_t)CNC_GetParameter(rdPtr);
	size_t y = (size_t)CNC_GetParameter(rdPtr);

	BYTE cost = (BYTE)CNC_GetParameter(rdPtr);
	MapType type = (MapType)CNC_GetParameter(rdPtr);

	bool useRealCoord = (bool)CNC_GetParameter(rdPtr);
	bool useIterateCoord = (bool)CNC_GetParameter(rdPtr);

	RetIfMapInvalid(0);

	if (useIterateCoord) {
		x = rdPtr->itGirdCoord.x;
		y = rdPtr->itGirdCoord.y;
	}
	else {
		if (useRealCoord) {
			Coord girdCoord = rdPtr->pFTW->GetGirdCoord(Coord{ x, y });

			x = girdCoord.x;
			y = girdCoord.y;
		}
	}

	rdPtr->pFTW->SetMap(x, y, cost, type);

	return 0;
}

short WINAPI DLLExport Continue(LPRDATA rdPtr, long param1, long param2) {
	return 0;		// Force fusion to evaluate conditions, as empty events will be skipped
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport OnSetMapByCollision_GetX(LPRDATA rdPtr, long param1) {
	return rdPtr->itRealCoord.x;
}
 
long WINAPI DLLExport OnSetMapByCollision_GetY(LPRDATA rdPtr, long param1) {
	return rdPtr->itRealCoord.y;
}

long WINAPI DLLExport GetStep(LPRDATA rdPtr, long param1) {
	size_t startX = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	size_t startY = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	size_t destinationX = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	size_t destinationY = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	bool forceFind = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	bool useRealCoord = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	wstring saveName = (LPCWSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	RetIfMapInvalid(STEP_UNREACHABLE);

	FindPath(rdPtr, Coord{ startX ,startY }, Coord{ destinationX ,destinationY }, forceFind, useRealCoord, saveName);

#ifdef _DEBUG
	auto map = rdPtr->pFTW->OutPutMapStr();
#endif // _DEBUG

	return rdPtr->pFTW->GetDistance();
}

long WINAPI DLLExport GetStepOfPath(LPRDATA rdPtr, long param1) {
	wstring pathName = (LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	RetIfMapInvalid(STEP_UNREACHABLE);

	if (pathName == L"") {
		return STEP_UNREACHABLE;
	}

	return rdPtr->pFTW->GetDistance(&pathName);
}

long WINAPI DLLExport GetStepCoordOfPath(LPRDATA rdPtr, long param1) {
	wstring pathName = (LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	size_t step = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	CoordType type = (CoordType)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	RetIfMapInvalid(COORD_INVALID);

	if (pathName == L"") {
		return COORD_INVALID;
	}

	return rdPtr->pFTW->GetCoordAtPath(step, type, &pathName);
}

long WINAPI DLLExport GetGirdCoord(LPRDATA rdPtr, long param1) {
	size_t coord = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	RetIfMapInvalid(COORD_INVALID);

	return rdPtr->pFTW->GetGirdCoord(Coord{ coord ,0 }).x;
}

long WINAPI DLLExport GetRealCoord(LPRDATA rdPtr, long param1) {
	size_t coord = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	RetIfMapInvalid(REAL_INVALID);

	return rdPtr->pFTW->GetRealCoord(Coord{ coord ,0 }).x;
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
			SetMapBySize,
			SetMapByBase64,
			SetMapByCollision,
			OnSetMapByCollision,
			
			OnPathFound,
			PathAvailable,
			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			SetMap,
			Continue,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			OnSetMapByCollision_GetX,
			OnSetMapByCollision_GetY,

			GetStep,
			GetStepOfPath,
			GetStepCoordOfPath,

			GetGirdCoord,
			GetRealCoord,

			0
			};