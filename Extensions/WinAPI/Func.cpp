// Common Include
#include	"common.h"

//全局窗口句柄
HWND CurrentWindowHandle = NULL;

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

//初始化矩形为整个显示器大小
RECT InitRect(){
	return RECT{ 0,0, GetSystemMetrics(SM_CXSCREEN) , GetSystemMetrics(SM_CYSCREEN) };
}

//锁定鼠标
//基于窗口
void LockMouse(LPRDATA rdPtr, LT Type) {
	//当前窗口矩形
	RECT CurrentWindowRect;
	::GetWindowRect(rdPtr->MainWindowHandle, &CurrentWindowRect);

	//获取锁定矩形
	RECT ClipRect = InitRect();

	switch (Type) {
		case LOCK_CURRENTWINDOW: {
			::GetWindowRect(rdPtr->MainWindowHandle, &ClipRect);
			break;
		}
		case LOCK_CLIENTAREA: {
			GetCurrentClientRectToScreen(rdPtr->MainWindowHandle, &ClipRect);
			break;
		}
		case LOCK_FRAMEAREA: {
			::GetWindowRect(rdPtr->FrameWindowHandle, &ClipRect);
			break;
		}
		//case:LOCK_WINDOWNAME{
		//	::GetWindowRect(FindWindow(NULL, (LPTSTR)param1), &ClipRect);
		//}
		default: {

		}
	}

	rdPtr->Lock = true;
	rdPtr->LockType = Type;	
	rdPtr->RectOffset = ClipRect - CurrentWindowRect;
	
	rdPtr->CurrentLockRect = ClipRect;
	::ClipCursor(&(rdPtr->CurrentLockRect));

	return;
}
//基于矩形
void LockMouse(LPRDATA rdPtr, RECT Rect, RT Type) {
	//当前窗口矩形
	RECT MainWindowRect;
	::GetWindowRect(rdPtr->MainWindowHandle, &MainWindowRect);

	//获取锁定矩形
	RECT ClipRect = InitRect();	

	DPOINT Scale = GetFrameScale(rdPtr);

	switch (Type) {
		case RELATIVE_SCREEN: {
			ClipRect = Rect;
			break;
		}
		case RELATIVE_CURRENTWINDOW: {
			RECT CurrentWindowRect;
			::GetWindowRect(rdPtr->MainWindowHandle, &CurrentWindowRect);
			ClipRect = { Rect.left+ CurrentWindowRect.left,Rect.top+ CurrentWindowRect.top ,Rect.right+ CurrentWindowRect.left,Rect.bottom + CurrentWindowRect.top };
			break;
		}
		case RELATIVE_CLIENTAREA: {
			RECT CurrentClinetRect;
			GetCurrentClientRectToScreen(rdPtr->MainWindowHandle, &CurrentClinetRect);
			ClipRect = { Rect.left + CurrentClinetRect.left,Rect.top + CurrentClinetRect.top ,Rect.right + CurrentClinetRect.left,Rect.bottom + CurrentClinetRect.top };
			break;
		}
		case RELATIVE_FRAMEAREA: {
			RECT CurrentFrameRect;
			::GetWindowRect(rdPtr->FrameWindowHandle, &CurrentFrameRect);
			ClipRect = { (LONG)(Rect.left * Scale.x) + CurrentFrameRect.left,(LONG)(Rect.top * Scale.y) + CurrentFrameRect.top ,(LONG)(Rect.right * Scale.x) + CurrentFrameRect.left,(LONG)(Rect.bottom * Scale.y) + CurrentFrameRect.top };
			rdPtr->FrameScale = Scale;
			break;
		}
		default: {

		}
	}

	rdPtr->Lock = true;
	rdPtr->LockType = LOCK_BYRECT;
	rdPtr->RectOffset_Type = Type;
	rdPtr->RectOffset = ClipRect - MainWindowRect;

	rdPtr->CurrentLockRect = ClipRect;
	::ClipCursor(&(rdPtr->CurrentLockRect));

	return;
}

//释放鼠标
void UnlockMouse(LPRDATA rdPtr) {
	::ClipCursor(NULL);
	rdPtr->Lock = false;
}

//获取当前客户区域相对于屏幕的矩形区域
BOOL GetCurrentClientRectToScreen(HWND hWnd, LPRECT lpRect)
{
	BOOL RetBool = 1;
	
	RECT CurrentWindowRect;
	POINT Point = { 0,0 };

	RetBool = RetBool && ::GetClientRect(hWnd, &CurrentWindowRect);
	RetBool = RetBool && ::ClientToScreen(hWnd,&Point);
	*lpRect = CurrentWindowRect + RECT{ Point.x , Point.y , Point.x , Point.y };

	return RetBool;
}

