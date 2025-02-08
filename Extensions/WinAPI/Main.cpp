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
short conditionsInfos[] =
{
	//IDMN_CONDITION, M_CONDITION, CND_CONDITION, EVFLAGS_ALWAYS, 3, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_CND_P1, M_CND_P2, M_CND_P3,
	IDMN_CONDITION_INAP, M_CONDITION_INAP, CND_CONDITION_INAP, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPSTRING,PARA_CONDITION_INAP,
	IDMN_CONDITION_IML, M_CONDITION_IML, CND_CONDITION_IML, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
	IDMN_CONDITION_RKS, M_CONDITION_RKS, CND_CONDITION_RKS, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPRESSION,PARA_CONDITION_RKS,
	IDMN_CONDITION_IFS, M_CONDITION_IFS, CND_CONDITION_IFS, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
	IDMN_CONDITION_ICA, M_CONDITION_ICA, CND_CONDITION_ICA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
	IDMN_CONDITION_IDHA, M_CONDITION_IDHA, CND_CONDITION_IDHA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,PARA_EXPRESSION_FIXED,PARA_CONDITION_DIR,
	IDMN_CONDITION_IAIR, M_CONDITION_IAIR, CND_CONDITION_IAIR, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,PARA_CONDITION_IAIR,PARA_CONDITION_CMP,
	IDMN_CONDITION_IWF, M_CONDITION_IWF, CND_CONDITION_IWF, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
	IDMN_CONDITION_ICLIHP, M_CONDITION_ICLIHP, CND_CONDITION_ICLIHP, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, PARA_EXPRESSION_CLI, PARA_EXPRESSION_PARAM,
	IDMN_CONDITION_OD, M_CONDITION_OD, CND_CONDITION_OD, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_OBJECT, PARA_CONDITION_OBJECT,
	IDMN_CONDITION_IDHA_S, M_CONDITION_IDHA, CND_CONDITION_IDHA_S, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2,PARAM_OBJECT,PARAM_EXPRESSION,PARA_CONDITION_OBJECT,PARA_CONDITION_DIR,
	
	IDMN_CONDITION_IRIE, M_CONDITION_IRIE, CND_CONDITION_IRIE, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
	IDMN_CONDITION_IAAT, M_CONDITION_IAAT, CND_CONDITION_IAAT, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_OBJECT, PARA_CONDITION_OBJECT,
	
	IDMN_CONDITION_OMC, M_CONDITION_OMC, CND_CONDITION_OMC, 0, 0,

	IDMN_CONDITION_IOHA, M_CONDITION_IOHA, CND_CONDITION_IOHA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,PARA_EXPRESSION_FIXED,PARA_CONDITION_ID,
	IDMN_CONDITION_IOHA_S, M_CONDITION_IOHA, CND_CONDITION_IOHA_S, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2,PARAM_OBJECT,PARAM_EXPRESSION,PARA_CONDITION_OBJECT,PARA_CONDITION_ID,

	IDMN_CONDITION_ORC, M_CONDITION_ORC, CND_CONDITION_ORC, 0, 0,
	IDMN_CONDITION_RMCX, M_CONDITION_RMCX, CND_CONDITION_RMCX, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,

	IDMN_CONDITION_OCB, M_CONDITION_OCB, CND_CONDITION_OCB, 0, 0,
	IDMN_CONDITION_OCF, M_CONDITION_OCF, CND_CONDITION_OCF, 0, 0,
};

// Definitions of parameters for each action
short actionsInfos[] =
{
	IDMN_ACTION_RUN,M_ACTION_RUN,ACT_ACTION_RUN,0, 2, PARAM_FILENAME2, PARAM_EXPSTRING,PARA_ACTION_RUN_1,PARA_ACTION_RUN_2,
	IDMN_ACTION_RUN_16,M_ACTION_RUN_16,ACT_ACTION_RUN_16,0, 2, PARAM_FILENAME2, PARAM_EXPSTRING,PARA_ACTION_RUN_1,PARA_ACTION_RUN_2,
	IDMN_ACTION_STOPBYNAME,M_ACTION_STOPBYNAME,ACT_ACTION_STOPBYNAME,0, 1, PARAM_EXPSTRING, PARA_ACTION_STOPBYNAME,
	IDMN_ACTION_STOPBYID,M_ACTION_STOPBYID,ACT_ACTION_STOPBYID,0, 1, PARAM_EXPRESSION, PARA_ACTION_STOPBYID,

	IDMN_ACTION_LOCKMOUSE,M_ACTION_LOCKMOUSE,ACT_ACTION_LOCKMOUSE,0, 1,PARAM_EXPRESSION,PARA_ACTION_LOCKMOUSE,
	IDMN_ACTION_LOCKMOUSEBR,M_ACTION_LOCKMOUSEBR,ACT_ACTION_LOCKMOUSEBR,0, 5, PARAM_EXPRESSION,PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARA_ACTION_LOCKMOUSEBR_L, PARA_ACTION_LOCKMOUSEBR_R, PARA_ACTION_LOCKMOUSEBR_T, PARA_ACTION_LOCKMOUSEBR_B,PARA_ACTION_LOCKMOUSEBR_TYPE,
	IDMN_ACTION_UNLOCKMOUSE,M_ACTION_UNLOCKMOUSE,ACT_ACTION_UNLOCKMOUSE,0, 0,

	IDMN_ACTION_SETMOUSE,M_ACTION_SETMOUSE,ACT_ACTION_SETMOUSE,0, 3, PARAM_EXPRESSION, PARAM_EXPRESSION,  PARAM_EXPRESSION, PARA_ACTION_SETMOUSE_X, PARA_ACTION_SETMOUSE_Y,PARA_ACTION_SETMOUSE_TYPE,

	IDMN_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_ON,M_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_ON,ACT_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_ON,0, 0,
	IDMN_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_OFF,M_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_OFF,ACT_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_OFF,0, 0,
	IDMN_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_ON,M_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_ON,ACT_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_ON,0, 0,
	IDMN_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_OFF,M_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_OFF,ACT_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_OFF,0, 0,
	IDMN_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_ON,M_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_ON,ACT_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_ON,0, 0,
	IDMN_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_OFF,M_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_OFF,ACT_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_OFF,0, 0,

	IDMN_ACTION_IME_DISABLE,M_ACTION_IME_DISABLE,ACT_ACTION_IME_DISABLE,0, 0,
	IDMN_ACTION_IME_ENABLE,M_ACTION_IME_ENABLE,ACT_ACTION_IME_ENABLE,0, 0,

	IDMN_ACTION_WINDOW_GF,M_ACTION_WINDOW_GF,ACT_ACTION_WINDOW_GF,0, 0,
	IDMN_ACTION_WINDOW_GW,M_ACTION_WINDOW_GW,ACT_ACTION_WINDOW_GW,0, 0,

	IDMN_ACTION_WINDOW_BFA,M_ACTION_WINDOW_BFA,ACT_ACTION_WINDOW_BFA,0, 5,PARAM_EXPRESSION,PARAM_EXPRESSION,PARAM_EXPRESSION,PARAM_EXPRESSION,PARAM_FILENAME2,PARA_ACTION_WINDOW_BFA_WIDTH,PARA_ACTION_WINDOW_BFA_HEIGHT,PARA_ACTION_WINDOW_BFA_SAVETOCLIPBOARD,PARA_ACTION_WINDOW_BFA_SAVETOFILE,PARA_ACTION_WINDOW_BFA_FILEPATH,

	IDMN_ACTION_WINDOW_LFC,M_ACTION_WINDOW_LFC,ACT_ACTION_WINDOW_LFC,0, 0,
	IDMN_ACTION_WINDOW_LFF,M_ACTION_WINDOW_LFF,ACT_ACTION_WINDOW_LFF,0, 1,PARAM_FILENAME2,0,

	IDMN_ACTION_WINDOW_RGB,M_ACTION_WINDOW_RGB,ACT_ACTION_WINDOW_RGB,0, 3,PARAM_EXPRESSION,PARAM_EXPRESSION,PARAM_EXPRESSION,PARA_ACTION_WINDOW_RGB_SIGMA,PARA_ACTION_WINDOW_RGB_SCALE,PARA_ACTION_WINDOW_MTGB_DIVIDE,

	IDMN_ACTION_WINDOW_MTGB,M_ACTION_WINDOW_MTGB,ACT_ACTION_WINDOW_MTGB,0, 3,PARAM_EXPRESSION,PARAM_EXPRESSION,PARAM_EXPRESSION,PARA_ACTION_WINDOW_MTGB_RADIUS,PARA_ACTION_WINDOW_RGB_SCALE,PARA_ACTION_WINDOW_MTGB_DIVIDE,
	IDMN_ACTION_WINDOW_LBG,M_ACTION_WINDOW_LBG,ACT_ACTION_WINDOW_LBG,0, 0,
	IDMN_ACTION_WINDOW_MTSB,M_ACTION_WINDOW_MTSB,ACT_ACTION_WINDOW_MTSB,0, 3,PARAM_EXPRESSION,PARAM_EXPRESSION,PARAM_EXPRESSION,PARA_ACTION_WINDOW_MTGB_RADIUS,PARA_ACTION_WINDOW_RGB_SCALE,PARA_ACTION_WINDOW_MTGB_DIVIDE,

	IDMN_ACTION_WINDOW_STF,M_ACTION_WINDOW_STF,ACT_ACTION_WINDOW_STF,0, 1,PARAM_FILENAME2,PARA_ACTION_WINDOW_BFA_FILEPATH,
	IDMN_ACTION_WINDOW_STC,M_ACTION_WINDOW_STC,ACT_ACTION_WINDOW_STC,0, 0,
	IDMN_ACTION_WINDOW_STT,M_ACTION_WINDOW_STT,ACT_ACTION_WINDOW_STT,0, 0,
	IDMN_ACTION_WINDOW_LFT,M_ACTION_WINDOW_LFT,ACT_ACTION_WINDOW_LFT,0, 0,

	IDMN_ACTION_SQ_F,M_ACTION_SQ_F,ACT_ACTION_SQ_F,0, 0,
	IDMN_ACTION_SQ_H,M_ACTION_SQ_H,ACT_ACTION_SQ_H,0, 0,

	IDMN_ACTION_SMS_ON,M_ACTION_SMS_ON,ACT_ACTION_SMS_ON,0, 0,
	IDMN_ACTION_SMS_OFF,M_ACTION_SMS_OFF,ACT_ACTION_SMS_OFF,0, 0,

	IDMN_ACTION_SDFN,M_ACTION_SDFN,ACT_ACTION_SDFN,0, 1,PARAM_EXPSTRING,PARA_ACTION_SDFN,

	IDMN_ACTION_GFL,M_ACTION_GFL,ACT_ACTION_GFL,0, 1,PARAM_EXPSTRING,PARA_ACTION_GFL,
	IDMN_ACTION_SB642I,M_ACTION_SB642I,ACT_ACTION_SB642I,0, 2,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_B64,PARA_ACTION_WINDOW_BFA_FILEPATH,

	IDMN_ACTION_SWS,M_ACTION_SWS,ACT_ACTION_SWS,0, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,PARA_ACTION_WINDOW_BFA_WIDTH,PARA_ACTION_WINDOW_BFA_HEIGHT,

	IDMN_ACTION_EF,M_ACTION_EF,ACT_ACTION_EF,0, 1,PARAM_EXPSTRING,PARA_ACTION_EF,

	IDMN_ACTION_SAOT,M_ACTION_SAOT,ACT_ACTION_SAOT,0, 1,PARAM_EXPRESSION, PARA_ACTION_WINDOW_ENABLE,
	
	IDMN_ACTION_GVF,M_ACTION_GVF,ACT_ACTION_GVF,0, 2,PARAM_EXPSTRING, PARAM_EXPRESSION,PARA_ACTION_FILEPATH,PARA_ACTION_MS,
	
	IDMN_ACTION_RMS,M_ACTION_RMS,ACT_ACTION_RMS,0, 0,

};

