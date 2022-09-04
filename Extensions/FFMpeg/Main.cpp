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
		IDMN_ACTION_OV, M_ACTION_OV, ACT_ACTION_OV,	0, 1, PARAM_EXPSTRING, M_FILEPATH,
		IDMN_ACTION_SP, M_ACTION_SP, ACT_ACTION_SP,	0, 1, PARAM_EXPRESSION, M_POSITION,
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

long WINAPI DLLExport Condition_VideoOpen(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->bOpen;
}

long WINAPI DLLExport Condition_VideoPlay(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->bPlay;
}

long WINAPI DLLExport Condition_VideoLoop(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->bLoop;
}

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport Action_OpenVideo(LPRDATA rdPtr, long param1, long param2) {
	std::wstring filePath = GetFullPathNameStr((LPCWSTR)CNC_GetStringParameter(rdPtr));

	Stat_Quit = 1;
	while (true) {
		if (Stat_QuitComplete) {
			break;
		}
	}

	delete rdPtr->pFFMpeg;
	rdPtr->pFFMpeg = nullptr;

	rdPtr->bOpen = false;
	rdPtr->bLoop = false;
	rdPtr->bPlay = false;

	try {
		rdPtr->pFFMpeg = new FFMpeg(filePath);

		rdPtr->bOpen = true;
		rdPtr->bPlay = true;

		// TODO在开始播放时刷新
		*rdPtr->pPreviousTimer = std::chrono::steady_clock::now();

		//auto cur = rdPtr->pFFMpeg->get_volume();
		//rdPtr->pFFMpeg->set_volume(0);

		BlitVideoFrame(rdPtr, 0, [&](LPSURFACE& pMemSf) {
			rdPtr->rc.rcScaleX = ((float)rdPtr->swidth) / pMemSf->GetWidth();
			rdPtr->rc.rcScaleY = ((float)rdPtr->sheight) / pMemSf->GetHeight();
			});

		ReDisplay(rdPtr);
	}
	catch (...) {

	}

	return 0;
}

short WINAPI DLLExport Action_SetPosition(LPRDATA rdPtr, long param1, long param2) {
	size_t ms = (size_t)CNC_GetIntParameter(rdPtr);

	if (!rdPtr->bOpen) {
		return 0;
	}

	BlitVideoFrame(rdPtr, ms, [&](LPSURFACE& pMemSf) {
		rdPtr->rc.rcScaleX = ((float)rdPtr->swidth) / pMemSf->GetWidth();
		rdPtr->rc.rcScaleY = ((float)rdPtr->sheight) / pMemSf->GetHeight();
		});

	ReDisplay(rdPtr);

	return 0;
}
// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Expression_GetGrabVideoFramePointer(LPRDATA rdPtr,long param1) {
	size_t ms = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	bool bHwa = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	if (!rdPtr->bOpen) {
		return 0;
	}

	BlitVideoFrame(rdPtr, ms, [&](LPSURFACE& pMemSf) {
		delete rdPtr->pGrabbedFrame;
		rdPtr->pGrabbedFrame = nullptr;

		if (bHwa) {
			if (rdPtr->pGrabbedFrame == nullptr) {
				rdPtr->pGrabbedFrame = CreateHWASurface(rdPtr, 32, pMemSf->GetWidth(), pMemSf->GetHeight(), ST_HWA_ROMTEXTURE);
				rdPtr->pGrabbedFrame->CreateAlpha();
			}

			pMemSf->Blit(*rdPtr->pGrabbedFrame);
		}
		else {
			if (rdPtr->pGrabbedFrame == nullptr) {
				rdPtr->pGrabbedFrame = CreateSurface(32, pMemSf->GetWidth(), pMemSf->GetHeight());
			}
		
			pMemSf->Blit(*rdPtr->pGrabbedFrame);
		}
		});
	
	return ConvertToLong(rdPtr->pGrabbedFrame);
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

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action_OpenVideo,
			Action_SetPosition,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression_GetGrabVideoFramePointer,

			0
			};