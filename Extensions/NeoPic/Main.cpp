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

		IDMN_CONDITION_OLC, M_CONDITION_OLC, CND_CONDITION_OLC, 0, 0,

		IDMN_CONDITION_IAF, M_CONDITION_IAF, CND_CONDITION_IAF, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,

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

		IDMN_ACTION_AOTL, M_ACTION_AOTL,	ACT_ACTION_AOTL,	0, 2,PARAM_OBJECT, PARAM_EXPSTRING,  M_ACTION_OBJECT, M_ACTION_FILENAME,
		IDMN_ACTION_DOFL, M_ACTION_DOFL,	ACT_ACTION_DOFL,	0, 0,

		IDMN_ACTION_S, M_ACTION_S,	ACT_ACTION_S,	0, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,M_ACTION_S_WIDTH,M_ACTION_S_HEIGHT,

		IDMN_ACTION_AB, M_ACTION_AB,	ACT_ACTION_AB,	0, 1,PARAM_PASTE, 0,
		IDMN_ACTION_ULCSF, M_ACTION_ULCSF,	ACT_ACTION_ULCSF,	0, 1, PARAM_EXPRESSION, M_ACTION_PSF,

		IDMN_ACTION_SLC, M_ACTION_SLC,	ACT_ACTION_SLC,	0, 1, PARAM_EXPRESSION, M_ACTION_HC,
		IDMN_ACTION_SQ, M_ACTION_SQ,	ACT_ACTION_SQ,	0, 1, PARAM_EXPRESSION, M_ACTION_RS,

		IDMN_ACTION_PT, M_ACTION_PT,	ACT_ACTION_PT,	0, 1, PARAM_EXPSTRING ,M_ACTION_PTMAT,
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
		IDMN_ACTION_SAFID, M_ACTION_SAFID, ACT_ACTION_SAFID, 0, 1, PARAM_EXPRESSION, M_ACTION_FID,
		IDMN_ACTION_SAFINDEX, M_ACTION_SAFINDEX, ACT_ACTION_SAFINDEX, 0, 1, PARAM_EXPRESSION, M_ACTION_FINDEX,
		IDMN_ACTION_SASTEP, M_ACTION_SASTEP, ACT_ACTION_SASTEP, 0, 1, PARAM_EXPRESSION, M_ACTION_STEP,

		IDMN_ACTION_SNSS, M_ACTION_SNSS,	ACT_ACTION_SNSS,	0, 3, PARAM_OBJECT,PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACTION_OBJECT, M_ACTION_BASEPATH, M_ACTION_KEY,
		IDMN_ACTION_LNS, M_ACTION_LNS, ACT_ACTION_LNS,	0, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACTION_FILENAME, M_ACTION_KEY,
		IDMN_ACTION_RNS, M_ACTION_RNS, ACT_ACTION_RNS,	0, 0,
		IDMN_ACTION_RENDERNS, M_ACTION_RENDERNS, ACT_ACTION_RENDERNS,	0, 0,
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

		IDMN_EXPRESSION_GA, M_EXPRESSION_GA, EXP_EXPRESSION_GA, EXPFLAG_DOUBLE, 0,

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
		
		IDMN_EXPRESSION_GGPUN, M_EXPRESSION_GGPUN, EXP_EXPRESSION_GGPUN, EXPFLAG_STRING, 1, EXPPARAM_LONG, M_EXPRESSION_GPUINDEX,
		
		IDMN_EXPRESSION_GVRUMB, M_EXPRESSION_GVRUMB, EXP_EXPRESSION_GVRUMB, 0, 1, EXPPARAM_LONG, M_EXPRESSION_GPUINDEX,
		IDMN_EXPRESSION_GVRBMB, M_EXPRESSION_GVRBMB, EXP_EXPRESSION_GVRBMB, 0, 1, EXPPARAM_LONG, M_EXPRESSION_GPUINDEX,
		IDMN_EXPRESSION_GVRAMB, M_EXPRESSION_GVRAMB, EXP_EXPRESSION_GVRAMB, 0, 1, EXPPARAM_LONG, M_EXPRESSION_GPUINDEX,

		IDMN_EXPRESSION_GAN, M_EXPRESSION_GAN, EXP_EXPRESSION_GAN, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GAFID, M_EXPRESSION_GAFID, EXP_EXPRESSION_GAFID, 0, 0,
		IDMN_EXPRESSION_GAFINDEX, M_EXPRESSION_GAFINDEX, EXP_EXPRESSION_GAFINDEX, 0, 0,
		IDMN_EXPRESSION_GAS, M_EXPRESSION_GAS, EXP_EXPRESSION_GAS, 0, 0,
		IDMN_EXPRESSION_GASTEP, M_EXPRESSION_GASTEP, EXP_EXPRESSION_GASTEP, EXPFLAG_DOUBLE, 0,

		IDMN_EXPRESSION_GLCFN, M_EXPRESSION_GLCFN, EXP_EXPRESSION_GLCFN, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GLCSF, M_EXPRESSION_GLCSF, EXP_EXPRESSION_GLCSF, 0, 0,

		IDMN_EXPRESSION_GAFN, M_EXPRESSION_GAFN, EXP_EXPRESSION_GAFN, EXPFLAG_STRING, 0,

		IDMN_EXPRESSION_GGIC, M_EXPRESSION_GGIC, EXP_EXPRESSION_GGIC, 0, 0,
		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Condition_OnPreloadComplete(LPRDATA rdPtr, long param1, long param2) {
	return TRUE;
}

long WINAPI DLLExport Condition_OnIterateRefCount(LPRDATA rdPtr, long param1, long param2) {
	return TRUE;
}

long WINAPI DLLExport Condition_CanDisplay(LPRDATA rdPtr, long param1, long param2) {
	return CanDisplay(rdPtr);
}

