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
		//IDMN_CONDITION, M_CONDITION, CND_CONDITION, EVFLAGS_ALWAYS, 3, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_CND_P1, M_CND_P2, M_CND_P3,
		IDMN_CONDITION_INAP, M_CONDITION_INAP, CND_CONDITION_INAP, EVFLAGS_ALWAYS|EVFLAGS_NOTABLE, 1, PARAM_EXPSTRING,PARA_CONDITION_INAP,
		IDMN_CONDITION_IML, M_CONDITION_IML, CND_CONDITION_IML, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		IDMN_CONDITION_RKS, M_CONDITION_RKS, CND_CONDITION_RKS, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 1, PARAM_EXPRESSION,PARA_CONDITION_RKS,
		IDMN_CONDITION_IFS, M_CONDITION_IFS, CND_CONDITION_IFS, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		IDMN_CONDITION_ICA, M_CONDITION_ICA, CND_CONDITION_ICA, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		};

// Definitions of parameters for each action
short actionsInfos[]=
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
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
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
short WINAPI DLLExport RunApplication(LPRDATA rdPtr, long param1, long param2){

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
		
	LPTSTR Space = L" ";
	LPTSTR lpApplicationName = (LPTSTR)param1;
	LPTSTR lpCommandLine = (LPTSTR)param2;
	
	AddNewApplicationName(lpApplicationName);

	//预留结尾分隔符
	total_length = wcslen(lpApplicationName) + wcslen(Space) + wcslen(lpCommandLine) + 1;
	
	if (total_length > _MAX_PATH) {
		return 0;
	}

	LPTSTR ANCL = new WCHAR[total_length];
	
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
	LockMouse(rdPtr, RECT{ left ,top ,right ,bottom }, RT(Type));
	
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
	rdPtr->DefaultFilterName = GetFilterName(rdPtr, DefaultFilterName);

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
		rdPtr->img.Delete();
		rdPtr->img.Create(rdPtr->swidth, rdPtr->sheight, proto);

		BltToSurface(hdcWindow, FrameWidth, FrameHeight, &(rdPtr->img));

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
		SavetoFile(&(rdPtr->img), FilePath, rdPtr, false);
	}
	return 0;
}

short WINAPI DLLExport SaveToClipBoard(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		SavetoClipBoard(&(rdPtr->img),rdPtr, false);
	}
	return 0;
}

short WINAPI DLLExport SaveToTemp(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		rdPtr->temp.Delete();
		rdPtr->temp.Clone(rdPtr->img);
	}
	return 0;
}

short WINAPI DLLExport LoadFromTemp(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		rdPtr->img.Delete();
		rdPtr->img.Clone(rdPtr->temp);
	}
	return 0;
}

short WINAPI DLLExport LoadFromClipBoard(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		if (IsClipboardFormatAvailable(CF_DIB) && OpenClipboard(rdPtr->MainWindowHandle)) {

			HANDLE handle = GetClipboardData(CF_DIB);
			BITMAPINFO* bmp = (BITMAPINFO*)GlobalLock(handle);

			//Surface相关
			cSurface img;
			LPSURFACE proto = nullptr;
			CImageFilterMgr* pImgMgr = rdPtr->rhPtr->rh4.rh4Mv->mvImgFilterMgr;
			CImageFilter    pFilter(pImgMgr);

			//Surface获取位图信息
			GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);

			img.Delete();
			img.Create(bmp->bmiHeader.biWidth, bmp->bmiHeader.biHeight, proto);
			img.LoadImage(bmp, GetDIBBitmap(bmp));

			rdPtr->img.Delete();
			rdPtr->img.Create(rdPtr->swidth, rdPtr->sheight, proto);
						
			Stretch(&img, &rdPtr->img, rdPtr->StretchQuality);

			// Redraw object
			ReDisplay(rdPtr);
		}
	}

	return 0;
}

