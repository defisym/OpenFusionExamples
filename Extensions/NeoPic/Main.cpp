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
		IDMN_CONDITION_OPLC, M_CONDITION_OPLC, CND_CONDITION_OPLC, 0, 0,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_LFF, M_ACTION_LFF,	ACT_ACTION_LFF,	0, 2,PARAM_EXPSTRING,PARAM_EXPSTRING,M_ACTION_FILENAME,M_ACTION_KEY,
		IDMN_ACTION_LFL, M_ACTION_LFL,	ACT_ACTION_LFL,	0, 3,PARAM_OBJECT,PARAM_EXPSTRING,PARAM_EXPSTRING,M_ACTION_OBJECT,M_ACTION_FILENAME,M_ACTION_KEY,
		
		IDMN_ACTION_RL, M_ACTION_RL,	ACT_ACTION_RL,	0, 0,
		IDMN_ACTION_EL, M_ACTION_EL,	ACT_ACTION_EL,	0, 1,PARAM_EXPSTRING,M_ACTION_FILENAME,
		IDMN_ACTION_UL, M_ACTION_UL,	ACT_ACTION_UL,	0, 2,PARAM_EXPSTRING,PARAM_EXPSTRING,M_ACTION_FILENAME,M_ACTION_KEY,
		
		IDMN_ACTION_SH, M_ACTION_SH,	ACT_ACTION_SH,	0, 3,PARAM_EXPRESSION,PARAM_EXPRESSION,PARAM_EXPRESSION,M_ACTION_HOTSPOT,M_ACTION_HOTSPOT_X,M_ACTION_HOTSPOT_Y,

		IDMN_ACTION_Z, M_ACTION_Z,	ACT_ACTION_Z,	0, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,M_ACTION_Z_XSCALE,M_ACTION_Z_YSCALE,

		IDMN_ACTION_R, M_ACTION_R,	ACT_ACTION_R,	0, 1,PARAM_EXPRESSION,M_ACTION_A,

		IDMN_ACTION_US, M_ACTION_US,	ACT_ACTION_US,	0, 0,
		IDMN_ACTION_RC, M_ACTION_RC,	ACT_ACTION_RC,	0, 0,

		IDMN_ACTION_S, M_ACTION_S,	ACT_ACTION_S,	0, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,M_ACTION_S_WIDTH,M_ACTION_S_HEIGHT,

		IDMN_ACTION_AB, M_ACTION_AB,	ACT_ACTION_AB,	0, 1,PARAM_PASTE,0,
		IDMN_ACTION_UC, M_ACTION_UC,	ACT_ACTION_UC,	0, 0,

		IDMN_ACTION_SC, M_ACTION_SC,	ACT_ACTION_SC,	0, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,M_ACTION_HC,M_ACTION_AUC,
		IDMN_ACTION_SQ, M_ACTION_SQ,	ACT_ACTION_SQ,	0, 1,PARAM_EXPRESSION,M_ACTION_RS,

		IDMN_ACTION_AT, M_ACTION_AT,	ACT_ACTION_AT,	0, 0,
		IDMN_ACTION_O, M_ACTION_O,	ACT_ACTION_O,	0, 3,PARAM_EXPRESSION,PARAM_EXPRESSION,PARAM_EXPRESSION,M_ACTION_XO,M_ACTION_YO,
		M_ACTION_W,

		IDMN_ACTION_LFD, M_ACTION_LFD,	ACT_ACTION_LFD,	0, 2,PARAM_OBJECT,PARAM_EXPRESSION,M_ACTION_OBJECT,M_ACTION_COPYCOEF,
		
		IDMN_ACTION_SPL, M_ACTION_SPL, ACT_ACTION_SPL,	0, 3, PARAM_EXPRESSION, PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACTION_PRELOAD, M_ACTION_BASEPATH, M_ACTION_KEY,
		IDMN_ACTION_SPP, M_ACTION_SPP, ACT_ACTION_SPP,	0, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACTION_BASEPATH, M_ACTION_KEY,
		IDMN_ACTION_CC, M_ACTION_CC, ACT_ACTION_CC,	0, 0,

		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GHSX, M_EXPRESSION_GHSX, EXP_EXPRESSION_GHSX, 0, 0,
		IDMN_EXPRESSION_GHSY, M_EXPRESSION_GHSY, EXP_EXPRESSION_GHSY, 0, 0,

		IDMN_EXPRESSION_GOW, M_EXPRESSION_GOW, EXP_EXPRESSION_GOW, 0, 0,
		IDMN_EXPRESSION_GOH, M_EXPRESSION_GOH, EXP_EXPRESSION_GOH, 0, 0,

		IDMN_EXPRESSION_GCW, M_EXPRESSION_GCW, EXP_EXPRESSION_GCW, 0, 0,
		IDMN_EXPRESSION_GCH, M_EXPRESSION_GCH, EXP_EXPRESSION_GCH, 0, 0,
		
		IDMN_EXPRESSION_GXZS, M_EXPRESSION_GXZS, EXP_EXPRESSION_GXZS, EXPFLAG_DOUBLE, 0,
		IDMN_EXPRESSION_GYZS, M_EXPRESSION_GYZS, EXP_EXPRESSION_GYZS, EXPFLAG_DOUBLE, 0,

		IDMN_EXPRESSION_GA, M_EXPRESSION_GA, EXP_EXPRESSION_GA, 0, 0,

		IDMN_EXPRESSION_GFN, M_EXPRESSION_GFN, EXP_EXPRESSION_GFN, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GK, M_EXPRESSION_GK, EXP_EXPRESSION_GK, EXPFLAG_STRING, 0,

		IDMN_EXPRESSION_GSP, M_EXPRESSION_GSP, EXP_EXPRESSION_GSP, 0, 2, EXPPARAM_STRING, EXPPARAM_STRING, M_ACTION_FILENAME, M_ACTION_KEY,

		IDMN_EXPRESSION_GAVGCX, M_EXPRESSION_GAVGCX, EXP_EXPRESSION_GAVGCX, 0, 2, EXPPARAM_LONG, EXPPARAM_LONG, M_EXPRESSION_COORD, M_EXPRESSION_REALCOORDMODE,
		IDMN_EXPRESSION_GAVGCY, M_EXPRESSION_GAVGCY, EXP_EXPRESSION_GAVGCY, 0, 2, EXPPARAM_LONG, EXPPARAM_LONG, M_EXPRESSION_COORD, M_EXPRESSION_REALCOORDMODE,

		IDMN_EXPRESSION_GFP, M_EXPRESSION_GFP, EXP_EXPRESSION_GFP, EXPFLAG_STRING, 0,
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