// Definitions of parameters for each expression
short expressionsInfos[] =
{
	//IDMN_EXPRESSION, M_EXPRESSION, EXP_EXPRESSION, 0, 3, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, 0, 0, 0,		
	//Note in the following.  If you are returning a string, you set the EXPFLAG_STRING.	
	//IDMN_EXPRESSION2, M_EXPRESSION2, EXP_EXPRESSION2, EXPFLAG_STRING, 1, EXPPARAM_STRING, 0,		
	//Note in the following.  If you are returning a float, you set the EXPFLAG_DOUBLE
	//IDMN_EXPRESSION3, M_EXPRESSION3, EXP_EXPRESSION3, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG, 0,

	IDMN_EXPRESSION_GPIDBN, M_EXPRESSION_GPIDBN, EXP_EXPRESSION_GPIDBN, 0, 1, EXPPARAM_STRING, PARA_EXPRESSION_GPIDBN,

	IDMN_EXPRESSION_GCLR_L, M_EXPRESSION_GCLR_L, EXP_EXPRESSION_GCLR_L, 0, 0,
	IDMN_EXPRESSION_GCLR_R, M_EXPRESSION_GCLR_R, EXP_EXPRESSION_GCLR_R, 0, 0,
	IDMN_EXPRESSION_GCLR_T, M_EXPRESSION_GCLR_T, EXP_EXPRESSION_GCLR_T, 0, 0,
	IDMN_EXPRESSION_GCLR_B, M_EXPRESSION_GCLR_B, EXP_EXPRESSION_GCLR_B, 0, 0,

	IDMN_EXPRESSION_RXO, M_EXPRESSION_RXO, EXP_EXPRESSION_RXO, 0, 1,EXPPARAM_LONG,PARA_EXPRESSION_RO_TYPE,
	IDMN_EXPRESSION_RYO, M_EXPRESSION_RYO, EXP_EXPRESSION_RYO, 0, 1,EXPPARAM_LONG,PARA_EXPRESSION_RO_TYPE,

	IDMN_EXPRESSION_RXS, M_EXPRESSION_RXS, EXP_EXPRESSION_RXS, EXPFLAG_DOUBLE, 0,
	IDMN_EXPRESSION_RYS, M_EXPRESSION_RYS, EXP_EXPRESSION_RYS, EXPFLAG_DOUBLE, 0,

	IDMN_EXPRESSION_GCWR_L, M_EXPRESSION_GCWR_L, EXP_EXPRESSION_GCWR_L, 0, 1,EXPPARAM_LONG,PARA_EXPRESSION_GCWR_TYPE,
	IDMN_EXPRESSION_GCWR_R, M_EXPRESSION_GCWR_R, EXP_EXPRESSION_GCWR_R, 0, 1,EXPPARAM_LONG,PARA_EXPRESSION_GCWR_TYPE,
	IDMN_EXPRESSION_GCWR_T, M_EXPRESSION_GCWR_T, EXP_EXPRESSION_GCWR_T, 0, 1,EXPPARAM_LONG,PARA_EXPRESSION_GCWR_TYPE,
	IDMN_EXPRESSION_GCWR_B, M_EXPRESSION_GCWR_B, EXP_EXPRESSION_GCWR_B, 0, 1,EXPPARAM_LONG,PARA_EXPRESSION_GCWR_TYPE,

	IDMN_EXPRESSION_GCLT, M_EXPRESSION_GCLT, EXP_EXPRESSION_GCLT, 0, 0,

	IDMN_EXPRESSION_IME_STATE, M_EXPRESSION_IME_STATE, EXP_EXPRESSION_IME_STATE, 0, 0,

	IDMN_EXPRESSION_DPISCALING, M_EXPRESSION_DPISCALING, EXP_EXPRESSION_DPISCALING, 0, 0,

	IDMN_EXPRESSION_IFS, M_EXPRESSION_IFS, EXP_EXPRESSION_IFS, 0, 0,

	IDMN_EXPRESSION_GFN, M_EXPRESSION_GFN, EXP_EXPRESSION_GFN, EXPFLAG_STRING, 0,

	IDMN_EXPRESSION_GT, M_EXPRESSION_GT, EXP_EXPRESSION_GT, EXPFLAG_STRING, 0,

	IDMN_EXPRESSION_GTPT, M_EXPRESSION_GTPT, EXP_EXPRESSION_GTPT, EXPFLAG_STRING, 1,EXPPARAM_LONG,PARA_EXPRESSION_GTPT,

	IDMN_EXPRESSION_GFLS, M_EXPRESSION_GFLS, EXP_EXPRESSION_GFLS, 0, 0,
	IDMN_EXPRESSION_GFLA, M_EXPRESSION_GFLA, EXP_EXPRESSION_GFLA, EXPFLAG_STRING, 1,EXPPARAM_LONG,PARA_EXPRESSION_GFLA,

	IDMN_EXPRESSION_GFH, M_EXPRESSION_GFH, EXP_EXPRESSION_GFH, EXPFLAG_STRING, 1, EXPPARAM_STRING, PARA_EXPRESSION_GFH,

	IDMN_EXPRESSION_GAD, M_EXPRESSION_GAD, EXP_EXPRESSION_GAD, 0, 1, EXPPARAM_LONG, PARA_EXPRESSION_FIXED,

	IDMN_EXPRESSION_GVWS, M_EXPRESSION_GVWS, EXP_EXPRESSION_GVWS, EXPFLAG_STRING, 1, EXPPARAM_LONG, PARA_EXPRESSION_GVWS,

	IDMN_EXPRESSION_HEX2RGB, M_EXPRESSION_HEX2RGB, EXP_EXPRESSION_HEX2RGB, 0, 1, EXPPARAM_STRING, PARA_EXPRESSION_HEX2RGB,

	IDMN_EXPRESSION_GSON, M_EXPRESSION_GSON, EXP_EXPRESSION_GSON, 0, 1, EXPPARAM_LONG, PARA_EXPRESSION_FIXED,

	IDMN_EXPRESSION_W2L, M_EXPRESSION_W2L, EXP_EXPRESSION_W2L, 0, 1, EXPPARAM_STRING, PARA_EXPRESSION_W2L,
	IDMN_EXPRESSION_L2W, M_EXPRESSION_L2W, EXP_EXPRESSION_L2W, EXPFLAG_STRING, 1, EXPPARAM_LONG, PARA_EXPRESSION_L2W,

	IDMN_EXPRESSION_GCLI, M_EXPRESSION_GCLI, EXP_EXPRESSION_GCLI, EXPFLAG_STRING, 2, EXPPARAM_STRING, EXPPARAM_STRING, PARA_EXPRESSION_CLI, PARA_EXPRESSION_PARAM,

	IDMN_EXPRESSION_C2B, M_EXPRESSION_C2B, EXP_EXPRESSION_C2B, 0, 1, EXPPARAM_LONG, PARA_EXPRESSION_C2B,

	IDMN_EXPRESSION_GVFP, M_EXPRESSION_GVFP, EXP_EXPRESSION_GVFP, 0, 0,

	IDMN_EXPRESSION_GD, M_EXPRESSION_GD, EXP_EXPRESSION_GD, 0, 1, EXPPARAM_LONG, PARA_EXPRESSION_GVWS,

	IDMN_EXPRESSION_GCMW, M_EXPRESSION_GCMW, EXP_EXPRESSION_GCMW, 0, 0,
	IDMN_EXPRESSION_GCMH, M_EXPRESSION_GCMH, EXP_EXPRESSION_GCMH, 0, 0,

	IDMN_EXPRESSION_GFFN, M_EXPRESSION_GFFN, EXP_EXPRESSION_GFFN, EXPFLAG_STRING, 1, EXPPARAM_STRING, PARA_ACTION_GFL,

	IDMN_EXPRESSION_GDT, M_EXPRESSION_GDT, EXP_EXPRESSION_GDT, EXPFLAG_STRING, 1, EXPPARAM_LONG,PARA_EXPRESSION_GDT,

	IDMN_EXPRESSION_GTPM, M_EXPRESSION_GTPM, EXP_EXPRESSION_GTPM, 0, 0,
	IDMN_EXPRESSION_GFPM, M_EXPRESSION_GFPM, EXP_EXPRESSION_GFPM, 0, 0,

	IDMN_EXPRESSION_GAID, M_EXPRESSION_GAID, EXP_EXPRESSION_GAID, 0, 1, EXPPARAM_LONG, PARA_EXPRESSION_FIXED,
	
	IDMN_EXPRESSION_GCFC, M_EXPRESSION_GCFC, EXP_EXPRESSION_GCFC, 0, 1, EXPPARAM_LONG, PARA_EXPRESSION_FIXED,
	IDMN_EXPRESSION_GADFC, M_EXPRESSION_GADFC, EXP_EXPRESSION_GADFC, 0, 3, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, PARA_EXPRESSION_FIXED, PARA_CONDITION_ID, PARA_CONDITION_DIR,


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

long WINAPI DLLExport IsNameAProcess(LPRDATA rdPtr, long param1, long param2) {
	return (GetProcessIDByName((LPCTSTR)param1) != 0) ? TRUE : FALSE;
}

long WINAPI DLLExport IsMouseLocked(LPRDATA rdPtr, long param1, long param2) {
	return rdPtr->Lock ? TRUE : FALSE;
}

//按键检测
long WINAPI DLLExport ReturnKeyState(LPRDATA rdPtr, long param1, long param2) {
	return  (GetKeyState((int)param1) & 1);
}

//全屏状态
long WINAPI DLLExport IsFullScreen(LPRDATA rdPtr, long param1, long param2) {
	return  IsZoomed(rdPtr->MainWindowHandle) ? TRUE : FALSE;
}

//全屏状态
long WINAPI DLLExport IsClipBoardAvailable(LPRDATA rdPtr, long param1, long param2) {
	return  IsClipboardFormatAvailable(CF_DIB) ? TRUE : FALSE;
}

//朝向动画
long WINAPI DLLExport IsObjectHasAnimationInDirection(LPRDATA rdPtr, long param1, long param2) {
	const int Fixed = (int)CNC_GetIntParameter(rdPtr);
	const size_t Dir = (size_t)CNC_GetIntParameter(rdPtr);

	return DirHasFrame(rdPtr, Fixed, Dir);
}

long WINAPI DLLExport IsObjectHasAnimationInDirection_Scope(LPRDATA rdPtr, long param1, long param2) {
	const bool negated = IsNegated(rdPtr);

	const short oil = (short)OIL_GetParameter(rdPtr);
	const size_t Dir = (size_t)CNC_GetIntParameter(rdPtr);

	return rdPtr->pSelect->FilterObjects(rdPtr, oil, negated,
		[Dir](LPRDATA rdPtr, LPRO object)->bool {
		return DirHasFrame(object, Dir);
		});
}

long WINAPI DLLExport IsObjectHasAnimationID(LPRDATA rdPtr, long param1, long param2) {
	const int fixed = (int)CNC_GetIntParameter(rdPtr);
	const size_t id = (size_t)CNC_GetIntParameter(rdPtr);

	return ObjectHasAnimationID(rdPtr, fixed, id);
}

long WINAPI DLLExport IsObjectHasAnimationID_Scope(LPRDATA rdPtr, long param1, long param2) {
	const bool negated = IsNegated(rdPtr);

	const short oil = (short)OIL_GetParameter(rdPtr);
	const size_t id = (size_t)CNC_GetIntParameter(rdPtr);

	return rdPtr->pSelect->FilterObjects(rdPtr, oil, negated,
		[id] (LPRDATA rdPtr, LPRO object)->bool {
			return ObjectHasAnimationID(object, id);
		});
}

long WINAPI DLLExport IsAnotherInstanceRunning(LPRDATA rdPtr, long param1, long param2) {
	bool byFullPath = (bool)CNC_GetIntParameter(rdPtr);
	int CMP = (int)CNC_GetIntParameter(rdPtr);

	bool ret = false;

	HANDLE snapshot;
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32* info = new PROCESSENTRY32;
	PROCESSENTRY32* curInfo = new PROCESSENTRY32;

	info->dwSize = sizeof(PROCESSENTRY32);
	curInfo->dwSize = sizeof(PROCESSENTRY32);

	MODULEENTRY32* minfo = new MODULEENTRY32;
	MODULEENTRY32* curMinfo = new MODULEENTRY32;

	minfo->dwSize = sizeof(MODULEENTRY32);
	curMinfo->dwSize = sizeof(MODULEENTRY32);

	DWORD ProcessID = _getpid();

	auto getMinfo = [] (PROCESSENTRY32* info, MODULEENTRY32* minfo)->void {
		HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, info->th32ProcessID);
		Module32First(hModule, minfo);
	};

	Process32First(snapshot, curInfo);
	while (Process32Next(snapshot, curInfo) != FALSE) {
		if (curInfo->th32ProcessID == ProcessID) {
			getMinfo(curInfo, curMinfo);
			break;
		}
	}

	BYTE* pCurData = GetFileVersion_GetFileVersionInfo(curMinfo->szExePath);

	Process32First(snapshot, info);
	while (Process32Next(snapshot, info) != FALSE) {
		if ((wcscmp(curInfo->szExeFile, info->szExeFile) == 0)
			&& (curInfo->th32ProcessID != info->th32ProcessID)) {
			bool localRet = true;

			getMinfo(info, minfo);

			if (localRet && (CMP != 0)) {
				BYTE* pData = GetFileVersion_GetFileVersionInfo(minfo->szExePath);

				localRet = localRet && GetFileVersion_CMPDefault(pCurData, pData, CMP);

				delete[] pData;
			}

			if (localRet && byFullPath) {
				if (wcscmp(curMinfo->szExePath, minfo->szExePath) == 0) {
					localRet = localRet && true;
				}
				else {
					localRet = localRet && false;
				}
			}

			if (localRet) {
				ret = localRet;
				break;
			}
		}
	}

	delete[] pCurData;

	delete info;
	delete curInfo;

	delete minfo;
	delete curMinfo;

	return ret ? TRUE : FALSE;
}