//获取当前场景区域相对于屏幕的矩形区域
BOOL GetCurrentFrameRectToScreen(HWND hWnd, LPRECT lpRect,POINT WindowSize)
{
	BOOL RetBool = 1;

	RECT CurrentClientRect;
	POINT Offset = { 0,0 };
	
	RetBool = RetBool && GetCurrentClientRectToScreen(hWnd, &CurrentClientRect);
	Offset = (POINT{ CurrentClientRect.right - CurrentClientRect.left, CurrentClientRect.bottom - CurrentClientRect.top }- WindowSize)/2;

	*lpRect = CurrentClientRect + RECT{ Offset.x , Offset.y , -Offset.x , -Offset.y };

	return RetBool;
}

//返回场景窗口缩放比例
DPOINT GetFrameScale(LPRDATA rdPtr) {
	//获取当前场景显示区域大小
	RECT CurrentFrameRect;
	::GetWindowRect(rdPtr->FrameWindowHandle, &CurrentFrameRect);
	POINT Cur = { CurrentFrameRect.right - CurrentFrameRect.left,CurrentFrameRect.bottom - CurrentFrameRect.top };

	//获取缩放前显示的最小区域大小
	POINT Min = { (std::min)(rdPtr->AppW,rdPtr->FrameW),(std::min)(rdPtr->AppH,rdPtr->FrameH) };

	return DPOINT{ Cur.x / (double)Min.x,Cur.y / (double)Min.y };
}

//指定鼠标坐标
void SetMousePosition(LPRDATA rdPtr, int x, int y, ST Type) {

	//最终指定的坐标
	POINT Coordinate = {0,0};

	//如果类型不合法，则返回当前坐标
	::GetCursorPos(&Coordinate);

	DPOINT Scale = GetFrameScale(rdPtr);

	switch (Type) {
		case SET_SCREEN: {
			Coordinate = { x,y };
			break;
		}
		case SET_CURRENTWINDOW: {
			RECT CurrentWindowRect;
			::GetWindowRect(rdPtr->MainWindowHandle, &CurrentWindowRect);
			Coordinate = POINT{ x,y } + POINT{ CurrentWindowRect.left, CurrentWindowRect.top };
			break;
		}
		case SET_CLIENTAREA: {
			RECT CurrentClinetRect;
			GetCurrentClientRectToScreen(rdPtr->MainWindowHandle, &CurrentClinetRect);
			Coordinate = POINT{ x,y } + POINT{ CurrentClinetRect.left, CurrentClinetRect.top };
			break;
		}
		case SET_FRAMEAREA: {
			RECT CurrentFrameRect;
			::GetWindowRect(rdPtr->FrameWindowHandle, &CurrentFrameRect);			
			Coordinate = POINT{ (LONG)(x * Scale.x),(LONG)(y * Scale.y) } + POINT{ CurrentFrameRect.left, CurrentFrameRect.top };
			break;
		}
		default: {
			
		}
	}
	
	::SetCursorPos(Coordinate.x, Coordinate.y);

	return;
}

//返回偏差值
POINT GetOffset(LPRDATA rdPtr, GT Type) {
	
	POINT Offset = { 0,0 };
	RECT Src = { 0,0,0,0 }, Des = { 0,0,0,0 }, Res = { 0,0,0,0 };

	switch (Type) {
		case GET_CLIENTTOCURRENTWINDOW: {
			::GetWindowRect(rdPtr->MainWindowHandle, &Des);
			GetCurrentClientRectToScreen(rdPtr->MainWindowHandle, &Src);
			break;
		}
		case GET_FRAMETOCLIENTAREA: {			
			GetCurrentClientRectToScreen(rdPtr->MainWindowHandle, &Des);
			::GetWindowRect(rdPtr->FrameWindowHandle, &Src);
			break;
		}
		case GET_FRAMEAREATOCURRENTWINDOW: {
			::GetWindowRect(rdPtr->MainWindowHandle, &Des);
			::GetWindowRect(rdPtr->FrameWindowHandle, &Src);
			break;
		}
		default: {
			
		}
	}

	Res = Src - Des;
	Offset = { Res.left,Res.top };

	return Offset;
}

