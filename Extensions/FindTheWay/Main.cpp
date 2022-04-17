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
		IDMN_CONDITION_SMBS, M_CONDITION_SMBS, CND_CONDITION_SMBS, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_WIDTH, M_HEIGHT,
		IDMN_CONDITION_SMBB, M_CONDITION_SMBB, CND_CONDITION_SMBB, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPSTRING, M_BASE64,
		IDMN_CONDITION_SMBC, M_CONDITION_SMBC, CND_CONDITION_SMBC, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 7, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPSTRING, M_GRIDSIZE, M_GRIDOFFSETX, M_GRIDOFFSETY, M_EVEIT, M_BASELAYER, M_TYPE, M_ITNAME,
		IDMN_CONDITION_OSMBC, M_CONDITION_OSMBC, CND_CONDITION_OSMBC, 0, 1, PARAM_EXPSTRING, M_ITNAME,

		IDMN_CONDITION_OPF, M_CONDITION_OPF, CND_CONDITION_OPF, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 10, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPSTRING, M_STARTX, M_STARTY, M_DESTINATIONX, M_DESTINATIONY, M_IGNOREFLAG, M_DIAGONAL, M_CHECKDIAGONALCORNER, M_FORCEFIND, M_USEREALCOORD, M_SAVENAME,

		IDMN_CONDITION_PA, M_CONDITION_PA, CND_CONDITION_PA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPSTRING, M_PATHNAME,
		
		IDMN_CONDITION_OITP, M_CONDITION_OITP, CND_CONDITION_OITP, 0, 1, PARAM_EXPSTRING, M_ITNAME,
		
		IDMN_CONDITION_MA, M_CONDITION_MA, CND_CONDITION_MA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		IDMN_CONDITION_CMCAC, M_CONDITION_CMCAC, CND_CONDITION_CMCAC, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 5, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION,M_X, M_Y, M_COST_CHECK, M_TYPE, M_USEREALCOORD,

		IDMN_CONDITION_OAG, M_CONDITION_OAG, CND_CONDITION_OAG, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 3, PARAM_OBJECT, PARAM_EXPRESSION, PARAM_EXPRESSION, M_OBJECT, M_X, M_Y,
		IDMN_CONDITION_OAO, M_CONDITION_OAO, CND_CONDITION_OAO, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2, PARAM_OBJECT, PARAM_EXPRESSION, M_OBJECT, M_TYPE,
		IDMN_CONDITION_OAC, M_CONDITION_OAC, CND_CONDITION_OAC, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 3, PARAM_OBJECT, PARAM_EXPRESSION, PARAM_EXPRESSION, M_OBJECT, M_X, M_Y,

		IDMN_CONDITION_SMBP, M_CONDITION_SMBP, CND_CONDITION_SMBP, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 4, PARAM_EXPSTRING, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_FILEPATH, M_GRIDSIZE, M_GRIDOFFSETX, M_GRIDOFFSETY,

		IDMN_CONDITION_CA, M_CONDITION_CA, CND_CONDITION_CA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 10, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_LINEMODE, M_ATTACK, M_X, M_Y, M_USEREALCOORD, M_RANGE, M_STARTRANGE, M_IGNOREFLAG, M_ALLRANGE, M_ALLRANGEATTACKRANGE,
		IDMN_CONDITION_OITA, M_CONDITION_OITA, CND_CONDITION_OITA, 0, 1, PARAM_EXPSTRING, M_ITNAME,
		IDMN_CONDITION_OITAA, M_CONDITION_OITAA, CND_CONDITION_OITAA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		IDMN_CONDITION_AITA, M_CONDITION_AITA, CND_CONDITION_AITA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,

		IDMN_CONDITION_OMC, M_CONDITION_OMC, CND_CONDITION_OMC, 0, 0,

		IDMN_CONDITION_OCOZ, M_CONDITION_OCOZ, CND_CONDITION_OCOZ, 0,3, PARAM_OBJECT, PARAM_OBJECT, PARAM_EXPSTRING, M_OBJECT, M_OBJECT, M_ITNAME,
		IDMN_CONDITION_ZV, M_CONDITION_ZV, CND_CONDITION_ZV, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_OBJECT, M_OBJECT,
		IDMN_CONDITION_ZAA, M_CONDITION_ZAA, CND_CONDITION_ZAA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2, PARAM_OBJECT, PARAM_EXPRESSION, M_OBJECT, M_ATTACKAREA,
		IDMN_CONDITION_SA, M_CONDITION_SA, CND_CONDITION_SA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_OBJECT, M_OBJECT,
		IDMN_CONDITION_OAOBJ, M_CONDITION_OAOBJ, CND_CONDITION_OAOBJ, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2, PARAM_OBJECT, PARAM_OBJECT, M_OBJECT, M_OBJECT,
		IDMN_CONDITION_POAO, M_CONDITION_POAO, CND_CONDITION_POAO, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_OBJECT, M_OBJECT,

		IDMN_CONDITION_NOAG, M_CONDITION_NOAG, CND_CONDITION_NOAG, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 3, PARAM_OBJECT, PARAM_EXPRESSION, PARAM_EXPRESSION, M_OBJECT, M_X, M_Y,
		IDMN_CONDITION_NOAC, M_CONDITION_NOAC, CND_CONDITION_NOAC, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 3, PARAM_OBJECT, PARAM_EXPRESSION, PARAM_EXPRESSION, M_OBJECT, M_X, M_Y,

		IDMN_CONDITION_SMBA, M_CONDITION_SMBA, CND_CONDITION_SMBA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 5, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_FIXED, M_FRAME, M_GRIDSIZE, M_GRIDOFFSETX, M_GRIDOFFSETY,
		IDMN_CONDITION_SMBSF, M_CONDITION_SMBSF, CND_CONDITION_SMBSF, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 4, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_PSF, M_GRIDSIZE, M_GRIDOFFSETX, M_GRIDOFFSETY,

		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_SM, M_ACTION_SM, ACT_ACTION_SM,	0, 5, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_X, M_Y, M_COST, M_TYPE, M_USEREALCOORD,
		
		IDMN_ACTION_C, M_ACTION_C, ACT_ACTION_C,	0, 0,
		
		IDMN_ACTION_ITP, M_ACTION_ITP, ACT_ACTION_ITP,	0, 3, PARAM_EXPSTRING, PARAM_EXPRESSION, PARAM_EXPSTRING, M_PATHNAME, M_USEREALCOORD, M_ITNAME,

		IDMN_ACTION_SMBO, M_ACTION_SMBO, ACT_ACTION_SMBO,	0, 3, PARAM_OBJECT, PARAM_EXPRESSION, PARAM_EXPRESSION, M_OBJECT, M_COST, M_TYPE,
		IDMN_ACTION_CM, M_ACTION_CM, ACT_ACTION_CM,	0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_COST, M_TYPE,

		IDMN_ACTION_SZBO, M_ACTION_SZBO, ACT_ACTION_SZBO,	0, 2, PARAM_OBJECT, PARAM_EXPRESSION, M_OBJECT, M_CENTER,
		IDMN_ACTION_CZ, M_ACTION_CZ, ACT_ACTION_CZ,	0, 0,

		IDMN_ACTION_ITA, M_ACTION_ITA, ACT_ACTION_ITA,	0, 2, PARAM_EXPSTRING, PARAM_EXPRESSION, M_ITNAME, M_COUNT,

		IDMN_ACTION_SUBO, M_ACTION_SUBO, ACT_ACTION_SUBO,	0, 2, PARAM_OBJECT, PARAM_EXPRESSION, M_OBJECT, M_ENEMY,
		IDMN_ACTION_CU, M_ACTION_CU, ACT_ACTION_CU,	0, 1, PARAM_EXPRESSION, M_ENEMY,

		IDMN_ACTION_COZ, M_ACTION_COZ, ACT_ACTION_COZ, 0, 2, PARAM_OBJECT, PARAM_CREATE, M_OBJECT, M_OBJECT,

		IDMN_ACTION_SS, M_ACTION_SS, ACT_ACTION_SS, 0, 1, PARAM_EXPRESSION, M_STASH,

		IDMN_ACTION_COZBE, M_ACTION_COZBE, ACT_ACTION_COZBE, 0, 2, PARAM_OBJECT, PARAM_EXPSTRING, M_OBJECT, M_ITNAME,
		IDMN_ACTION_COZBN, M_ACTION_COZBN, ACT_ACTION_COZBN, 0, 2, PARAM_OBJECT, PARAM_EXPSTRING, M_OBJECT, M_OBJECT,

		IDMN_ACTION_CAO, M_ACTION_CAO, ACT_ACTION_CAO, 0, 1, PARAM_CREATE, M_OBJECT,
		IDMN_ACTION_CABNO, M_ACTION_CABNO, ACT_ACTION_CABNO, 0, 1, PARAM_EXPSTRING, M_OBJECT,

		IDMN_ACTION_CAOE, M_ACTION_CAOE, ACT_ACTION_CAOE, 0, 5, PARAM_CREATE, PARAM_OBJECT, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_OBJECT, M_OBJECT, M_DIR, M_AOETYPE, M_IGNOREFLAG,

		IDMN_ACTION_CAOEBN, M_ACTION_CAOEBN, ACT_ACTION_CAOEBN, 0, 5, PARAM_EXPSTRING, PARAM_OBJECT, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_OBJECT, M_OBJECT, M_DIR, M_AOETYPE, M_IGNOREFLAG,

		IDMN_ACTION_CG, M_ACTION_CG, ACT_ACTION_CG, 0, 2, PARAM_OBJECT, PARAM_EXPRESSION, M_OBJECT, M_LAYER,

		IDMN_ACTION_SGS, M_ACTION_SGS, ACT_ACTION_SGS, 0, 3, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_GRIDSIZE, M_GRIDOFFSETX, M_GRIDOFFSETY,

		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GITX, M_EXPRESSION_GITX, EXP_EXPRESSION_GITX, 0, 0,
		IDMN_EXPRESSION_GITY, M_EXPRESSION_GITY, EXP_EXPRESSION_GITY, 0, 0,

		IDMN_EXPRESSION_GS, M_EXPRESSION_GS, EXP_EXPRESSION_GS, 0, 10, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_STRING, M_STARTX_EXP, M_STARTY_EXP, M_DESTINATIONX_EXP, M_DESTINATIONY_EXP, M_IGNOREFLAG, M_DIAGONAL_EXP, M_CHECKDIAGONALCORNER_EXP, M_FORCEFIND_EXP, M_USEREALCOORD_EXP, M_SAVENAME_EXP,
		IDMN_EXPRESSION_GSOP, M_EXPRESSION_GSOP, EXP_EXPRESSION_GSOP, 0, 1, EXPPARAM_STRING, M_PATHNAME,
		IDMN_EXPRESSION_GSCOP, M_EXPRESSION_GSCOP, EXP_EXPRESSION_GSCOP, 0, 3, EXPPARAM_STRING, EXPPARAM_LONG, EXPPARAM_LONG, M_PATHNAME, M_STEP, M_COORDTYPE,

		IDMN_EXPRESSION_GGC, M_EXPRESSION_GGC, EXP_EXPRESSION_GGC, 0, 3, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, M_COORD, M_COORD, M_COORDTYPE,
		IDMN_EXPRESSION_GRC, M_EXPRESSION_GRC, EXP_EXPRESSION_GRC, 0, 3, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, M_COORD, M_COORD, M_COORDTYPE,

		IDMN_EXPRESSION_GITI, M_EXPRESSION_GITI, EXP_EXPRESSION_GITI, 0, 0,

		IDMN_EXPRESSION_GMC, M_EXPRESSION_GMC, EXP_EXPRESSION_GMC, 0, 4, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, M_X, M_Y, M_USEREALCOORD, M_TYPE,
		IDMN_EXPRESSION_GMB64, M_EXPRESSION_GMB64, EXP_EXPRESSION_GMB64, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GMS, M_EXPRESSION_GMS, EXP_EXPRESSION_GMS, EXPFLAG_STRING, 3, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_STRING, M_TYPE, M_SHOWPATH, M_PATHNAME,

		IDMN_EXPRESSION_GALR, M_EXPRESSION_GALR, EXP_EXPRESSION_GALR, 0, 7, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, M_X, M_Y, M_USEREALCOORD, M_DIR, M_RANGE, M_IGNOREFLAG, M_ATTACK,

		IDMN_EXPRESSION_GIF, M_EXPRESSION_GIF, EXP_EXPRESSION_GIF, 0, 5, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, M_MIZ, M_MIA, M_MIE, M_AIA, M_AIE,

		IDMN_EXPRESSION_GMTM, M_EXPRESSION_GMTM, EXP_EXPRESSION_GMTM, 0, 0,
		IDMN_EXPRESSION_GMTT, M_EXPRESSION_GMTT, EXP_EXPRESSION_GMTT, 0, 0,
		IDMN_EXPRESSION_GMTD, M_EXPRESSION_GMTD, EXP_EXPRESSION_GMTD, 0, 0,

		IDMN_EXPRESSION_GMCP, M_EXPRESSION_GMCP, EXP_EXPRESSION_GMCP, 0, 0,
		IDMN_EXPRESSION_GMCO, M_EXPRESSION_GMCO, EXP_EXPRESSION_GMCO, 0, 0,
		IDMN_EXPRESSION_GMCV, M_EXPRESSION_GMCV, EXP_EXPRESSION_GMCV, 0, 1, EXPPARAM_LONG, M_COST,

		IDMN_EXPRESSION_GIGS, M_EXPRESSION_GIGS, EXP_EXPRESSION_GIGS, 0, 2, EXPPARAM_LONG, EXPPARAM_LONG, M_ISOGW, M_ISOGH,

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
		rdPtr->pFTW->SetUpdateMapCallBack(UpdateMapCallBackFunc, rdPtr);
		rdPtr->pFTW->SetIsometric(rdPtr->isometric);
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

	wstring base64 = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	try {
		rdPtr->pFTW = new FindTheWayClass(base64);
		rdPtr->pFTW->SetUpdateMapCallBack(UpdateMapCallBackFunc, rdPtr);
		rdPtr->pFTW->SetIsometric(rdPtr->isometric);
	}
	catch (Exception) {
		return FALSE;
	}

