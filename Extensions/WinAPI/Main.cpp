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
		IDMN_CONDITION_INAP, M_CONDITION_INAP, CND_CONDITION_INAP, EVFLAGS_ALWAYS|EVFLAGS_NOTABLE, 1, PARAM_EXPSTRING,PARA_CONDITION_INAPM,
		IDMN_CONDITION_IML, M_CONDITION_IML, CND_CONDITION_IML, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 0,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_RUN,M_ACTION_RUN,ACT_ACTION_RUN,0, 2, PARAM_FILENAME2, PARAM_EXPSTRING,PARA_ACTION_RUN_1,PARA_ACTION_RUN_2,
		IDMN_ACTION_RUN_16,M_ACTION_RUN_16,ACT_ACTION_RUN_16,0, 2, PARAM_FILENAME2, PARAM_EXPSTRING,PARA_ACTION_RUN_1,PARA_ACTION_RUN_2,
		IDMN_ACTION_STOPBYNAME,M_ACTION_STOPBYNAME,ACT_ACTION_STOPBYNAME,0, 1, PARAM_EXPSTRING, PARA_ACTION_STOPBYNAME,
		IDMN_ACTION_STOPBYID,M_ACTION_STOPBYID,ACT_ACTION_STOPBYID,0, 1, PARAM_EXPRESSION, PARA_ACTION_STOPBYID,

		IDMN_ACTION_LOCKMOUSE,M_ACTION_LOCKMOUSE,ACT_ACTION_LOCKMOUSE,0, 0,
		IDMN_ACTION_LOCKMOUSEBWN,M_ACTION_LOCKMOUSEBWN,ACT_ACTION_LOCKMOUSEBWN,0, 1, PARAM_EXPSTRING, PARA_ACTION_LOCKMOUSEBWN,
		IDMN_ACTION_LOCKMOUSEBR,M_ACTION_LOCKMOUSEBR,ACT_ACTION_LOCKMOUSEBR,0, 4, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, PARA_ACTION_LOCKMOUSEBR_L, PARA_ACTION_LOCKMOUSEBR_R, PARA_ACTION_LOCKMOUSEBR_T, PARA_ACTION_LOCKMOUSEBR_B,
		IDMN_ACTION_UNLOCKMOUSE,M_ACTION_UNLOCKMOUSE,ACT_ACTION_UNLOCKMOUSE,0, 0,
		
		IDMN_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_ON,M_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_ON,ACT_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_ON,0, 0,
		IDMN_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_OFF,M_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_OFF,ACT_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_OFF,0, 0,
		IDMN_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_ON,M_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_ON,ACT_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_ON,0, 0,
		IDMN_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_OFF,M_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_OFF,ACT_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_OFF,0, 0,
		IDMN_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_ON,M_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_ON,ACT_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_ON,0, 0,
		IDMN_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_OFF,M_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_OFF,ACT_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_OFF,0, 0,

		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GPIDBN, M_EXPRESSION_GPIDBN, EXP_EXPRESSION_GPIDBN, 0, 1, EXPPARAM_STRING, PARA_EXPRESSION_GPIDBN,

		IDMN_EXPRESSION_GCLR_L, M_EXPRESSION_GCLR_L, EXP_EXPRESSION_GCLR_L, 0, 0,
		IDMN_EXPRESSION_GCLR_R, M_EXPRESSION_GCLR_R, EXP_EXPRESSION_GCLR_R, 0, 0,
		IDMN_EXPRESSION_GCLR_T, M_EXPRESSION_GCLR_T, EXP_EXPRESSION_GCLR_T, 0, 0,
		IDMN_EXPRESSION_GCLR_B, M_EXPRESSION_GCLR_B, EXP_EXPRESSION_GCLR_B, 0, 0,

		IDMN_EXPRESSION_GCWR_L, M_EXPRESSION_GCWR_L, EXP_EXPRESSION_GCWR_L, 0, 0,
		IDMN_EXPRESSION_GCWR_R, M_EXPRESSION_GCWR_R, EXP_EXPRESSION_GCWR_R, 0, 0,
		IDMN_EXPRESSION_GCWR_T, M_EXPRESSION_GCWR_T, EXP_EXPRESSION_GCWR_T, 0, 0,
		IDMN_EXPRESSION_GCWR_B, M_EXPRESSION_GCWR_B, EXP_EXPRESSION_GCWR_B, 0, 0,

		IDMN_EXPRESSION_GCLT, M_EXPRESSION_GCLT, EXP_EXPRESSION_GCLT, 0, 0,

		//IDMN_EXPRESSION, M_EXPRESSION, EXP_EXPRESSION, 0, 3, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, 0, 0, 0,
		
		//Note in the following.  If you are returning a string, you set the EXPFLAG_STRING.	
		//IDMN_EXPRESSION2, M_EXPRESSION2, EXP_EXPRESSION2, EXPFLAG_STRING, 1, EXPPARAM_STRING, 0,
		
		//Note in the following.  If you are returning a float, you set the EXPFLAG_DOUBLE
		//IDMN_EXPRESSION3, M_EXPRESSION3, EXP_EXPRESSION3, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG, 0,
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
	return Lock ? TRUE : FALSE;	
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