//返回矩形区域
RECT GetRect(LPRDATA rdPtr,GR Type) {
	//Windows在处理DPI缩放时，是将整个显示器的分辨率进行缩放后再处理
	//即一个50*50的窗口无论是否进行缩放，获取到的大小均是50*50
	RECT CurrentRect = InitRect();

	switch (Type) {
		case GET_CURRENTWINDOW: {
			::GetWindowRect(rdPtr->MainWindowHandle, &CurrentRect);		
			break;
		}
		case GET_CLIENTAREA: {
			GetCurrentClientRectToScreen(rdPtr->MainWindowHandle, &CurrentRect);		
			break;
		}
		case GET_FRAMEAREA: {
			::GetWindowRect(rdPtr->FrameWindowHandle, &CurrentRect);
			break;
		}
		default: {

		}
	}

	return CurrentRect;
}

//返回DPI缩放
int ReturnDPIScaling(bool bAppScaled) {
	// No scaling
	if (bAppScaled) { return 100; }

	// Get desktop dc
	const HDC desktopDc = GetDC(nullptr);

	// Get native resolution
	const auto caps = GetDeviceCaps(desktopDc, LOGPIXELSX);
	const auto scale = static_cast<int>(100 * (caps / 96.0));

	ReleaseDC(nullptr, desktopDc);

	return scale;
}

//返回注册表字符串值
char* ReturnRegValue(HKEY hkey,LPWSTR lpSubKey, LPWSTR lpValue) {
	DWORD reg_type = REG_SZ;
	char* reg_value = new char[MAX_PATH];
	DWORD res_size = 0;

	RegGetValue(
		hkey,
		lpSubKey,
		lpValue,
		reg_type,
		NULL,
		(LPBYTE)reg_value,
		&res_size
	);

	/*int result = atoi(reg_value);
	delete[] reg_value;
	return result;*/
	return reg_value;
}

//输入法状态
bool IMEState;

//输入法控制
BOOL IMEStateControl(HWND hWnd, bool State)
{
	IMEState = State;
	return ImmSetOpenStatus(ImmGetContext(hWnd), State);
}

//Surface
void ReDisplay(LPRDATA rdPtr) {
	//callRunTimeFunction(rdPtr, RFUNCTION_REDRAW, 0, 0);
	rdPtr->rc.rcChanged = true;
	rdPtr->rHo.hoImgWidth = rdPtr->img->GetWidth();
	rdPtr->rHo.hoImgHeight = rdPtr->img->GetHeight();
	return;
}

//CustomStretch
void Stretch(bool YReverse, LPBYTE Src, uint SW, uint SH, uint SBW, LPBYTE Des, uint DW, uint DH, uint DBW) {
	float ScaleX = (float)(1.0 * SW / DW);
	float ScaleY = (float)(1.0 * SH / DH);

	int SP = SBW * SW;
	int DP = DBW * DW;

	ulong* SrcX_Table = (ulong*)malloc(sizeof(ulong) * DW);
	for (ulong x = 0; x < DW; ++x) {
		SrcX_Table[x] = (ulong)(x * ScaleX);
	}
	
	LPBYTE pDstLine = Des;

	for (ulong y = 0; y < DH; ++y) {
		ulong srcy = (ulong)(y * ScaleY);
		LPBYTE pSrcLine = Src + (YReverse ? ((SH - 1 - srcy) * SP) : (srcy * SP));
		for (ulong x = 0; x < DW; ++x) {
			for (uint i = 0; i < DBW; i++) {
				pDstLine[x * DBW + i] = pSrcLine[SrcX_Table[x] * SBW + i];
			}
		}
		pDstLine += DP;
	}

	free(SrcX_Table);

	return;

	//auto StretchCore = [=](LPBYTE Src, LPBYTE Des, uint DH, int div) {
	//	LPBYTE pDstLine = Des;
	//	for (ulong y = 0; y < DH; ++y) {
	//		ulong srcy = (ulong)(y * ScaleY);
	//		LPBYTE pSrcLine = Src + (YReverse ? ((SH - 1 - DH * div - srcy) * SP) : (srcy * SP));
	//		for (ulong x = 0; x < DW; ++x) {
	//			for (uint i = 0; i < DBW; i++) {
	//				pDstLine[x * DBW + i] = pSrcLine[SrcX_Table[x] * SBW + i];
	//			}
	//		}
	//		pDstLine += DP;
	//	}
	//};

	//auto StretchMT = [=](LPBYTE Src, LPBYTE Des, uint divide) {
	//	std::vector<std::thread> t_vec;

	//	int t_sheight = SH / divide;
	//	int t_dheight = DH / divide;

	//	for (int i = 0; i < divide; i++) {
	//		//边缘处理
	//		int t_risize = t_dheight;

	//		if (i == divide - 1) {
	//			t_risize = DH - i * t_risize;
	//		}

	//		int soffset = i * t_sheight;
	//		int doffset = i * t_dheight;

	//		//StretchCore(Src + soffset * SP, Des + doffset * DP, t_risize, i);
	//		t_vec.emplace_back(std::thread(StretchCore, Src + soffset * SP, Des + doffset * DP, t_risize, i));
	//	}

	//	for (auto& it : t_vec) {
	//		it.join();
	//	}
	//};

	////StretchMT(Src, Des, std::thread::hardware_concurrency());
	//StretchMT(Src, Des, 2);

	//free(SrcX_Table);	

	//return;
}