#ifdef _DEBUG
	auto map = rdPtr->pFTW->OutPutMapStr();
#endif // _DEBUG

	return TRUE;
}

long WINAPI DLLExport SetMapByPicture(LPRDATA rdPtr, long param1, long param2) {
	delete rdPtr->pFTW;
	rdPtr->pFTW = nullptr;

	wstring filePath = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	size_t gridSize = (size_t)CNC_GetParameter(rdPtr);
	size_t gridOffsetX = (size_t)CNC_GetParameter(rdPtr);
	size_t gridOffsetY = (size_t)CNC_GetParameter(rdPtr);

	auto pSf = new cSurface();

	CImageFilterMgr* pImgMgr = rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvImgFilterMgr;
	CImageFilter    pFilter(pImgMgr);

	ImportImage(pImgMgr, filePath.c_str(), pSf, 0, 0);

	auto ret = SetMapBySurface(rdPtr, pSf, gridSize, gridOffsetX, gridOffsetY);

	delete pSf;

	if (ret) {
		rdPtr->pFTW->SetIsometric(rdPtr->isometric);
	}

#ifdef _DEBUG
	auto map = rdPtr->pFTW->OutPutMapStr(MapType::TERRAIN);
#endif // _DEBUG

	return ret;
}

long WINAPI DLLExport SetMapByActive(LPRDATA rdPtr, long param1, long param2) {
	delete rdPtr->pFTW;
	rdPtr->pFTW = nullptr;

	long fixed = (long)CNC_GetParameter(rdPtr);

	size_t frame = (size_t)CNC_GetParameter(rdPtr);

	size_t gridSize = (size_t)CNC_GetParameter(rdPtr);
	size_t gridOffsetX = (size_t)CNC_GetParameter(rdPtr);
	size_t gridOffsetY = (size_t)CNC_GetParameter(rdPtr);

	LPRO object = LproFromFixed(rdPtr, fixed);

	if (!LPROValid(object, IDENTIFIER_ACTIVE)) {		// must be active object
		return 0;
	}

	cSurface imageSurface;
	auto rhPtr = rdPtr->rHo.hoAdRunHeader;
	short nFrame = object->roa.raAnimDirOffset->adNumberOfFrame;

	if (frame >= (size_t)nFrame) {
		return false;
	}

	DWORD hImage = object->roa.raAnimDirOffset->adFrame[frame];
	LockImageSurface(rhPtr->rhIdAppli, hImage, imageSurface);

	auto ret = SetMapBySurface(rdPtr, &imageSurface, gridSize, gridOffsetX, gridOffsetY);

	UnlockImageSurface(imageSurface);

	if (ret) {
		rdPtr->pFTW->SetIsometric(rdPtr->isometric);
	}

#ifdef _DEBUG
	auto map = rdPtr->pFTW->OutPutMapStr(MapType::TERRAIN);
#endif // _DEBUG

	return ret;
}