short WINAPI DLLExport LoadFromFile(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {

		LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);

		//Surface相关
		cSurface img;
		LPSURFACE proto = nullptr;
		CImageFilterMgr* pImgMgr = rdPtr->rhPtr->rh4.rh4Mv->mvImgFilterMgr;
		CImageFilter    pFilter(pImgMgr);

		//Surface获取位图信息	
		ImportImage(pImgMgr, FilePath, &img, 0, 0);
		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);

		rdPtr->img.Delete();
		rdPtr->img.Create(rdPtr->swidth, rdPtr->sheight, proto);

		Stretch(&img, &rdPtr->img, rdPtr->StretchQuality);

		// Redraw object
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport LoadFromBackDrop(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		//Dimensions
		int width = rdPtr->img.GetWidth(), height = rdPtr->img.GetHeight();
		int screenX = rdPtr->rHo.hoX - rdPtr->rhPtr->rhWindowX;
		int screenY = rdPtr->rHo.hoY - rdPtr->rhPtr->rhWindowY;

		LPSURFACE proto = nullptr;
		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);

		rdPtr->img.Delete();
		rdPtr->img.Create(width, height, proto);

		HDC sdc = GetDC(rdPtr->FrameWindowHandle);		
		HDC ddc = rdPtr->img.GetDC();		

		SetStretchBltMode(ddc, COLORONCOLOR);
		SetBrushOrgEx(ddc, 0, 0, NULL);

		StretchBlt(ddc, 0, 0, width, height, sdc, screenX, screenY, width, height, SRCCOPY);

		ReleaseDC(rdPtr->FrameWindowHandle,sdc);
		rdPtr->img.ReleaseDC(ddc);
	
		// Redraw object			
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport RecursiveGaussBlur(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->Display) {
		//获取参数
		constexpr auto MIN_SIGMA = 0;
		constexpr auto MAX_SIGMA = 1.615;
		
		long p1 = CNC_GetFloatParameter(rdPtr);
		double sigma = *(float*)&p1;
		sigma = min(MAX_SIGMA, max(MIN_SIGMA, sigma));

		long p2 = CNC_GetFloatParameter(rdPtr);
		float scale = *(float*)&p2;
		GetValidScale(&scale);

		int divide = CNC_GetIntParameter(rdPtr);
		GetMaxmiumDivide(&divide);

		auto GetCoef = [](double sigma) -> GCoef {
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

			c.b[0] = (1.57825 + (2.44413 * q1) + (1.4281 * q2) + (0.422205 * q3));
			c.b[1] = ((2.44413 * q1) + (2.85619 * q2) + (1.26661 * q3));
			c.b[2] = (-(1.4281 * q2) + (1.26661 * q3));
			c.b[3] = (0.422205 * q3);
			c.B = 1.0 - ((c.b[1] + c.b[2] + c.b[3]) / c.b[0]);

			c.sigma = sigma;
			c.N = 3;

			return c;
		};

		GCoef c = GetCoef(sigma);		
		
		//Dimensions
		int owidth = rdPtr->img.GetWidth(), oheight = rdPtr->img.GetHeight();
		int width = (int)(owidth / scale);
		int height = (int)(oheight / scale);
		
		// 降采样
		cSurface ResizedImg;
		LPSURFACE proto = nullptr;
		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);

		if (!(scale == 1.0)) {
			ResizedImg.Clone(rdPtr->img);

			rdPtr->img.Delete();
			rdPtr->img.Create(width, height, proto);

			Stretch(&ResizedImg, &rdPtr->img, Fast);
		}

		//Lock buffer, get pitch etc.
		BYTE* buff = rdPtr->img.LockBuffer();		
		if (!buff) return 0;

		int pitch = rdPtr->img.GetPitch();
		if (pitch < 0)
		{
			pitch *= -1;
			buff -= pitch * (height - 1);
		}
		int size = pitch * height;
		int byte = rdPtr->img.GetDepth() >> 3;

		auto RecursiveGaussFilter = [c](BYTE* src, BYTE* des, int size, int stride) {
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

		auto Filter1D = [RecursiveGaussFilter, byte, pitch, c](BYTE* buff, int it_size, int filter_size, int it_stride, int filter_stride) {
			for (int i = 0; i < it_size; i++) {
				RecursiveGaussFilter(buff + i * it_stride, buff + i * it_stride, filter_size, filter_stride);
			}
		};

		auto multithread = [Filter1D, divide, t_width, t_height, width, height, byte, pitch](BYTE* buff, bool dir) {
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

		rdPtr->img.UnlockBuffer(buff);

		//还原大小
		if (!(scale == 1.0)) {
			ResizedImg.Clone(rdPtr->img);

			rdPtr->img.Delete();
			rdPtr->img.Create(owidth, oheight, proto);

			Stretch(&ResizedImg, &rdPtr->img, Fast);
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
		radius = min(GB_MAX_RADIUS, max(GB_MIN_RADIUS, radius));

		float sigma = (float)(radius - 0.5) / 3;

		long p2 = CNC_GetFloatParameter(rdPtr);
		float scale = *(float*)&p2;
		GetValidScale(&scale);

		int divide = CNC_GetIntParameter(rdPtr);
		GetMaxmiumDivide(&divide);		

		//计算参数
		float a = (float)(1 / (sigma * sqrt(2 * 3.1415926)));
		float* weight = (float*)malloc(sizeof(float) * (2 * radius + 1));

		auto GetGauss = [a,sigma](int x) -> float {
			float index = (float)(-1.0 * ((double)x * (double)x) / (2.0 * sigma * sigma));
			float result = (float)(a * exp(index));
			return result;
		};
		
		auto GetGaussWeight1D = [GetGauss, radius, sigma](float* weight) {
			float Normalization = 0;
			for (int i = 0; i < 2 * radius + 1; i++) {
				weight[i] = GetGauss(abs(i - radius));
				Normalization += weight[i];
			}
			for (int j = 0; j < 2 * radius + 1; j++) {
				weight[j] = weight[j] / Normalization;
			}
		};

		GetGaussWeight1D(weight);

		//Dimensions
		int owidth = rdPtr->img.GetWidth(), oheight = rdPtr->img.GetHeight();
		int width = (int)(owidth / scale);
		int height = (int)(oheight / scale);

		// 降采样
		cSurface ResizedImg;
		LPSURFACE proto = nullptr;
		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);

		if (!(scale == 1.0)) {
			ResizedImg.Clone(rdPtr->img);

			rdPtr->img.Delete();
			rdPtr->img.Create(width, height, proto);

			Stretch(&ResizedImg, &rdPtr->img, Fast);
		}

		//Lock buffer, get pitch etc.
		BYTE* buff = rdPtr->img.LockBuffer();
		if (!buff) return 0;

		int pitch = rdPtr->img.GetPitch();
		if (pitch < 0)
		{
			pitch *= -1;
			buff -= pitch * (height - 1);
		}
		int size = pitch * height;
		int byte = rdPtr->img.GetDepth() >> 3;		
				
		auto Edge = [](int Input, int Max)->int {
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
				
		auto Gauss1DFilter = [Edge, radius, weight, byte, pitch, height, width](BYTE* src, BYTE* des, int size, bool dir) {
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
					RGBA calcpixels = RGBA{ (double)Pixel[2],(double)Pixel[1],(double)Pixel[0],0 }*weight[j + radius];
					Sum = Sum + calcpixels;
				}
				des[i * stride + 2] = (BYTE)Sum.r;
				des[i * stride + 1] = (BYTE)Sum.g;
				des[i * stride + 0] = (BYTE)Sum.b;
			}
		};

		auto Filter1D = [Gauss1DFilter, byte, pitch](BYTE* src, BYTE* temp, int it_size, int filter_size, bool dir) {
			int stride = dir ? pitch : byte;
			int o_stride = (!dir) ? pitch : byte;

			for (int i = 0; i < it_size; i++) {
				Gauss1DFilter(src + i * o_stride, temp + i * o_stride, filter_size, dir);
			}
		};
		
		auto multithread = [Filter1D, divide, t_width, t_height, width, height, byte, pitch](BYTE* buff, BYTE* temp, bool dir) {
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

		rdPtr->img.UnlockBuffer(buff);

		//还原大小
		if (!(scale == 1.0)) {
			ResizedImg.Clone(rdPtr->img);

			rdPtr->img.Delete();
			rdPtr->img.Create(owidth, oheight, proto);

			Stretch(&ResizedImg, &rdPtr->img, Fast);
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
		constexpr auto SB_MIN_RADIUS = 0;
		constexpr auto SB_MAX_RADIUS = 254;		
		
		int radius = CNC_GetIntParameter(rdPtr);		
		radius = min(SB_MAX_RADIUS, max(SB_MIN_RADIUS, radius));

		long p2 = CNC_GetFloatParameter(rdPtr);
		float scale = *(float*)&p2;
		GetValidScale(&scale);

		int divide = CNC_GetIntParameter(rdPtr);
		GetMaxmiumDivide(&divide);

		//Dimensions
		int owidth = rdPtr->img.GetWidth(), oheight = rdPtr->img.GetHeight();
		int width = (int)(owidth / scale);
		int height = (int)(oheight / scale);

		// 降采样
		cSurface ResizedImg;
		LPSURFACE proto = nullptr;
		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);
		
		if (!(scale == 1.0)) {
			ResizedImg.Clone(rdPtr->img);

			rdPtr->img.Delete();
			rdPtr->img.Create(width, height, proto);

			Stretch(&ResizedImg, &rdPtr->img, Fast);			
		}

		//Lock buffer, get pitch etc.
		BYTE* buff = rdPtr->img.LockBuffer();
		if (!buff) return 0;

		int pitch = rdPtr->img.GetPitch();
		if (pitch < 0)
		{
			pitch *= -1;
			buff -= pitch * (height - 1);
		}
		int size = pitch * height;
		int byte = rdPtr->img.GetDepth() >> 3;

		static unsigned short const stackblur_mul[255] =
		{
				512,512,456,512,328,456,335,512,405,328,271,456,388,335,292,512,
				454,405,364,328,298,271,496,456,420,388,360,335,312,292,273,512,
				482,454,428,405,383,364,345,328,312,298,284,271,259,496,475,456,
				437,420,404,388,374,360,347,335,323,312,302,292,282,273,265,512,
				497,482,468,454,441,428,417,405,394,383,373,364,354,345,337,328,
				320,312,305,298,291,284,278,271,265,259,507,496,485,475,465,456,
				446,437,428,420,412,404,396,388,381,374,367,360,354,347,341,335,
				329,323,318,312,307,302,297,292,287,282,278,273,269,265,261,512,
				505,497,489,482,475,468,461,454,447,441,435,428,422,417,411,405,
				399,394,389,383,378,373,368,364,359,354,350,345,341,337,332,328,
				324,320,316,312,309,305,301,298,294,291,287,284,281,278,274,271,
				268,265,262,259,257,507,501,496,491,485,480,475,470,465,460,456,
				451,446,442,437,433,428,424,420,416,412,408,404,400,396,392,388,
				385,381,377,374,370,367,363,360,357,354,350,347,344,341,338,335,
				332,329,326,323,320,318,315,312,310,307,304,302,299,297,294,292,
				289,287,285,282,280,278,275,273,271,269,267,265,263,261,259
		};

		static unsigned char const stackblur_shr[255] =
		{
				9, 11, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17,
				17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
				19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20,
				20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
				21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
				21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22,
				22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
				22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
				23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
				23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
				23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
				23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
				24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
				24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
				24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
				24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24
		};

		int t_width = width / divide;
		int t_height = height / divide;

		auto StackBlur1DFilter = [=](BYTE* src, BYTE* des, int size, bool dir) {
			int div = radius * 2 + 1;
			int sizem = size - 1;
			int stride = dir ? pitch : byte;		

			int src_offset = 0;
			int des_offset = 0;

			int mul_sum = stackblur_mul[radius];
			char shr_sum = stackblur_shr[radius];

			RGBA Sum = { 0,0,0,0 };
			RGBA Sum_In = { 0,0,0,0 };
			RGBA Sum_Out = { 0,0,0,0 };
			RGBA* stack = (RGBA*)malloc(sizeof(RGBA) * (div));

			for (int i = 0; i <= radius; i++) {
				int stack_offset = i;
				RGBA src_pixel = { (double)src[2],(double)src[1],(double)src[0],0 };
				stack[stack_offset]=src_pixel;
				Sum = Sum + src_pixel * (i + 1);
				Sum_Out = Sum_Out + src_pixel;
			}

			src_offset = 0;

			for (int i = 1; i <= radius; i++) {				
				if (i <= sizem) {
					src_offset += stride;
				}
				int stack_offset = i + radius;
				RGBA src_pixel = { (double)src[src_offset + 2],(double)src[src_offset + 1],(double)src[src_offset + 0],0 };
				stack[stack_offset] = src_pixel;
				Sum = Sum + src_pixel * (radius + 1 - i);
				Sum_In = Sum_In + src_pixel;
			}
			
			int sp = radius;
			int xp = radius;

			if (xp > sizem) {
				xp = sizem;
			}

			src_offset = xp * stride;
			des_offset = 0;

			for (int i = 0; i < size; i++) {
				RGBA des_pixel = (Sum * mul_sum) >> shr_sum;
				des[des_offset + 2] = (BYTE)des_pixel.r;
				des[des_offset + 1] = (BYTE)des_pixel.g;
				des[des_offset + 0] = (BYTE)des_pixel.b;
				des_offset += stride;

				Sum = Sum - Sum_Out;

				int stack_offset = sp + div - radius;
				if (stack_offset >= div) {
					stack_offset -= div;
				}
				Sum_Out = Sum_Out - stack[stack_offset];

				if (xp < sizem) {
					src_offset += stride;
					xp++;
				}

				RGBA src_pixel = { (double)src[src_offset + 2],(double)src[src_offset + 1],(double)src[src_offset + 0],0 };
				stack[stack_offset] = src_pixel;

				Sum_In = Sum_In + src_pixel;
				Sum = Sum + Sum_In;

				sp++;

				if (sp >= div) {
					sp = 0;
				}

				stack_offset = sp;

				Sum_Out = Sum_Out + stack[stack_offset];
				Sum_In = Sum_In - stack[stack_offset];
			}

			free(stack);
		};

		auto Filter1D = [StackBlur1DFilter, byte, pitch](BYTE* src, int it_size, int filter_size, bool dir) {
			int stride = dir ? pitch : byte;
			int o_stride = (!dir) ? pitch : byte;

			for (int i = 0; i < it_size; i++) {
				StackBlur1DFilter(src + i * o_stride, src + i * o_stride, filter_size, dir);
			}
		};

		auto multithread = [Filter1D, divide, t_width, t_height, width, height, byte, pitch](BYTE* buff, bool dir) {
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
				//Filter1D(buff + offset * o_stride, t_risize, t_rsize, dir);
				t_vec.emplace_back(std::thread(Filter1D, buff + offset * o_stride, t_risize, t_rsize, dir));
			}

			for (auto& it : t_vec) {
				it.join();
			}
		};

		multithread(buff, Dir_X);
		multithread(buff, Dir_Y);

		rdPtr->img.UnlockBuffer(buff);

		//还原大小
		if (!(scale == 1.0)) {
			ResizedImg.Clone(rdPtr->img);

			rdPtr->img.Delete();
			rdPtr->img.Create(owidth, oheight, proto);

			Stretch(&ResizedImg, &rdPtr->img, Fast);			
		}

		// Redraw object			
		ReDisplay(rdPtr);
	}

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
long WINAPI DLLExport GetProcessIDByName(LPRDATA rdPtr, long param1){
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
	return rdPtr->Lock?rdPtr->LockType:-1;
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
			IsNameAProcess,
			IsMouseLocked,
			ReturnKeyState,
			IsFullScreen,
			IsClipBoardAvailable,
			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
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

			//结尾必定是零
			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
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

			0
			};