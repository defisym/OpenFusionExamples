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
		IDMN_CONDITION_OITRC, M_CONDITION_OITRC, CND_CONDITION_OITRC, 0, 0,

		IDMN_CONDITION_CD, M_CONDITION_CD, CND_CONDITION_CD, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		
		IDMN_CONDITION_CDT, M_CONDITION_CDT, CND_CONDITION_CDT, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		IDMN_CONDITION_LHI, M_CONDITION_LHI, CND_CONDITION_LHI, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPSTRING, M_ACTION_FILENAME,
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
		IDMN_ACTION_CC, M_ACTION_CC, ACT_ACTION_CC,	0, 1, PARAM_EXPRESSION, M_ACTION_MEMLIMIT,

		IDMN_ACTION_SKL, M_ACTION_SKL, ACT_ACTION_SKL,	0, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACTION_KEEPLIST, M_ACTION_BASEPATH,

		IDMN_ACTION_ITRC, M_ACTION_ITRC, ACT_ACTION_ITRC,	0, 1, PARAM_EXPRESSION, M_ACTION_ITSIZE,

		IDMN_ACTION_LFP, M_ACTION_LFP, ACT_ACTION_LFP,	0, 2, PARAM_EXPRESSION, PARAM_EXPSTRING, M_ACTION_PSF, M_ACTION_FILENAME,

		IDMN_ACTION_SB, M_ACTION_SB, ACT_ACTION_SB, 0, 3, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_ACTION_RADIUS, M_ACTION_SCALE, M_ACTION_DIVIDE,

		IDMN_ACTION_SESP, M_ACTION_SESP,	ACT_ACTION_SESP, 0, 4,PARAM_OBJECT,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPSTRING,M_ACTION_OBJECT,M_ACTION_FILENAME,M_ACTION_KEY,M_ACTION_EFFECTNAME,

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
		IDMN_EXPRESSION_GRFP, M_EXPRESSION_GRFP, EXP_EXPRESSION_GRFP, EXPFLAG_STRING, 1, EXPPARAM_STRING, M_EXPRESSION_BASEPATH,

		IDMN_EXPRESSION_GITRCK, M_EXPRESSION_GITRCK, EXP_EXPRESSION_GITRCK, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GITRCVC, M_EXPRESSION_GITRCVC, EXP_EXPRESSION_GITRCVC, 0, 0,
		IDMN_EXPRESSION_GITRCVP, M_EXPRESSION_GITRCVP, EXP_EXPRESSION_GITRCVP, 0, 0,

		IDMN_EXPRESSION_GFFN, M_EXPRESSION_GFFN, EXP_EXPRESSION_GFFN, EXPFLAG_STRING, 1, EXPPARAM_STRING, M_ACTION_FILENAME,
		IDMN_EXPRESSION_GFRFP, M_EXPRESSION_GFRFP, EXP_EXPRESSION_GFRFP, EXPFLAG_STRING, 2, EXPPARAM_STRING, EXPPARAM_STRING, M_ACTION_FILENAME, M_EXPRESSION_BASEPATH,
		
		IDMN_EXPRESSION_GRUMB, M_EXPRESSION_GRUMB, EXP_EXPRESSION_GRUMB, 0, 0,
		
		IDMN_EXPRESSION_GGPUN, M_EXPRESSION_GGPUN, EXP_EXPRESSION_GGPUN, EXPFLAG_STRING, 0,
		
		IDMN_EXPRESSION_GVRUMB, M_EXPRESSION_GVRUMB, EXP_EXPRESSION_GVRUMB, 0, 0,
		IDMN_EXPRESSION_GVRBMB, M_EXPRESSION_GVRBMB, EXP_EXPRESSION_GVRBMB, 0, 0,
		IDMN_EXPRESSION_GVRAMB, M_EXPRESSION_GVRAMB, EXP_EXPRESSION_GVRAMB, 0, 0,
		
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

long WINAPI DLLExport OnIterateRefCount(LPRDATA rdPtr, long param1, long param2) {
	return TRUE;
}

long WINAPI DLLExport CanDisplay(LPRDATA rdPtr, long param1, long param2) {
	return CanDisplay(rdPtr);
}