long WINAPI DLLExport SetMapBySurface(LPRDATA rdPtr, long param1, long param2) {
	delete rdPtr->pFTW;
	rdPtr->pFTW = nullptr;

	long p1 = (long)CNC_GetParameter(rdPtr);
	cSurface* imageSurface = ConvertToType<cSurface*>(p1);	

	size_t gridSize = (size_t)CNC_GetParameter(rdPtr);
	size_t gridOffsetX = (size_t)CNC_GetParameter(rdPtr);
	size_t gridOffsetY = (size_t)CNC_GetParameter(rdPtr);

	auto ret = SetMapBySurface(rdPtr, imageSurface, gridSize, gridOffsetX, gridOffsetY);

	if (ret) {
		rdPtr->pFTW->SetIsometric(rdPtr->isometric);
	}

#ifdef _DEBUG
	auto map = rdPtr->pFTW->OutPutMapStr(MapType::TERRAIN);
#endif // _DEBUG

	return ret;
}

long WINAPI DLLExport SetMapByCollision(LPRDATA rdPtr, long param1, long param2) {
	delete rdPtr->pFTW;
	rdPtr->pFTW = nullptr;

	size_t gridSize = (size_t)CNC_GetParameter(rdPtr);
	size_t gridOffsetX = (size_t)CNC_GetParameter(rdPtr);
	size_t gridOffsetY = (size_t)CNC_GetParameter(rdPtr);

	bool eventIterate = (bool)CNC_GetParameter(rdPtr);
	int baseLayer = (int)CNC_GetParameter(rdPtr) - 1;		// Index start from 0, LAYER_ALL = -1 for All layer
	MapType type = (MapType)CNC_GetParameter(rdPtr);

	*rdPtr->pOnItCollisionName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	RetIfSetMapDirectly(type, FALSE);

	const auto& frameRect = rdPtr->rHo.hoAdRunHeader->rhFrame->m_leVirtualRect;

	auto frameWidth = frameRect.right - frameRect.left;
	auto frameHeight = frameRect.bottom - frameRect.top;

	size_t width = FindTheWayClass::CalcMapWidth(frameWidth, gridSize, rdPtr->isometric);
	size_t height = FindTheWayClass::CalcMapHeight(frameHeight, gridSize, rdPtr->isometric); 

	try {
		rdPtr->pFTW = new FindTheWayClass(width, height);
		rdPtr->pFTW->SetUpdateMapCallBack(UpdateMapCallBackFunc, rdPtr);
		rdPtr->pFTW->SetIsometric(rdPtr->isometric);
	}
	catch (Exception) {
		return FALSE;
	}
	
	rdPtr->pFTW->SetGridSize(gridSize, gridOffsetX, gridOffsetY);

	auto hoPtr = &(rdPtr->rHo);
	npWin win = hoPtr->hoAdRunHeader->rh4.rh4Mv->mvIdEditWin;

	int layerOffsetX = hoPtr->hoAdRunHeader->rh3.rh3DisplayX;
	int layerOffsetY = hoPtr->hoAdRunHeader->rh3.rh3DisplayY;

	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			rdPtr->itCoord = rdPtr->pFTW->GetRealCoord(Coord{ x,y });

			if (eventIterate) {
				CallEvent(ONSETMAPBYCOLLISION);
			}
			// Ref: https://community.clickteam.com/threads/59029-Accessing-backdrops-from-extensions
			else if (!callColMaskTestPoint(hoPtr					// colMaskTestPoint in Java/OC/JS
				, rdPtr->itCoord.x - layerOffsetX
				, rdPtr->itCoord.y - layerOffsetY
				, baseLayer, 0)) {
				rdPtr->pFTW->SetMap(x, y, MAP_OBSTACLE, type);
			}
		}
	}

#ifdef _DEBUG
	auto map = rdPtr->pFTW->OutPutMapStr(MapType::TERRAIN);
#endif // _DEBUG

	return TRUE;
}

long WINAPI DLLExport OnSetMapByCollision(LPRDATA rdPtr, long param1, long param2) {
	wstring iterateName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	return (*rdPtr->pOnItCollisionName) == iterateName;
}

long WINAPI DLLExport OnPathFound(LPRDATA rdPtr, long param1, long param2) {
	size_t startX = (size_t)CNC_GetParameter(rdPtr);
	size_t startY = (size_t)CNC_GetParameter(rdPtr);	

	size_t destinationX = (size_t)CNC_GetParameter(rdPtr);
	size_t destinationY = (size_t)CNC_GetParameter(rdPtr);

	size_t ignoreFlag = (size_t)CNC_GetParameter(rdPtr);

	bool diagonal = (bool)CNC_GetParameter(rdPtr);
	bool checkDiagonalCorner = (bool)CNC_GetParameter(rdPtr);

	bool forceFind=(bool)CNC_GetParameter(rdPtr);
	bool useRealCoord = (bool)CNC_GetParameter(rdPtr);

	wstring saveName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	FindPath(rdPtr, Coord{ startX ,startY }, Coord{ destinationX ,destinationY }, ignoreFlag, diagonal, checkDiagonalCorner, forceFind, useRealCoord, saveName);

#ifdef _DEBUG
	auto map = rdPtr->pFTW->OutPutMapStr();
#endif // _DEBUG

	return rdPtr->pFTW->GetPathValidity();
}

long WINAPI DLLExport PathAvailable(LPRDATA rdPtr, long param1, long param2) {
	wstring pathName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	RetIfMapInvalid(FALSE);

	return rdPtr->pFTW->GetPathValidity(&pathName);
}

long WINAPI DLLExport OnIteratePath(LPRDATA rdPtr, long param1, long param2) {
	wstring iterateName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	return (*rdPtr->pOnItPathName) == iterateName;
}

long WINAPI DLLExport MapAvailable(LPRDATA rdPtr, long param1, long param2) {
	RetIfMapInvalid(FALSE);

	return TRUE;
}

long WINAPI DLLExport CheckMapCostAtPoint(LPRDATA rdPtr, long param1, long param2) {
	size_t x = (size_t)CNC_GetParameter(rdPtr);
	size_t y = (size_t)CNC_GetParameter(rdPtr);

	int cost = (int)CNC_GetParameter(rdPtr);
	MapType type = (MapType)CNC_GetParameter(rdPtr);

	bool useRealCoord = (bool)CNC_GetParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	Coord gridCoord = Coord{ x, y };

	if (useRealCoord) {
		gridCoord = rdPtr->pFTW->GetGridCoord(gridCoord);
	}

	auto& [gridX, gridY] = gridCoord;

	auto mapCost = rdPtr->pFTW->GetMap(gridX, gridY, type);

	if (cost ==-1 && mapCost != MAP_OBSTACLE) {					// Check for path
		return TRUE;
	}
	else if (cost == 65536 && mapCost == MAP_OBSTACLE) {		// Check for obstacle
		return TRUE;
	}
	else if (cost == mapCost) {									// Check accurate cost
		return TRUE;
	}
	else {
		return FALSE;
	}
}

