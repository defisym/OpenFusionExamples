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
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_SM, M_ACTION_SM, ACT_ACTION_SM,	0, 6, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_X, M_Y, M_COST, M_TYPE, M_USEREALCOORD, M_USEITERATECOORD,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_OSMBC_GX, M_EXPRESSION_OSMBC_GX, EXP_EXPRESSION_OSMBC_GX, 0, 0,
		IDMN_EXPRESSION_OSMBC_GY, M_EXPRESSION_OSMBC_GY, EXP_EXPRESSION_OSMBC_GY, 0, 0,
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
			else if (!callColMaskTestPoint(hoPtr
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

	wstring saveName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	Coord start = Coord{ startX ,startY };
	Coord destination = Coord{ destinationX ,destinationY };

	if (useRealCoord) {
		start = rdPtr->pFTW->GetGirdCoord(start);
		destination = rdPtr->pFTW->GetGirdCoord(destination);
	}

	if (!forceFind) {
		rdPtr->pFTW->Find(start, destination, rdPtr->diagonal);
	}
	else {
		rdPtr->pFTW->ForceFind(start, destination, rdPtr->diagonal);
	}
	
	if (saveName != L"") {
		rdPtr->pFTW->SaveLastPath(saveName);
	}

#ifdef _DEBUG
	auto map = rdPtr->pFTW->OutPutMapStr();
#endif // _DEBUG

	return rdPtr->pFTW->GetPathValidity();
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
			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			//SetMapByCollision,
			SetMap,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			OnSetMapByCollision_GetX,
			OnSetMapByCollision_GetY,

			0
			};