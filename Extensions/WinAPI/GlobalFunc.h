#ifndef _GLOBALFUNC_
#define _GLOBALFUNC_

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
DWORD GetFilterIDByFileName(LPRDATA rdPtr, LPCTSTR FilePath);

void GetValidScale(float* scale);
void GetMaxmiumDivide(int* divide);

//GetAlphaBuff
LPBYTE GetBuff(LPSURFACE Src);
//StretchAlpha
BOOL Stretch(LPBYTE Src, int SW, int SH, LPBYTE Des, int DW, int DH);
//StretchSurfaceWithAlpha
BOOL Stretch(LPSURFACE Src, LPSURFACE Des, LPRDATA rdPtr);
//StretchSurface
BOOL Stretch(LPSURFACE Src, LPSURFACE Des);


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