long WINAPI DLLExport ObjectAtGrid(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);
	size_t x = (size_t)CNC_GetParameter(rdPtr);
	size_t y = (size_t)CNC_GetParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	return rdPtr->pSelect->FilterObjects(rdPtr, oil, negated, [&](LPRDATA rdPtr, LPRO object)->bool {
		if (object->roHo.hoX < 0 || object->roHo.hoY < 0) {
			return false;
		}
		else {
			return Coord{ x,y } == rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX, (size_t)object->roHo.hoY });
		}
		});
}

long WINAPI DLLExport NoObjectAtGrid(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);
	size_t x = (size_t)CNC_GetParameter(rdPtr);
	size_t y = (size_t)CNC_GetParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	try {
		rdPtr->pSelect->ForEach(rdPtr, oil, [&](LPRO object) {
			Coord objectCoord = rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX, (size_t)object->roHo.hoY });
			if (objectCoord == Coord{ x,y }) {
				throw false;
			}
		});
	}
	catch (bool) {
		return false;
	}

	return true;
}

long WINAPI DLLExport ObjectAtObstacle(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);
	MapType type = (MapType)CNC_GetParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	return rdPtr->pSelect->FilterObjects(rdPtr, oil, negated, [&](LPRDATA rdPtr, LPRO object)->bool {
		if (object->roHo.hoX < 0 || object->roHo.hoY < 0) {
			return false;
		}
		else {
			auto [gridX, gridY] = rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX, (size_t)object->roHo.hoY });

			return rdPtr->pFTW->GetMap(gridX, gridY, type) == MAP_OBSTACLE;
		}
		});
}

long WINAPI DLLExport ObjectAtCoord(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);
	int x = (int)CNC_GetParameter(rdPtr);
	int y = (int)CNC_GetParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	return rdPtr->pSelect->FilterObjects(rdPtr, oil, negated, [&](LPRDATA rdPtr, LPRO object)->bool {
		return object->roHo.hoX == x && object->roHo.hoY == y;
		});
}

long WINAPI DLLExport NoObjectAtCoord(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);
	int x = (int)CNC_GetParameter(rdPtr);
	int y = (int)CNC_GetParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	try {
		rdPtr->pSelect->ForEach(rdPtr, oil, [&](LPRO object) {
			if (object->roHo.hoX == x && object->roHo.hoY == y) {
				throw false;
			}

			});
	}
	catch (bool) {
		return false;
	}

	return true;
}

long WINAPI DLLExport CalcArea(LPRDATA rdPtr, long param1, long param2) {
	bool lineMode = (bool)CNC_GetParameter(rdPtr);
	bool attack = (bool)CNC_GetParameter(rdPtr);

	size_t x = (size_t)CNC_GetParameter(rdPtr);
	size_t y = (size_t)CNC_GetParameter(rdPtr);

	bool useRealCoord = (bool)CNC_GetParameter(rdPtr);	

	size_t range = (size_t)CNC_GetParameter(rdPtr);
	size_t startRange = (size_t)CNC_GetParameter(rdPtr);

	size_t ignoreFlag = (size_t)CNC_GetParameter(rdPtr);	

	bool allRange = (bool)CNC_GetParameter(rdPtr);
	size_t allRangeAttackRange = (size_t)CNC_GetParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	rdPtr->isAttack = allRange ? false : attack;

	rdPtr->areaPos = 0;
	rdPtr->areaSize = 0;
	rdPtr->extraRangeStartPos = DEFAULT_EXTRARANGESTARTPOS;

	Coord gridCoord = Coord{ x, y };

	if (useRealCoord) {
		gridCoord = rdPtr->pFTW->GetGridCoord(gridCoord);
	}

	if (!lineMode) {
		rdPtr->pFTW->CalcArea(gridCoord, range, startRange
			, rdPtr->pAlly, rdPtr->pEnemy, rdPtr->pZoc, ignoreFlag
			, attack
			, allRange, allRangeAttackRange, &rdPtr->extraRangeStartPos);
	}
	else {
		rdPtr->pFTW->CalcLineArea(gridCoord, range, startRange
			, rdPtr->pAlly, rdPtr->pEnemy, rdPtr->pZoc, ignoreFlag
			, attack);
	}

	rdPtr->areaSize = rdPtr->pFTW->GetArea().size();

	return TRUE;
}

long WINAPI DLLExport OnItArea(LPRDATA rdPtr, long param1, long param2) {
	wstring iterateName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	return iterateName == *rdPtr->pOnItAreaName;
}

long WINAPI DLLExport OnItAttackArea(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->isAttack;
}

long WINAPI DLLExport AbletoItArea(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->areaPos < rdPtr->areaSize;
}

long WINAPI DLLExport OnMapUpdate(LPRDATA rdPtr, long param1, long param2) {
	return TRUE;
}

long WINAPI DLLExport OnCreateObjectZoc(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);
	short oilZoc = (short)OIL_GetParameter(rdPtr);
	wstring iterateName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	auto itGridCoord = rdPtr->pFTW->GetGridCoord(rdPtr->itCoord);

	return iterateName == *rdPtr->pOnItZocName
		//// is overlap obstacle
		//&& (rdPtr->pFTW->GetMap(itGridCoord.x, itGridCoord.y, MapType::TERRAIN) != MAP_OBSTACLE)
		//// is overlap unit
		//&& !OverlapUnit(rdPtr, itGridCoord)
		// is current it object
		&& rdPtr->pSelect->FilterObjects(rdPtr, oil, negated
			, [&](LPRDATA rdPtr, LPRO object)->bool { return object == rdPtr->pObject; })
		// no zoc overlap
		//&& !rdPtr->pSelect->FilterObjects(rdPtr, oilZoc, negated
		//	, [&](LPRDATA rdPtr, LPRO object)->bool { return object->roHo.hoX == rdPtr->itCoord.x
		//												&& object->roHo.hoY == rdPtr->itCoord.y; })		
		;
}

long WINAPI DLLExport ZocValid(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	std::vector<LPRO> objects;
	
	rdPtr->pSelect->SelectAll(oil);
	objects.reserve(rdPtr->pSelect->GetNumberOfSelected(oil));

	rdPtr->pSelect->ForEach(rdPtr, oil, [&](LPRO object) {
		auto objectCoord = rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX ,(size_t)object->roHo.hoY });
			// not overlap obstacle
		if ((rdPtr->pFTW->GetMap(objectCoord.x, objectCoord.y, MapType::TERRAIN) != MAP_OBSTACLE)
			// not overlap unit
			&& !OverlapUnit(rdPtr, objectCoord)) {
			objects.emplace_back(object);
		}
		});

	rdPtr->pSelect->SelectObjects(oil,objects);

	return !objects.empty();
}

long WINAPI DLLExport ZocAtArea(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);	
	int mode = (int)CNC_GetParameter(rdPtr);
	
	bool atAttack = mode == 1;	// 0 = Move Area, 1 = Attack Area
	bool all = mode == -1;

	RetIfMapInvalid(FALSE);

	auto& area = rdPtr->pFTW->GetArea();

	return rdPtr->pSelect->FilterObjects(rdPtr, oil, negated
		, [&](LPRDATA rdPtr, LPRO object)->bool {
			auto objectCoord = rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX ,(size_t)object->roHo.hoY });
			
			for (size_t it = 0; it < area.size(); it++) {
				auto isAttack = !(it < rdPtr->extraRangeStartPos);
				
				if (std::find(area[it].begin(), area[it].end(), objectCoord) != area[it].end()) {
					return all
						|| (atAttack && isAttack)
						|| (!atAttack && !isAttack);
				}
			}

			return false;
		});
}