//StretchBlt Stretch Surface
BOOL Stretch(LPSURFACE Src, LPSURFACE Des) {
	BOOL res = 0;

	if (Src->HasAlpha()) {
		res = Src->Stretch(*Des, 0, 0, Des->GetWidth(), Des->GetHeight(), BMODE_OPAQUE, BOP_COPY, 0, STRF_RESAMPLE | STRF_COPYALPHA);
		return res;
	}

	if (Src->GetType() != ST_MEMORYWITHDC) {
		LPSURFACE proto = nullptr;
		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);
		cSurface src;
		src.Create(Src->GetWidth(), Src->GetHeight(), proto);
		Src->Blit(src);

		HDC sdc = src.GetDC();
		HDC ddc = Des->GetDC();

		SetStretchBltMode(ddc, COLORONCOLOR);
		SetBrushOrgEx(ddc, 0, 0, NULL);

		res = StretchBlt(ddc, 0, 0, Des->GetWidth(), Des->GetHeight(), sdc, 0, 0, Src->GetWidth(), Src->GetHeight(), SRCCOPY);

		src.ReleaseDC(sdc);
		Des->ReleaseDC(ddc);
	}
	else {
		HDC sdc = Src->GetDC();
		HDC ddc = Des->GetDC();

		SetStretchBltMode(ddc, COLORONCOLOR);
		SetBrushOrgEx(ddc, 0, 0, NULL);

		res = StretchBlt(ddc, 0, 0, Des->GetWidth(), Des->GetHeight(), sdc, 0, 0, Src->GetWidth(), Src->GetHeight(), SRCCOPY);

		Src->ReleaseDC(sdc);
		Des->ReleaseDC(ddc);
	}
	return res;
}

//Blt To Surface
void BltToSurface(HDC Src, int SH, int SW, LPSURFACE Des) {
	HDC temp = Des->GetDC();

	SetStretchBltMode(temp, COLORONCOLOR);
	SetBrushOrgEx(temp, 0, 0, NULL);

	StretchBlt(temp, 0, 0, Des->GetWidth(), Des->GetHeight(), Src, 0, 0, SH, SW, SRCCOPY);

	Des->ReleaseDC(temp);

	return;
}

//Save to Clipboard
void SavetoClipBoard(LPSURFACE Src, LPRDATA rdPtr, bool release) {
	if (!rdPtr->MultiThreadSave) {
		_SavetoClipBoard(Src, false, rdPtr->MainWindowHandle);
	}
	else {
		std::thread t(_SavetoClipBoard, Src, release && true, rdPtr->MainWindowHandle);
		t.detach();		
	}

	return;
}

//Save to File
void SavetoFile(LPSURFACE Src, LPCWSTR FilePath, LPRDATA rdPtr, bool release) {
	if (!rdPtr->MultiThreadSave) {
		_SavetoFile(Src, FilePath, rdPtr, false, rdPtr->DefaultFilterName);
	}
	else {
		std::thread t(_SavetoFile, Src, FilePath, rdPtr, release && true, rdPtr->DefaultFilterName);
		t.detach();
	}

	return;
}

//Refresh Monitor State
void RefreshMonitorState(LPRDATA rdPtr) {
	auto newMonitorHandle = MonitorFromWindow(rdPtr->MainWindowHandle, MONITOR_DEFAULTTONEAREST);

	if (newMonitorHandle != NULL
		&& rdPtr->curMonitorHandle != newMonitorHandle) {
		rdPtr->curMonitorHandle = newMonitorHandle;

		MONITORINFO info = { 0 };
		info.cbSize = sizeof(MONITORINFO);
		auto ret = GetMonitorInfo(rdPtr->curMonitorHandle, &info);

		if (ret != 0) {
			rdPtr->curMonitorWidth = info.rcMonitor.right - info.rcMonitor.left;
			rdPtr->curMonitorHeight = info.rcMonitor.bottom - info.rcMonitor.top;

			CallEvent(ONMONITORCHANGE);
		}
	}
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
	HANDLE snapshot;
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
	CloseHandle(snapshot);

	return;
}