long WINAPI DLLExport Condition_CurrentDisplayTransparent(LPRDATA rdPtr, long param1, long param2) {
	if (!CanDisplay(rdPtr)) {
		rdPtr->bCurrentDisplayTransparent = true;
	}
	else {
		if (rdPtr->fromLib) {
			if (rdPtr->pLibValue->isTransparent == transpTBD) {	// not updated yet
				rdPtr->pLibValue->isTransparent = IsTransparent(rdPtr->src);
			}

			rdPtr->bCurrentDisplayTransparent = rdPtr->pLibValue->isTransparent;
		}
		else {
			if (rdPtr->isTransparent == transpTBD) {
				rdPtr->bCurrentDisplayTransparent = IsTransparent(rdPtr->src);
			}
		}
	}

	return rdPtr->bCurrentDisplayTransparent;
}

long WINAPI DLLExport Condition_LibHasItem(LPRDATA rdPtr, long param1, long param2) {
	const auto pFileName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const auto fullPath = GetFullPathNameStr(pFileName);

	return rdPtr->isLib && rdPtr->pLib->find(fullPath) != rdPtr->pLib->end() ? true : false;
}

long WINAPI DLLExport Condition_OnAnimationFinished(LPRDATA rdPtr, long param1, long param2) {
	return true;
}

long WINAPI DLLExport Condition_IsAnimationFinished(LPRDATA rdPtr, long param1, long param2) {
	return 	rdPtr->pAI->AnimationValid()
		? rdPtr->pAI->pA->AnimationFinished()
		: true;
}

long WINAPI DLLExport Condition_IsAnimationPlaying(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->pAI->AnimationValid();
}

long WINAPI DLLExport Condition_IsAnimationPaused(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->pAI->GetPauseState();
}

long WINAPI DLLExport Condition_OnLoadCallback(LPRDATA rdPtr, long param1, long param2) {
	return TRUE;
}

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport Action_LoadFromFile(LPRDATA rdPtr, long param1, long param2) {
	const auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const auto Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pAI->StopAnimation();
	rdPtr->pNS->ResetNineSlice();

	LoadFromFile(rdPtr, FilePath, Key);

	return 0;
}

short WINAPI DLLExport Action_LoadFromLib(LPRDATA rdPtr, long param1, long param2) {
	const auto object = (LPRO)CNC_GetParameter(rdPtr);

	const auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const auto Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pAI->StopAnimation();
	rdPtr->pNS->ResetNineSlice();

	LoadFromLib(rdPtr, object, FilePath, Key);

	return 0;
}

short WINAPI DLLExport Action_LoadFromDisplay(LPRDATA rdPtr, long param1, long param2) {
	const auto object = (LPRO)CNC_GetParameter(rdPtr);
	const bool UpdateCoef = (bool)CNC_GetIntParameter(rdPtr);

	rdPtr->pAI->StopAnimation();
	rdPtr->pNS->ResetNineSlice();

	LoadFromDisplay(rdPtr, object, UpdateCoef);

	return 0;
}

short WINAPI DLLExport Action_LoadFromPointer(LPRDATA rdPtr, long param1, long param2) {
	const auto pSf = ConvertToType<LPSURFACE>(CNC_GetParameter(rdPtr));
	const auto pFileName= (LPCWSTR)CNC_GetParameter(rdPtr);

	rdPtr->pAI->StopAnimation();
	rdPtr->pNS->ResetNineSlice();

	LoadFromPointer(rdPtr, pFileName, pSf);

	return 0;
}

short WINAPI DLLExport Action_SetPreloadList(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring listSrc = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring basePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	const auto Key = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (rdPtr->isLib) {		
		List list = SplitString(listSrc, Delimiter);		
		rdPtr->pData->StartPreloadProcess(rdPtr, &list, false, basePath, Key);
	}

	return 0;
}

short WINAPI DLLExport Action_SetPreloadListByPointer(LPRDATA rdPtr, long param1, long param2) {
	long list = (long)CNC_GetIntParameter(rdPtr);

	auto BasePath = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	auto Key = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (rdPtr->isLib) {
#ifndef _DEBUG		
		// only works in runtime due to /MD & /MDd
		auto pList = ConvertToType<pPreLoadList>(list);

		if (pList != nullptr) {
			rdPtr->pData->StartPreloadProcess(rdPtr, pList, false, BasePath, Key);
		}
#else				
		//load base path instead for test		
		List fileList;
		GetFileList(&fileList, BasePath);

		rdPtr->pData->StartPreloadProcess(rdPtr, &fileList, true, BasePath, Key);
#endif // !_DEBUG	
	}

	return 0;
}

short WINAPI DLLExport Action_SetPreloadPath(LPRDATA rdPtr, long param1, long param2) {
	const auto BasePath = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const auto Key = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (rdPtr->isLib) {
		const auto pFileList = rdPtr->pData->GetFileList(BasePath);
		rdPtr->pData->StartPreloadProcess(rdPtr, pFileList, true, BasePath, Key);
	}

	return 0;
}

short WINAPI DLLExport Action_CleanCache(LPRDATA rdPtr, long param1, long param2) {
	const size_t memLimit = (size_t)CNC_GetIntParameter(rdPtr);

	if (rdPtr->isLib) {
		rdPtr->pData->CleanCache(true, memLimit);
	}

	return 0;
}

short WINAPI DLLExport Action_SetKeepList(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring keepListSrc = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring basePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	if (rdPtr->isLib) {
		if(keepListSrc.empty()) {
			rdPtr->pData->pGC->ClearKeepList();
		}
		else {
			const KeepList keepList = SplitString(keepListSrc, Delimiter);
			rdPtr->pData->pGC->AppendKeepList(keepList, basePath);
		}
	}

	return 0;
}