long WINAPI DLLExport IsWindowForcused(LPRDATA rdPtr, long param1, long param2) {
	return GetForegroundWindow() == rdPtr->MainWindowHandle;
}

long WINAPI DLLExport IsCommandLineHasParam(LPRDATA rdPtr, long param1, long param2) {
	auto commandLine = ConvertWStrToStr((LPCWSTR)CNC_GetStringParameter(rdPtr));
	auto param = ConvertWStrToStr((LPCWSTR)CNC_GetStringParameter(rdPtr));

	CLI::App app;

	app.allow_windows_style_options();

	bool result = false;
	app.add_flag(param, result);

	//commandLine = "\"C:\\example.exe\"";

	//CLI11_PARSE(app, commandLine, true);	
	try {
		app.parse(commandLine, true);
	}
	catch (const CLI::ParseError& e) {
		auto name = e.get_name();
		//return app.exit(e);
	}

	return result ? TRUE : FALSE;
}

long WINAPI DLLExport IsObjectDestroyed(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);

	return rdPtr->pSelect->FilterObjects(rdPtr, oil, negated, [](LPRDATA rdPtr, LPRO object)->bool {
		return IsDestroyed(object);
		});
}

long WINAPI DLLExport IsRunningInEditor(LPRDATA rdPtr, long param1, long param2) {
#ifdef RUN_ONLY
	return false;
#else
	return true;
#endif // RUN_ONLY	
}

long WINAPI DLLExport IsActiveAtTop(LPRDATA rdPtr, long param1, long param2) {
	bool negated = IsNegated(rdPtr);

	short oil = (short)OIL_GetParameter(rdPtr);

	auto selected = rdPtr->pSelect->Selected(oil);
	auto pObj = rdPtr->pSelect->GetFirstObject(oil, selected);

	if (pObj != nullptr && LPROValid(pObj, IDENTIFIER_ACTIVE)) {
		LPRO pResult = nullptr;
		int zOrder = (std::numeric_limits<int>::min)();

		rdPtr->pSelect->ForEach(rdPtr, oil, [&](LPRO pObject) {
			if (pObject->ros.rsZOrder >= zOrder) {
				pResult = pObject;
				zOrder = pObject->ros.rsZOrder;
			}
			}, selected ? ForEachFlag_SelectedOnly : ForEachFlag_ForceAll);

		if (pResult != nullptr) {
			rdPtr->pSelect->SelectOneObject(pResult);

			return true;
		}
	}

	rdPtr->pSelect->SelectNone(oil);

	return false;
}

long WINAPI DLLExport OnMonitorChange(LPRDATA rdPtr, long param1, long param2) {
	return true;
}

long WINAPI DLLExport OnResizingComplete(LPRDATA rdPtr, long param1, long param2) {
	return true;
}

long WINAPI DLLExport ResizingMainlyChangedX(LPRDATA rdPtr, long param1, long param2) {
	return windowResizing.bMainlyChangedX;
}

long WINAPI DLLExport OnClickBackword(LPRDATA rdPtr, long param1, long param2) {
	return true;
}

long WINAPI DLLExport OnClickForward(LPRDATA rdPtr, long param1, long param2) {
	return true;
}

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

// Actions work just like Conditions

// Use directly param1 and/or param2 if this action has 1 or 2 parameters

// Use this if this action has 3 parameters or more
//	long p1 = CNC_GetParameter(rdPtr);
//	long p2 = CNC_GetParameter(rdPtr);
//	long p3 = CNC_GetParameter(rdPtr);
//	etc.