long WINAPI DLLExport OnPreloadComplete(LPRDATA rdPtr, long param1, long param2) {
	return TRUE;
}


// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport LoadFromFile(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	*rdPtr->FilePath = FilePath;
	*rdPtr->Key = Key;

	LoadFromFile(rdPtr, FilePath, Key);

	return 0;
}

short WINAPI DLLExport LoadFromLib(LPRDATA rdPtr, long param1, long param2) {
	LPRO object = (LPRO)CNC_GetParameter(rdPtr);

	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	LoadFromLib(rdPtr, object, FilePath, Key);

	return 0;
}

short WINAPI DLLExport LoadFromDisplay(LPRDATA rdPtr, long param1, long param2) {
	LPRO object = (LPRO)CNC_GetParameter(rdPtr);
	bool UpdateCoef = (bool)CNC_GetIntParameter(rdPtr);

	LoadFromDisplay(rdPtr, object, UpdateCoef);

	return 0;
}

short WINAPI DLLExport SetPreloadList(LPRDATA rdPtr, long param1, long param2) {
	long list = (long)CNC_GetIntParameter(rdPtr);

	LPCWSTR BasePath = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	LPCWSTR Key = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	using pPreLoadList = decltype(rdPtr->pPreloadList);
	using PreLoadList = std::remove_pointer_t<pPreLoadList>;

	if (!rdPtr->preloading
		&& rdPtr->isLib) {
		delete rdPtr->pPreloadList;
		rdPtr->pPreloadList = nullptr;

		auto pList = ConvertToType<pPreLoadList>(list);

		if (pList != nullptr) {
			rdPtr->pPreloadList = new std::remove_pointer_t<PreLoadList>;
			rdPtr->pPreloadList->reserve(pList->size());

			// filter duplicate
			for (auto& it : *pList) {
				if (std::find(rdPtr->pPreloadList->begin(), rdPtr->pPreloadList->end(), it) == rdPtr->pPreloadList->end()) {
					rdPtr->pPreloadList->emplace_back(it);
				}
			}

			rdPtr->preloading = true;
			std::thread pl(PreloadLibFromVec, rdPtr, *rdPtr->pPreloadList, BasePath, Key
				, [rdPtr](const SurfaceLib& lib) {
					for (auto& it : lib) {
						auto& [name, pSf] = it;

						if (rdPtr->lib->find(name) == rdPtr->lib->end()) {
							rdPtr->lib->emplace(name, pSf);
						}
					}

					rdPtr->preloading = false;
					CallEvent(ONPRELOADCOMPLETE);				
				});

			pl.detach();
		}
	}

	return 0;
}