long WINAPI DLLExport SelectAll(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);

	rdPtr->pSelect->SelectAll(oil);

	return TRUE;
}

long WINAPI DLLExport ObjectAtObject(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oilObjA = (short)OIL_GetParameter(rdPtr);
	short oilObjB = (short)OIL_GetParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	CoordSet objects;

	rdPtr->pSelect->ForEach(rdPtr, oilObjB, [&](LPRO object) {
		objects.emplace_back(rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX ,(size_t)object->roHo.hoY }));
		});

	return rdPtr->pSelect->FilterObjects(rdPtr, oilObjA, negated, [&](LPRDATA rdPtr, LPRO object)->bool {
		if (std::find(objects.begin(), objects.end()
			, rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX ,(size_t)object->roHo.hoY })) != objects.end()) {
			return true;
		}

		return false;
		});
}

long WINAPI DLLExport PickOneObjectAtObject(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);

	RetIfMapInvalid(FALSE);

	vector<tuple<Coord, LPRO>> objects;
	vector<tuple<Coord, LPRO>> selected;
	vector<LPRO> toSelect;

	//auto num = rdPtr->pSelect->GetNumberOfSelected(oil);
	auto num = rdPtr->pSelect->GetNumberOfObject(oil);
	
	objects.reserve(num);
	selected.reserve(num);
	toSelect.reserve(num);

	auto find = [](vector<tuple<Coord, LPRO>>& objects, Coord objectCoord) {
		return std::find_if(objects.begin(), objects.end(), [&](tuple<Coord, LPRO>& it)->bool {
			auto& [c, p] = it;
			return c == objectCoord;
			}) != objects.end();
	};

	rdPtr->pSelect->ForEach(rdPtr, oil, [&](LPRO object) {
		objects.emplace_back(
			std::make_tuple(rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX ,(size_t)object->roHo.hoY })
				, object));
		});

	rdPtr->pSelect->ForEach(rdPtr, oil, [&](LPRO object) {
		auto objectCoord = rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX ,(size_t)object->roHo.hoY });

		if (!find(selected, objectCoord)) {
			selected.emplace_back(std::make_tuple(objectCoord, object));
		}
		});

	if (!negated) {
		for (auto& it : selected) {
			auto& [c, p] = it;
			toSelect.emplace_back(p);
		}
	}
	else {
		for (auto& it : objects) {
			auto& [c, p] = it;

			if (std::find(selected.begin(), selected.end(), it) == selected.end()) {
				toSelect.emplace_back(p);
			}
		}
	}

	rdPtr->pSelect->SelectObjects(oil, toSelect);

	return !toSelect.empty();
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

	RetIfMapInvalid(0);
	RetIfSetMapDirectly(type, 0);

	Coord gridCoord = Coord{ x, y };

	if (useRealCoord) {
		gridCoord = rdPtr->pFTW->GetGridCoord(gridCoord);
	}

	auto& [gridX, gridY] = gridCoord;

	rdPtr->pFTW->SetMap(gridX, gridY, cost, type);

	return 0;
}

short WINAPI DLLExport SetMapByObject(LPRDATA rdPtr, long param1, long param2) {
	LPRO object = (LPRO)CNC_GetParameter(rdPtr);
	BYTE cost = (BYTE)CNC_GetParameter(rdPtr);
	MapType type = (MapType)CNC_GetParameter(rdPtr);

	if (!LPROValid(object)) {
		return 0;
	}

	RetIfMapInvalid(0);
	RetIfSetMapDirectly(type, 0);

	auto [gridX, gridY] = rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX, (size_t)object->roHo.hoY });
	rdPtr->pFTW->SetMap(gridX, gridY, cost, type);

	return 0;
}

short WINAPI DLLExport ClearMap(LPRDATA rdPtr, long param1, long param2) {
	BYTE cost = (BYTE)CNC_GetParameter(rdPtr);
	MapType type = (MapType)CNC_GetParameter(rdPtr);

	RetIfMapInvalid(0);
	RetIfSetMapDirectly(type, 0);

	rdPtr->pFTW->ClearMap(type, cost);

	return 0;
}

short WINAPI DLLExport Continue(LPRDATA rdPtr, long param1, long param2) {
	return 0;		// Force fusion to evaluate conditions, as empty events will be skipped
}

short WINAPI DLLExport IteratePath(LPRDATA rdPtr, long param1, long param2) {
	wstring pathName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	bool useRealCoord = (bool)CNC_GetIntParameter(rdPtr);

	*rdPtr->pOnItPathName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	RetIfMapInvalid(0);

	wstring* pPathName = pathName == L"" ? nullptr : &pathName;

	if (!rdPtr->pFTW->GetPathValidity(pPathName)) {
		return 0;
	}

	rdPtr->itIndex = 0;
	size_t totalStep = rdPtr->pFTW->GetDistance(pPathName);

	for (size_t step = 0; step < totalStep; step++) {
		auto coord = Coord{ rdPtr->pFTW->GetCoordAtPath(step, CoordType::X, pPathName)
						,rdPtr->pFTW->GetCoordAtPath(step, CoordType::Y, pPathName) };
		rdPtr->itCoord = useRealCoord ? rdPtr->pFTW->GetRealCoord(coord) : coord;
		rdPtr->itIndex = step;

		CallEvent(ONITERATESTEP);
	}
	return 0;
}

short WINAPI DLLExport SetZocByObject(LPRDATA rdPtr, long param1, long param2) {
	LPRO object = (LPRO)CNC_GetParameter(rdPtr);
	bool center = (bool)CNC_GetParameter(rdPtr);

	if (!LPROValid(object)) {
		return 0;
	}

	RetIfMapInvalid(0);

	Coord objectCoord = rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX, (size_t)object->roHo.hoY }); 

	if (center) {		
		rdPtr->pFTW->GenerateZoc(objectCoord, rdPtr->pZoc, true);
	}
	else if (std::find(rdPtr->pZoc->begin(), rdPtr->pZoc->end(), objectCoord) == rdPtr->pZoc->end()) {
		rdPtr->pZoc->emplace_back(objectCoord); 
	}

	return 0;
}

short WINAPI DLLExport ClearZoc(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pZoc->clear();

	return 0;
}

short WINAPI DLLExport IterateArea(LPRDATA rdPtr, long param1, long param2) {
	*rdPtr->pOnItAreaName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	bool once = (bool)CNC_GetParameter(rdPtr);

	RetIfMapInvalid(0);

	auto& area = rdPtr->pFTW->GetArea();

	auto loop = [&](const CoordSet& s) {
		for (auto& it_c : s) {
			rdPtr->itCoord = it_c;

			CallEvent(ONITERATEAREA)
		}
	};

	if (once) {
		if (rdPtr->areaPos < area.size()) {
			rdPtr->isAttack = !(rdPtr->areaPos < rdPtr->extraRangeStartPos);

			loop(area[rdPtr->areaPos]);

			rdPtr->areaPos++;
		}
	
	}
	else {
		for (size_t it = 0; it < area.size(); it++) {
			rdPtr->isAttack = !(it < rdPtr->extraRangeStartPos);

			loop(area[it]);

			rdPtr->areaPos++;
		}
	}

	return 0;
}

short WINAPI DLLExport CreateAreaOnce(LPRDATA rdPtr, long param1, long param2) {
	auto param = OCP_GetParameter(rdPtr);

	RetIfMapInvalid(0);

	auto& area = rdPtr->pFTW->GetArea();

	if (rdPtr->areaPos < area.size()) {
		rdPtr->isAttack = !(rdPtr->areaPos + 1 < rdPtr->extraRangeStartPos);

		for (auto& it : area[rdPtr->areaPos]) {
			auto realCoord = rdPtr->pFTW->GetRealCoord(it);

			rdPtr->pOc->OCCreateObject([&](ObjectCreation* oc, CreateDuplicateParam* cdp) {
				*cdp = param;

				cdp->cdpPos.posX = (short)realCoord.x;
				cdp->cdpPos.posY = (short)realCoord.y;
				});
		}

		rdPtr->areaPos++;
	}

	return 0;
}