short WINAPI DLLExport Action_SetKeepListByPointer(LPRDATA rdPtr, long param1, long param2) {
	long list = (long)CNC_GetIntParameter(rdPtr);
	auto basePath = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (rdPtr->isLib) {
#ifndef _DEBUG		
		// only works in runtime due to /MD & /MDd
		auto pKeepList = ConvertToType<pPreLoadList>(list);
		if (pKeepList == nullptr) {
			rdPtr->pData->pGC->ClearKeepList();
		}
		else {
			rdPtr->pData->pGC->AppendKeepList(*pKeepList, basePath);
		}
#else				
		//load base path instead for test		
		List keepList;
		GetFileList(&keepList, basePath);
		rdPtr->pData->pGC->AppendKeepList(keepList, basePath);
#endif // !_DEBUG	
	}

	return 0;
}

short WINAPI DLLExport Action_IterateRefCount(LPRDATA rdPtr, long param1, long param2) {
	auto num = (int)CNC_GetIntParameter(rdPtr);

	if (rdPtr->isLib) {
		const auto mapSz = rdPtr->pLib->size();

		RefCountVec refVec;
		refVec.reserve(mapSz);

		for (auto& it : *rdPtr->pLib) {
			refVec.emplace_back(it.first, it.second.refCount);
		}

		const auto countWeight = mapSz;		// weight of ref count
		std::ranges::sort(refVec, [&] (const RefCountPair& l, const RefCountPair& r) {
			return l.second.GetWeight(countWeight) > r.second.GetWeight(countWeight);	// decending
			});

		for (const auto& it : refVec) {
			*rdPtr->itCountVecStr = it.first;
			*rdPtr->itCountVecCount = it.second;

			CallEvent(ONITREFCOUNT);

			num--;
			if (num <= 0) {
				break;
			}
		}
	}

	return 0;
}

short WINAPI DLLExport Action_ResetLib(LPRDATA rdPtr, long param1, long param2) {
	if(rdPtr->isLib){
		rdPtr->pData->ResetLib();
	}

	return 0;
}

short WINAPI DLLExport Action_EraseLib(LPRDATA rdPtr, long param1, long param2) {
	const auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	
	if (rdPtr->isLib) {
		rdPtr->pData->EraseLib(FilePath);		
	}

	return 0;
}

short WINAPI DLLExport Action_UpdateLib(LPRDATA rdPtr, long param1, long param2) {
	const auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const auto Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	
	if (rdPtr->isLib) {		
		if (rdPtr->pData->NeedUpdateLib(FilePath)) {
			LoadFromFile(rdPtr, FilePath, Key);
		}
	}

	return 0;
}

short WINAPI DLLExport Action_AttachObjectToLib(LPRDATA rdPtr, long param1, long param2) {
	const auto object = (LPRDATA)CNC_GetParameter(rdPtr);
	const auto pFileName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	if (CanDisplay(rdPtr)) {
		if (rdPtr->fromLib) {
			return 0;
		}

		if (!ObjIsLib(object)) {
			return 0;
		}

		//__SavetoFile(rdPtr, CreateCloneSurface(rdPtr, rdPtr->pSf_Nor),L"F:\\Trans.png");

		const auto fullPath = GetFullPathNameStr(pFileName);

		*rdPtr->FilePath = fullPath;
		object->pData->UpdateLib(fullPath, fullPath, rdPtr->pSf_Nor);

		const auto pLibValue = &object->pLib->find(fullPath)->second;
		pLibValue->pSf_VF = rdPtr->pSf_VF;
		pLibValue->pSf_HF = rdPtr->pSf_HF;
		pLibValue->pSf_VHF = rdPtr->pSf_VHF;

		rdPtr->fromLib = true;
		AttachToLib(rdPtr, pLibValue);
	}

	return 0;
}

short WINAPI DLLExport Action_DetachObjectFromLib(LPRDATA rdPtr, long param1, long param2) {
	if (CanDisplay(rdPtr)) {
		if (!rdPtr->fromLib) {
			return 0;
		}

		// protect old path & key
		const auto oldPath = *rdPtr->FilePath;
		const auto oldKey = *rdPtr->Key;

		LoadFromPointer(rdPtr, rdPtr->FilePath->c_str(), rdPtr->src);

		*rdPtr->FilePath = oldPath;
		*rdPtr->Key = oldKey;
	}

	return 0;
}

short WINAPI DLLExport Action_SetHotSpot(LPRDATA rdPtr, long param1, long param2) {
	const auto Pos = (HotSpotPos)CNC_GetIntParameter(rdPtr);

	const int X = (int)CNC_GetIntParameter(rdPtr);
	const int Y = (int)CNC_GetIntParameter(rdPtr);
	
	rdPtr->pAI->StopAnimation();
	UpdateHotSpot(rdPtr, Pos, X, Y);
	rdPtr->pAI->objectCoef.UpdateInternal(X, Y, Pos);
	
	return 0;
}

short WINAPI DLLExport Action_Zoom(LPRDATA rdPtr, long param1, long param2) {
	const float XScale = GetFloatParam(rdPtr);
	const float YScale = GetFloatParam(rdPtr);
	
	rdPtr->pAI->StopAnimation();
	
	if (CanDisplay(rdPtr)) {
		Zoom(rdPtr, XScale, YScale);
		rdPtr->pAI->objectCoef.UpdateInternal(rdPtr->zoomScale);
	}	

	return 0;
}