long WINAPI DLLExport CurrentDisplayTransparent(LPRDATA rdPtr, long param1, long param2) {
	if (!CanDisplay(rdPtr)) {
		rdPtr->bCurrentDisplayTransparent = true;
	}
	else {
		if (rdPtr->src != rdPtr->pOldSf) {
			rdPtr->pOldSf = rdPtr->src;

			if (rdPtr->fromLib) {
				if (rdPtr->pLibValue->isTransparent == transpTBD) {	// not updated yet
					rdPtr->pLibValue->isTransparent = IsTransparent(rdPtr->src);
				}

				rdPtr->bCurrentDisplayTransparent = rdPtr->pLibValue->isTransparent;
			}
			else {
				rdPtr->bCurrentDisplayTransparent = IsTransparent(rdPtr->src);
			}
		}
	}

	return rdPtr->bCurrentDisplayTransparent;
}

long WINAPI DLLExport LibHasItem(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR pFileName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	auto fullPath = GetFullPathNameStr(pFileName);

	return rdPtr->isLib && rdPtr->pLib->find(fullPath) != rdPtr->pLib->end() ? true : false;
}

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport LoadFromFile(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);

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

short WINAPI DLLExport LoadFromPointer(LPRDATA rdPtr, long param1, long param2) {
	LPSURFACE pSf = ConvertToType<LPSURFACE>(CNC_GetParameter(rdPtr));
	LPCWSTR pFileName= (LPCWSTR)CNC_GetParameter(rdPtr);	

	LoadFromPointer(rdPtr, pFileName, pSf);

	return 0;
}

short WINAPI DLLExport SetPreloadList(LPRDATA rdPtr, long param1, long param2) {
	long list = (long)CNC_GetIntParameter(rdPtr);

	LPCWSTR BasePath = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	LPCWSTR Key = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (!rdPtr->preloading
		&& rdPtr->isLib) {
#ifndef _DEBUG		// only works in runtime due to /MD & /MDd
		auto pList = ConvertToType<pPreLoadList>(list);

		if (pList != nullptr) {
			CreatePreloadProcess(rdPtr, pList, false, BasePath, Key);
		}
#else				// load base path instead for test
		//std::vector<std::wstring> fileList = { L"dialog1.png",
		//										L"dialog2.png",
		//										L"dialog3.png",
		//										L"dianull.png",
		//										L"nameback1.png",
		//										L"nameback2.png",
		//										L"nameback3.png",
		//										L"nameback4.png",
		//										L"namenull.png" };
		//CreatePreloadProcess(rdPtr, &fileList, false, BasePath, Key);
		
		//std::vector<std::wstring> fileList;
		//GetFileList(&fileList, BasePath);

		//CreatePreloadProcess(rdPtr, &fileList, true, BasePath, Key);
#endif // !_DEBUG	
	}


	return 0;
}

short WINAPI DLLExport SetPreloadPath(LPRDATA rdPtr, long param1, long param2) {
	LPCWSTR BasePath = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	LPCWSTR Key = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (!rdPtr->preloading
		&&rdPtr->isLib) {
		FileList* pFileList = GetFileList(rdPtr, BasePath);

		CreatePreloadProcess(rdPtr, pFileList, true, BasePath, Key);
	}

	return 0;
}

short WINAPI DLLExport CleanCache(LPRDATA rdPtr, long param1, long param2) {
	size_t memLimit = (size_t)CNC_GetIntParameter(rdPtr);
	
	CleanCache(rdPtr, true, memLimit);

	return 0;
}

short WINAPI DLLExport SetKeepList(LPRDATA rdPtr, long param1, long param2) {
	std::wstring keepListSrc = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring basePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	if (rdPtr->isLib) {
		KeepList keepList;
		size_t start = keepListSrc.find_first_not_of(L'|');
		size_t end = start;

		while (start != std::wstring::npos) {
			end = keepListSrc.find(L'|', start);
			keepList.emplace_back(std::wstring_view(keepListSrc.c_str() + start, end - start));
			start = keepListSrc.find_first_not_of(L'|', end);
		}

		GetKeepList(rdPtr, keepList, basePath);
	}

	return 0;
}