//运行程序
short WINAPI DLLExport RunApplication(LPRDATA rdPtr, long param1, long param2) {

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	LPCTSTR lpApplicationName = (LPCTSTR)param1;
	LPTSTR lpCommandLine = (LPTSTR)param2;

	AddNewApplicationName((LPTSTR)param1);

	CreateProcess
	(
		//LPCTSTR lpApplicationName,		
		lpApplicationName,
		//LPTSTR lpCommandLine,		
		lpCommandLine,
		//LPSECURITY_ATTRIBUTES lpProcessAttributes,
		NULL,
		//LPSECURITY_ATTRIBUTES lpThreadAttributes,
		NULL,
		//BOOL bInheritHandles,
		false,
		//DWORD dwCreationFlags,
		CREATE_NO_WINDOW,
		//LPVOID lpEnvironment,
		NULL,
		//LPCTSTR lpCurrentDirectory,
		NULL,
		//LPSTARTUPINFO lpStartupInfo,
		&si,
		//LPPROCESS_INFORMATION lpProcessInformation
		&pi
	);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}

short WINAPI DLLExport Run16BitApplication(LPRDATA rdPtr, long param1, long param2) {

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	//如果可执行模块是16位应用程序，lpApplicationName应为NULL
	//lpCommandLine指向的字符串应指定可执行模块及其参数。

	//拼接字符串
	size_t total_length;

	LPCTSTR Space = L" ";
	LPTSTR lpApplicationName = (LPTSTR)param1;
	LPTSTR lpCommandLine = (LPTSTR)param2;

	AddNewApplicationName(lpApplicationName);

	//预留结尾分隔符
	total_length = wcslen(lpApplicationName) + wcslen(Space) + wcslen(lpCommandLine) + 1;

	if (total_length > _MAX_PATH) {
		return 0;
	}

	LPTSTR ANCL = new WCHAR [total_length];

	wcscpy_s(ANCL, total_length, lpApplicationName);
	wcscat_s(ANCL, total_length, Space);
	wcscat_s(ANCL, total_length, lpCommandLine);

	CreateProcess
	(
		//LPCTSTR lpApplicationName,		
		NULL,
		//LPTSTR lpCommandLine,		
		ANCL,
		//LPSECURITY_ATTRIBUTES lpProcessAttributes,
		NULL,
		//LPSECURITY_ATTRIBUTES lpThreadAttributes,
		NULL,
		//BOOL bInheritHandles,
		false,
		//DWORD dwCreationFlags,
		CREATE_NO_WINDOW,
		//LPVOID lpEnvironment,
		NULL,
		//LPCTSTR lpCurrentDirectory,
		NULL,
		//LPSTARTUPINFO lpStartupInfo,
		&si,
		//LPPROCESS_INFORMATION lpProcessInformation
		&pi
	);

	delete[] ANCL;

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}

short WINAPI DLLExport StopApplicationByName(LPRDATA rdPtr, long param1, long param2) {
	//输入参数	
	TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, GetProcessIDByName((LPCTSTR)param1)), 0);
	DeleteRunApplicationName((LPCTSTR)param1);
	return 0;
}

short WINAPI DLLExport StopApplicationByPID(LPRDATA rdPtr, long param1, long param2) {

	DWORD ProcessID = (DWORD)param1;

	if (ProcessID == 0) {
		return 0;
	}

	TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, ProcessID), 0);

	return 0;
}

//锁定鼠标
short WINAPI DLLExport LockMouse(LPRDATA rdPtr, long param1, long param2) {
	LockMouse(rdPtr, LT((int)param1));
	return 0;
}

short WINAPI DLLExport LockMouseByRect(LPRDATA rdPtr, long param1, long param2) {

	LONG left = CNC_GetIntParameter(rdPtr);
	LONG right = CNC_GetIntParameter(rdPtr);
	LONG top = CNC_GetIntParameter(rdPtr);
	LONG bottom = CNC_GetIntParameter(rdPtr);

	int Type = CNC_GetIntParameter(rdPtr);

	rdPtr->UserSetRect = { left ,top ,right ,bottom };
	LockMouse(rdPtr, RECT { left ,top ,right ,bottom }, RT(Type));

	return 0;
}

short WINAPI DLLExport UnlockMouse(LPRDATA rdPtr, long param1, long param2) {
	UnlockMouse(rdPtr);
	return 0;
}

//更新设定
short WINAPI DLLExport KeepLock_SetON(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->KeepLock = true;
	return 0;
}
short WINAPI DLLExport KeepLock_SetOFF(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->KeepLock = false;
	return 0;
}
short WINAPI DLLExport UpdateLock_SetON(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->UpdateLock = true;
	return 0;
}
short WINAPI DLLExport UpdateLock_SetOFF(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->UpdateLock = false;
	return 0;
}
short WINAPI DLLExport RectOffset_SetON(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->RectOffset_State = true;
	return 0;
}
short WINAPI DLLExport RectOffset_SetOFF(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->RectOffset_State = false;
	return 0;
}

//指定鼠标坐标
short WINAPI DLLExport SetMousePos(LPRDATA rdPtr, long param1, long param2) {
	int X = CNC_GetIntParameter(rdPtr);
	int Y = CNC_GetIntParameter(rdPtr);
	int Type = CNC_GetIntParameter(rdPtr);
	SetMousePosition(rdPtr, X, Y, ST(Type));
	return 0;
}

//禁用输入法
short WINAPI DLLExport IME_Disable(LPRDATA rdPtr, long param1, long param2) {
	IMEStateControl(rdPtr->MainWindowHandle, false);
	return 0;
}

//启用输入法
short WINAPI DLLExport IME_Enable(LPRDATA rdPtr, long param1, long param2) {
	IMEStateControl(rdPtr->MainWindowHandle, true);
	return 0;
}

//窗口控制
short WINAPI DLLExport GoFullScreen(LPRDATA rdPtr, long param1, long param2) {
	ShowWindow(rdPtr->MainWindowHandle, SW_MAXIMIZE);
	return 0;
}
short WINAPI DLLExport GoWindowed(LPRDATA rdPtr, long param1, long param2) {
	ShowWindow(rdPtr->MainWindowHandle, SW_RESTORE);
	return 0;
}
short WINAPI DLLExport SetSize(LPRDATA rdPtr, long param1, long param2) {
	const auto wWidth = (size_t)CNC_GetIntParameter(rdPtr);
	const auto wHeight = (size_t)CNC_GetIntParameter(rdPtr);
	
	RECT wRect;
	::GetWindowRect(rdPtr->MainWindowHandle, &wRect);

	RECT clientRect;
	GetCurrentClientRectToScreen(rdPtr->MainWindowHandle, &clientRect);
	
	const auto clientWidth= clientRect.right - clientRect.left;
	const auto clientHeight = clientRect.bottom - clientRect.top;

	const auto owWidth = wRect.right - wRect.left;
	const auto owHeight = wRect.bottom - wRect.top;

	const auto cwOffsetW = owWidth - clientWidth;
	const auto cwOffsetH = owHeight - clientHeight;

	const auto oldX = wRect.left;
	const auto oldY = wRect.top;

	const auto newW = wWidth + cwOffsetW;
	const auto newH = wHeight + cwOffsetH;

	auto newX = oldX + static_cast<int>(clientWidth - wWidth) / 2;
	auto newY = oldY + static_cast<int>(clientHeight - wHeight) / 2;

	//Make sure sized window won't exceed the monitor rect
	//https://www.cnblogs.com/163yun/p/9583512.html
	//https://learn.microsoft.com/zh-cn/windows/win32/gdi/multiple-monitor-system-metrics
	RECT monitor;
	monitor.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	monitor.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
	monitor.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	monitor.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	// RB
	newX = (std::min)(newX, static_cast<long>(monitor.right - newW));
	newY = (std::min)(newY, static_cast<long>(monitor.bottom - newH));

	// LT
	// Title bar has higher priority
	newX = (std::max)(newX, monitor.left);
	newY = (std::max)(newY, monitor.top);

	SetWindowPos(rdPtr->MainWindowHandle
		, nullptr
		, newX, newY
		, newW
		, newH
		, SWP_SHOWWINDOW| SWP_NOZORDER);

	return 0;
}

short WINAPI DLLExport SetAlwaysOnTop(LPRDATA rdPtr, long param1, long param2) {
	bool enable = (bool)CNC_GetIntParameter(rdPtr);

	SetWindowPos(rdPtr->MainWindowHandle
		, enable ? HWND_TOPMOST : HWND_NOTOPMOST
		, 0, 0
		, 0
		, 0
		, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

	return 0;
}

short WINAPI DLLExport StretchQuality_Fast(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->StretchQuality = Fast;
	return 0;
}
short WINAPI DLLExport StretchQuality_High(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->StretchQuality = HighQuality;
	return 0;
}

short WINAPI DLLExport MultiThreadSave_ON(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->MultiThreadSave = true;
	return 0;
}
short WINAPI DLLExport MultiThreadSave_OFF(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->MultiThreadSave = false;
	return 0;
}

short WINAPI DLLExport SetDefaultFilterName(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR DefaultFilterName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	rdPtr->DefaultFilterName = GetFilterName(DefaultFilterName);

	return 0;
}

short WINAPI DLLExport BitBltFrameArea(LPRDATA rdPtr, long param1, long param2) {
	//获取参数
	int Width = CNC_GetIntParameter(rdPtr);
	int Height = CNC_GetIntParameter(rdPtr);
	bool SaveToClipboard = CNC_GetIntParameter(rdPtr);
	bool SaveToFile = CNC_GetIntParameter(rdPtr);
	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	//计算截取区域
	RECT CurrentFrameRect;
	::GetWindowRect(rdPtr->FrameWindowHandle, &CurrentFrameRect);
	int FrameWidth = CurrentFrameRect.right - CurrentFrameRect.left;
	int FrameHeight = CurrentFrameRect.bottom - CurrentFrameRect.top;

	//Surface Prototype
	LPSURFACE proto = nullptr;
	GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);

	HDC hdcWindow = GetDC(rdPtr->FrameWindowHandle);

	if (rdPtr->Display) {
		// Update Display Surface
		rdPtr->img->Delete();
		rdPtr->img->Create(rdPtr->swidth, rdPtr->sheight, proto);

		BltToSurface(hdcWindow, FrameWidth, FrameHeight, rdPtr->img);

#ifdef _DEBUG
		_SavetoClipBoard(rdPtr->img, false);
#endif

		// Redraw object
		ReDisplay(rdPtr);
	}

	//需要输出
	if (SaveToClipboard || SaveToFile) {
		//保存到剪贴板
		if (SaveToClipboard) {
			LPSURFACE img = new cSurface;
			img->Create(Width, Height, proto);
			BltToSurface(hdcWindow, FrameWidth, FrameHeight, img);

			SavetoClipBoard(img, rdPtr, true);
		}

		//保存到文件
		if (SaveToFile) {
			LPSURFACE img = new cSurface;
			img->Create(Width, Height, proto);
			BltToSurface(hdcWindow, FrameWidth, FrameHeight, img);

			SavetoFile(img, FilePath, rdPtr, true);
		}
	}

	//清理
	ReleaseDC(rdPtr->FrameWindowHandle, hdcWindow);

	return 0;
}