short WINAPI DLLExport Action_Stretch(LPRDATA rdPtr, long param1, long param2) {
	const int Width = (int)CNC_GetIntParameter(rdPtr);
	const int Height = (int)CNC_GetIntParameter(rdPtr);

	rdPtr->pAI->StopAnimation();

	if (CanDisplay(rdPtr)) {
		const float XScale = (1.0f * Width / rdPtr->src->GetWidth());
		const float YScale = (1.0f * Height / rdPtr->src->GetHeight());

		Zoom(rdPtr, XScale, YScale);
		rdPtr->pAI->objectCoef.UpdateInternal(rdPtr->zoomScale);
	}

	return 0;
}

short WINAPI DLLExport Action_Rotate(LPRDATA rdPtr, long param1, long param2) {
	const float Angle = fmod(GetFloatParam(rdPtr), 360.0f);
	
	rdPtr->pAI->StopAnimation();

	if (CanDisplay(rdPtr)) {
		Rotate(rdPtr, Angle);
		rdPtr->pAI->objectCoef.UpdateInternal(Angle);
	}

	return 0;
}

short WINAPI DLLExport Action_Offset(LPRDATA rdPtr, long param1, long param2) {
	const int XOffset = (int)CNC_GetIntParameter(rdPtr);
	const int YOffset = (int)CNC_GetIntParameter(rdPtr);

	const bool Wrap = (bool)CNC_GetIntParameter(rdPtr);

	rdPtr->pAI->StopAnimation();
	rdPtr->pNS->ResetNineSlice();

	if (CanDisplay(rdPtr)) {
		if (XOffset == 0 && YOffset == 0) {
			return 0;
		}

		const OffsetCoef offset = { XOffset ,YOffset, Wrap };

		const auto pRTT = GetRenderTargetSurface(rdPtr, rdPtr->src->GetWidth(), rdPtr->src->GetHeight());
		pRTT->CreateAlpha();

		ProcessHWA(rdPtr, rdPtr->src, [&] (const LPSURFACE pHWA) {
			Offset(pHWA, pRTT, offset);
			});

		// ConvertToHWATexture can't handle HWA conversions
		// so convert to bitmap first
		DetachFromLib(rdPtr, ConvertBitmap(pRTT));		
		ReDisplay(rdPtr);

		delete pRTT;
	}

	return 0;
}

short WINAPI DLLExport Action_AddBackdrop(LPRDATA rdPtr, long param1, long param2) {
	const int nObstacleType = ((LPEVP)param1)->evp.evpW.evpW0;
	
	if (CanDisplay(rdPtr)) {
		GetTransformedBitmap(rdPtr,rdPtr->src, [&](LPSURFACE pBitmap) {
			const auto xScale = abs(rdPtr->zoomScale.XScale);
			const auto yScale = abs(rdPtr->zoomScale.YScale);

			auto x = static_cast<int>(static_cast<float>(rdPtr->hotSpot.x) * xScale);
			auto y = static_cast<int>(static_cast<float>(rdPtr->hotSpot.y) * yScale);

			RotatePoint(rdPtr->angle, &x, &y,
				static_cast<int>(static_cast<float>(rdPtr->src->GetWidth()) * xScale),
				static_cast<int>(static_cast<float>(rdPtr->src->GetHeight()) * yScale));

			AddBackdrop(rdPtr, pBitmap,
				rdPtr->rHo.hoX - rdPtr->rHo.hoAdRunHeader->rhWindowX - x,
				rdPtr->rHo.hoY - rdPtr->rHo.hoAdRunHeader->rhWindowY - y,
				rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam, nObstacleType, rdPtr->rs.rsLayer);
			});
	}
	
	return 0;
}

short WINAPI DLLExport Action_UpdateLoadCallbackSf(LPRDATA rdPtr, long param1, long param2) {
	const auto pSf = ConvertToType<LPSURFACE>(CNC_GetParameter(rdPtr));	

	if (pSf->IsValid()) {
		// files are loaded as bitmap
		ProcessBitmap(rdPtr, pSf, [&] (const LPSURFACE pBitmap) {
			(*rdPtr->pLoadCallbackInfo->ppSf)->Clone(*pBitmap);
		});
	}

	return 0;
}

short WINAPI DLLExport Action_SetLoadCallback(LPRDATA rdPtr, long param1, long param2) {
	const auto bCallback = (bool)CNC_GetIntParameter(rdPtr);

	rdPtr->bLoadCallback = bCallback;

	return 0;
}

short WINAPI DLLExport Action_SetQuality(LPRDATA rdPtr, long param1, long param2) {
	const bool Quality = (bool)CNC_GetIntParameter(rdPtr);

	rdPtr->stretchQuality = Quality;

	return 0;
}

