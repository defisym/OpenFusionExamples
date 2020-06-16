#ifndef _GLOBALFUNC_
#define _GLOBALFUNC_

//全局窗口句柄
extern HWND CurrentWindowHandle;

//窗口锁定情况
extern bool Lock;

//窗口锁定类别
enum {
	LOCK_CURRENTWINDOW,
	LOCK_FRAMEAREA,
	LOCK_BYWINDOWNAME,
	LOCK_BYRECT,
};

//当前鼠标锁定的矩形区域
extern RECT CurrentLockRect;

//当前鼠标锁定的矩形区域相对于窗口的偏移
extern RECT RectOffset;

//RECT运算符重载 +
RECT operator+(RECT A, RECT B);

//RECT运算符重载 -
RECT operator-(RECT A, RECT B);

//枚举窗体回调
BOOL CALLBACK WINAPIEXT_EnumWindowsProc(
	HWND hwnd,      // handle to parent window
	LPARAM lParam   // application-defined value
);

void UnlockLockedMouse();
HWND ReturnCurrentWindowHandle();

//返回注册表字符串值
int ReturnRegValue(HKEY hkey, LPCSTR lpSubKey, LPCSTR lpValue);

//返回窗体菜单栏高度
int ReturnMenuHeight();

//返回窗体标题栏高度
int ReturnCaptionHeight();

//输入法状态
extern bool IMEState;

//输入法控制
BOOL IMEStateControl(bool State);

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