short WINAPI DLLExport SetPreloadPath(LPRDATA rdPtr, long param1, long param2) {
	LPCWSTR BasePath = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	LPCWSTR Key = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (rdPtr->isLib) {
		std::thread pl(PreloadLibFromPath, rdPtr, BasePath, Key
			, [rdPtr](const SurfaceLib& lib) {
				for (auto& it : lib) {
					auto& [name, pSf] = it;

					if (rdPtr->lib->find(name) == rdPtr->lib->end()) {
						rdPtr->lib->emplace(name, pSf);
					}
				}

				CallEvent(ONPRELOADCOMPLETE);
			});

		pl.detach();
	}

	return 0;
}

short WINAPI DLLExport ResetLib(LPRDATA rdPtr, long param1, long param2) {
	if(rdPtr->isLib){
		ResetLib(rdPtr->lib);
	}

	return 0;
}

short WINAPI DLLExport EraseLib(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	if (rdPtr->isLib) {
		EraseLib(rdPtr->lib, FilePath);		
	}

	return 0;
}

short WINAPI DLLExport UpdateLib(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	
	if (rdPtr->isLib) {
		EraseLib(rdPtr->lib, FilePath);
		LoadFromFile(rdPtr, FilePath, Key);
	}

	return 0;
}

short WINAPI DLLExport UpdateSrc(LPRDATA rdPtr, long param1, long param2) {
	if (!rdPtr->isLib && rdPtr->img->IsValid()) {
		//rdPtr->src->Delete();
		//rdPtr->src->Clone(*rdPtr->img);

		//rdPtr->hotSpot = rdPtr->imgHotSpot;
		
	}

	return 0;
}
short WINAPI DLLExport RestoreCur(LPRDATA rdPtr, long param1, long param2) {
	if (!rdPtr->isLib && rdPtr->src->IsValid()) {
		//rdPtr->imgHotSpot = rdPtr->hotSpot;

		//rdPtr->img->Delete();
		//rdPtr->img->Clone(*rdPtr->src);
	}

	return 0;
}

short WINAPI DLLExport SetHotSpot(LPRDATA rdPtr, long param1, long param2) {
	HotSpotPos Pos = (HotSpotPos)CNC_GetIntParameter(rdPtr);

	int X = (int)CNC_GetIntParameter(rdPtr);
	int Y = (int)CNC_GetIntParameter(rdPtr);
	
	UpdateHotSpot(rdPtr, Pos, X, Y);
	rdPtr->imgHotSpot = rdPtr->hotSpot;
	
	return 0;
}

