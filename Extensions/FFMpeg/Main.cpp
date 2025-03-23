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
		IDMN_CONDITION_VO, M_CONDITION_VO, CND_CONDITION_VO, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		IDMN_CONDITION_VP, M_CONDITION_VP, CND_CONDITION_VP, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		IDMN_CONDITION_VL, M_CONDITION_VL, CND_CONDITION_VL, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		IDMN_CONDITION_VF, M_CONDITION_VF, CND_CONDITION_VF, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		
		IDMN_CONDITION_OVF, M_CONDITION_OVF, CND_CONDITION_OVF, 0, 1, PARAM_EXPSTRING, M_FILEPATH,
		IDMN_CONDITION_OVOF, M_CONDITION_OVOF, CND_CONDITION_OVOF, 0, 1, PARAM_EXPSTRING, M_FILEPATH,

		// object
		IDMN_CONDITION_VHD, M_CONDITION_VHD, CND_CONDITION_VHD, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		IDMN_CONDITION_VWHD, M_CONDITION_VWHD, CND_CONDITION_VWHD, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,

		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_OV, M_ACTION_OV, ACT_ACTION_OV,	0, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_FILEPATH, M_KEY,
		IDMN_ACTION_CV, M_ACTION_CV, ACT_ACTION_CV,	0, 0,
		IDMN_ACTION_PLAYV, M_ACTION_PLAYV, ACT_ACTION_PLAYV,	0, 0,
		IDMN_ACTION_PAUSEV, M_ACTION_PAUSEV, ACT_ACTION_PAUSEV,	0, 0,

		IDMN_ACTION_SV, M_ACTION_SV, ACT_ACTION_SV,	0, 1, PARAM_EXPRESSION, M_VOLUME,
		IDMN_ACTION_SL, M_ACTION_SL, ACT_ACTION_SL,	0, 1, PARAM_EXPRESSION, M_LOOP,

		IDMN_ACTION_SP, M_ACTION_SP, ACT_ACTION_SP,	0, 1, PARAM_EXPRESSION, M_POSITION,
		IDMN_ACTION_SPWF, M_ACTION_SPWF, ACT_ACTION_SPWF,	0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_POSITION_WF, M_FLAGS,

		IDMN_ACTION_STC, M_ACTION_STC, ACT_ACTION_STC, 0, 1, PARAM_EXPRESSION, M_THEADCOUNT,

		IDMN_ACTION_SAS, M_ACTION_SAS, ACT_ACTION_SAS, 0, 1, PARAM_EXPRESSION, M_ACCURATESEEK,

		IDMN_ACTION_CACHEV, M_ACTION_CACHEV, ACT_ACTION_CACHEV,	0, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_FILEPATH, M_KEY,
		IDMN_ACTION_ERASEV, M_ACTION_ERASEV, ACT_ACTION_ERASEV,	0, 1, PARAM_EXPSTRING, M_FILEPATH,

		IDMN_ACTION_OVT, M_ACTION_OVT, ACT_ACTION_OVT,	0, 3, PARAM_EXPSTRING, PARAM_EXPSTRING, PARAM_EXPRESSION, M_FILEPATH, M_KEY, M_POSITION,
		
		IDMN_ACTION_SHDE, M_ACTION_SHDE, ACT_ACTION_SHDE, 0, 1, PARAM_EXPSTRING, M_HWDECODEDEVICE,
		
		IDMN_ACTION_STRETCH, M_ACTION_STRETCH, ACT_ACTION_STRETCH, 0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_WIDTH, M_HEIGHT,

		IDMN_ACTION_SAT, M_ACTION_SAT, ACT_ACTION_SAT, 0, 1, PARAM_EXPRESSION, M_ATEMPO,

		IDMN_ACTION_SFNA, M_ACTION_SFNA, ACT_ACTION_SFNA, 0, 1, PARAM_EXPRESSION, M_FORCENOAUDIO,

		IDMN_ACTION_SOC, M_ACTION_SOC, ACT_ACTION_SOC, 0, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_VCODEC, M_ACODEC,

		IDMN_ACTION_RD, M_ACTION_RD, ACT_ACTION_RD,	0, 0,

        IDMN_ACTION_SCTT, M_ACTION_SCTT, ACT_ACTION_SCTT, 0, 1, PARAM_EXPRESSION, M_COPYTOTEXTURE,

		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GVN, M_EXPRESSION_GVN, EXP_EXPRESSION_GVN, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GVP, M_EXPRESSION_GVP, EXP_EXPRESSION_GVP, 0, 0,
		IDMN_EXPRESSION_GVD, M_EXPRESSION_GVD, EXP_EXPRESSION_GVD, 0, 0,

		IDMN_EXPRESSION_GV, M_EXPRESSION_GV, EXP_EXPRESSION_GV, 0, 0,

		IDMN_EXPRESSION_GCVFP, M_EXPRESSION_GCVFP, EXP_EXPRESSION_GCVFP, 0, 1, EXPPARAM_LONG, M_HWA,
		IDMN_EXPRESSION_GGVFP, M_EXPRESSION_GGVFP, EXP_EXPRESSION_GGVFP, 0, 2, EXPPARAM_LONG, EXPPARAM_LONG, M_POSITION, M_HWA,

		IDMN_EXPRESSION_GVO, M_EXPRESSION_GVO, EXP_EXPRESSION_GVO, 0, 0,
		IDMN_EXPRESSION_GVPLAY, M_EXPRESSION_GVPLAY, EXP_EXPRESSION_GVPLAY, 0, 0,
		IDMN_EXPRESSION_GVL, M_EXPRESSION_GVL, EXP_EXPRESSION_GVL, 0, 0,
		IDMN_EXPRESSION_GVF, M_EXPRESSION_GVF, EXP_EXPRESSION_GVF, 0, 0,

		IDMN_EXPRESSION_GHDS, M_EXPRESSION_GHDS, EXP_EXPRESSION_GHDS, 0, 0,
		IDMN_EXPRESSION_GAHDE, M_EXPRESSION_GAHDE, EXP_EXPRESSION_GAHDE, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GWHDE, M_EXPRESSION_GWHDE, EXP_EXPRESSION_GWHDE, EXPFLAG_STRING, 0,

		IDMN_EXPRESSION_GAT, M_EXPRESSION_GAT, EXP_EXPRESSION_GAT, EXPFLAG_DOUBLE, 0,
		
		IDMN_EXPRESSION_GVOCN, M_EXPRESSION_GVOCN, EXP_EXPRESSION_GVOCN, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GAOCN, M_EXPRESSION_GAOCN, EXP_EXPRESSION_GAOCN, EXPFLAG_STRING, 0,

        IDMN_EXPRESSION_GTC, M_EXPRESSION_GTC, EXP_EXPRESSION_GTC, 0, 0,
		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Condition_VideoOpen(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->bOpen;
}