short WINAPI DLLExport CreateAreaByNameOnce(LPRDATA rdPtr, long param1, long param2) {
	wstring objectName = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	auto Oi = rdPtr->pOc->GetCreationOI(objectName);

	RetIfMapInvalid(0);

	auto& area = rdPtr->pFTW->GetArea();

	if (rdPtr->areaPos < area.size()) {
		rdPtr->isAttack = !(rdPtr->areaPos + 1 < rdPtr->extraRangeStartPos);

		for (auto& it : area[rdPtr->areaPos]) {
			auto realCoord = rdPtr->pFTW->GetRealCoord(it);

			rdPtr->pOc->OCCreateObject([&](ObjectCreation* oc, CreateDuplicateParam* cdp) {
				cdp->cdpOi = Oi;

				cdp->cdpPos.posX = (short)realCoord.x;
				cdp->cdpPos.posY = (short)realCoord.y;
				cdp->cdpPos.posLayer = 1;
				cdp->cdpPos.posOINUMParent = -1;
				cdp->cdpPos.posFlags = 8;
				});
		}

		rdPtr->areaPos++;
	}

	return 0;
}

short WINAPI DLLExport SetUnitByObject(LPRDATA rdPtr, long param1, long param2) {
	LPRO object = (LPRO)CNC_GetParameter(rdPtr);
	int type = (int)CNC_GetParameter(rdPtr);

	if (!LPROValid(object)) {
		return 0;
	}

	RetIfMapInvalid(0);

	Coord objectCoord = rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX, (size_t)object->roHo.hoY });

	constexpr auto ALLY = 0;
	constexpr auto ENEMY = 1;
	constexpr auto UNIT = 255;

	switch (type) {
	case ALLY: {
		rdPtr->pAlly->emplace_back(objectCoord);
		break;
	}
	case ENEMY: {
		rdPtr->pEnemy->emplace_back(objectCoord);
		break;
	}
	case UNIT: {
		rdPtr->pUnit->emplace_back(objectCoord);
		break;
	}
	}

	return 0;
}

short WINAPI DLLExport ClearUnit(LPRDATA rdPtr, long param1, long param2) {
	int target = (int)CNC_GetParameter(rdPtr);

	constexpr auto ALL = -1;
	constexpr auto ALLY = 0;
	constexpr auto ENEMY = 1;
	constexpr auto UNIT = 255;

	switch (target) {
	case ALL: {
		rdPtr->pAlly->clear();
		rdPtr->pEnemy->clear();
		rdPtr->pUnit->clear();
		break;
	}
	case ALLY: {
		rdPtr->pAlly->clear();
		break;
	}
	case ENEMY: {
		rdPtr->pEnemy->clear();
		break;
	}
	case UNIT: {
		rdPtr->pUnit->clear();
		break;
	}
	}

	return 0;
}

short WINAPI DLLExport CreateObjectZoc(LPRDATA rdPtr, long param1, long param2) {	
	LPRO object = (LPRO)CNC_GetParameter(rdPtr);
	auto param = OCP_GetParameter(rdPtr);

	if (!LPROValid(object)) {
		return 0;
	}

	RetIfMapInvalid(0);

	Coord objectCoord = rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX, (size_t)object->roHo.hoY });
	rdPtr->pFTW->GenerateZoc(objectCoord, rdPtr->pObjZoc, false);

	for (auto& it : *rdPtr->pObjZoc) {
		auto realCoord = rdPtr->pFTW->GetRealCoord(it);

		rdPtr->pOc->OCCreateObject([&](ObjectCreation* oc, CreateDuplicateParam* cdp) {
			*cdp = param;

			cdp->cdpPos.posX = (short)realCoord.x;
			cdp->cdpPos.posY = (short)realCoord.y;
			});
	}

	return 0;
}

short WINAPI DLLExport CreateObjectZocByEvent(LPRDATA rdPtr, long param1, long param2) {
	short oil = (short)OIL_GetParameter(rdPtr);
	*rdPtr->pOnItZocName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	RetIfMapInvalid(0);

	// usually fusion will do a internal for-each for all selected objects
	// but when you call immediate events, this for-each will be terminated
	// so here need a manual for-each to force it
	rdPtr->pSelect->ForEach(rdPtr, oil, [&](LPRO object) {
		Coord objectCoord = rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX, (size_t)object->roHo.hoY });

		rdPtr->pObject = object;
		rdPtr->pFTW->GenerateZoc(objectCoord, rdPtr->pObjZoc, false);

		for (auto& it : *rdPtr->pObjZoc) {
			rdPtr->itCoord = rdPtr->pFTW->GetRealCoord(it);

			CallEvent(ONCREATEZOC)
		}

		rdPtr->pObject = nullptr;
		});

	return 0;
}

short WINAPI DLLExport CreateObjectZocByName(LPRDATA rdPtr, long param1, long param2) {
	LPRO object = (LPRO)CNC_GetParameter(rdPtr);
	wstring objectName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (!LPROValid(object)) {
		return 0;
	}

	RetIfMapInvalid(0);

	auto Oi = rdPtr->pOc->GetCreationOI(objectName);

	Coord objectCoord = rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX, (size_t)object->roHo.hoY });
	rdPtr->pFTW->GenerateZoc(objectCoord, rdPtr->pObjZoc, false);

	for (auto& it : *rdPtr->pObjZoc) {
		auto realCoord = rdPtr->pFTW->GetRealCoord(it);

		rdPtr->pOc->OCCreateObject([&](ObjectCreation* oc, CreateDuplicateParam* cdp) {
			cdp->cdpOi = Oi;

			cdp->cdpPos.posX = (short)realCoord.x;
			cdp->cdpPos.posY = (short)realCoord.y;
			cdp->cdpPos.posLayer = 1;
			cdp->cdpPos.posOINUMParent = -1;
			cdp->cdpPos.posFlags = 8;
			});
	}

	return 0;
}

short WINAPI DLLExport SetStash(LPRDATA rdPtr, long param1, long param2) {
	bool enable = (bool)CNC_GetParameter(rdPtr);

	RetIfMapInvalid(0);

	rdPtr->pFTW->SetStash(enable);

	if (!enable) {
		rdPtr->pFTW->ClearStash();
	}

	return 0;
}

short WINAPI DLLExport CreateAOE(LPRDATA rdPtr, long param1, long param2) {
	auto param = OCP_GetParameter(rdPtr);

	LPRO object = (LPRO)CNC_GetParameter(rdPtr);
	size_t dir = (size_t)CNC_GetParameter(rdPtr);
	size_t type = (size_t)CNC_GetParameter(rdPtr);

	size_t ignoreFlag = (size_t)CNC_GetParameter(rdPtr);

	bool moveIgnoreZoc = ignoreFlag & 0b10000;			// Move through zoc
	bool moveIgnoreAlly = ignoreFlag & 0b01000;			// Move through ally
	bool moveIgnoreEnemy = ignoreFlag & 0b00100;		// Move through enemy	
	bool attackIgnoreAlly = ignoreFlag & 0b00010;		// Attack ally (e.g., heal)	
	bool attackIgnoreEnemy = ignoreFlag & 0b00001;		// Attack enemy	

	if (!LPROValid(object)) {
		return 0;
	}

	RetIfMapInvalid(0);

	Coord objectCoord = rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX, (size_t)object->roHo.hoY });
	AOEClass::coord start = AOEClass::coord{ (int)objectCoord.x,(int)objectCoord.y };

	rdPtr->pAOE->GetAOE(start, dir, type, rdPtr->pAOECoord);

	auto find = [](CoordSet* c, Coord& p) {
		return std::find(c->begin(), c->end(), p) != c->end();
	};

	for (auto& it : *rdPtr->pAOECoord) {
		auto gridCoord = Coord{ (size_t)it.x,(size_t)it.y };
		auto realCoord = rdPtr->pFTW->GetRealCoord(gridCoord);

		if (rdPtr->pFTW->GetMap(gridCoord.x, gridCoord.y, MapType::MAP) == MAP_OBSTACLE) {
			continue;
		}

		if (!attackIgnoreAlly && find(rdPtr->pAlly, gridCoord)) {
			continue;
		}
		if (!attackIgnoreEnemy && find(rdPtr->pEnemy, gridCoord)) {
			continue;
		}

		rdPtr->pOc->OCCreateObject([&](ObjectCreation* oc, CreateDuplicateParam* cdp) {
			*cdp = param;

			cdp->cdpPos.posX = (short)realCoord.x;
			cdp->cdpPos.posY = (short)realCoord.y;
			});
	}

	return 0;
}

