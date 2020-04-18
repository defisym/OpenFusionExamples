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
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_RUN,M_ACTION_RUN,ACT_ACTION_RUN,0, 2, PARAM_FILENAME2, PARAM_EXPSTRING,PARA_ACTION_RUN_1,PARA_ACTION_RUN_2,
		IDMN_ACTION_RUN_16,M_ACTION_RUN_16,ACT_ACTION_RUN_16,0, 2, PARAM_FILENAME2, PARAM_EXPSTRING,PARA_ACTION_RUN_1,PARA_ACTION_RUN_2,
		IDMN_ACTION_STOPBYNAME,M_ACTION_STOPBYNAME,ACT_ACTION_STOPBYNAME,0, 1, PARAM_EXPSTRING, PARA_ACTION_STOPBYNAME,
		IDMN_ACTION_STOPBYID,M_ACTION_STOPBYID,ACT_ACTION_STOPBYID,0, 1, PARAM_EXPRESSION, PARA_ACTION_STOPBYID,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GPIDBN, M_EXPRESSION_GPIDBN, EXP_EXPRESSION_GPIDBN, 0, 1, EXPPARAM_STRING, PARA_EXPRESSION_GPIDBN,
		
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
	//输入参数
	LPCTSTR ApplicationName = (LPCTSTR)param1;

	//获取快照
	HANDLE	snapshot;
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	//循环遍历
	PROCESSENTRY32* info;
	info = new PROCESSENTRY32;
	info->dwSize = sizeof(PROCESSENTRY32);

	Process32First(snapshot, info);
	while (Process32Next(snapshot, info) != FALSE) {
		//进程名一致则结束进程		
		if (wcscmp(ApplicationName, info->szExeFile) == 0) {			
			delete info;
			return TRUE;
		}
	}
	
	delete info;
	return FALSE;
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
	LPCTSTR ApplicationName = (LPCTSTR)param1;

	//获取快照
	HANDLE	snapshot;
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	//循环遍历
	PROCESSENTRY32* info;
	info = new PROCESSENTRY32;
	info->dwSize = sizeof(PROCESSENTRY32);
	
	Process32First(snapshot, info);
	while (Process32Next(snapshot, info) != FALSE) {	
		//进程名一致则结束进程		
		if (wcscmp(ApplicationName, info->szExeFile) == 0) {
			//PROCESS_TERMINATE表示为结束操作打开,FALSE=可继承,info.th32ProcessID=进程ID    
			TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, info->th32ProcessID), 0);
		}
	}

	DeleteRunApplicationName(ApplicationName);

	delete info;
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
long WINAPI DLLExport Expression(LPRDATA rdPtr,long param1)
{

	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	long p3 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	// Performs the wonderfull calculation
	return p1+p2+p3;
}

//Reverse the string passed in.
long WINAPI DLLExport Expression2(LPRDATA rdPtr,long param1)
{
	char *temp;

	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	//I'm storing the string pointer returned into a char *
	temp = (LPSTR)p1;

	//Reversing the string.
	_strrev(temp);
	
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;
	
	//This returns a pointer to the string for MMF.
	return (long)temp;
}

//Divide the float by 2.
long WINAPI DLLExport Expression3(LPRDATA rdPtr,long param1)
{
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT);

	//Floats are tricky.  If you want to pass in a float, you must do the
	//following to convert the long to a true float, but only when you use
	//TYPE_FLOAT.
	float fp1 = *(float *)&p1;

	//Just doing simple math now.
	fp1 /=2;

	//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
	rdPtr->rHo.hoFlags |= HOF_FLOAT;

	//Return the float without conversion
	return *((int*)&fp1);
}

//返回指定进程名的Process ID
long WINAPI DLLExport GetProcessIDByName(LPRDATA rdPtr, long param1){
	long param = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	//输入参数
	LPCTSTR ApplicationName = (LPCTSTR)param;
	//返回参数
	DWORD	ProcessID = 0;

	//获取快照
	HANDLE	snapshot;
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	//循环遍历
	PROCESSENTRY32* info;
	info = new PROCESSENTRY32;
	info->dwSize = sizeof(PROCESSENTRY32);

	Process32First(snapshot, info);
	while (Process32Next(snapshot, info) != FALSE) {
		//进程名一致则结束进程		
		if (wcscmp(ApplicationName, info->szExeFile) == 0) {
			ProcessID = info->th32ProcessID;
			break;
		}
	}
	
	delete info;
	return ProcessID;
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
			//结尾必定是零
			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			GetProcessIDByName,
			0
			};