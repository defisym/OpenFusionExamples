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

		IDMN_CONDITION_OAF, M_CONDITION_OAF, CND_CONDITION_OAF, 0, 0,
		IDMN_CONDITION_IAP, M_CONDITION_IAP, CND_CONDITION_IAP, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		IDMN_CONDITION_IAPAUSED, M_CONDITION_IAPAUSED, CND_CONDITION_IAPAUSED, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,

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
		
		IDMN_ACTION_SPLBP, M_ACTION_SPLBP, ACT_ACTION_SPLBP,	0, 3, PARAM_EXPRESSION, PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACTION_PRELOAD, M_ACTION_BASEPATH, M_ACTION_KEY,
		IDMN_ACTION_SPP, M_ACTION_SPP, ACT_ACTION_SPP,	0, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACTION_BASEPATH, M_ACTION_KEY,
		IDMN_ACTION_CC, M_ACTION_CC, ACT_ACTION_CC,	0, 1, PARAM_EXPRESSION, M_ACTION_MEMLIMIT,

		IDMN_ACTION_SKL, M_ACTION_SKL, ACT_ACTION_SKL,	0, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACTION_KEEPLIST, M_ACTION_BASEPATH,
		
		IDMN_ACTION_ITRC, M_ACTION_ITRC, ACT_ACTION_ITRC,	0, 1, PARAM_EXPRESSION, M_ACTION_ITSIZE,

		IDMN_ACTION_LFP, M_ACTION_LFP, ACT_ACTION_LFP,	0, 2, PARAM_EXPRESSION, PARAM_EXPSTRING, M_ACTION_PSF, M_ACTION_FILENAME,

		IDMN_ACTION_SB, M_ACTION_SB, ACT_ACTION_SB, 0, 3, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_ACTION_RADIUS, M_ACTION_SCALE, M_ACTION_DIVIDE,

		IDMN_ACTION_SESP, M_ACTION_SESP,	ACT_ACTION_SESP, 0, 4,PARAM_OBJECT,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPSTRING,M_ACTION_OBJECT,M_ACTION_FILENAME,M_ACTION_KEY,M_ACTION_EFFECTNAME,

		IDMN_ACTION_SPL, M_ACTION_SPL, ACT_ACTION_SPL,	0, 3, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACTION_KEEPLIST, M_ACTION_BASEPATH, M_ACTION_KEY,
		IDMN_ACTION_SKLBP, M_ACTION_SKLBP, ACT_ACTION_SKLBP,	0, 2, PARAM_EXPRESSION, PARAM_EXPSTRING, M_ACTION_PRELOAD, M_ACTION_BASEPATH,
		
		IDMN_ACTION_FM, M_ACTION_FM, ACT_ACTION_FM,	0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_ACTION_S_WIDTH, M_ACTION_S_HEIGHT,

		IDMN_ACTION_STF, M_ACTION_STF, ACT_ACTION_STF,	0, 3, PARAM_EXPSTRING, PARAM_EXPSTRING ,PARAM_EXPSTRING, M_ACTION_FILENAME, M_ACTION_KEY, M_ACTION_SAVEFILENAME,
		
		IDMN_ACTION_CF, M_ACTION_CF, ACT_ACTION_CF,	0, 0,

		IDMN_ACTION_STFWS, M_ACTION_STFWS, ACT_ACTION_STFWS,	0, 5, PARAM_EXPSTRING, PARAM_EXPSTRING ,PARAM_EXPSTRING, PARAM_EXPRESSION, PARAM_EXPRESSION, M_ACTION_FILENAME, M_ACTION_KEY, M_ACTION_SAVEFILENAME, M_ACTION_S_WIDTH, M_ACTION_S_HEIGHT,

		IDMN_ACTION_STC, M_ACTION_STC, ACT_ACTION_STC, 0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_ACTION_COLOR, M_ACTION_ALPHA,

		IDMN_ACTION_SAS, M_ACTION_SAS,	ACT_ACTION_SAS,	0, 3, PARAM_OBJECT,PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACTION_OBJECT, M_ACTION_BASEPATH, M_ACTION_KEY,
		IDMN_ACTION_LA, M_ACTION_LA, ACT_ACTION_LA,	0, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACTION_FILENAME, M_ACTION_KEY,
		IDMN_ACTION_SA, M_ACTION_SA, ACT_ACTION_SA,	0, 0,
		IDMN_ACTION_SASPEED, M_ACTION_SASPEED, ACT_ACTION_SASPEED,	0, 1, PARAM_EXPRESSION, M_ACTION_SPEED,
		IDMN_ACTION_PA, M_ACTION_PA, ACT_ACTION_PA,	0, 0,
		IDMN_ACTION_RA, M_ACTION_RA, ACT_ACTION_RA,	0, 0,

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

		IDMN_EXPRESSION_GAN, M_EXPRESSION_GAN, EXP_EXPRESSION_GAN, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GAFID, M_EXPRESSION_GAFID, EXP_EXPRESSION_GAFID, 0, 0,
		IDMN_EXPRESSION_GAFINDEX, M_EXPRESSION_GAFINDEX, EXP_EXPRESSION_GAFINDEX, 0, 0,
		IDMN_EXPRESSION_GAS, M_EXPRESSION_GAS, EXP_EXPRESSION_GAS, 0, 0,

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
	auto pFileName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	auto fullPath = GetFullPathNameStr(pFileName);

	return rdPtr->isLib && rdPtr->pLib->find(fullPath) != rdPtr->pLib->end() ? true : false;
}