long WINAPI DLLExport Condition_VideoPlay(LPRDATA rdPtr, long param1, long param2) {
	return GetVideoPlayState(rdPtr);
}

long WINAPI DLLExport Condition_VideoLoop(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->bLoop;
}

long WINAPI DLLExport Condition_VideoFinish(LPRDATA rdPtr, long param1, long param2) {
	return GetVideoFinishState(rdPtr);
}

long WINAPI DLLExport Condition_OnVideoFinish(LPRDATA rdPtr, long param1, long param2) {
	std::wstring filePath = GetFullPathNameStr((LPCWSTR)CNC_GetStringParameter(rdPtr));

	return *rdPtr->pFilePath == filePath;
}

long WINAPI DLLExport Condition_OnVideoOpenFailed(LPRDATA rdPtr, long param1, long param2) {
	std::wstring filePath = GetFullPathNameStr((LPCWSTR)CNC_GetStringParameter(rdPtr));

	return *rdPtr->pFilePath == filePath;
}

long WINAPI DLLExport Condition_VideoHardwareDecode(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->pFFMpeg != nullptr && rdPtr->pFFMpeg->get_hwDecodeState();
}

long WINAPI DLLExport Condition_VideoWantedHardwareDecode(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->hwDeviceType != AV_HWDEVICE_TYPE_NONE;
}

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport Action_OpenVideo(LPRDATA rdPtr, long param1, long param2) {
	std::wstring filePath = GetFullPathNameStr((LPCWSTR)CNC_GetStringParameter(rdPtr));
	std::wstring key = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	OpenGeneral(rdPtr, filePath, key, GetOptions(rdPtr));

	return 0;
}

