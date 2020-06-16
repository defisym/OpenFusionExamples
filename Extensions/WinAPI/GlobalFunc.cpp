// Common Include
#include	"common.h"

//全局窗口句柄
HWND CurrentWindowHandle = NULL;

//窗口锁定情况
bool Lock = false;

//当前鼠标锁定的矩形区域
RECT CurrentLockRect;

//当前鼠标锁定的矩形区域相对于窗口的偏移
RECT RectOffset;

//RECT运算符重载 +
RECT operator +(RECT A, RECT B) {
	A.left += B.left;
	A.right += B.right;
	A.top += B.top;
	A.bottom += B.bottom;
	return A;
}

//RECT运算符重载 -
RECT operator -(RECT A, RECT B) {
	A.left -= B.left;
	A.right -= B.right;
	A.top -= B.top;
	A.bottom -= B.bottom;
	return A;
}

//枚举窗体回调
BOOL CALLBACK WINAPIEXT_EnumWindowsProc(
	HWND hwnd,      // handle to parent window
	LPARAM lParam   // application-defined value
) {
	DWORD PID;
	GetWindowThreadProcessId(hwnd, &PID);
	//为父窗口且与当前进程PID一致
	if ((PID == GetCurrentProcessId()) && (GetParent(hwnd) == NULL)) {
		//传递与目标PID相符的句柄至全局变量CurrentWindowHandle
		CurrentWindowHandle = hwnd;
		return FALSE;
	}
	return TRUE;
}

//返回当前窗口句柄
HWND ReturnCurrentWindowHandle() {

	CurrentWindowHandle = NULL;

	EnumWindows(
		WINAPIEXT_EnumWindowsProc,
		NULL
	);

	return CurrentWindowHandle;
}

//释放鼠标
void UnlockLockedMouse() {
	if (Lock) {
		::ClipCursor(NULL);
		Lock = false;
	}
}

//返回注册表字符串值
int ReturnRegValue(HKEY hkey,LPCSTR lpSubKey,LPCSTR lpValue) {
	DWORD reg_type = REG_SZ;
	char* reg_value = new char[MAX_PATH];
	DWORD res_size;

	RegGetValueA(
		hkey,
		lpSubKey,
		lpValue,
		reg_type,
		NULL,
		(LPBYTE)reg_value,
		&res_size
	);

	int result = atoi(reg_value);
	delete[] reg_value;
	return result;
}

//返回窗体菜单栏高度
int ReturnMenuHeight() {
	return GetSystemMetrics(SM_CYMENU);
	//return (int)ceil((-1) * (ReturnRegValue(HKEY_CURRENT_USER, "Control Panel\\Desktop\\WindowMetrics", "MenuHeight") / 15.0));
}

//返回窗体标题栏高度
int ReturnCaptionHeight() {
	return GetSystemMetrics(SM_CYCAPTION);
	//return (int)ceil((-1) * (ReturnRegValue(HKEY_CURRENT_USER, "Control Panel\\Desktop\\WindowMetrics", "CaptionHeight") / 15.0));
}

//输入法状态
bool IMEState = ImmGetOpenStatus(ImmGetContext(ReturnCurrentWindowHandle()));

//输入法控制
BOOL IMEStateControl(bool State)
{
	IMEState = State;
	return ImmSetOpenStatus(ImmGetContext(ReturnCurrentWindowHandle()), State);
}


//所有创建线程的进程名
std::deque <LPTSTR> RunApplicationName;

//开辟新的内存空间复制，并保存指向这片内存空间的指针
void AddNewApplicationName(LPTSTR lpApplicationName) {
	size_t total_length = _MAX_FNAME + _MAX_EXT;
	LPTSTR ANCL = new WCHAR[total_length];
	wcscpy_s(ANCL, total_length, PathFindFileName(lpApplicationName));
	RunApplicationName.push_back(ANCL);
}

//清理全部空间
void DeleteAllApplicationName() {
	for (auto t = RunApplicationName.begin(); t != RunApplicationName.end(); t++) {
		delete[](*t);
	}
	RunApplicationName.clear();
}

//通过位置清理清理空间
void DeleteRunApplicationNameByPos(size_t pos) {
	delete[] RunApplicationName[pos];

	auto t = RunApplicationName.begin() + pos;
	RunApplicationName.erase(t);	
}

//通过进程名清理清理空间
void DeleteRunApplicationName(LPCTSTR ApplicationName) {
	for (auto t = RunApplicationName.begin(); t != RunApplicationName.end();) {
		//依照是否删除元素更新迭代器
		if (wcscmp(ApplicationName, *t) == 0) {
			delete[](*t);
			t = RunApplicationName.erase(t);
		}
		else {
			t++;
		}
	}
	return;
}

//通过句柄停止全部创建的线程
void StopAllApplication() {
	
	//如果没有运行程序则跳过这一阶段
	if (RunApplicationName.size() == 0) {
		return;
	}
		
	//获取快照
	HANDLE	snapshot;
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	//循环遍历
	PROCESSENTRY32* info;
	info = new PROCESSENTRY32;
	info->dwSize = sizeof(PROCESSENTRY32);

	Process32First(snapshot, info);
	while (Process32Next(snapshot, info) != FALSE) {
		for (size_t i = 0; i < RunApplicationName.size(); i++) {
			//进程名一致则结束进程		
			if (wcscmp(RunApplicationName[i], info->szExeFile) == 0) {
				//PROCESS_TERMINATE表示为结束操作打开,FALSE=可继承,info.th32ProcessID=进程ID    
				TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, info->th32ProcessID), 0);
			}
		}
	}
	
	DeleteAllApplicationName();	
	
	delete info;
	return;
}


DWORD GetProcessIDByName(LPCTSTR ApplicationName) {
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