short WINAPI DLLExport CreateAOEByName(LPRDATA rdPtr, long param1, long param2) {
	wstring objectName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	LPRO object = (LPRO)CNC_GetParameter(rdPtr);
	size_t dir = (size_t)CNC_GetParameter(rdPtr);
	size_t type = (size_t)CNC_GetParameter(rdPtr);

	size_t ignoreFlag = (size_t)CNC_GetParameter(rdPtr);

	bool moveIgnoreZoc = ignoreFlag & 0b10000;			// Move through zoc
	bool moveIgnoreAlly = ignoreFlag & 0b01000;			// Move through ally
	bool moveIgnoreEnemy = ignoreFlag & 0b00100;		// Move through enemy	
	bool attackIgnoreAlly = ignoreFlag & 0b00010;		// Attack ally (e.g., heal)	
	bool attackIgnoreEnemy = ignoreFlag & 0b00001;		// Attack enemy	


	if (!LPROValid(object)) {
		return 0;
	}

	RetIfMapInvalid(0);

	auto Oi = rdPtr->pOc->GetCreationOI(objectName);

	Coord objectCoord = rdPtr->pFTW->GetGridCoord(Coord{ (size_t)object->roHo.hoX, (size_t)object->roHo.hoY });
	AOEClass::coord start = AOEClass::coord{ (int)objectCoord.x,(int)objectCoord.y };

	rdPtr->pAOE->GetAOE(start, dir, type, rdPtr->pAOECoord);

	auto find = [](CoordSet* c,Coord& p) {
		return std::find(c->begin(), c->end(), p) != c->end();
	};

	for (auto& it : *rdPtr->pAOECoord) {
		auto gridCoord = Coord{ (size_t)it.x,(size_t)it.y };
		auto realCoord = rdPtr->pFTW->GetRealCoord(gridCoord);

		if (rdPtr->pFTW->GetMap(gridCoord.x, gridCoord.y, MapType::MAP) == MAP_OBSTACLE) {
			continue;
		}

		if (!attackIgnoreAlly && find(rdPtr->pAlly, gridCoord)) {
			continue;
		}
		if (!attackIgnoreEnemy && find(rdPtr->pEnemy, gridCoord)) {
			continue;
		}

		rdPtr->pOc->OCCreateObject([&](ObjectCreation* oc, CreateDuplicateParam* cdp) {
			cdp->cdpOi = Oi;

			cdp->cdpPos.posX = (short)realCoord.x;
			cdp->cdpPos.posY = (short)realCoord.y;
			cdp->cdpPos.posLayer = 1;
			cdp->cdpPos.posOINUMParent = -1;
			cdp->cdpPos.posFlags = 8;
			});
	}

	return 0;
}

short WINAPI DLLExport CreateGrid(LPRDATA rdPtr, long param1, long param2) {
	LPRO object = (LPRO)CNC_GetParameter(rdPtr);
	int nLayer = (int)CNC_GetParameter(rdPtr) - 1;

	if (!LPROValid(object, IDENTIFIER_ACTIVE)) {		// must be active object
		return 0;
	}

	// int nObstacleType = rdPtr->rHo.hoCurrentParam->evp.evpW.evpW0;
	// CNC_GetParameter(rdPtr);
	int nObstacleType = OBSTACLE_TRANSPARENT;		// no effect on collisions

	auto rhPtr = rdPtr->rHo.hoAdRunHeader;
	short nFrame = object->roa.raAnimDirOffset->adNumberOfFrame;

	// get alterable value
	long rOffset = ((CValue*)(object->rov.rvpValues))->m_long;
	long bOffset = ((CValue*)(object->rov.rvpValues) + 1)->m_long;

	auto lockSf=[&](cSurface& sf,short offset)->BOOL{
		if (offset >= nFrame) {
			return false;
		}

		// get animation
		DWORD hImage = object->roa.raAnimDirOffset->adFrame[offset];
		return LockImageSurface(rhPtr->rhIdAppli, hImage, sf);
	};

	cSurface imageSurfaceLT;
	cSurface imageSurfaceR;
	cSurface imageSurfaceB;

	lockSf(imageSurfaceLT, 0);
	lockSf(imageSurfaceR, 1);
	lockSf(imageSurfaceB, 2);

	auto hoX = object->roHo.hoImgXSpot;
	auto hoY = object->roHo.hoImgYSpot;

	auto effect = object->ros.rsEffect;
	auto effectParam = object->ros.rsEffectParam;

	RetIfMapInvalid(0);

	auto width = rdPtr->pFTW->GetMapWidth();
	auto height = rdPtr->pFTW->GetMapHeight();

	auto addBKD = [&](cSurface* pSf, int x, int y) {
		if (!pSf->IsValid()) {
			return;
		}

		rhPtr->rh4.rh4Mv->mvAddBackdrop(
					pSf,
					x,
					y,
					effect,
					effectParam,
					nObstacleType,
					nLayer
				);
	};

	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			if (rdPtr->pFTW->GetMap(x, y, MapType::MAP) != MAP_OBSTACLE) {
				auto [rx, ry] = rdPtr->pFTW->GetRealCoord(Coord{ x, y });

				addBKD(&imageSurfaceLT, rx - hoX, ry - hoY);

				if (rdPtr->pFTW->GetMap(x + 1, y, MapType::MAP) == MAP_OBSTACLE) {
					addBKD(&imageSurfaceR, rx - hoX + (x + 1 == width ? 0 : rOffset), ry - hoY);
				}

				if (rdPtr->pFTW->GetMap(x, y + 1, MapType::MAP) == MAP_OBSTACLE) {
					addBKD(&imageSurfaceB, rx - hoX, ry - hoY + (y + 1 == height ? 0 : bOffset));
				}
			}
		}
	}

	UnlockImageSurface(imageSurfaceLT);
	UnlockImageSurface(imageSurfaceR);
	UnlockImageSurface(imageSurfaceB);

	return 0;
}