short WINAPI DLLExport Zoom(LPRDATA rdPtr, long param1, long param2) {
	float XScale = GetFloatParam(rdPtr);
	float YScale = GetFloatParam(rdPtr);
	
	if (!rdPtr->isLib && rdPtr->src->IsValid()) {
		Zoom(rdPtr, XScale, YScale);
	}	

	return 0;
}

short WINAPI DLLExport Rotate(LPRDATA rdPtr, long param1, long param2) {
	int Angle = (int)CNC_GetIntParameter(rdPtr);
	Angle = Angle % 360;
	
	if (!rdPtr->isLib && rdPtr->src->IsValid()) {
		Rotate(rdPtr, Angle);
	}

	return 0;
}

short WINAPI DLLExport Stretch(LPRDATA rdPtr, long param1, long param2) {
	int Width = (int)CNC_GetIntParameter(rdPtr);
	int Height = (int)CNC_GetIntParameter(rdPtr);

	float XScale = (1.0f * Width/ rdPtr->src->GetWidth());
	float YScale = (1.0f * Height / rdPtr->src->GetHeight());

	if (!rdPtr->isLib && rdPtr->src->IsValid()) {
		Zoom(rdPtr, XScale, YScale);
	}

	return 0;
}

short WINAPI DLLExport AddBackdrop(LPRDATA rdPtr, long param1, long param2) {
	int nObstacleType = ((LPEVP)param1)->evp.evpW.evpW0;
	
	UpdateImg(rdPtr);

	if (!rdPtr->isLib && rdPtr->img->IsValid()) {
		AddBackdrop(rdPtr, rdPtr->img,
			rdPtr->rHo.hoX - rdPtr->rHo.hoAdRunHeader->rhWindowX - rdPtr->imgHotSpot.x,
			rdPtr->rHo.hoY - rdPtr->rHo.hoAdRunHeader->rhWindowY - rdPtr->imgHotSpot.y,
			rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam, nObstacleType, rdPtr->rs.rsLayer);
	}

	return 0;
}
short WINAPI DLLExport UpdateCollision(LPRDATA rdPtr, long param1, long param2) {
	UpdateImg(rdPtr);

	return 0;
}

short WINAPI DLLExport SetCollision(LPRDATA rdPtr, long param1, long param2) {
	bool collision = (bool)CNC_GetIntParameter(rdPtr);
	bool autoUpdateCollision = (bool)CNC_GetIntParameter(rdPtr);

	rdPtr->collision = collision;
	rdPtr->autoUpdateCollision = autoUpdateCollision;

	FreeColMask(rdPtr->pColMask);	

	return 0;
}

short WINAPI DLLExport SetQuality(LPRDATA rdPtr, long param1, long param2) {
	bool Quality = (bool)CNC_GetIntParameter(rdPtr);

	rdPtr->stretchQuality = Quality;

	UpdateImg(rdPtr, false, true);

	return 0;
}