short WINAPI DLLExport IterateRefCount(LPRDATA rdPtr, long param1, long param2) {
	size_t num = (size_t)CNC_GetIntParameter(rdPtr);

	if (rdPtr->isLib) {
		UpdateRefCountVec(rdPtr);

		auto itSz = min(num, rdPtr->pCountVec->size());

		for (size_t it = 0; it < itSz; it++) {
			auto& refCount = (*rdPtr->pCountVec)[it];
			*rdPtr->itCountVecStr = refCount.first;
			*rdPtr->itCountVecCount = refCount.second;

			CallEvent(ONITREFCOUNT);
		}
	}
	
	return 0;
}

short WINAPI DLLExport ResetLib(LPRDATA rdPtr, long param1, long param2) {
	if(rdPtr->isLib){
		ResetLib(rdPtr, rdPtr->pLib);
	}

	return 0;
}

short WINAPI DLLExport EraseLib(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	
	if (rdPtr->isLib) {
		EraseLib(rdPtr->pLib, FilePath);		
	}

	return 0;
}

short WINAPI DLLExport UpdateLib(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	
	if (rdPtr->isLib) {
		if (NeedUpdateLib(rdPtr->pLib, FilePath)) {
			EraseLib(rdPtr->pLib, FilePath);
			LoadFromFile(rdPtr, FilePath, Key);
		}
	}

	return 0;
}