short WINAPI DLLExport LockMouse(LPRDATA rdPtr, long param1, long param2) {	
	if (Lock) {
		return 0;
	}
	
	//获取当前窗口矩形	
	::GetWindowRect(ReturnCurrentWindowHandle(), &CurrentLockRect);
	::ClipCursor(&CurrentLockRect);
	
	rdPtr->LockType = LOCK_CURRENTWINDOW;
	Lock = true;

	return 0;

}

short WINAPI DLLExport LockMouseByWindowName(LPRDATA rdPtr, long param1, long param2) {
	if (Lock) {
		return 0;
	}
		
	::GetWindowRect(FindWindow(NULL, (LPTSTR)param1), &CurrentLockRect);
	::ClipCursor(&CurrentLockRect);

	rdPtr->LockType = LOCK_BYWINDOWNAME;
	Lock = true;

	return 0;
}

short WINAPI DLLExport LockMouseByRect(LPRDATA rdPtr, long param1, long param2) {
	if (Lock) {
		return 0;
	}

	CurrentLockRect.left = CNC_GetParameter(rdPtr);
	CurrentLockRect.right = CNC_GetParameter(rdPtr);
	CurrentLockRect.top = CNC_GetParameter(rdPtr);
	CurrentLockRect.bottom = CNC_GetParameter(rdPtr);

	// 若设定了相对锁定，则更新锁定
	if (rdPtr->RectOffset) {
		RECT CurrentWindowRect;
		::GetWindowRect(ReturnCurrentWindowHandle(), &CurrentWindowRect);
		RectOffset = CurrentLockRect - CurrentWindowRect;
	}

	::ClipCursor(&CurrentLockRect);
	
	rdPtr->LockType = LOCK_BYRECT;
	Lock = true;

	return 0;
}

short WINAPI DLLExport UnlockMouse(LPRDATA rdPtr, long param1, long param2) {
	UnlockLockedMouse();	
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
	rdPtr->RectOffset = true;
	return 0;
}
short WINAPI DLLExport RectOffset_SetOFF(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->RectOffset = false;
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
	return Lock ? CurrentLockRect.left : 0;
}
long WINAPI DLLExport GetCurrentLockRect_R(LPRDATA rdPtr, long param1) {
	return Lock ? CurrentLockRect.right : 0;
}
long WINAPI DLLExport GetCurrentLockRect_T(LPRDATA rdPtr, long param1) {
	return Lock ? CurrentLockRect.top : 0;
}
long WINAPI DLLExport GetCurrentLockRect_B(LPRDATA rdPtr, long param1) {
	return Lock ? CurrentLockRect.bottom : 0;
}

//返回当前窗口矩形区域
long WINAPI DLLExport GetCurrentWindowRect_L(LPRDATA rdPtr, long param1) {
	RECT CurrentWindowRect;
	::GetWindowRect(ReturnCurrentWindowHandle(), &CurrentWindowRect);
	return CurrentWindowRect.left;
}
long WINAPI DLLExport GetCurrentWindowRect_R(LPRDATA rdPtr, long param1) {
	RECT CurrentWindowRect;
	::GetWindowRect(ReturnCurrentWindowHandle(), &CurrentWindowRect);
	return CurrentWindowRect.right;	
}
long WINAPI DLLExport GetCurrentWindowRect_T(LPRDATA rdPtr, long param1) {
	RECT CurrentWindowRect;
	::GetWindowRect(ReturnCurrentWindowHandle(), &CurrentWindowRect);
	return CurrentWindowRect.top;	
}
long WINAPI DLLExport GetCurrentWindowRect_B(LPRDATA rdPtr, long param1) {
	RECT CurrentWindowRect;
	::GetWindowRect(ReturnCurrentWindowHandle(), &CurrentWindowRect);
	return CurrentWindowRect.bottom;	
}

//返回当前锁定模式
long WINAPI DLLExport ReturnCurrentLockType(LPRDATA rdPtr, long param1) {
	return rdPtr->LockType;
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
			LockMouseByWindowName,
			LockMouseByRect,
			UnlockMouse,
			//更新设定
			KeepLock_SetON,
			KeepLock_SetOFF,
			UpdateLock_SetON,
			UpdateLock_SetOFF,
			RectOffset_SetON,
			RectOffset_SetOFF,
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

			GetCurrentWindowRect_L,
			GetCurrentWindowRect_R,
			GetCurrentWindowRect_T,
			GetCurrentWindowRect_B,

			ReturnCurrentLockType,
			0
			};