short WINAPI DLLExport Action_PerspectiveTrans(LPRDATA rdPtr, long param1, long param2) {
	std::wstring arr = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	
	rdPtr->pAI->StopAnimation();
	rdPtr->pNS->ResetNineSlice();

	if (CanDisplay(rdPtr)) {
		constexpr double doNothing[3][3] = { {1,0,0}, {0,1,0}, {0,0,1} };
		constexpr double doScale[3][3] = { {2,0,0}, {0,2,0}, {0,0,1} };
		constexpr double doRotate[3][3] = { {1.732 / 2,-0.5,0}, {0.5,1.732 / 2,0}, {0,0,1} };
		constexpr double doShear[3][3] = { {1,0.5,0}, {0.5,1,0}, {0,0,1} };

		double matrix[3][3] = { 0.0 };
		std::copy_n(&doNothing[0][0], 9, &matrix[0][0]);

		try {
			const std::vector<wchar_t> frontChars = { L' ',L'{',L',' };
			const std::vector<wchar_t> backChars = { L' ',L'}',L',' };

			TrimStr(arr, frontChars, backChars);

			const List rows = SplitString(arr, L'}');

			if (rows.size() < 3) {
				throw std::exception();
			}

			for (int y = 0; y < 3; y++) {
				constexpr auto arrayDelimiter = L',';
				auto row = rows[y];
				TrimStr(row, frontChars, backChars);

				const List elements = SplitString(row, arrayDelimiter);

				if (elements.size() < 3) {
					throw std::exception();
				}

				for (int x = 0; x < 3; x++) {
					auto element = elements[x];
					TrimStr(element, frontChars, backChars);

					matrix[y][x] = _stod(element);
				}
			}
		} catch (...) {
#if !defined(RUN_ONLY)
			MSGBOX(L"Invalid transform matrix");
#endif
			std::copy_n(&doNothing[0][0], 9, &matrix[0][0]);
		}

		const auto bEqual = memcmp(&doNothing[0][0], &matrix[0][0], 9 * sizeof(double)) == 0;

		if (bEqual) {
			return 0;
		}

		LPSURFACE pSf = nullptr;
		ProcessBitmap(rdPtr->src, [&] (const LPSURFACE pBitmap) {
			const auto pResultSf = PerspectiveTransformation(pBitmap, matrix);			
			if(pResultSf == nullptr){ return; }

			pSf = PerspectiveTransformation(pBitmap, matrix);

			const auto newHotSpot = PerspectiveTransformationPoint(rdPtr->hotSpot.x, rdPtr->hotSpot.y, matrix);
			rdPtr->hotSpot.x = static_cast<LONG>(std::get<0>(newHotSpot));
			rdPtr->hotSpot.y = static_cast<LONG>(std::get<1>(newHotSpot));
		});

		DetachFromLib(rdPtr, pSf);
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport Action_StackBlur(LPRDATA rdPtr, long param1, long param2) {
	const int radius = CNC_GetIntParameter(rdPtr);
	const float scale = GetFloatParam(rdPtr);
	const int divide = CNC_GetIntParameter(rdPtr);

	rdPtr->pAI->StopAnimation();
	rdPtr->pNS->ResetNineSlice();

	if (CanDisplay(rdPtr)) {
		const auto pSf = new cSurface;
		ProcessBitmap(rdPtr->src, [&] (const LPSURFACE pBitmap) {
			pSf->Clone(*pBitmap);
			StackBlur(pSf, radius, scale, divide);
		});

		DetachFromLib(rdPtr, pSf);
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport Action_SetEffectSurfaceParam(LPRDATA rdPtr, long param1, long param2) {
	const auto object = (LPRDATA)CNC_GetParameter(rdPtr);

	const auto pFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const auto pKey = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	const std::wstring paramName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	if (!object->isLib) {
		return 0;
	}

	// surface used in surface param must be hwa type
	const auto pLibHWAType = object->HWA;
	object->HWA = true;

	do {
		const auto it = LoadLib(rdPtr, object, pFilePath, pKey);

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

short WINAPI DLLExport Action_FillMosaic(LPRDATA rdPtr, long param1, long param2) {
	const int width = (int)CNC_GetIntParameter(rdPtr);
	const int height = (int)CNC_GetIntParameter(rdPtr);

	rdPtr->pAI->StopAnimation();
	rdPtr->pNS->ResetNineSlice();

	if (CanDisplay(rdPtr)) {
		const auto pSf = CreateSurface(rdPtr->src->GetDepth(), width, height);

		if (rdPtr->src->HasAlpha()) {
			//_AddAlpha(pSf);
			pSf->CreateAlpha();
		}

		ProcessBitmap(rdPtr, rdPtr->src, [&] (const LPSURFACE pBitmap) {
			// Fill -> alpha channel issue
			// Fill -> not work under HWA (blank result)
			//CFillMosaic mosaic(pBitmap);		
			//auto bResult = pSf->Fill(&mosaic);
			
			// if mod != 0 then need another blit
			const auto itx = width / pBitmap->GetWidth() + ((width % pBitmap->GetWidth()) != 0);
			const auto ity = height / pBitmap->GetHeight() + ((height % pBitmap->GetHeight()) != 0);

			for (int y = 0; y < ity; y++) {
				for (int x = 0; x < itx; x++) {
					const auto destX = x * pBitmap->GetWidth();
					const auto destY = y * pBitmap->GetHeight();

					pBitmap->Blit(*pSf, destX, destY, BMODE_OPAQUE, BOP_COPY, 0, BLTF_COPYALPHA);
				}
			}
		});
	
		DetachFromLib(rdPtr, pSf);
		NewPic(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport Action_SaveToFile(LPRDATA rdPtr, long param1, long param2) {
	const auto pFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const auto pKey = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	const auto pSaveFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	const auto pSf = GetSurfacePointer(rdPtr, pFilePath, pKey);

	if (pSf == nullptr) { return 0; }

	__SavetoFile(rdPtr, pSf, pSaveFilePath);

	return 0;
}

short WINAPI DLLExport Action_CaptureFrameArea(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pFrameCapture->Capture();

	const auto capturedName = L"_TempCapture";
	LoadFromPointer(rdPtr, capturedName, rdPtr->pFrameCapture->pCaptureSf);

	return 0;
}

short WINAPI DLLExport Action_SaveToFileWithStretch(LPRDATA rdPtr, long param1, long param2) {
	const auto pFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const auto pKey = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	const auto pSaveFilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	const int width = (int)CNC_GetIntParameter(rdPtr);
	const int height = (int)CNC_GetIntParameter(rdPtr);

	const auto pSf = GetSurfacePointer(rdPtr, pFilePath, pKey);	

	if (pSf == nullptr) { return 0; }

	ProcessBitmap(pSf,[&](const LPSURFACE pBitmap) {
#ifdef _DEBUG
		//__SavetoClipBoard(pBitmap);
#endif // _DEBUG

		// don't need to stretch
		if (pBitmap->GetWidth() == width && pBitmap->GetHeight() == height) {
			__SavetoFile(rdPtr, pBitmap, pSaveFilePath);

			return;
		}

		// do stretch
		const auto pSave = CreateSurface(32, width, height);

		Stretch(pBitmap, pSave, true);

#ifdef _DEBUG
		//__SavetoClipBoard(pSave);
#endif // _DEBUG

		__SavetoFile(rdPtr, pSave, pSaveFilePath);

		delete pSave;
	});

	return 0;
}

short WINAPI DLLExport Action_ChangeTransparentByColor(LPRDATA rdPtr, long param1, long param2) {
	// -1 for first pixel
	int color = static_cast<int>(CNC_GetIntParameter(rdPtr));
	// -1 for keep alpha
	const int alpha = static_cast<int>(CNC_GetIntParameter(rdPtr));

	rdPtr->pAI->StopAnimation();
	rdPtr->pNS->ResetNineSlice();

	if (CanDisplay(rdPtr)) {
		const auto pSf = ConvertBitmap(rdPtr->src);
		// reset to 255
		//_ForceAddAlpha(pSf);

		ProcessBitmap(rdPtr, pSf, [&] (const LPSURFACE pBitmap) {
			IteratePixel(pSf,[&](const int x,const int y, const SfCoef& sfCoef,
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

		if (rdPtr->pData->bPreMulAlpha) {
			pSf->PremultiplyAlpha();
		}

		DetachFromLib(rdPtr, pSf);
		NewPic(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport Action_SetAnimationSource(LPRDATA rdPtr, long param1, long param2) {
	const auto object = (LPRO)CNC_GetParameter(rdPtr);
	const auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const auto Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);
		
	rdPtr->pAI->SetAnimationSource((LPRDATA)object,FilePath,Key);

	return 0;
}

short WINAPI DLLExport Action_SetAnimationFrameID(LPRDATA rdPtr, long param1, long param2) {
	const auto frameID = (int)CNC_GetStringParameter(rdPtr);

	if (rdPtr->pAI->AnimationValid()) {
		rdPtr->pAI->pA->SetFrameID(frameID);
	}
	
	return 0;
}

short WINAPI DLLExport Action_SetAnimationFrameIndex(LPRDATA rdPtr, long param1, long param2) {
	const auto frameIndex = (int)CNC_GetStringParameter(rdPtr);

	if (rdPtr->pAI->AnimationValid()) {
		rdPtr->pAI->pA->SetFrameIndex(frameIndex);
	}

	return 0;
}

short WINAPI DLLExport Action_SetAnimationStep(LPRDATA rdPtr, long param1, long param2) {	
	const auto frameIndex = GetFloatParam(rdPtr);

	if (rdPtr->pAI->AnimationValid()) {
		rdPtr->pAI->pA->SetFrameStep(static_cast<double>(frameIndex));
	}

	return 0;
}

short WINAPI DLLExport Action_SetAnimationSpeed(LPRDATA rdPtr, long param1, long param2) {
	const auto speed = (int)CNC_GetStringParameter(rdPtr);

	rdPtr->pAI->SetSpeed(speed);

	return 0;
}

short WINAPI DLLExport Action_LoadAnimation(LPRDATA rdPtr, long param1, long param2) {
	const auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const auto Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pNS->ResetNineSlice();
	rdPtr->pAI->LoadAnimation(FilePath, Key);

	return 0;
}

short WINAPI DLLExport Action_StopAnimation(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pAI->StopAnimation();

	return 0;
}

short WINAPI DLLExport Action_PauseAnimation(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pAI->PauseAnimation();

	return 0;
}

short WINAPI DLLExport Action_ResumeAnimation(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pAI->ResumeAnimation();

	return 0;
}

short WINAPI DLLExport Action_SetNineSliceSource(LPRDATA rdPtr, long param1, long param2) {
	const auto object = (LPRO)CNC_GetParameter(rdPtr);
	const auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const auto Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pNS->SetNineSliceSource((LPRDATA)object, FilePath, Key);

	return 0;
}

short WINAPI DLLExport Action_LoadNineSlice(LPRDATA rdPtr, long param1, long param2) {
	const auto FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const auto Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);
		
	rdPtr->pAI->StopAnimation();
	rdPtr->pNS->LoadNineSlice(FilePath, Key);

	// create default surface
	DetachFromLib(rdPtr, CreateHWASurface(32, 4, 4));

	return 0;
}

short WINAPI DLLExport Action_ResetNineSlice(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pNS->ResetNineSlice();

	return 0;
}

short WINAPI DLLExport Action_RenderNineSlice(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->pNS->Render();

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Expression_GetHotSpotX(LPRDATA rdPtr, long param1) {
	rdPtr->pNS->Render();
	return GetHotSpotX(rdPtr);
}
long WINAPI DLLExport Expression_GetHotSpotY(LPRDATA rdPtr, long param1) {
	rdPtr->pNS->Render();
	return GetHotSpotY(rdPtr);
}

long WINAPI DLLExport Expression_GetOriginalWidth(LPRDATA rdPtr, long param1) {
	rdPtr->pNS->Render();
	return GetOriginalWidth(rdPtr);
}
long WINAPI DLLExport Expression_GetOriginalHeight(LPRDATA rdPtr, long param1) {
	rdPtr->pNS->Render();
	return GetOriginalHeight(rdPtr);
}

long WINAPI DLLExport Expression_GetCurrentWidth(LPRDATA rdPtr, long param1) {
	rdPtr->pNS->Render();
	return GetCurrentWidth(rdPtr);
}
long WINAPI DLLExport Expression_GetCurrentHeight(LPRDATA rdPtr, long param1) {
	rdPtr->pNS->Render();
	return GetCurrentHeight(rdPtr);
}

long WINAPI DLLExport Expression_GetXZoomScale(LPRDATA rdPtr, long param1) {
	rdPtr->pNS->Render();
	return ReturnFloat(GetXZoomScale(rdPtr));
}
long WINAPI DLLExport Expression_GetYZoomScale(LPRDATA rdPtr, long param1) {
	rdPtr->pNS->Render();
	return ReturnFloat(GetYZoomScale(rdPtr));
}

long WINAPI DLLExport Expression_GetAngle(LPRDATA rdPtr, long param1) {
	return ReturnFloat(GetAngle(rdPtr));
}

long WINAPI DLLExport Expression_GetFileName(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->FileName->c_str();
}

long WINAPI DLLExport Expression_GetFileFileName(LPRDATA rdPtr, long param1) {
	std::wstring FilePath = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	const auto FileName = GetFileName(FilePath);	

	return ReturnString(FileName);
}

long WINAPI DLLExport Expression_GetFilePath(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->FilePath->c_str();
}

long WINAPI DLLExport Expression_GetRelativeFilePath(LPRDATA rdPtr, long param1) {
	const std::wstring BasePath = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	const auto pos = BasePath.size() + 1;

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

long WINAPI DLLExport Expression_GetFileRelativeFilePath(LPRDATA rdPtr, long param1) {
	std::wstring FilePath = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	std::wstring BasePath = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	const auto FileName = GetRelativeFilePath(FilePath, BasePath);

	return ReturnString(FileName);
}

long WINAPI DLLExport Expression_GetKey(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->Key->c_str();
}

long WINAPI DLLExport Expression_GetSurfacePointer(LPRDATA rdPtr, long param1) {
	const std::wstring FilePath = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	const std::wstring Key = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	const auto pSf = GetSurfacePointer(rdPtr, FilePath, Key);
	
	return ConvertToLong<cSurface*>(pSf);
}

long WINAPI DLLExport Expression_GetAVGCoordX(LPRDATA rdPtr, long param1) {
	//Get AVG Coord
	const int coord=(int)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	const bool realCoordMode = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	const auto XLF = rdPtr->rHo.hoAdRunHeader->rh3.rh3DisplayX;
	const auto WW = rdPtr->rHo.hoAdRunHeader->rh3.rh3WindowSx;

	if (!realCoordMode) {
		return XLF + coord + WW / 2;
	}
	else {
		return XLF + coord;
	}
}

long WINAPI DLLExport Expression_GetAVGCoordY(LPRDATA rdPtr, long param1) {
	//Get AVG Coord
	const int coord = (int)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	const bool realCoordMode = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	const auto YTF = rdPtr->rHo.hoAdRunHeader->rh3.rh3DisplayY;
	const auto WH = rdPtr->rHo.hoAdRunHeader->rh3.rh3WindowSy;

	if (!realCoordMode) {
		return YTF + (WH - coord) - GetCurrentHeight(rdPtr) / 2;
	}
	else {
		return YTF + coord;
	}
}

long WINAPI DLLExport Expression_GetIterateRefCountKey(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->itCountVecStr->c_str();
}

long WINAPI DLLExport Expression_GetIterateRefCountValueCount(LPRDATA rdPtr, long param1) {
	return rdPtr->itCountVecCount->totalRef;
}

long WINAPI DLLExport Expression_GetIterateRefCountValuePriority(LPRDATA rdPtr, long param1) {
	return rdPtr->itCountVecCount->priority;
}

long WINAPI DLLExport Expression_GetRAMUsageMB(LPRDATA rdPtr, long param1) {
	return long(GetProcessMemoryUsageMB(rdPtr->pData->processHandle.hProcess));
}

long WINAPI DLLExport Expression_GetGPUInfoCount(LPRDATA rdPtr, long param1) {
#ifdef QUERY_VRAM
    if (rdPtr->pVRAMU == nullptr) {
#endif	
        return 0;
#ifdef QUERY_VRAM
    }
       
    return long(rdPtr->pVRAMU->GetInfoCount());
#endif	
}

long WINAPI DLLExport Expression_GetGPUName(LPRDATA rdPtr, long param1) {
    const size_t index = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
#ifdef QUERY_VRAM
    if (rdPtr->pVRAMU == nullptr) {
        return (long)L"DXGI Init Failed";
    }

    return (long)rdPtr->pVRAMU->GetDesc(index).Description;
#else
    return (long)L"DXGI Not Enabled";
#endif
}

long WINAPI DLLExport Expression_GetVRAMUsageMB(LPRDATA rdPtr, long param1) {
    const size_t index = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

#ifdef QUERY_VRAM
    if (rdPtr->pVRAMU == nullptr) {
#endif	
        const auto [estimateRAMSizeMB, estimateVRAMSizeMB] = rdPtr->pData->GetEstimateMemUsage(rdPtr->pData->pLib);

        return static_cast<long>(estimateVRAMSizeMB);
#ifdef QUERY_VRAM
    }

    rdPtr->pVRAMU->UpdateVideoMemoryInfo(index);
    return long(rdPtr->pVRAMU->GetLocalVideoMemoryInfo(index).CurrentUsage >> 20);
#endif	
}

long WINAPI DLLExport Expression_GetVRAMBudgetMB(LPRDATA rdPtr, long param1) {
    const size_t index = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

#ifdef QUERY_VRAM
    if (rdPtr->pVRAMU == nullptr) {
#endif	
        return -1;
#ifdef QUERY_VRAM
    }

    rdPtr->pVRAMU->UpdateVideoMemoryInfo(index);
    return long(rdPtr->pVRAMU->GetLocalVideoMemoryInfo(index).Budget >> 20);
#endif	
}

long WINAPI DLLExport Expression_GetVRAMAvailableMB(LPRDATA rdPtr, long param1) {
    const size_t index = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

#ifdef QUERY_VRAM
    if (rdPtr->pVRAMU == nullptr) {
#endif	
        return -1;
#ifdef QUERY_VRAM
    }

    rdPtr->pVRAMU->UpdateVideoMemoryInfo(index);
    return long(rdPtr->pVRAMU->GetLocalVideoMemoryInfo(index).AvailableForReservation >> 20);
#endif
}

long WINAPI DLLExport Expression_GetAnimationFileName(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return rdPtr->pAI->AnimationValid()
		? (long)rdPtr->pAI->fileName.c_str()
		: (long)Empty_Str;
}

long WINAPI DLLExport Expression_GetAnimationName(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return rdPtr->pAI->AnimationValid()
		? (long)rdPtr->pAI->pA->GetAnimationInfo()->name.c_str()
		: (long)Empty_Str;
}

long WINAPI DLLExport Expression_GetAnimationFrameID(LPRDATA rdPtr, long param1) {
	return rdPtr->pAI->AnimationValid()
		? rdPtr->pAI->pA->GetFrameID()
		: -1;
}

long WINAPI DLLExport Expression_GetAnimationFrameIndex(LPRDATA rdPtr, long param1) {
	return rdPtr->pAI->AnimationValid()
		? rdPtr->pAI->pA->GetFrameIndex()
		: -1;
}

long WINAPI DLLExport Expression_GetAnimationStep(LPRDATA rdPtr, long param1) {
	//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
	rdPtr->rHo.hoFlags |= HOF_FLOAT;

	//Return the float without conversion
	const auto ret = rdPtr->pAI->AnimationValid()
		? static_cast<float>(rdPtr->pAI->pA->GetFrameStep())
		: -1.0f;

	return *((int*)&ret);
}

long WINAPI DLLExport Expression_GetAnimationSpeed(LPRDATA rdPtr, long param1) {
	return rdPtr->pAI->GetSpeed();
}

long WINAPI DLLExport Expression_GetLoadCallbackFileName(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->pLoadCallbackInfo->callbackFileName.c_str();
}

long WINAPI DLLExport Expression_GetLoadCallbackSf(LPRDATA rdPtr, long param1) {
	return ConvertToLong<cSurface*>(rdPtr->pLoadCallbackInfo->GetSurfacePointer());
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
			Condition_OnPreloadComplete,
			Condition_OnIterateRefCount,

			Condition_CanDisplay,

			Condition_CurrentDisplayTransparent,
			Condition_LibHasItem,
			
			Condition_OnAnimationFinished,
			Condition_IsAnimationPlaying,
			Condition_IsAnimationPaused,

			Condition_OnLoadCallback,

			Condition_IsAnimationFinished,

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action_LoadFromFile,
			Action_LoadFromLib,

			Action_ResetLib,
			Action_EraseLib,
			Action_UpdateLib,

			Action_SetHotSpot,
			
			Action_Zoom,
			Action_Rotate,

			Action_AttachObjectToLib,
			Action_DetachObjectFromLib,

			Action_Stretch,

			Action_AddBackdrop,
			Action_UpdateLoadCallbackSf,

			Action_SetLoadCallback,
			Action_SetQuality,

			Action_PerspectiveTrans,
			Action_Offset,

			Action_LoadFromDisplay,

			Action_SetPreloadListByPointer,
			Action_SetPreloadPath,
			Action_CleanCache,

			Action_SetKeepList,

			Action_IterateRefCount,

			Action_LoadFromPointer,

			Action_StackBlur,

			Action_SetEffectSurfaceParam,

			Action_SetPreloadList,
			Action_SetKeepListByPointer,

			Action_FillMosaic,

			Action_SaveToFile,

			Action_CaptureFrameArea,

			Action_SaveToFileWithStretch,

			Action_ChangeTransparentByColor,

			Action_SetAnimationSource,
			Action_LoadAnimation,
			Action_StopAnimation,
			Action_SetAnimationSpeed,
			Action_PauseAnimation,
			Action_ResumeAnimation,
			Action_SetAnimationFrameID,
			Action_SetAnimationFrameIndex,
			Action_SetAnimationStep,

			Action_SetNineSliceSource,
			Action_LoadNineSlice,
			Action_ResetNineSlice,
			Action_RenderNineSlice,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression_GetHotSpotX,
			Expression_GetHotSpotY,

			Expression_GetOriginalWidth,
			Expression_GetOriginalHeight,

			Expression_GetCurrentWidth,
			Expression_GetCurrentHeight,

			Expression_GetXZoomScale,
			Expression_GetYZoomScale,

			Expression_GetAngle,

			Expression_GetFileName,
			Expression_GetKey,

			Expression_GetSurfacePointer,

			Expression_GetAVGCoordX,
			Expression_GetAVGCoordY,

			Expression_GetFilePath,
			Expression_GetRelativeFilePath,

			Expression_GetIterateRefCountKey,
			Expression_GetIterateRefCountValueCount,
			Expression_GetIterateRefCountValuePriority,

			Expression_GetFileFileName,
			Expression_GetFileRelativeFilePath,

			Expression_GetRAMUsageMB,

			Expression_GetGPUName,

			Expression_GetVRAMUsageMB,
			Expression_GetVRAMBudgetMB,
			Expression_GetVRAMAvailableMB,

			Expression_GetAnimationName,
			Expression_GetAnimationFrameID,
			Expression_GetAnimationFrameIndex,
			Expression_GetAnimationSpeed,
			Expression_GetAnimationStep,

			Expression_GetLoadCallbackFileName,
			Expression_GetLoadCallbackSf,

			Expression_GetAnimationFileName,

			Expression_GetGPUInfoCount,

			0
			};