long WINAPI DLLExport OnAnimationFinished(LPRDATA rdPtr, long param1, long param2) {
	return TRUE;
}

long WINAPI DLLExport IsAnimationPlaying(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->pAI->pA != nullptr;
}

long WINAPI DLLExport IsAnimationPaused(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->pAI->GetPauseState();

}

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport LoadFromFile(LPRDATA rdPtr, long param1, long param2) {
	auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	auto Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pAI->StopAnimation();
	LoadFromFile(rdPtr, FilePath, Key);

	return 0;
}

short WINAPI DLLExport LoadFromLib(LPRDATA rdPtr, long param1, long param2) {
	auto object = (LPRO)CNC_GetParameter(rdPtr);

	auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	auto Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pAI->StopAnimation();
	LoadFromLib(rdPtr, object, FilePath, Key);

	return 0;
}

short WINAPI DLLExport LoadFromDisplay(LPRDATA rdPtr, long param1, long param2) {
	auto object = (LPRO)CNC_GetParameter(rdPtr);
	bool UpdateCoef = (bool)CNC_GetIntParameter(rdPtr);

	rdPtr->pAI->StopAnimation();
	LoadFromDisplay(rdPtr, object, UpdateCoef);

	return 0;
}

short WINAPI DLLExport LoadFromPointer(LPRDATA rdPtr, long param1, long param2) {
	auto pSf = ConvertToType<LPSURFACE>(CNC_GetParameter(rdPtr));
	auto pFileName= (LPCWSTR)CNC_GetParameter(rdPtr);	

#ifdef _DEBUG
	//__SavetoClipBoard(pSf);
#endif // _DEBUG

	rdPtr->pAI->StopAnimation();
	LoadFromPointer(rdPtr, pFileName, pSf);

	return 0;
}

short WINAPI DLLExport SetPreloadList(LPRDATA rdPtr, long param1, long param2) {
	std::wstring listSrc = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	std::wstring basePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	auto Key = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (!rdPtr->preloading
		&& rdPtr->isLib) {		
		List list = std::move(SplitString(listSrc, Delimiter));		

		CreatePreloadProcess(rdPtr, &list, false, basePath, Key);
	}

	return 0;
}

short WINAPI DLLExport SetPreloadListByPointer(LPRDATA rdPtr, long param1, long param2) {
	long list = (long)CNC_GetIntParameter(rdPtr);

	auto BasePath = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	auto Key = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (!rdPtr->preloading
		&& rdPtr->isLib) {
#ifndef _DEBUG		
		// only works in runtime due to /MD & /MDd
		auto pList = ConvertToType<pPreLoadList>(list);

		if (pList != nullptr) {
			CreatePreloadProcess(rdPtr, pList, false, BasePath, Key);
		}
#else				
		//load base path instead for test		
		List fileList;
		GetFileList(&fileList, BasePath);

		CreatePreloadProcess(rdPtr, &fileList, true, BasePath, Key);
#endif // !_DEBUG	
	}

	return 0;
}

short WINAPI DLLExport SetPreloadPath(LPRDATA rdPtr, long param1, long param2) {
	auto BasePath = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	auto Key = (LPCWSTR)CNC_GetStringParameter(rdPtr);

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
		KeepList keepList = std::move(SplitString(keepListSrc, Delimiter));

		GetKeepList(rdPtr, keepList, basePath);
	}

	return 0;
}