short WINAPI DLLExport UpdateSrc(LPRDATA rdPtr, long param1, long param2) {
	if (CanDisplay(rdPtr)) {
		//rdPtr->src->Delete();
		//rdPtr->src->Clone(*rdPtr->img);

		//rdPtr->hotSpot = rdPtr->imgHotSpot;
		
	}

	return 0;
}
short WINAPI DLLExport RestoreCur(LPRDATA rdPtr, long param1, long param2) {
	if (CanDisplay(rdPtr)) {
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
	
	if (CanDisplay(rdPtr)) {
		Zoom(rdPtr, XScale, YScale);
	}	

	return 0;
}

short WINAPI DLLExport Stretch(LPRDATA rdPtr, long param1, long param2) {
	int Width = (int)CNC_GetIntParameter(rdPtr);
	int Height = (int)CNC_GetIntParameter(rdPtr);

	if (CanDisplay(rdPtr)) {
		float XScale = (1.0f * Width / rdPtr->src->GetWidth());
		float YScale = (1.0f * Height / rdPtr->src->GetHeight());

		Zoom(rdPtr, XScale, YScale);
	}

	return 0;
}

short WINAPI DLLExport Rotate(LPRDATA rdPtr, long param1, long param2) {
	int Angle = (int)CNC_GetIntParameter(rdPtr);
	Angle = Angle % 360;
	
	if (CanDisplay(rdPtr)) {
		Rotate(rdPtr, Angle);
	}

	return 0;
}

short WINAPI DLLExport Offset(LPRDATA rdPtr, long param1, long param2) {
	int XOffset = (int)CNC_GetIntParameter(rdPtr);
	int YOffset = (int)CNC_GetIntParameter(rdPtr);

	bool Wrap = (bool)CNC_GetIntParameter(rdPtr);

	if (CanDisplay(rdPtr)) {
		rdPtr->offset = { XOffset ,YOffset, Wrap };

		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport AddBackdrop(LPRDATA rdPtr, long param1, long param2) {
	int nObstacleType = ((LPEVP)param1)->evp.evpW.evpW0;
	
	if (CanDisplay(rdPtr)) {
		GetTransfromedBitmap(rdPtr, [&](LPSURFACE pCollideBitmap) {
			AddBackdrop(rdPtr, pCollideBitmap,
				rdPtr->rHo.hoX - rdPtr->rHo.hoAdRunHeader->rhWindowX - rdPtr->hotSpot.x,
				rdPtr->rHo.hoY - rdPtr->rHo.hoAdRunHeader->rhWindowY - rdPtr->hotSpot.y,
				rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam, nObstacleType, rdPtr->rs.rsLayer);
			});
	}
	
	return 0;
}
short WINAPI DLLExport UpdateCollision(LPRDATA rdPtr, long param1, long param2) {
	UpdateImg(rdPtr);

	return 0;
}

short WINAPI DLLExport SetCollision(LPRDATA rdPtr, long param1, long param2) {
	//bool collision = (bool)CNC_GetIntParameter(rdPtr);
	//bool autoUpdateCollision = (bool)CNC_GetIntParameter(rdPtr);

	//rdPtr->collision = collision;
	//rdPtr->autoUpdateCollision = autoUpdateCollision;

	//FreeColMask(rdPtr->pColMask);	

	return 0;
}

short WINAPI DLLExport SetQuality(LPRDATA rdPtr, long param1, long param2) {
	bool Quality = (bool)CNC_GetIntParameter(rdPtr);

	rdPtr->stretchQuality = Quality;

	//UpdateImg(rdPtr, false, true);

	return 0;
}

short WINAPI DLLExport AffineTrans(LPRDATA rdPtr, long param1, long param2) {
	bool Quality = (bool)CNC_GetIntParameter(rdPtr);

	ATArray A = {};

	if (CanDisplay(rdPtr)) {
		rdPtr->AT = A;

		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport StackBlur(LPRDATA rdPtr, long param1, long param2) {
	int radius = CNC_GetIntParameter(rdPtr);
	float scale = GetFloatParam(rdPtr);
	int divide = CNC_GetIntParameter(rdPtr);

	if (CanDisplay(rdPtr)) {
		bool bReleaseOld = false;
		LPSURFACE pOldSf = nullptr;

		if (rdPtr->fromLib) {
			pOldSf = rdPtr->src;
			rdPtr->src = nullptr;

			UpdateRef(rdPtr, false);
			rdPtr->pRefCount = nullptr;

			bReleaseOld = false;
		}
		else {
			pOldSf = rdPtr->src;
			rdPtr->src = nullptr;

			bReleaseOld = true;
		}

		LPSURFACE pMemSf = nullptr;
		auto bHwa = IsHWA(pOldSf);

		// convert to bitmap
		if (rdPtr->fromLib) {	// copy & convert, no matter HWA or not
			pMemSf = ConvertBitmap(rdPtr, pOldSf);

			bReleaseOld = false;
		}
		else {
			if (bHwa) {
				pMemSf = ConvertBitmap(rdPtr, pOldSf);
								
				bReleaseOld = true;
			}
			else {
				pMemSf = pOldSf;

				bReleaseOld = false;
			}
		}		

		if (bReleaseOld) {
			delete pOldSf;
		}	

		rdPtr->fromLib = false;

		StackBlur(pMemSf, radius, scale, divide);

		rdPtr->src = pMemSf;
		
		if (bHwa) {
			ConvertToHWATexture(rdPtr, rdPtr->src);
		}

		NewImg(rdPtr);
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport SetEffectSurfaceParam(LPRDATA rdPtr, long param1, long param2) {
	LPRDATA object = (LPRDATA)CNC_GetParameter(rdPtr);

	LPCTSTR pFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR pKey = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	LPCTSTR pParamName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	if (!object->isLib) {
		return 0;
	}

	// surface used in surface param must be hwa type
	auto pLibHWAType = object->HWA;
	object->HWA = true;

	auto it = _LoadLib(rdPtr, object, pFilePath, pKey);

	if (it != object->pLib->end()) {
		// set param
		auto pCurEffect = GetEffect(rdPtr);

		if (pCurEffect == nullptr) {
			return 0;
		}

		auto paramIndex = pCurEffect->GetParamIndex(ConvertWStrToStr(pParamName).c_str());		

		if (paramIndex == -1) {
			return 0;
		}

		auto paramType = pCurEffect->GetParamType(paramIndex);

		if (paramType != EFFECTPARAM_SURFACE) {
			return 0;
		}

		auto pSf = it->second.pSf;		
		auto pImpl = GetSurfaceImplementation(*pSf);

		pCurEffect->SetParamSurfaceValue(paramIndex, pImpl);

		// add to keep list
		it->second.bUsedInShader = true;

		auto fullPath = GetFullPathNameStr(pFilePath);

		if (std::find(object->pKeepList->begin(), object->pKeepList->end(), fullPath) == object->pKeepList->end()) {
			object->pKeepList->emplace_back(fullPath);
		}
	}

	object->HWA = pLibHWAType;

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

long WINAPI DLLExport GetFileFileName(LPRDATA rdPtr, long param1) {
	std::wstring FilePath = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	
	auto FileName = GetFileName(FilePath);	

	return ReturnString(FileName);
}

long WINAPI DLLExport GetFilePath(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->FilePath->c_str();
}

long WINAPI DLLExport GetRelativeFilePath(LPRDATA rdPtr, long param1) {
	std::wstring BasePath = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	
	auto pos = BasePath.size() + 1;

	try {
		*rdPtr->RelativeFilePath = rdPtr->FilePath->substr(pos, rdPtr->FilePath->size() - pos);
	}
	catch(...) {
		//*rdPtr->RelativeFilePath = L"Invalid";
		*rdPtr->RelativeFilePath = *rdPtr->FilePath;
	}

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->RelativeFilePath->c_str();
}

long WINAPI DLLExport GetFileRelativeFilePath(LPRDATA rdPtr, long param1) {
	std::wstring FilePath = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	std::wstring BasePath = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);	

	auto FileName = GetRelativeFilePath(FilePath, BasePath);

	return ReturnString(FileName);
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
		auto it = _LoadLib(rdPtr, rdPtr, FilePath.c_str(), Key.c_str());

		if (it == rdPtr->pLib->end()) {
			ret = nullptr;
		}
		else {
			ret = it->second.pSf;
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

long WINAPI DLLExport GetIterateRefCountKey(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->itCountVecStr->c_str();
}

long WINAPI DLLExport GetIterateRefCountValueCount(LPRDATA rdPtr, long param1) {
	return rdPtr->itCountVecCount->count;
}

long WINAPI DLLExport GetIterateRefCountValuePriority(LPRDATA rdPtr, long param1) {
	return rdPtr->itCountVecCount->priority;
}

long WINAPI DLLExport GetRAMUsageMB(LPRDATA rdPtr, long param1) {
	return long(GetProcessMemoryUsageMB());
}

long WINAPI DLLExport GetGPUName(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
#ifdef _USE_DXGI
	return (long)rdPtr->pD3DU->GetDesc().Description;
#else
	return (long)L"No DXGI Enabled";
#endif
}

long WINAPI DLLExport GetVRAMUsageMB(LPRDATA rdPtr, long param1) {
#ifdef _USE_DXGI
	rdPtr->pD3DU->UpdateVideoMemoryInfo();

	return long(rdPtr->pD3DU->GetLocalVideoMemoryInfo().CurrentUsage >> 20);
#else
	return -1;
#endif	
}

long WINAPI DLLExport GetVRAMBudgetMB(LPRDATA rdPtr, long param1) {
#ifdef _USE_DXGI
	rdPtr->pD3DU->UpdateVideoMemoryInfo();

	return long(rdPtr->pD3DU->GetLocalVideoMemoryInfo().Budget >> 20);
#else
	return -1;
#endif	
}

long WINAPI DLLExport GetVRAMAvailableMB(LPRDATA rdPtr, long param1) {
#ifdef _USE_DXGI
	rdPtr->pD3DU->UpdateVideoMemoryInfo();

	return long(rdPtr->pD3DU->GetLocalVideoMemoryInfo().AvailableForReservation >> 20);
#else
	return -1;
#endif
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
			OnIterateRefCount,

			CanDisplay,

			CurrentDisplayTransparent,
			LibHasItem,

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

			SetKeepList,

			IterateRefCount,

			LoadFromPointer,

			StackBlur,

			SetEffectSurfaceParam,

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
			GetRelativeFilePath,

			GetIterateRefCountKey,
			GetIterateRefCountValueCount,
			GetIterateRefCountValuePriority,

			GetFileFileName,
			GetFileRelativeFilePath,

			GetRAMUsageMB,

			GetGPUName,

			GetVRAMUsageMB,
			GetVRAMBudgetMB,
			GetVRAMAvailableMB,

			0
			};