short WINAPI DLLExport AffineTrans(LPRDATA rdPtr, long param1, long param2) {
	bool Quality = (bool)CNC_GetIntParameter(rdPtr);

	ATArray A = {};

	if (rdPtr->AT != A) {
		rdPtr->AT = A;

		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport Offset(LPRDATA rdPtr, long param1, long param2) {
	int XOffset = (int)CNC_GetIntParameter(rdPtr);
	int YOffset = (int)CNC_GetIntParameter(rdPtr);

	bool Wrap = (bool)CNC_GetIntParameter(rdPtr);

	if (rdPtr->offset != OffsetCoef{ XOffset, YOffset, Wrap }) {
		rdPtr->offset = { XOffset ,YOffset, Wrap };

		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport CleanCache(LPRDATA rdPtr, long param1, long param2) {
	CleanCache(rdPtr, true);

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport GetHotSpotX(LPRDATA rdPtr, long param1) {
	return GetHotSpotX(rdPtr);
}
long WINAPI DLLExport GetHotSpotY(LPRDATA rdPtr, long param1) {
	return  GetHotSpotY(rdPtr);
}

long WINAPI DLLExport GetOriginalWidth(LPRDATA rdPtr, long param1) {
	return GetOriginalWidth(rdPtr);
}
long WINAPI DLLExport GetOriginalHeight(LPRDATA rdPtr, long param1) {
	return GetOriginalHeight(rdPtr);
}

long WINAPI DLLExport GetCurrentWidth(LPRDATA rdPtr, long param1) {
	return GetCurrentWidth(rdPtr);
}
long WINAPI DLLExport GetCurrentHeight(LPRDATA rdPtr, long param1) {
	return GetCurrentHeight(rdPtr);
}

long WINAPI DLLExport GetXZoomScale(LPRDATA rdPtr, long param1) {
	return ReturnFloat(GetXZoomScale(rdPtr));
}
long WINAPI DLLExport GetYZoomScale(LPRDATA rdPtr, long param1) {
	return ReturnFloat(GetYZoomScale(rdPtr));
}

long WINAPI DLLExport GetAngle(LPRDATA rdPtr, long param1) {
	return GetAngle(rdPtr);
}

long WINAPI DLLExport GetFileName(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->FileName->c_str();
}

long WINAPI DLLExport GetFilePath(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->FilePath->c_str();
}

long WINAPI DLLExport GetKey(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->Key->c_str();
}

long WINAPI DLLExport GetSurfacePointer(LPRDATA rdPtr, long param1) {
	std::wstring FilePath = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	std::wstring Key = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	cSurface* ret = nullptr;

	if (!rdPtr->isLib) {
		if (*rdPtr->FilePath != FilePath || *rdPtr->Key != Key) {
			ret = nullptr;
		}
		else {
			ret = rdPtr->src;
		}
	}
	else {
		auto it = rdPtr->lib->find(FilePath);
		if (it == rdPtr->lib->end()) {
			LoadFromFile(rdPtr, FilePath.c_str(), Key.c_str());
		}

		it = rdPtr->lib->find(FilePath);
		if (it == rdPtr->lib->end()) {
			ret = nullptr;
		}
		else {
			ret = it->second;
		}
	}
	
	return ConvertToLong<cSurface*>(ret);
}

long WINAPI DLLExport GetAVGCoordX(LPRDATA rdPtr, long param1) {
	//Get AVG Coord
	int coord=(int)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	bool realCoordMode = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	
	auto XLF = rdPtr->rHo.hoAdRunHeader->rh3.rh3DisplayX;
	auto WW = rdPtr->rHo.hoAdRunHeader->rh3.rh3WindowSx;

	if (!realCoordMode) {
		return XLF + coord + WW / 2;
	}
	else {
		return XLF + coord;
	}
}

long WINAPI DLLExport GetAVGCoordY(LPRDATA rdPtr, long param1) {
	//Get AVG Coord
	int coord = (int)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	bool realCoordMode = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	auto YTF = rdPtr->rHo.hoAdRunHeader->rh3.rh3DisplayY;
	auto WH = rdPtr->rHo.hoAdRunHeader->rh3.rh3WindowSy;

	if (!realCoordMode) {
		return YTF + (WH - coord) - GetCurrentHeight(rdPtr) / 2;
	}
	else {
		return YTF + coord;
	}
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
			OnPreloadComplete,

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			LoadFromFile,
			LoadFromLib,

			ResetLib,
			EraseLib,
			UpdateLib,

			SetHotSpot,
			
			Zoom,
			Rotate,

			UpdateSrc,
			RestoreCur,

			Stretch,

			AddBackdrop,
			UpdateCollision,

			SetCollision,
			SetQuality,

			AffineTrans,
			Offset,

			LoadFromDisplay,

			SetPreloadList,
			SetPreloadPath,
			CleanCache,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			GetHotSpotX,
			GetHotSpotY,

			GetOriginalWidth,
			GetOriginalHeight,

			GetCurrentWidth,
			GetCurrentHeight,

			GetXZoomScale,
			GetYZoomScale,

			GetAngle,

			GetFileName,
			GetKey,

			GetSurfacePointer,

			GetAVGCoordX,
			GetAVGCoordY,

			GetFilePath,

			0
			};