short WINAPI DLLExport SaveToFile(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
		SavetoFile(rdPtr->img, FilePath, rdPtr, false);
	}
	return 0;
}

short WINAPI DLLExport SaveToClipBoard(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		SavetoClipBoard(rdPtr->img, rdPtr, false);
	}
	return 0;
}

short WINAPI DLLExport SaveToTemp(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		rdPtr->temp->Delete();
		rdPtr->temp->Clone(*rdPtr->img);
	}
	return 0;
}

short WINAPI DLLExport LoadFromTemp(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		rdPtr->img->Delete();
		rdPtr->img->Clone(*rdPtr->temp);
	}
	return 0;
}

short WINAPI DLLExport LoadFromClipBoard(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		_LoadFromClipBoard(rdPtr->img, rdPtr->swidth, rdPtr->sheight, false, rdPtr->StretchQuality, rdPtr->MainWindowHandle);

		// Redraw object
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport LoadFromFile(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
		_LoadFromFile(rdPtr->img, FilePath, rdPtr, rdPtr->swidth, rdPtr->sheight, false, rdPtr->StretchQuality);

		// Redraw object
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport LoadFromBackDrop(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		//Dimensions
		int width = rdPtr->img->GetWidth(), height = rdPtr->img->GetHeight();
		int screenX = rdPtr->rHo.hoX - rdPtr->rhPtr->rhWindowX;
		int screenY = rdPtr->rHo.hoY - rdPtr->rhPtr->rhWindowY;

		LPSURFACE proto = nullptr;
		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);

		rdPtr->img->Delete();
		rdPtr->img->Create(width, height, proto);

		HDC sdc = GetDC(rdPtr->FrameWindowHandle);
		HDC ddc = rdPtr->img->GetDC();

		SetStretchBltMode(ddc, COLORONCOLOR);
		SetBrushOrgEx(ddc, 0, 0, NULL);

		StretchBlt(ddc, 0, 0, width, height, sdc, screenX, screenY, width, height, SRCCOPY);

		ReleaseDC(rdPtr->FrameWindowHandle, sdc);
		rdPtr->img->ReleaseDC(ddc);

		// Redraw object			
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport RecursiveGaussBlur(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		//获取参数
		constexpr auto MIN_SIGMA = 0.0;
		constexpr auto MAX_SIGMA = 1.615;

		long p1 = CNC_GetFloatParameter(rdPtr);
		double sigma = *(float*)&p1;
		sigma = (std::min)(MAX_SIGMA, (std::max)(MIN_SIGMA, sigma));

		long p2 = CNC_GetFloatParameter(rdPtr);
		float scale = *(float*)&p2;
		GetValidScale(&scale);

		int divide = CNC_GetIntParameter(rdPtr);
		GetMaximumDivide(&divide);

		auto GetCoef = [] (double sigma) -> GCoef {
			double q1, q2, q3;

			if (sigma >= 2.5) {
				q1 = 0.98711 * sigma - 0.96330;
			}
			else if ((sigma >= 0.5) && (sigma < 2.5)) {
				q1 = 3.97156 - 4.14554 * (double)sqrt((double)1.0 - 0.26891 * sigma);
			}
			else {
				q1 = 0.1147705018520355224609375;
			}

			q2 = q1 * q1;
			q3 = q1 * q2;

			GCoef c;

			c.b [0] = (1.57825 + (2.44413 * q1) + (1.4281 * q2) + (0.422205 * q3));
			c.b [1] = ((2.44413 * q1) + (2.85619 * q2) + (1.26661 * q3));
			c.b [2] = (-(1.4281 * q2) + (1.26661 * q3));
			c.b [3] = (0.422205 * q3);
			c.B = 1.0 - ((c.b [1] + c.b [2] + c.b [3]) / c.b [0]);

			c.sigma = sigma;
			c.N = 3;

			return c;
		};

		GCoef c = GetCoef(sigma);

		//Dimensions
		int owidth = rdPtr->img->GetWidth(), oheight = rdPtr->img->GetHeight();
		int width = (int)(owidth / scale);
		int height = (int)(oheight / scale);

		// 降采样
		cSurface ResizedImg;
		LPSURFACE proto = nullptr;
		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);

		if (!(scale == 1.0)) {
			ResizedImg.Clone(*rdPtr->img);

			rdPtr->img->Delete();
			rdPtr->img->Create(width, height, proto);

			Stretch(&ResizedImg, rdPtr->img, Fast);
		}

		//Lock buffer, get pitch etc.
		BYTE* buff = rdPtr->img->LockBuffer();
		if (!buff) return 0;

		int pitch = rdPtr->img->GetPitch();
		if (pitch < 0) {
			pitch *= -1;
			buff -= pitch * (height - 1);
		}
		int size = pitch * height;
		int byte = rdPtr->img->GetDepth() >> 3;

		auto RecursiveGaussFilter = [c] (BYTE* src, BYTE* des, int size, int stride) {
			RGBA* W = (RGBA*)malloc(sizeof(RGBA) * (size + 3));
			RGBA* W_Temp = (RGBA*)malloc(sizeof(RGBA) * (size + 3));

			//init			
			RGBA Input = { (double)src[2],(double)src[1],(double)src[0],0 };
			RGBA Output = { 0,0,0,0 };

			//forward
			W[0] = W[1] = W[2] = Input;
			for (int n1 = 3; n1 < size + 3; n1++) {
				int offset = (n1 - 3) * stride;
				Input = { (double)src[offset + 2],(double)src[offset + 1],(double)src[offset + 0],0 };
				W[n1] = (c.B * Input + (c.b[1] * W[n1 - 1] + c.b[2] * W[n1 - 2] + c.b[3] * W[n1 - 3]) / c.b[0]);
			}

			//backword
			W_Temp[size] = W_Temp[size + 1] = W_Temp[size + 2] = W[size + 2];
			for (int n2 = size - 1; n2 >= 0; n2--) {
				int offset = n2 * stride;
				W_Temp[n2] = (c.B * W[n2] + (c.b[1] * W_Temp[n2 + 1] + c.b[2] * W_Temp[n2 + 2] + c.b[3] * W_Temp[n2 + 3]) / c.b[0]);
				Output = Range(W_Temp[n2]);
				des[offset + 2] = (BYTE)Output.r;
				des[offset + 1] = (BYTE)Output.g;
				des[offset + 0] = (BYTE)Output.b;
			}

			free(W);
			free(W_Temp);
		};

		//单线程

		////通常写法
		//for (int y = 0; y < height; y++) {	
		//	//Xstride = byte;		
		//	RecursiveGaussFilter(buff + y * pitch, buff + y * pitch, width, byte);
		//}
		//for (int x = 0; x < width; x++) {
		//	//Ystride = pitch;
		//	RecursiveGaussFilter(buff + x * byte, buff + x * byte, height, pitch);
		//}

		////Lambda
		//auto Filter = [RecursiveGaussFilter, byte, pitch, c](BYTE* buff, int width, int height) {
		//	for (int y = 0; y < height; y++) {
		//		//Xstride = byte;		
		//		RecursiveGaussFilter(buff + y * pitch, buff + y * pitch, width, byte);
		//	}
		//	for (int x = 0; x < width; x++) {
		//		//Ystride = pitch;
		//		RecursiveGaussFilter(buff + x * byte, buff + x * byte, height, pitch);
		//	}
		//};

		//Filter(buff, width, height);

		//多线程
		int t_width = width / divide;
		int t_height = height / divide;

		auto Filter1D = [RecursiveGaussFilter, byte, pitch, c] (BYTE* buff, int it_size, int filter_size, int it_stride, int filter_stride) {
			for (int i = 0; i < it_size; i++) {
				RecursiveGaussFilter(buff + i * it_stride, buff + i * it_stride, filter_size, filter_stride);
			}
		};

		auto multithread = [Filter1D, divide, t_width, t_height, width, height, byte, pitch] (BYTE* buff, bool dir) {
			std::vector<std::thread> t_vec;
			int stride = dir ? pitch : byte;
			int o_stride = (!dir) ? pitch : byte;

			for (int i = 0; i < divide; i++) {
				//边缘处理
				int t_rsize = dir ? height : width;
				int t_risize = ((!dir) ? t_height : t_width);

				if (i == divide - 1) {
					t_risize = ((!dir) ? height : width) - i * t_risize;
				}

				int offset = i * ((!dir) ? t_height : t_width);
				//Filter1D(buff + offset * o_stride, t_risize, t_rsize, o_stride, stride);
				t_vec.emplace_back(std::thread(Filter1D, buff + offset * o_stride, t_risize, t_rsize, o_stride, stride));
			}

			for (auto& it : t_vec) {
				it.join();
			}
		};

		multithread(buff, Dir_X);
		multithread(buff, Dir_Y);

		rdPtr->img->UnlockBuffer(buff);

		//还原大小
		if (!(scale == 1.0)) {
			ResizedImg.Clone(*rdPtr->img);

			rdPtr->img->Delete();
			rdPtr->img->Create(owidth, oheight, proto);

			Stretch(&ResizedImg, rdPtr->img, Fast);
		}

		// Redraw object			
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport MultiThreadGaussBlur(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		//获取参数
		constexpr auto GB_MIN_RADIUS = 0;
		constexpr auto GB_MAX_RADIUS = 65535;

		int radius = CNC_GetIntParameter(rdPtr);
		radius = (std::min)(GB_MAX_RADIUS, (std::max)(GB_MIN_RADIUS, radius));

		float sigma = (float)(radius - 0.5) / 3;

		long p2 = CNC_GetFloatParameter(rdPtr);
		float scale = *(float*)&p2;
		GetValidScale(&scale);

		int divide = CNC_GetIntParameter(rdPtr);
		GetMaximumDivide(&divide);

		//计算参数
		float a = (float)(1 / (sigma * sqrt(2 * 3.1415926)));
		float* weight = (float*)malloc(sizeof(float) * (2 * radius + 1));

		auto GetGauss = [a, sigma] (int x) -> float {
			float index = (float)(-1.0 * ((double)x * (double)x) / (2.0 * sigma * sigma));
			float result = (float)(a * exp(index));
			return result;
		};

		auto GetGaussWeight1D = [GetGauss, radius, sigma] (float* weight) {
			float Normalization = 0;
			for (int i = 0; i < 2 * radius + 1; i++) {
				weight [i] = GetGauss(abs(i - radius));
				Normalization += weight [i];
			}
			for (int j = 0; j < 2 * radius + 1; j++) {
				weight [j] = weight [j] / Normalization;
			}
		};

		GetGaussWeight1D(weight);

		//Dimensions
		int owidth = rdPtr->img->GetWidth(), oheight = rdPtr->img->GetHeight();
		int width = (int)(owidth / scale);
		int height = (int)(oheight / scale);

		// 降采样
		cSurface ResizedImg;
		LPSURFACE proto = nullptr;
		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);

		if (!(scale == 1.0)) {
			ResizedImg.Clone(*rdPtr->img);

			rdPtr->img->Delete();
			rdPtr->img->Create(width, height, proto);

			Stretch(&ResizedImg, rdPtr->img, Fast);
		}

		//Lock buffer, get pitch etc.
		BYTE* buff = rdPtr->img->LockBuffer();
		if (!buff) return 0;

		int pitch = rdPtr->img->GetPitch();
		if (pitch < 0) {
			pitch *= -1;
			buff -= pitch * (height - 1);
		}
		int size = pitch * height;
		int byte = rdPtr->img->GetDepth() >> 3;

		auto Edge = [] (int Input, int Max)->int {
			if (Input < 0) {
				Input = 0 + (0 - Input);
			}
			else if (Input > Max) {
				Input = Max - (Input - Max);
			}

			return Input;
		};

		int t_width = width / divide;
		int t_height = height / divide;

		auto Gauss1DFilter = [Edge, radius, weight, byte, pitch, height, width] (BYTE* src, BYTE* des, int size, bool dir) {
			int stride = dir ? pitch : byte;
			int EdgeSize = dir ? height : width;

			for (int i = 0; i < size; i++) {
				RGBA Sum = { 0,0,0,0 };
				for (int j = -radius; j <= radius; j++) {
					int Pos = Edge(i + j, EdgeSize - 1);
					int offset = Pos * stride;
					BYTE* Pixel = src + offset;
					//This method will take transparent pixels as black/{0,0,0,0}, which will cause black edges around pic with alpha channel.
					//I will not fix this cause this method is **VERY** slow and useless in game.
					RGBA calcpixels = RGBA { (double)Pixel [2],(double)Pixel [1],(double)Pixel [0],0 }*weight [j + radius];
					Sum = Sum + calcpixels;
				}
				des [i * stride + 2] = (BYTE)Sum.r;
				des [i * stride + 1] = (BYTE)Sum.g;
				des [i * stride + 0] = (BYTE)Sum.b;
			}
		};

		auto Filter1D = [Gauss1DFilter, byte, pitch] (BYTE* src, BYTE* temp, int it_size, int filter_size, bool dir) {
			int stride = dir ? pitch : byte;
			int o_stride = (!dir) ? pitch : byte;

			for (int i = 0; i < it_size; i++) {
				Gauss1DFilter(src + i * o_stride, temp + i * o_stride, filter_size, dir);
			}
		};

		auto multithread = [Filter1D, divide, t_width, t_height, width, height, byte, pitch] (BYTE* buff, BYTE* temp, bool dir) {
			std::vector<std::thread> t_vec;
			int stride = dir ? pitch : byte;
			int o_stride = (!dir) ? pitch : byte;

			for (int i = 0; i < divide; i++) {
				//边缘处理
				int t_rsize = dir ? height : width;
				int t_risize = ((!dir) ? t_height : t_width);

				if (i == divide - 1) {
					t_risize = ((!dir) ? height : width) - i * t_risize;
				}

				int offset = i * ((!dir) ? t_height : t_width);
				t_vec.emplace_back(std::thread(Filter1D, buff + offset * o_stride, temp + offset * o_stride, t_risize, t_rsize, dir));
			}

			for (auto& it : t_vec) {
				it.join();
			}
		};

		BYTE* temp = (BYTE*)malloc(size);

		multithread(buff, temp, Dir_X);
		multithread(temp, buff, Dir_Y);

		free(temp);

		rdPtr->img->UnlockBuffer(buff);

		//还原大小
		if (!(scale == 1.0)) {
			ResizedImg.Clone(*rdPtr->img);

			rdPtr->img->Delete();
			rdPtr->img->Create(owidth, oheight, proto);

			Stretch(&ResizedImg, rdPtr->img, Fast);
		}

		//清理
		free(weight);

		// Redraw object			
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport MultiThreadStackBlur(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		//获取参数
		int radius = CNC_GetIntParameter(rdPtr);

		long p2 = CNC_GetFloatParameter(rdPtr);
		float scale = *(float*)&p2;

		int divide = CNC_GetIntParameter(rdPtr);

		StackBlur(rdPtr->img, radius, scale, divide);

		// Redraw object			
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport LoadFileList(LPRDATA rdPtr, long param1, long param2) {
	std::wstring FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->FileList->clear();
	GetFileList(rdPtr->FileList, FilePath);

	return 0;
}

short WINAPI DLLExport SaveBase64ImgToFile(LPRDATA rdPtr, long param1, long param2) {
	// read param
	std::wstring base64 = (LPCWSTR)CNC_GetStringParameter(rdPtr);
	std::wstring filePath = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	// decode
	Base64<std::wstring> base64Decoder;

	base64Decoder.base64_decode_callback(base64, [&] (const BYTE* buf, const size_t sz) {
		// Load to surface
		CInputMemFile MemFile;		
		MemFile.Create(const_cast<BYTE*>(buf), sz);

		//MGR
		CImageFilterMgr* pImgMgr = rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvImgFilterMgr;
		CImageFilter    pFilter(pImgMgr);

		cSurface* pSf = new cSurface;
		ImportImageFromInputFile(pImgMgr, &MemFile, pSf, 0, 0);

		if (!pSf->IsValid()) {
			return;
		}

		_SavetoFile(pSf, filePath.c_str(), rdPtr, false, rdPtr->DefaultFilterName);

		delete pSf;
	});

	return 0;
}

// https://docs.microsoft.com/zh-cn/windows/win32/gdi/font-installation-and-deletion
// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-addfontresourceexa
short WINAPI DLLExport EmbedFont(LPRDATA rdPtr, long param1, long param2) {
	std::wstring FilePath = GetFullPathNameStr((LPCTSTR)CNC_GetStringParameter(rdPtr));
	
	AddFontResourceEx(FilePath.c_str(), FR_PRIVATE, 0);

	return 0;
}

short WINAPI DLLExport GetVideoFrame(LPRDATA rdPtr, long param1, long param2) {
	std::wstring FilePath = GetFullPathNameStr((LPCTSTR)CNC_GetStringParameter(rdPtr));
	size_t ms= (size_t)CNC_GetIntParameter(rdPtr);

#ifdef _FFMPEG
	get_videoFrame(FilePath, ms, [rdPtr](const unsigned char* pData, const int width, const int height) {
		auto pMemSf = CreateSurface(24, width, height);

		auto sfCoef = GetSfCoef(pMemSf);
		if (sfCoef.pData == nullptr) {		
			return;
		}

		auto lineSz = sfCoef.pitch;
		auto alphaSz = sfCoef.sz / sfCoef.byte;

		for (int y = 0; y < height; y++) {
			auto pMemData = sfCoef.pData + y * sfCoef.pitch;
			auto pVideo = pData + (height - 1 - y) * sfCoef.pitch;

			memcpy(pMemData, pVideo, lineSz);
		}

		ReleaseSfCoef(pMemSf, sfCoef);

		auto pAlpha = new BYTE[alphaSz];
		memset(pAlpha, 255, alphaSz);

		pMemSf->SetAlpha(pAlpha, pMemSf->GetWidth());

		delete[] pAlpha;
		pAlpha = nullptr;

#ifdef _DEBUG
		_SavetoClipBoard(pMemSf, false);
#endif // _DEBUG

		delete rdPtr->pHwaSf_Video;
		rdPtr->pHwaSf_Video = nullptr;

		rdPtr->pHwaSf_Video = CreateHWASurface(rdPtr,32, width, height, ST_HWA_ROMTEXTURE);

		//pMemSf->DemultiplyAlpha();
		if (PreMulAlpha(rdPtr)) {
			pMemSf->PremultiplyAlpha();		// only needed in DX11 premultiplied mode
		}

		pMemSf->Blit(*rdPtr->pHwaSf_Video);

		delete pMemSf;

		return;
		});
#endif

	return 0;
}

short WINAPI DLLExport RefreshMonitorState(LPRDATA rdPtr, long param1, long param2) {
	RefreshMonitorState(rdPtr);

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

// -----------------
// Sample expression
// -----------------
// Add three values
// 
//long WINAPI DLLExport Expression(LPRDATA rdPtr,long param1)
//{
//
//	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
//	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
//	long p3 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
//
//	// Performs the wonderfull calculation
//	return p1+p2+p3;
//}
//
////Reverse the string passed in.
//long WINAPI DLLExport Expression2(LPRDATA rdPtr,long param1)
//{
//	char *temp;
//
//	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
//
//	//I'm storing the string pointer returned into a char *
//	temp = (LPSTR)p1;
//
//	//Reversing the string.
//	_strrev(temp);
//	
//	//Setting the HOF_STRING flag lets MMF know that you are a string.
//	rdPtr->rHo.hoFlags |= HOF_STRING;
//	
//	//This returns a pointer to the string for MMF.
//	return (long)temp;
//}
//
////Divide the float by 2.
//long WINAPI DLLExport Expression3(LPRDATA rdPtr,long param1)
//{
//	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
//
//	//Floats are tricky.  If you want to pass in a float, you must do the
//	//following to convert the long to a true float, but only when you use
//	//TYPE_FLOAT.
//	float fp1 = *(float *)&p1;
//
//	//Just doing simple math now.
//	fp1 /=2;
//
//	//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
//	rdPtr->rHo.hoFlags |= HOF_FLOAT;
//
//	//Return the float without conversion
//	return *((int*)&fp1);
//}

//返回指定进程名的Process ID
long WINAPI DLLExport GetProcessIDByName(LPRDATA rdPtr, long param1) {
	//输入参数
	long param = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	return GetProcessIDByName((LPCTSTR)param);

}

//返回鼠标锁定区域
long WINAPI DLLExport GetCurrentLockRect_L(LPRDATA rdPtr, long param1) {
	return  rdPtr->Lock ? rdPtr->CurrentLockRect.left : 0;
}
long WINAPI DLLExport GetCurrentLockRect_R(LPRDATA rdPtr, long param1) {
	return  rdPtr->Lock ? rdPtr->CurrentLockRect.right : 0;
}
long WINAPI DLLExport GetCurrentLockRect_T(LPRDATA rdPtr, long param1) {
	return  rdPtr->Lock ? rdPtr->CurrentLockRect.top : 0;
}
long WINAPI DLLExport GetCurrentLockRect_B(LPRDATA rdPtr, long param1) {
	return  rdPtr->Lock ? rdPtr->CurrentLockRect.bottom : 0;
}


//返回相对X偏移量
long WINAPI DLLExport ReturnXOffset(LPRDATA rdPtr, long param1) {
	int Type = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	return GetOffset(rdPtr, GT(Type)).x;
}

//返回相对于Y偏移量
long WINAPI DLLExport ReturnYOffset(LPRDATA rdPtr, long param1) {
	int Type = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	return GetOffset(rdPtr, GT(Type)).y;
}


//返回Frame X Scale
long WINAPI DLLExport ReturnXScale(LPRDATA rdPtr, long param1) {
	float fp1 = (float)GetFrameScale(rdPtr).x;
	//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	//Return the float without conversion
	return *((int*)&fp1);
}

//返回Frame Y Scale
long WINAPI DLLExport ReturnYScale(LPRDATA rdPtr, long param1) {
	float fp1 = (float)GetFrameScale(rdPtr).y;
	//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	//Return the float without conversion
	return *((int*)&fp1);
}

//返回当前窗口矩形区域
long WINAPI DLLExport GetRect_L(LPRDATA rdPtr, long param1) {
	int Type = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	return GetRect(rdPtr, GR(Type)).left;
}
long WINAPI DLLExport GetRect_R(LPRDATA rdPtr, long param1) {
	int Type = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	return GetRect(rdPtr, GR(Type)).right;
}
long WINAPI DLLExport GetRect_T(LPRDATA rdPtr, long param1) {
	int Type = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	return GetRect(rdPtr, GR(Type)).top;
}
long WINAPI DLLExport GetRect_B(LPRDATA rdPtr, long param1) {
	int Type = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	return GetRect(rdPtr, GR(Type)).bottom;
}

//返回当前锁定模式
long WINAPI DLLExport ReturnCurrentLockType(LPRDATA rdPtr, long param1) {
	return rdPtr->Lock ? rdPtr->LockType : -1;
}

//返回IME状态
long WINAPI DLLExport ReturnIMEState(LPRDATA rdPtr, long param1) {
	return ImmGetOpenStatus(ImmGetContext(rdPtr->MainWindowHandle));
}

//返回DPI Sacling
long WINAPI DLLExport ReturnDPIScaling(LPRDATA rdPtr, long param1) {
	return ReturnDPIScaling(rdPtr->AppScaled);
}

//返回全屏状态
long WINAPI DLLExport ReturnFullScreen(LPRDATA rdPtr, long param1) {
	return  IsZoomed(rdPtr->MainWindowHandle) ? TRUE : FALSE;
}

//获取当前场景名称
long WINAPI DLLExport GetFrameName(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->rhPtr->rhFrame->m_name;
}

//获取当前时间
long WINAPI DLLExport GetCurTime(LPRDATA rdPtr, long param1) {
	LPSYSTEMTIME lpSystemTime = new SYSTEMTIME;
	GetLocalTime(lpSystemTime);

	//Release Old
	if (rdPtr->CurrentTime != nullptr) {
		delete[] rdPtr->CurrentTime;
	}

	//2000-01-01 00:00:01
	//格式长19字符，留出1字符用作结束符号
	size_t Length = 19;
	rdPtr->CurrentTime = new WCHAR [Length + 1];
	rdPtr->CurrentTime [Length] = 0;

	swprintf(rdPtr->CurrentTime, Length + 1, _T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d"), (int)lpSystemTime->wYear, (int)lpSystemTime->wMonth, (int)lpSystemTime->wDay, (int)lpSystemTime->wHour, (int)lpSystemTime->wMinute, (int)lpSystemTime->wSecond);

	delete lpSystemTime;

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->CurrentTime;
}

//获取游玩时间
long WINAPI DLLExport GetPlayTime(LPRDATA rdPtr, long param1) {
	int TotalPlayFrame = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	//60FPS Per Sec
	TotalPlayFrame /= 60;

	int Hour = TotalPlayFrame / 3600;
	TotalPlayFrame %= 3600;

	int Minute = TotalPlayFrame / 60;
	TotalPlayFrame %= 60;

	int Second = TotalPlayFrame;

	//Release Old
	if (rdPtr->TotalPlayTime != nullptr) {
		delete[] rdPtr->TotalPlayTime;
	}

	//00:00:01
	//格式至少长8字符，留出1字符用作结束符号，至少9字符
	//99999999:59:59
	//格式最长14字符
	size_t Length = 14;
	rdPtr->TotalPlayTime = new WCHAR [Length + 1];
	memset(rdPtr->TotalPlayTime, 0, (Length + 1) * sizeof(WCHAR));

	swprintf(rdPtr->TotalPlayTime, Length + 1, _T("%.2d:%.2d:%.2d"), Hour, Minute, Second);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->TotalPlayTime;
}

long WINAPI DLLExport GetDurationTime(LPRDATA rdPtr, long param1) {
	int timeInMs = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	auto timeInSecond = timeInMs / 1000;

	int timeInHour = timeInSecond / 3600;
	timeInSecond %= 3600;

	int timeInMinute = timeInSecond / 60;
	timeInSecond %= 60;
	
	//Release Old
	if (rdPtr->TotalPlayTime != nullptr) {
		delete[] rdPtr->TotalPlayTime;
	}

	std::wstring time = L"";

	auto GetFormatedTime = [](int time) {
		return time!=0
			?std::format(L"{}:", time)
			:L"";
	};

	time += GetFormatedTime(timeInHour);
	time += GetFormatedTime(timeInMinute);
	time += GetFormatedTime(timeInSecond);

	size_t length = !time.empty() ? time.length() - 1 : 0;

	rdPtr->TotalPlayTime = new WCHAR[length + 1];
	memset(rdPtr->TotalPlayTime, 0, (length + 1) * sizeof(WCHAR));

	if (!time.empty()) {
		memcpy(rdPtr->TotalPlayTime, time.c_str(), length * sizeof(WCHAR));
	}

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->TotalPlayTime;
}

long WINAPI DLLExport GetFileListSize(LPRDATA rdPtr, long param1) {
	return rdPtr->FileList->size();
}

long WINAPI DLLExport GetFileListAt(LPRDATA rdPtr, long param1) {
	size_t Pos = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	Pos = (std::max)(0u, (std::min)(Pos, rdPtr->FileList->size() - 1));

	NewStr(rdPtr->FileListOutPut, rdPtr->FileList->at(Pos));

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->FileListOutPut;
}

long WINAPI DLLExport GetFileHash(LPRDATA rdPtr, long param1) {
	LPCWSTR FilePath = (LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	Encryption Hash;
	Hash.OpenFile(FilePath);

	LPCWSTR Temp = Hash.GetHash();
	if (Temp != nullptr) {
		NewStr(rdPtr->HashOutput, Hash.GetHash());
	}
	else {
		NewStr(rdPtr->HashOutput, Empty_Str);
	}

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->HashOutput;
}

//获取对象当前显示动画朝向
long WINAPI DLLExport GetObjectAnimationDirection(LPRDATA rdPtr, long param1) {
	const int Fixed = (int)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	return (long)DisplayAnimationDirection(rdPtr, Fixed);
}

long WINAPI DLLExport GetObjectAnimationID(LPRDATA rdPtr, long param1) {
	const int Fixed = (int)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	return (long)DisplayAnimationID(rdPtr, Fixed);
}

long WINAPI DLLExport GetObjectCurrentFrameCount(LPRDATA rdPtr, long param1) {
	const int fixed = (int)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	return (long)GetCurrentFrameCount(rdPtr, fixed);
}

long WINAPI DLLExport GetObjectAnimDirFrameCount(LPRDATA rdPtr, long param1) {
	const int fixed = (int)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	const int id = (int)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	const int dir = (int)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	return (long)GetAnimDirFrameCount(rdPtr, fixed, id, dir);
}

long WINAPI DLLExport GetValWithSign(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float Val = *(float*)&p1;

	delete[] rdPtr->ValWithSignOutput;
	rdPtr->ValWithSignOutput = nullptr;

	_ftos_signed_s(Val, &rdPtr->ValWithSignOutput);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->ValWithSignOutput;
}

long WINAPI DLLExport Hex2RGB(LPRDATA rdPtr, long param1) {
	LPCWSTR Hex = (LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	return DEC2RGB(_h2d(Hex));
}

long WINAPI DLLExport GetScopedObjectNumber(LPRDATA rdPtr, long param1) {
	int Fixed = (int)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	LPRO object = LproFromFixed(rdPtr, Fixed);

	LPOIL pObjectInfo = rdPtr->pSelect->GetLPOIL(object->roHo.hoOi);
	if (pObjectInfo == nullptr) {
		return 0;
	}

	auto next = object->roHo.hoNextSelected;

	if (next == -1) {		// unselected, return total
		return pObjectInfo->oilNObjects;
	}
	else {
		return pObjectInfo->oilNumOfSelected;
	}
}

long WINAPI DLLExport WCharToLong(LPRDATA rdPtr, long param1) {
	LPWSTR pStr = (LPWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	auto len = wcslen(pStr);

	if (len == 0) {
		return 0;
	}

	short high = ConvertToType<wchar_t, short>(pStr [0]);
	short low = ConvertToType<wchar_t, short>(pStr [1]);

	return (high << 16) + low;
}

long WINAPI DLLExport LongToWChar(LPRDATA rdPtr, long param1) {
	long num = (long)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	short low = num & 0b00000000000000001111111111111111;
	short high = (num - low) >> 16;

	auto sz = (2 + 1) * sizeof(wchar_t);

	//auto pStr = (wchar_t*)_mvCalloc((2 + 1) * sizeof(wchar_t));
	auto pStr = (wchar_t*)callRunTimeFunction(rdPtr, RFUNCTION_GETSTRINGSPACE_EX, 0, sz);
	memset(pStr, 0, sz);

	pStr [0] = ConvertToType<short, wchar_t>(high);
	pStr [1] = ConvertToType<short, wchar_t>(low);

	pStr [2] = '\0';

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)pStr;
}

long WINAPI DLLExport GetCommandLineByCLI(LPRDATA rdPtr, long param1) {
	auto commandLine = ConvertWStrToStr((LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING));
	auto param = ConvertWStrToStr((LPCWSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING));

	//commandLine = "\"C:\\Steam\\steamapps\\common\\Clickteam Fusion 2.5\\Data\\Runtime\\Unicode\\edrt.exe\" -f \"gundamAGE\" -v /J1";
	//commandLine = "\"C:\\Steam\\steamapps\\common\\Clickteam Fusion 2.5\\Data\\Runtime\\Unicode\\edrt.exe\" -f /J1";

	CLI::App app;

	app.allow_windows_style_options();

	std::string coef = "";
	app.add_option(param, coef);

	//CLI11_PARSE(app, commandLine, true);	
	try {
		app.parse(commandLine, true);
	}
	catch (const CLI::ParseError& e) {
		auto name = e.get_name();
		//return app.exit(e);
	}

	return ReturnString(ConvertStrToWStr(coef));
}

long WINAPI DLLExport CastToBool(LPRDATA rdPtr, long param1) {
	return bool(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_LONG));
}

long WINAPI DLLExport GetVideoFramePointer(LPRDATA rdPtr, long param1) {
#ifdef _FFMPEG
	return ConvertToLong(rdPtr->pHwaSf_Video);
#else
	return  ConvertToLong(nullptr);
#endif
}

long WINAPI DLLExport GetDirection(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);
	float Val = *(float*)&p1;

	return Val >= 0 ? 1 : -1;
}

long WINAPI DLLExport GetCurMonitorWidth(LPRDATA rdPtr, long param1) {
	return rdPtr->curMonitorWidth;
}

long WINAPI DLLExport GetCurMonitorHeight(LPRDATA rdPtr, long param1) {
	return rdPtr->curMonitorHeight;
}

long WINAPI DLLExport GetFullFileName(LPRDATA rdPtr, long param1) {
	LPCWSTR pFilePath = (LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(pFilePath, &wfd);

	auto GetPathName = [](LPCWSTR pFilePath) {
		auto fullFileName = GetFullPathNameStr(pFilePath);
		auto slashPos = fullFileName.find_last_of(L'\\');
		auto dotPos = fullFileName.find_last_of(L'.');

		auto path = fullFileName.substr(0, slashPos);

		if (slashPos == std::wstring::npos) {
			return std::make_tuple(path, std::wstring(L""), std::wstring(L""));
		}

		auto fileName = fullFileName.substr(slashPos + 1, dotPos - slashPos - 1);

		if (dotPos == std::wstring::npos) {
			return std::make_tuple(path, fileName, std::wstring(L""));
		}

		auto ext = fullFileName.substr(dotPos + 1);

		return std::make_tuple(path, fileName, ext);
	};

	do {
		if (INVALID_HANDLE_VALUE != hFind) {
			NewStr(rdPtr->FileListOutPut, pFilePath);
		}
		else {
			const auto pInvalidRet = L"INVALID";

			auto [path, fileName, ext] = std::move(GetPathName(pFilePath));

			std::vector<std::wstring> fileList;
			GetFileList(&fileList, path);

			if (fileList.empty()) {
				NewStr(rdPtr->FileListOutPut, pInvalidRet);

				break;
			}

			auto it = std::find_if(fileList.begin(), fileList.end(), [&](const std::wstring& filePath) {
				auto [localPath, localFileName, localExt] = std::move(GetPathName(filePath.c_str()));
			
				if (StrIEqu(fileName.c_str(), localFileName.c_str())) {
					return true;
				}

				return false;
				});

			if (it == fileList.end()) {
				NewStr(rdPtr->FileListOutPut, pInvalidRet);

				break;
			}

			auto [localPath, localFileName, localExt] = std::move(GetPathName(it->c_str()));
			auto acutalFileName = std::wstring(pFilePath) + std::wstring(L".") + localExt;

			NewStr(rdPtr->FileListOutPut, acutalFileName);

			break;
		}
	} while (0);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->FileListOutPut;
}

long WINAPI DLLExport GetTotalPhysicalMemory(LPRDATA rdPtr, long param1) {
	return (long)GetSystemMemoryInfoMB(MemoryInfoType::TotalPhysicalMemory);
}

long WINAPI DLLExport GetFreePhysicalMemory(LPRDATA rdPtr, long param1) {
	return (long)GetSystemMemoryInfoMB(MemoryInfoType::FreePhysicalMemory);
}

// ----------------------------------------------------------
// Condition / Action / Expression jump table
// ----------------------------------------------------------
// Contains the address inside the extension of the different
// routines that handle the action, conditions and expressions.
// Located at the end of the source for convinience
// Must finish with a 0
//
long (WINAPI* ConditionJumps[])(LPRDATA rdPtr, long param1, long param2) =
{
	IsNameAProcess,
	IsMouseLocked,
	ReturnKeyState,
	IsFullScreen,
	IsClipBoardAvailable,
	IsObjectHasAnimationInDirection,

	IsAnotherInstanceRunning,

	IsWindowForcused,

	IsCommandLineHasParam,

	IsObjectDestroyed,

	IsObjectHasAnimationInDirection_Scope,

	IsRunningInEditor,

	IsActiveAtTop,

	OnMonitorChange,

	IsObjectHasAnimationID,
	IsObjectHasAnimationID_Scope,

	OnResizingComplete,
	ResizingMainlyChangedX,

	OnClickBackword,
	OnClickForward,

	0
};

short (WINAPI* ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
{
	//运行
	RunApplication,
	Run16BitApplication,

	//停止
	StopApplicationByName,
	StopApplicationByPID,

	//锁定/解锁鼠标
	LockMouse,
	LockMouseByRect,
	UnlockMouse,

	//设定鼠标位置
	SetMousePos,

	//更新设定
	KeepLock_SetON,
	KeepLock_SetOFF,
	UpdateLock_SetON,
	UpdateLock_SetOFF,
	RectOffset_SetON,
	RectOffset_SetOFF,

	//输入法控制
	IME_Disable,
	IME_Enable,

	//窗口控制
	GoFullScreen,
	GoWindowed,

	//截取场景区域到剪贴板
	BitBltFrameArea,
	LoadFromClipBoard,
	LoadFromFile,

	RecursiveGaussBlur,
	MultiThreadGaussBlur,
	LoadFromBackDrop,
	MultiThreadStackBlur,
	SaveToFile,
	SaveToClipBoard,
	SaveToTemp,
	LoadFromTemp,

	StretchQuality_Fast,
	StretchQuality_High,

	MultiThreadSave_ON,
	MultiThreadSave_OFF,

	SetDefaultFilterName,

	LoadFileList,
	SaveBase64ImgToFile,

	SetSize,

	EmbedFont,

	SetAlwaysOnTop,

	GetVideoFrame,

	RefreshMonitorState,

	//结尾必定是零
	0
};

long (WINAPI* ExpressionJumps[])(LPRDATA rdPtr, long param) =
{
	GetProcessIDByName,

	GetCurrentLockRect_L,
	GetCurrentLockRect_R,
	GetCurrentLockRect_T,
	GetCurrentLockRect_B,

	ReturnXOffset,
	ReturnYOffset,

	ReturnXScale,
	ReturnYScale,

	GetRect_L,
	GetRect_R,
	GetRect_T,
	GetRect_B,

	ReturnCurrentLockType,

	ReturnIMEState,

	ReturnDPIScaling,

	ReturnFullScreen,

	GetFrameName,

	GetCurTime,
	GetPlayTime,

	GetFileListSize,
	GetFileListAt,

	GetFileHash,

	GetObjectAnimationDirection,

	GetValWithSign,

	Hex2RGB,

	GetScopedObjectNumber,

	WCharToLong,
	LongToWChar,

	GetCommandLineByCLI,

	CastToBool,

	GetVideoFramePointer,

	GetDirection,

	GetCurMonitorWidth,
	GetCurMonitorHeight,

	GetFullFileName,

	GetDurationTime,
	
	GetTotalPhysicalMemory,
	GetFreePhysicalMemory,

	GetObjectAnimationID,

	GetObjectCurrentFrameCount,
	GetObjectAnimDirFrameCount,

	0
};