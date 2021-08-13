#ifndef _GLOBALFUNC_
#define _GLOBALFUNC_

//全局窗口句柄
extern HWND CurrentWindowHandle;

//窗口锁定类别
typedef enum _LOCKTYPE{
	LOCK_CURRENTWINDOW,
	LOCK_CLIENTAREA,
	LOCK_FRAMEAREA,	
	LOCK_BYRECT,
}LT;

//矩形锁定时的相对位置
typedef enum _RELATIVETYPE {
	RELATIVE_SCREEN,
	RELATIVE_CURRENTWINDOW,
	RELATIVE_CLIENTAREA,
	RELATIVE_FRAMEAREA,	
}RT;

//指定鼠标坐标类别
typedef enum _SETMOUSETYPE {
	SET_SCREEN,
	SET_CURRENTWINDOW,
	SET_CLIENTAREA,
	SET_FRAMEAREA,
}ST;

//获取偏差值类别
typedef enum _GETOFFSETTYPE {
	GET_CLIENTTOCURRENTWINDOW,
	GET_FRAMETOCLIENTAREA,
	GET_FRAMEAREATOCURRENTWINDOW,
}GT;

//获取矩形区域类别
typedef enum _GETRECT {
	GET_CURRENTWINDOW,
	GET_CLIENTAREA,
	GET_FRAMEAREA,	
}GR;

//RECT运算符重载 +
RECT operator+(RECT A, RECT B);

//RECT运算符重载 +=
RECT operator +=(RECT A, RECT B);

//RECT运算符重载 -
RECT operator-(RECT A, RECT B);

//RECT运算符重载 -=
RECT operator -=(RECT A, RECT B);

//POINT运算符重载 +
POINT operator +(POINT A, POINT B);

//POINT运算符重载 +=
POINT operator +=(POINT A, POINT B);

//POINT运算符重载 -
POINT operator -(POINT A, POINT B);

//POINT运算符重载 -=
POINT operator -=(POINT A, POINT B);

//POINT运算符重载 *
POINT operator *(POINT A, LONG B);

//POINT运算符重载 /
POINT operator /(POINT A, LONG B);


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

typedef struct DOUBLEPOINT
{
	DOUBLE  x;
	DOUBLE  y;
} DPOINT;

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