short WINAPI DLLExport SetKeepListByPointer(LPRDATA rdPtr, long param1, long param2) {
	long list = (long)CNC_GetIntParameter(rdPtr);

	auto basePath = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (rdPtr->isLib) {
#ifndef _DEBUG		
		// only works in runtime due to /MD & /MDd
		auto pKeepList = ConvertToType<pPreLoadList>(list);

		GetKeepList(rdPtr, *pKeepList, basePath);
#else				
		//load base path instead for test		
		List keepList;
		GetFileList(&keepList, basePath);

		GetKeepList(rdPtr, keepList, basePath);
#endif // !_DEBUG	
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
	auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	
	if (rdPtr->isLib) {
		EraseLib(rdPtr->pLib, FilePath);		
	}

	return 0;
}

short WINAPI DLLExport UpdateLib(LPRDATA rdPtr, long param1, long param2) {
	auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	auto Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	
	if (rdPtr->isLib) {
		if (NeedUpdateLib(rdPtr->pLib, FilePath)) {
			EraseLib(rdPtr->pLib, FilePath);
			LoadFromFile(rdPtr, FilePath, Key);
		}
	}

	return 0;
}

// deprecated
short WINAPI DLLExport UpdateSrc(LPRDATA rdPtr, long param1, long param2) {
	if (CanDisplay(rdPtr)) {
		
	}

	return 0;
}
// deprecated
short WINAPI DLLExport RestoreCur(LPRDATA rdPtr, long param1, long param2) {
	if (CanDisplay(rdPtr)) {

	}

	return 0;
}

short WINAPI DLLExport SetHotSpot(LPRDATA rdPtr, long param1, long param2) {
	auto Pos = (HotSpotPos)CNC_GetIntParameter(rdPtr);

	int X = (int)CNC_GetIntParameter(rdPtr);
	int Y = (int)CNC_GetIntParameter(rdPtr);
	
	rdPtr->pAI->StopAnimation();
	UpdateHotSpot(rdPtr, Pos, X, Y);
	
	return 0;
}

short WINAPI DLLExport Zoom(LPRDATA rdPtr, long param1, long param2) {
	float XScale = GetFloatParam(rdPtr);
	float YScale = GetFloatParam(rdPtr);
	
	rdPtr->pAI->StopAnimation();
	
	if (CanDisplay(rdPtr)) {
		Zoom(rdPtr, XScale, YScale);
	}	

	return 0;
}

short WINAPI DLLExport Stretch(LPRDATA rdPtr, long param1, long param2) {
	int Width = (int)CNC_GetIntParameter(rdPtr);
	int Height = (int)CNC_GetIntParameter(rdPtr);

	rdPtr->pAI->StopAnimation();

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
	
	rdPtr->pAI->StopAnimation();

	if (CanDisplay(rdPtr)) {
		Rotate(rdPtr, Angle);
	}

	return 0;
}

short WINAPI DLLExport Offset(LPRDATA rdPtr, long param1, long param2) {
	int XOffset = (int)CNC_GetIntParameter(rdPtr);
	int YOffset = (int)CNC_GetIntParameter(rdPtr);

	bool Wrap = (bool)CNC_GetIntParameter(rdPtr);

	rdPtr->pAI->StopAnimation();

	if (CanDisplay(rdPtr)) {
		rdPtr->offset = { XOffset ,YOffset, Wrap };

		if (rdPtr->offset.XOffset == 0 && rdPtr->offset.YOffset == 0) {
			return 0;
		}

		const auto pRTT = GetSurface(rdPtr, rdPtr->src->GetWidth(), rdPtr->src->GetHeight());
		pRTT->CreateAlpha();

		ProcessHWA(rdPtr, rdPtr->src, [&] (const LPSURFACE pBitmap) {
			Offset(rdPtr->src, pRTT, rdPtr->offset);
			});

		if (rdPtr->fromLib) {
			rdPtr->fromLib = false;

			UpdateRef(rdPtr, false);
			rdPtr->pRefCount = nullptr;

			rdPtr->pLibValue = nullptr;
		}
		else {
			ReleaseNonFromLib(rdPtr);
		}

		// ConvertToHWATexture can't handle HWA conversions
		rdPtr->src = ConvertBitmap(pRTT);

		if(rdPtr->pData->bPreMulAlpha) {
			rdPtr->src->PremultiplyAlpha();
		}

		if (rdPtr->HWA) {
			ConvertToHWATexture(rdPtr, rdPtr->src);
		}

		delete pRTT;

		NewNonFromLib(rdPtr, rdPtr->src);
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

// deprecated
short WINAPI DLLExport UpdateCollision(LPRDATA rdPtr, long param1, long param2) {
	return 0;
}

// deprecated
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

// deprecated
short WINAPI DLLExport AffineTrans(LPRDATA rdPtr, long param1, long param2) {
	bool Quality = (bool)CNC_GetIntParameter(rdPtr);

	ATArray A = {};

	rdPtr->pAI->StopAnimation();

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

	rdPtr->pAI->StopAnimation();

	if (CanDisplay(rdPtr)) {
		bool bReleaseOld = false;
		LPSURFACE pOldSf = nullptr;

		if (rdPtr->fromLib) {
			pOldSf = rdPtr->src;
			rdPtr->src = nullptr;

			UpdateRef(rdPtr, false);
			rdPtr->pRefCount = nullptr;

			rdPtr->pLibValue = nullptr;

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
			// delete pOldSf;
			// pOldSf is one of the following
			ReleaseNonFromLib(rdPtr);
		}	

		rdPtr->fromLib = false;

		StackBlur(pMemSf, radius, scale, divide);

		rdPtr->src = pMemSf;
		
		if (bHwa) {
			ConvertToHWATexture(rdPtr, rdPtr->src);
		}

		// update latet (may do hwa conversion)
		NewNonFromLib(rdPtr, rdPtr->src);

		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport SetEffectSurfaceParam(LPRDATA rdPtr, long param1, long param2) {
	auto object = (LPRDATA)CNC_GetParameter(rdPtr);

	auto pFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	auto pKey = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	std::wstring paramName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	if (!object->isLib) {
		return 0;
	}

	// surface used in surface param must be hwa type
	auto pLibHWAType = object->HWA;
	object->HWA = true;

	do {
		const auto it = _LoadLib(rdPtr, object, pFilePath, pKey);

		if (it == object->pLib->end()) {
			break;
		}
		const auto pEffect = EffectUtilities::GetEffect(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam);

		if (pEffect == nullptr) {
			break;
		}

		// set param
		EffectUtilities::SetParam(pEffect, paramName, EFFECTPARAM_SURFACE, it->second.pSf);
		it->second.RefShader(ShaderRef((LPRO)rdPtr, pEffect, paramName));

		// add to keep list
		//auto fullPath = GetFullPathNameStr(pFilePath);

		//if (std::find(object->pKeepList->begin(), object->pKeepList->end(), fullPath) == object->pKeepList->end()) {
		//	object->pKeepList->emplace_back(fullPath);
		//}
	} while (0);

	object->HWA = pLibHWAType;

	return 0;
}

short WINAPI DLLExport FillMosaic(LPRDATA rdPtr, long param1, long param2) {
	int width = (int)CNC_GetIntParameter(rdPtr);
	int height = (int)CNC_GetIntParameter(rdPtr);

	rdPtr->pAI->StopAnimation();

	if (CanDisplay(rdPtr)) {
		bool bReleaseBitmap = false;
		auto pBitmap = rdPtr->src;
		auto pSf = CreateSurface(32, width, height);

		if (rdPtr->src->HasAlpha()) {
			//_AddAlpha(pSf);
			pSf->CreateAlpha();
		}

		if (IsHWA(rdPtr->src)) {
			pBitmap = ConvertBitmap(rdPtr, rdPtr->src);
			bReleaseBitmap = true;
		}

		// Fill -> alpha channel issue
		// Fill -> not work under HWA (blank result)
		//CFillMosaic mosaic(pBitmap);		
		//auto bResult = pSf->Fill(&mosaic);

		// if mod != 0 then need another blit
		auto itx = width / pBitmap->GetWidth() + ((width % pBitmap->GetWidth()) != 0);
		auto ity = height / pBitmap->GetHeight() + ((height % pBitmap->GetHeight()) != 0);

		for (int y = 0; y < ity; y++) {
			for (int x = 0; x < itx; x++) {
				auto destX = x * pBitmap->GetWidth();
				auto destY = y * pBitmap->GetHeight();

				pBitmap->Blit(*pSf, destX, destY, BMODE_OPAQUE, BOP_COPY, 0, BLTF_COPYALPHA);
			}
		}

		if (bReleaseBitmap) {
			delete pBitmap;
		}

		if (rdPtr->fromLib) {
			rdPtr->fromLib = false;

			UpdateRef(rdPtr, false);
			rdPtr->pRefCount = nullptr;

			rdPtr->pLibValue = nullptr;
		}
		else {
			ReleaseNonFromLib(rdPtr);
		}

		if (rdPtr->HWA) {
			ConvertToHWATexture(rdPtr, pSf);
		}

		rdPtr->src = pSf;

		NewNonFromLib(rdPtr, rdPtr->src);

		NewPic(rdPtr);
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport SaveToFile(LPRDATA rdPtr, long param1, long param2) {
	auto pFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	auto pKey = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	auto pSaveFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	auto pSf = _GetSurfacePointer(rdPtr, pFilePath, pKey);

	if (pSf == nullptr) {
		return 0;
	}

	__SavetoFile(rdPtr, pSf, pSaveFilePath);

	return 0;
}

short WINAPI DLLExport CaptureFrameArea(LPRDATA rdPtr, long param1, long param2) {
	auto hFrameWindowHandle = rdPtr->rHo.hoAdRunHeader->rhHEditWin;
	HDC hdcWindow = GetDC(hFrameWindowHandle);

	RECT frameRect;
	::GetWindowRect(hFrameWindowHandle, &frameRect);
	int frameWidth = frameRect.right - frameRect.left;
	int frameHeight = frameRect.bottom - frameRect.top;

	auto pMemSf = CreateSurface(24, frameWidth, frameHeight);
	_AddAlpha(pMemSf);

	auto hdcSf = pMemSf->GetDC();
	StretchBlt(hdcSf, 0, 0, frameWidth, frameHeight
		, hdcWindow, 0, 0, frameWidth, frameHeight
		, SRCCOPY);
	pMemSf->ReleaseDC(hdcSf);
	_AddAlpha(pMemSf);

	auto capturedName = L"_TempCapture";
	LoadFromPointer(rdPtr, capturedName, pMemSf);

#ifdef _DEBUG
	//__SavetoClipBoard(pMemSf);
#endif // _DEBUG

	delete pMemSf;

	return 0;
}

short WINAPI DLLExport SaveToFileWithStretch(LPRDATA rdPtr, long param1, long param2) {
	auto pFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	auto pKey = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	auto pSaveFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	int width = (int)CNC_GetIntParameter(rdPtr);
	int height = (int)CNC_GetIntParameter(rdPtr);

	auto pSave = CreateSurface(32, width, height);
	auto pSf = _GetSurfacePointer(rdPtr, pFilePath, pKey);	

	if (pSf == nullptr) {
		return 0;
	}

	ProcessBitmap(pSf,[&](const LPSURFACE pBitmap) {
		Stretch(pBitmap, pSave, true);

#ifdef _DEBUG
		//__SavetoClipBoard(pBitmap);
		//__SavetoClipBoard(pSave);
#endif // _DEBUG
	});

	__SavetoFile(rdPtr, pSave, pSaveFilePath);

	delete pSave;

	return 0;
}

short WINAPI DLLExport ChangeTransparentByColor(LPRDATA rdPtr, long param1, long param2) {
	// -1 for first pixel
	int color = static_cast<int>(CNC_GetIntParameter(rdPtr));
	// -1 for keep alpha
	const int alpha = static_cast<int>(CNC_GetIntParameter(rdPtr));

	rdPtr->pAI->StopAnimation();

	if (CanDisplay(rdPtr)) {
		auto pSf = ConvertBitmap(rdPtr->src);

		// reset to 255
		//_ForceAddAlpha(pSf);

		ProcessBitmap(rdPtr, pSf, [&] (const LPSURFACE pBitmap) {
			IteratePixel(pSf,[&](const int x,const int y, const SfCoef sfCoef,
			const BYTE* pSrcPixel, BYTE* pAlphaPixel) {
				const auto mul = rdPtr->pData->bPreMulAlpha
					? pAlphaPixel[0] / 255.0
					: 1;

				const auto r = static_cast<BYTE>(pSrcPixel[2] / mul);
				const auto g = static_cast<BYTE>(pSrcPixel[1] / mul);
				const auto b = static_cast<BYTE>(pSrcPixel[0] / mul);

				const auto curColor = RGB(r, g, b);

				if (color == -1) {
					color = static_cast<int>(curColor);
				}

				if (color == static_cast<int>(curColor)) {
					pAlphaPixel[0] = 0;
				}
				else if (alpha != -1) {
					pAlphaPixel[0] = static_cast<BYTE>(alpha);
				}
			});
		});
		
		if (rdPtr->fromLib) {
			rdPtr->fromLib = false;

			UpdateRef(rdPtr, false);
			rdPtr->pRefCount = nullptr;

			rdPtr->pLibValue = nullptr;
		}
		else {
			ReleaseNonFromLib(rdPtr);
		}

		if (rdPtr->pData->bPreMulAlpha) {
			pSf->PremultiplyAlpha();
		}

		if (rdPtr->HWA) {
			ConvertToHWATexture(rdPtr, pSf);
		}

		rdPtr->src = pSf;

		NewNonFromLib(rdPtr, rdPtr->src);

		NewPic(rdPtr);
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport SetAnimationSource(LPRDATA rdPtr, long param1, long param2) {
	auto object = (LPRO)CNC_GetParameter(rdPtr);
	auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	auto Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);
		
	rdPtr->pAI->SetAnimationSource((LPRDATA)object,FilePath,Key);

	return 0;
}

short WINAPI DLLExport SetAnimationSpeed(LPRDATA rdPtr, long param1, long param2) {
	auto speed = (int)CNC_GetStringParameter(rdPtr);

	rdPtr->pAI->SetSpeed(speed);

	return 0;
}

short WINAPI DLLExport LoadAnimation(LPRDATA rdPtr, long param1, long param2) {
	auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	auto Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pAI->LoadAnimation(FilePath, Key);

	return 0;
}

short WINAPI DLLExport StopAnimation(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pAI->StopAnimation();

	return 0;
}

short WINAPI DLLExport PauseAnimation(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pAI->PauseAnimation();

	return 0;
}

short WINAPI DLLExport ResumeAnimation(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pAI->ResumeAnimation();

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

	auto pSf = _GetSurfacePointer(rdPtr, FilePath, Key);
	
	return ConvertToLong<cSurface*>(pSf);
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
	return (long)L"DXGI Not Enabled";
#endif
}

long WINAPI DLLExport GetVRAMUsageMB(LPRDATA rdPtr, long param1) {
#ifdef _USE_DXGI
	rdPtr->pD3DU->UpdateVideoMemoryInfo();

	return long(rdPtr->pD3DU->GetLocalVideoMemoryInfo().CurrentUsage >> 20);
#else
	GetEstimateMemUsage(rdPtr->pData);

	return static_cast<long>(rdPtr->pData->estimateVRAMSizeMB);
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

long WINAPI DLLExport GetAnimationName(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return rdPtr->pAI->AnimationValid()
		? (long)rdPtr->pAI->pA->GetAnimationInfo()->name.c_str()
		: (long)Empty_Str;
}

long WINAPI DLLExport GetAnimationFrameID(LPRDATA rdPtr, long param1) {
	return rdPtr->pAI->AnimationValid()
		? rdPtr->pAI->pA->GetFrameID()
		: -1;
}

long WINAPI DLLExport GetAnimationFrameIndex(LPRDATA rdPtr, long param1) {
	return rdPtr->pAI->AnimationValid()
		? rdPtr->pAI->pA->GetFrameIndex()
		: -1;
}

long WINAPI DLLExport GetAnimationSpeed(LPRDATA rdPtr, long param1) {
	return rdPtr->pAI->GetSpeed();
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
			
			OnAnimationFinished,
			IsAnimationPlaying,
			IsAnimationPaused,

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

			SetPreloadListByPointer,
			SetPreloadPath,
			CleanCache,

			SetKeepList,

			IterateRefCount,

			LoadFromPointer,

			StackBlur,

			SetEffectSurfaceParam,

			SetPreloadList,
			SetKeepListByPointer,

			FillMosaic,

			SaveToFile,

			CaptureFrameArea,

			SaveToFileWithStretch,

			ChangeTransparentByColor,

			SetAnimationSource,
			LoadAnimation,
			StopAnimation,
			SetAnimationSpeed,
			PauseAnimation,
			ResumeAnimation,

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

			GetAnimationName,
			GetAnimationFrameID,
			GetAnimationFrameIndex,
			GetAnimationSpeed,

			0
			};