short WINAPI DLLExport SetGridSize(LPRDATA rdPtr, long param1, long param2) {
	size_t gridSize = (size_t)CNC_GetParameter(rdPtr);
	size_t gridOffsetX = (size_t)CNC_GetParameter(rdPtr);
	size_t gridOffsetY = (size_t)CNC_GetParameter(rdPtr);

	RetIfMapInvalid(0);

	rdPtr->pFTW->SetGridSize(gridSize, gridOffsetX, gridOffsetY);

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport GetIterateX(LPRDATA rdPtr, long param1) {
	return rdPtr->itCoord.x;
}
 
long WINAPI DLLExport GetIterateY(LPRDATA rdPtr, long param1) {
	return rdPtr->itCoord.y;
}

long WINAPI DLLExport GetIterateIndex(LPRDATA rdPtr, long param1) {
	return rdPtr->itIndex;
}

long WINAPI DLLExport GetStep(LPRDATA rdPtr, long param1) {
	size_t startX = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	size_t startY = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	size_t destinationX = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	size_t destinationY = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	size_t ignoreFlag = (size_t)CNC_GetParameter(rdPtr);

	bool diagonal = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	bool checkDiagonalCorner = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	bool forceFind = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	bool useRealCoord = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	wstring saveName = (LPCWSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	RetIfMapInvalid(STEP_UNREACHABLE);

	FindPath(rdPtr, Coord{ startX ,startY }, Coord{ destinationX ,destinationY }, ignoreFlag, diagonal, checkDiagonalCorner, forceFind, useRealCoord, saveName);

#ifdef _DEBUG
	auto map = rdPtr->pFTW->OutPutMapStr();
#endif // _DEBUG

	return rdPtr->pFTW->GetDistance();
}

long WINAPI DLLExport GetStepOfPath(LPRDATA rdPtr, long param1) {
	wstring pathName = (LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	RetIfMapInvalid(STEP_UNREACHABLE);

	wstring* pPathName = pathName == L"" ? nullptr : &pathName;

	return rdPtr->pFTW->GetDistance(pPathName);
}

long WINAPI DLLExport GetStepCoordOfPath(LPRDATA rdPtr, long param1) {
	wstring pathName = (LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	size_t step = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	CoordType type = (CoordType)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	RetIfMapInvalid(COORD_INVALID);

	wstring* pPathName = pathName == L"" ? nullptr : &pathName;

	return rdPtr->pFTW->GetCoordAtPath(step, type, pPathName);
}

long WINAPI DLLExport GetGridCoord(LPRDATA rdPtr, long param1) {
	size_t coordX = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	size_t coordY = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	CoordType type = (CoordType)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	RetIfMapInvalid(COORD_INVALID);

	return type == CoordType::X
		? rdPtr->pFTW->GetGridCoord(Coord{ coordX ,coordY }).x
		: rdPtr->pFTW->GetGridCoord(Coord{ coordX ,coordY }).y;
}

long WINAPI DLLExport GetRealCoord(LPRDATA rdPtr, long param1) {
	size_t coordX = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	size_t coordY = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	CoordType type = (CoordType)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	RetIfMapInvalid(REAL_INVALID);

	return type == CoordType::X
		? rdPtr->pFTW->GetRealCoord(Coord{ coordX ,coordY }).x
		: rdPtr->pFTW->GetRealCoord(Coord{ coordX ,coordY }).y;
}

long WINAPI DLLExport GetMapCost(LPRDATA rdPtr, long param1) {
	size_t x = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	size_t y = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	bool useRealCoord = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	MapType type = (MapType)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	RetIfMapInvalid(MAP_OBSTACLE);

	Coord gridCoord = Coord{ x, y };

	if (useRealCoord) {
		gridCoord = rdPtr->pFTW->GetGridCoord(gridCoord);
	}

	auto& [gridX, gridY] = gridCoord;

	return rdPtr->pFTW->GetMap(gridX, gridY, type);
}

long WINAPI DLLExport GetMapBase64(LPRDATA rdPtr, long param1) {
	// RetIfMapInvalid((long)Empty_Str);

	*rdPtr->pMapBase64Str = (rdPtr->pFTW == nullptr) ? L"InvalidMap" : rdPtr->pFTW->GetMap();

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(*rdPtr->pMapBase64Str).c_str();
}

long WINAPI DLLExport GetMapStr(LPRDATA rdPtr, long param1) {
	MapType type = (MapType)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	bool showPath = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	wstring pathName = (LPCWSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	// RetIfMapInvalid((long)Empty_Str);

	wstring* pPathName = pathName == L"" ? nullptr : &pathName;

	*rdPtr->pMapStr = (rdPtr->pFTW == nullptr) ? L"InvalidMap" : rdPtr->pFTW->OutPutMapStr(type, showPath, pPathName);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(*rdPtr->pMapStr).c_str();
}

long WINAPI DLLExport GetAbleLineRange(LPRDATA rdPtr, long param1) {
	size_t x = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	size_t y = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	bool useRealCoord = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	size_t dir = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	size_t range = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	size_t ignoreFlag = (size_t)CNC_GetParameter(rdPtr);

	bool attack = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	RetIfMapInvalid(0);

	Coord gridCoord = Coord{ x, y };

	if (useRealCoord) {
		gridCoord = rdPtr->pFTW->GetGridCoord(gridCoord);
	}

	return rdPtr->pFTW->GetAbleLineRange(gridCoord, range, dir, rdPtr->pAlly, rdPtr->pEnemy, rdPtr->pZoc, ignoreFlag, attack);
}

long WINAPI DLLExport GetIgnoreFlag(LPRDATA rdPtr, long param1) {
	bool moveIgnoreZoc = (bool)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	bool moveIgnoreAlly= (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	bool moveIgnoreEnemy = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	bool attackIgnoreAlly= (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	bool attackIgnoreEnemy= (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	
	return FindTheWayClass::GetIgnoreFlag(moveIgnoreZoc, moveIgnoreAlly, moveIgnoreEnemy, attackIgnoreAlly, attackIgnoreEnemy);
}

long WINAPI DLLExport GetMapTypeMap(LPRDATA rdPtr, long param1) {
	return (long)MapType::MAP;
}

long WINAPI DLLExport GetMapTypeTerrain(LPRDATA rdPtr, long param1) {
	return (long)MapType::TERRAIN;
}

long WINAPI DLLExport GetMapTypeDynamic(LPRDATA rdPtr, long param1) {
	return (long)MapType::DYNAMIC;
}

long WINAPI DLLExport GetMapCostPath(LPRDATA rdPtr, long param1) {
	return MAP_PATH;
}

long WINAPI DLLExport GetMapCostObstacle(LPRDATA rdPtr, long param1) {
	return MAP_OBSTACLE;
}

long WINAPI DLLExport GetMapCostValid(LPRDATA rdPtr, long param1) {
	size_t cost = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	return Range(cost);
}

long WINAPI DLLExport GetIsometricGridSize(LPRDATA rdPtr, long param1) {
	size_t isoGridWidth = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	size_t isoGridHeight = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	return FindTheWayClass::GetIsometricGridSize(isoGridWidth, isoGridHeight);
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
			OnIteratePath,

			MapAvailable,
			CheckMapCostAtPoint,

			ObjectAtGrid,
			ObjectAtObstacle,
			ObjectAtCoord,

			SetMapByPicture,

			CalcArea,
			OnItArea,
			OnItAttackArea,
			AbletoItArea,

			OnMapUpdate,
			OnCreateObjectZoc,
			ZocValid,
			ZocAtArea,
			SelectAll,
			ObjectAtObject,
			PickOneObjectAtObject,

			NoObjectAtGrid,
			NoObjectAtCoord,

			SetMapByActive,
			SetMapBySurface,
			
			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			SetMap,
			Continue,

			IteratePath,

			SetMapByObject,
			ClearMap,

			SetZocByObject,
			ClearZoc,
			IterateArea,

			SetUnitByObject,
			ClearUnit,

			CreateObjectZoc,

			SetStash,

			CreateObjectZocByEvent,
			CreateObjectZocByName,

			CreateAreaOnce,
			CreateAreaByNameOnce,
			CreateAOE,
			CreateAOEByName,

			CreateGrid,
			SetGridSize,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			GetIterateX,
			GetIterateY,

			GetStep,
			GetStepOfPath,
			GetStepCoordOfPath,

			GetGridCoord,
			GetRealCoord,

			GetIterateIndex,
			
			GetMapCost,
			GetMapBase64,
			GetMapStr,

			GetAbleLineRange,
			GetIgnoreFlag,

			GetMapTypeMap,
			GetMapTypeTerrain,
			GetMapTypeDynamic,
			GetMapCostPath,
			GetMapCostObstacle,
			GetMapCostValid,

			GetIsometricGridSize,

			0
			};