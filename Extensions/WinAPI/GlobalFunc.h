#ifndef _GLOBALFUNC_
#define _GLOBALFUNC_

//don't use this func if Str = nullptr, return Default_Str directly
inline void NewStr(LPTSTR& Tar, LPCTSTR Str) {
	release_arr(Tar);
	rsize_t total_length = wcslen(Str) + 1;

	Tar = new WCHAR[total_length];
	wcscpy_s(Tar, total_length, Str);
}

inline void NewStr(LPTSTR& Tar, const std::wstring& Str) {
	NewStr(Tar, Str.c_str());
}

//全局窗口句柄
extern HWND CurrentWindowHandle;

//枚举窗体回调
BOOL CALLBACK WINAPIEXT_EnumWindowsProc(
	HWND hwnd,      // handle to parent window
	LPARAM lParam   // application-defined value
);

//返回当前窗口句柄
HWND ReturnCurrentWindowHandle();

//初始化矩形为整个显示器大小
RECT InitRect();

//锁定鼠标
//基于窗口
void LockMouse(LPRDATA rdPtr, LT Type);
//基于矩形
void LockMouse(LPRDATA rdPtr, RECT Rect, RT Type);

//释放鼠标
void UnlockMouse(LPRDATA rdPtr);

//获取当前客户区域相对于屏幕的矩形区域
BOOL GetCurrentClientRectToScreen(HWND hWnd,LPRECT lprect);

//获取当前场景区域相对于屏幕的矩形区域
BOOL GetCurrentFrameRectToScreen(HWND hWnd, LPRECT lpRect, POINT offset);

//返回场景窗口缩放比例
DPOINT GetFrameScale(LPRDATA rdPtr);

//指定鼠标坐标
void SetMousePosition(LPRDATA rdPtr, int x, int y, ST Type);

//返回偏差值
POINT GetOffset(LPRDATA rdPtr, GT Type);

//返回矩形区域
RECT GetRect(LPRDATA rdPtr, GR Type);

//返回DPI缩放
int ReturnDPIScaling(bool AppScaled);

//返回注册表字符串值
char* ReturnRegValue(HKEY hkey, LPWSTR lpSubKey, LPWSTR lpValue);

//输入法状态
extern bool IMEState;

//输入法控制
BOOL IMEStateControl(HWND hWnd, bool State);

//Surface
void ReDisplay(LPRDATA rdPtr);

//CustomStretch
void Stretch(bool YReverse, LPBYTE Src, uint SW, uint SH, uint SBW, LPBYTE Des, uint DW, uint DH, uint DBW);

//StretchBlt Stretch Surface
BOOL Stretch(LPSURFACE Src, LPSURFACE Des);

//Blt To Surface
void BltToSurface(HDC Src, int SH, int SW, LPSURFACE Des);

//Mutex
extern std::mutex mtx;

//Save to Clipboard
void _SavetoClipBoard(LPSURFACE Src, LPRDATA rdPtr, bool release);
//Save to Clipboard
void SavetoClipBoard(LPSURFACE Src, LPRDATA rdPtr, bool release);
//Save to File
void _SavetoFile(LPSURFACE Src, LPCWSTR FilePath, LPRDATA rdPtr, bool release);
//Save to File
void SavetoFile(LPSURFACE Src, LPCWSTR FilePath, LPRDATA rdPtr, bool release);

//iterate file
void GetFileList(LPRDATA rdPtr, std::wstring& Src);

//所有创建线程的句柄
extern std::deque <LPTSTR> RunApplicationName;

//进程管理
void StopAllApplication();
void DeleteRunApplicationName(LPCTSTR ApplicationName);
void DeleteRunApplicationNameByPos(size_t pos);
void AddNewApplicationName(LPTSTR lpApplicationName);
void DeleteAllApplicationName();

DWORD GetProcessIDByName(LPCTSTR ApplicationName);

#endif // !_GLOBALFUNC_