short WINAPI DLLExport Action_OpenVideoTo(LPRDATA rdPtr, long param1, long param2) {
	std::wstring filePath = GetFullPathNameStr((LPCWSTR)CNC_GetStringParameter(rdPtr));
	std::wstring key = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	size_t msRaw = (size_t)CNC_GetIntParameter(rdPtr);

	OpenGeneral(rdPtr, filePath, key, GetOptions(rdPtr), msRaw);

	return 0;
}

short WINAPI DLLExport Action_CloseVideo(LPRDATA rdPtr, long param1, long param2) {
	CloseGeneral(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_PlayVideo(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->bPlay) { return 0; }

	rdPtr->bPlay = true;
	rdPtr->bPlayStateUpdated = true;

	return 0;
}

short WINAPI DLLExport Action_PauseVideo(LPRDATA rdPtr, long param1, long param2) {
	if (!rdPtr->bPlay) { return 0; }

	rdPtr->bPlay = false;
	rdPtr->bPlayStateUpdated = true;

	return 0;
}

short WINAPI DLLExport Action_SetVolume(LPRDATA rdPtr, long param1, long param2) {
	int newVolume = (int)CNC_GetIntParameter(rdPtr);

    rdPtr->volume = ::Range(newVolume, 0, 100);
	if (rdPtr->pFFMpeg != nullptr) {
		rdPtr->pFFMpeg->set_volume(rdPtr->volume);
	}

	return 0;
}

short WINAPI DLLExport Action_SetLoop(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->bLoop = (bool)CNC_GetIntParameter(rdPtr);
	
	if (rdPtr->pFFMpeg != nullptr) {
		rdPtr->pFFMpeg->set_loop(rdPtr->bLoop);
	}

	return 0;
}

short WINAPI DLLExport Action_SetPosition(LPRDATA rdPtr, long param1, long param2) {
	int msRaw = (int)CNC_GetIntParameter(rdPtr);

	SetPositionGeneral(rdPtr, msRaw);
    // true if goto target position
    rdPtr->bPositionSet = true;

	return 0;
}

short WINAPI DLLExport Action_SetPositionWithFlag(LPRDATA rdPtr, long param1, long param2) {
	int msRaw = (int)CNC_GetIntParameter(rdPtr);
	int flag = (int)CNC_GetIntParameter(rdPtr);

	SetPositionGeneral(rdPtr, msRaw, flag);
    // true if goto target position
    rdPtr->bPositionSet = !(flag & SeekFlag_NoGoto);

	return 0;
}

short WINAPI DLLExport Action_SetThreadCount(LPRDATA rdPtr, long param1, long param2) {
    const auto threadCount = (int)CNC_GetIntParameter(rdPtr);
    rdPtr->threadCount = FFMpegOptions::GetValidThreadCount(threadCount);

    return 0;
}

short WINAPI DLLExport Action_SetAccurateSeek(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->bAccurateSeek = (bool)CNC_GetIntParameter(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_CacheVideo(LPRDATA rdPtr, long param1, long param2) {
	std::wstring filePath = GetFullPathNameStr((LPCWSTR)CNC_GetStringParameter(rdPtr));
	std::wstring key = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (!rdPtr->bCache) {
		return 0;
	}

	if (!StrEmpty(key.c_str())) {
		LoadMemVideo(rdPtr, filePath, key);
	}

	return 0;
}

short WINAPI DLLExport Action_EraseVideo(LPRDATA rdPtr, long param1, long param2) {
	std::wstring filePath = GetFullPathNameStr((LPCWSTR)CNC_GetStringParameter(rdPtr));

	if (!rdPtr->bCache) {
		return 0;
	}

	// erase all
	if (filePath.empty()) {
		CleanCache(rdPtr, true);
	}
	else {
		const auto it = rdPtr->pData->pMemVideoLib->GetItem(filePath);

		if (it != rdPtr->pData->pMemVideoLib->data.end()) {
			auto pBufs = GetRefList(rdPtr);
			const auto findIt = std::ranges::find(pBufs
				, it->second->GetOutputData());

			if (findIt == pBufs.end()) {
				rdPtr->pData->pMemVideoLib->EraseItem(filePath);
			}
		}
	}

	return 0;
}

short WINAPI DLLExport Action_SetHWDevice(LPRDATA rdPtr, long param1, long param2) {
	std::wstring deviceName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->hwDeviceType = FFMpeg::get_hwDeviceTypeByName(deviceName);

    // auto managed
    if (rdPtr->hwDeviceType != AV_HWDEVICE_TYPE_D3D11VA) {
        rdPtr->bCopyToTexture = false;
    }

	return 0;
}

short WINAPI DLLExport Action_SetCopyToTexture(LPRDATA rdPtr, long param1, long param2) {
    rdPtr->bCopyToTexture = (bool)CNC_GetIntParameter(rdPtr);
    
    // auto managed
    if (rdPtr->bCopyToTexture) {
        rdPtr->hwDeviceType = AV_HWDEVICE_TYPE_D3D11VA;
    }

    return 0;
}

short WINAPI DLLExport Action_Stretch(LPRDATA rdPtr, long param1, long param2) {
	int width = (int)CNC_GetIntParameter(rdPtr);
	int height = (int)CNC_GetIntParameter(rdPtr);
	
	rdPtr->swidth = (std::max)(0, width);
	rdPtr->sheight = (std::max)(0, height);
	
	rdPtr->bStretch = true;

	if (rdPtr->pFFMpeg != nullptr) {
		UpdateScale(rdPtr, rdPtr->pFFMpeg->get_width(), rdPtr->pFFMpeg->get_height());
	}

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_SetAudioTempo(LPRDATA rdPtr, long param1, long param2) {
	float atempo = GetFloatParam(rdPtr);
	
	rdPtr->atempo = atempo;

	if (rdPtr->pFFMpeg != nullptr) {
		rdPtr->pFFMpeg->set_audioTempo(rdPtr->atempo);
		rdPtr->atempo = rdPtr->pFFMpeg->get_audioTempo();
	}

	return 0;
}

short WINAPI DLLExport Action_SetForceNoAudio(LPRDATA rdPtr, long param1, long param2) {
	bool bForceNoAudio = (bool)CNC_GetIntParameter(rdPtr);

	rdPtr->bForceNoAudio = bForceNoAudio;

	return 0;
}

short WINAPI DLLExport Action_SetOverrideCodec(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring videoCodec = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring audioCodec = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	*rdPtr->pVideoOverrideCodecName = ConvertWStrToStr(videoCodec);
	*rdPtr->pAudioOverrideCodecName = ConvertWStrToStr(audioCodec);

	return 0;
}

short WINAPI DLLExport Action_ResetDisplay(LPRDATA rdPtr, long param1, long param2) {
    if (!GetVideoPlayState(rdPtr)
        && rdPtr->pDisplaySf != nullptr && rdPtr->pDisplaySf->IsValid()) {
        rdPtr->bResetDisplay = true;
    }

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Expression_GetVideoName(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->pFilePath->c_str();
}

long WINAPI DLLExport Expression_GetVideoPosition(LPRDATA rdPtr, long param1) {
	return rdPtr->pFFMpeg == nullptr
		? -1
		: long(rdPtr->pFFMpeg->get_videoPosition());
}

long WINAPI DLLExport Expression_GetVideoDuration(LPRDATA rdPtr, long param1) {
	return rdPtr->pFFMpeg != nullptr ? (long)rdPtr->pFFMpeg->get_videoDuration() : -1;
}

long WINAPI DLLExport Expression_GetVolume(LPRDATA rdPtr, long param1) {
	return rdPtr->volume;
}

long WINAPI DLLExport Expression_GetCurrentVideoFramePointer(LPRDATA rdPtr, long param1) {
	bool bWantHWA = (bool)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	if (!rdPtr->bOpen) {
		return 0;
	}

	return ReturnVideoFrame(rdPtr, bWantHWA, rdPtr->pDisplaySf, rdPtr->pReturnSf);
}

long WINAPI DLLExport Expression_GetGrabbedVideoFramePointer(LPRDATA rdPtr,long param1) {
	size_t ms = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	bool bWantHWA = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	if (!rdPtr->bOpen) {
		return 0;
	}

	InitSurface(rdPtr->pGrabbedFrame,
        rdPtr->pFFMpeg->get_width(), rdPtr->pFFMpeg->get_height(),
        rdPtr->pFFMpeg->get_copyToTextureState());
	BlitVideoFrame(rdPtr, ms, rdPtr->pGrabbedFrame);
	//__SavetoClipBoard(rdPtr->pGrabbedFrame);

	return ReturnVideoFrame(rdPtr, bWantHWA, rdPtr->pGrabbedFrame, rdPtr->pReturnSf);
}

long WINAPI DLLExport Expression_GetVideoOpen(LPRDATA rdPtr, long param1) {
	return rdPtr->bOpen;
}

long WINAPI DLLExport Expression_GetVideoPlay(LPRDATA rdPtr, long param1) {
	return GetVideoPlayState(rdPtr);
}

long WINAPI DLLExport Expression_GetVideoLoop(LPRDATA rdPtr, long param1) {
	return rdPtr->bLoop;
}

long WINAPI DLLExport Expression_GetVideoFinish(LPRDATA rdPtr, long param1) {
	return GetVideoFinishState(rdPtr);
}

long WINAPI DLLExport Expression_GetHardwareDecodeState(LPRDATA rdPtr, long param1) {
	return rdPtr->pFFMpeg != nullptr && rdPtr->pFFMpeg->get_hwDecodeState();
}

long WINAPI DLLExport Expression_GetActualHardwareDevice(LPRDATA rdPtr, long param1) {
	rdPtr->rHo.hoFlags |= HOF_STRING;

	return (long)(rdPtr->pFFMpeg == nullptr
		? L"NOOBJECT"
		: rdPtr->pFFMpeg->get_hwDeviceName());
}

long WINAPI DLLExport Expression_GetWantedHardwareDevice(LPRDATA rdPtr, long param1) {
	rdPtr->rHo.hoFlags |= HOF_STRING;

	return (long)FFMpeg::get_hwDeviceNameByType(rdPtr->hwDeviceType);
}

long WINAPI DLLExport Expression_GetAudioTempo(LPRDATA rdPtr, long param1) {
	return ReturnFloat(rdPtr->pFFMpeg == nullptr
		? 0
		: rdPtr->pFFMpeg->get_audioTempo());
}

long WINAPI DLLExport Expression_GetVideoOverrideCodecName(LPRDATA rdPtr, long param1) {
	rdPtr->rHo.hoFlags |= HOF_STRING;

	*rdPtr->pRetStr = ConvertStrToWStr(*rdPtr->pVideoOverrideCodecName);

	return (long)rdPtr->pRetStr->c_str();
}

long WINAPI DLLExport Expression_GetAudioOverrideCodecName(LPRDATA rdPtr, long param1) {
	rdPtr->rHo.hoFlags |= HOF_STRING;

	*rdPtr->pRetStr = ConvertStrToWStr(*rdPtr->pAudioOverrideCodecName);

	return (long)rdPtr->pRetStr->c_str();
}

long WINAPI DLLExport Expression_GetThreadCount(LPRDATA rdPtr, long param1) {
    return rdPtr->threadCount;
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
			Condition_VideoOpen,
			Condition_VideoPlay,
			Condition_VideoLoop,
			Condition_VideoFinish,

			Condition_OnVideoFinish,
			Condition_OnVideoOpenFailed,

			Condition_VideoHardwareDecode,
			Condition_VideoWantedHardwareDecode,

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action_OpenVideo,
			Action_CloseVideo,
			Action_PlayVideo,
			Action_PauseVideo,

			Action_SetVolume,			
			Action_SetLoop,
			
			Action_SetPosition,
			Action_SetPositionWithFlag,

            Action_SetThreadCount,

			Action_SetAccurateSeek,

			Action_CacheVideo,
			Action_EraseVideo,

			Action_OpenVideoTo,

			Action_SetHWDevice,

			Action_Stretch,

			Action_SetAudioTempo,

			Action_SetForceNoAudio,

			Action_SetOverrideCodec,

			Action_ResetDisplay,

            Action_SetCopyToTexture,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression_GetVideoName,
			Expression_GetVideoPosition,
			Expression_GetVideoDuration,
			
			Expression_GetVolume,

			Expression_GetCurrentVideoFramePointer,
			Expression_GetGrabbedVideoFramePointer,

			Expression_GetVideoOpen,
			Expression_GetVideoPlay,
			Expression_GetVideoLoop,
			Expression_GetVideoFinish,

			Expression_GetHardwareDecodeState,
			Expression_GetActualHardwareDevice,
			Expression_GetWantedHardwareDevice,

			Expression_GetAudioTempo,

			Expression_GetVideoOverrideCodecName,
			Expression_GetAudioOverrideCodecName,

            Expression_GetThreadCount,

			0
			};