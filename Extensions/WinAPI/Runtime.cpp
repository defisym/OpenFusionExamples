﻿// ============================================================================
//
// This file contains routines that are handled during the Runtime.
//
// Including creating, display, and handling your object.
// 
// ============================================================================

// Common Include
#include	"common.h"

// DEBUGGER /////////////////////////////////////////////////////////////////

#if !defined(RUN_ONLY)
// Identifiers of items displayed in the debugger
enum
{
// Example
// -------
//	DB_CURRENTSTRING,
//	DB_CURRENTVALUE,
//	DB_CURRENTCHECK,
//	DB_CURRENTCOMBO
};

// Items displayed in the debugger
WORD DebugTree[]=
{
// Example
// -------
//	DB_CURRENTSTRING|DB_EDITABLE,
//	DB_CURRENTVALUE|DB_EDITABLE,
//	DB_CURRENTCHECK,
//	DB_CURRENTCOMBO,

	// End of table (required)
	DB_END
};

#endif // !defined(RUN_ONLY)


// --------------------
// GetRunObjectDataSize
// --------------------
// Returns the size of the runtime datazone of the object
// 
ushort WINAPI DLLExport GetRunObjectDataSize(fprh rhPtr, LPEDATA edPtr)
{
	return(sizeof(RUNDATA));
}


// ---------------
// CreateRunObject
// ---------------
// The routine where the object is actually created
// 
short WINAPI DLLExport CreateRunObject(LPRDATA rdPtr, LPEDATA edPtr, fpcob cobPtr)
{
	/*
	   This routine runs when your object is created, as you might have guessed.
	   It is here that you must transfer any data you need in rdPtr from edPtr,
	   because after this has finished you cannot access it again!
	   Also, if you have anything to initialise (e.g. dynamic arrays, surface objects)
	   you should do it here, and free your resources in DestroyRunObject.
	*/

	//rhPtr
	rdPtr->rhPtr = rdPtr->rHo.hoAdRunHeader;

	//坐标
	rdPtr->rHo.hoX = cobPtr->cobX;
	rdPtr->rHo.hoY = cobPtr->cobY;	
	
	//you must set the fields rdPtr->rHo.hoImgWidth, rdPtr->rHo.hoImgHeight, rdPtr->rHo.hoImgXSpotand rdPtr->rHo.hoImgYSpot of your object to obtain a correct display.

	//热点
	rdPtr->rHo.hoImgXSpot = 0;
	rdPtr->rHo.hoImgYSpot = 0;
	
	//大小
	rdPtr->swidth = edPtr->swidth;
	rdPtr->sheight = edPtr->sheight;

	rdPtr->rHo.hoImgWidth = edPtr->swidth;
	rdPtr->rHo.hoImgHeight = edPtr->sheight;	
	
	//Display
	rdPtr->Display = edPtr->Display;

	if (rdPtr->Display) {
		//Proto
		LPSURFACE proto = nullptr;
		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);
		
		////Surface指针使用流程
		//LPSURFACE DIS;
		//DIS = NewSurface();
		//DIS->Create(rdPtr->swidth, rdPtr->sheight, proto);
		//DeleteSurface(DIS);
		
		//Surface初始化
		rdPtr->img = new cSurface;
		rdPtr->temp = new cSurface;

		rdPtr->img->Create(rdPtr->swidth, rdPtr->sheight, proto);
		rdPtr->img->Fill(BLACK);
	}
	
	//主窗口句柄	
	rdPtr->MainWindowHandle = rdPtr->rhPtr->rhHMainWin;
	//场景区域窗口句柄
	rdPtr->FrameWindowHandle = rdPtr->rhPtr->rhHEditWin;

	//APP分辨率
	rdPtr->AppW = rdPtr->rhPtr->rhApp->m_hdr.gaCxWin;
	rdPtr->AppH = rdPtr->rhPtr->rhApp->m_hdr.gaCyWin;	

	//场景大小
	rdPtr->FrameW = rdPtr->rhPtr->rhFrame->m_hdr.leWidth;
	rdPtr->FrameH = rdPtr->rhPtr->rhFrame->m_hdr.leHeight;
	
	//锁定模式
	rdPtr->KeepLock = edPtr->KeepLock;
	rdPtr->UpdateLock = edPtr->UpdateLock;
	rdPtr->RectOffset_State = edPtr->RectOffset_State;
	
	//锁定区域
	rdPtr->CurrentLockRect = InitRect();

	IMEState = ImmGetOpenStatus(ImmGetContext(rdPtr->MainWindowHandle));
	rdPtr->KeepIMEState = edPtr->KeepIMEState; 

	rdPtr->AppScaled = !IsProcessDPIAware();
	
	//初始化FilterID
	rdPtr->DefaultFilterName = nullptr;

	rdPtr->FileList = new std::vector<std::wstring>;

	rdPtr->pSelect = new ObjectSelection(rdPtr->rHo.hoAdRunHeader);

	rdPtr->pHwaSf_Video = nullptr;

	rdPtr->bSecondFrame = false;
	rdPtr->curMonitorHandle = nullptr;
	rdPtr->curMonitorWidth = 0;
	rdPtr->curMonitorHeight = 0;

	// No errors
	return 0;
}


// ----------------
// DestroyRunObject
// ----------------
// Destroys the run-time object
// 
short WINAPI DLLExport DestroyRunObject(LPRDATA rdPtr, long fast)
{
/*
   When your object is destroyed (either with a Destroy action or at the end of
   the frame) this routine is called. You must free any resources you have allocated!
*/
	//停止创建的进程
	StopAllApplication();
	
	//释放鼠标
	UnlockMouse(rdPtr);

	//Release Surface
	delete rdPtr->img;
	delete rdPtr->temp;

	//释放时间字符串
	delete[] rdPtr->CurrentTime;
	delete[] rdPtr->TotalPlayTime;

	delete rdPtr->FileList;
	delete[] rdPtr->FileListOutPut;

	delete[] rdPtr->HashOutput;

	delete[] rdPtr->ValWithSignOutput;

	delete rdPtr->pSelect;

	delete rdPtr->pHwaSf_Video;
	rdPtr->pHwaSf_Video = nullptr;

	// No errors
	return 0;
}


// ----------------
// HandleRunObject
// ----------------
// Called (if you want) each loop, this routine makes the object live
// 
short WINAPI DLLExport HandleRunObject(LPRDATA rdPtr)
{	
/*
   If your extension will draw to the MMF window you should first 
   check if anything about its display has changed :

       if (rdPtr->roc.rcChanged)
          return REFLAG_DISPLAY;
       else
          return 0;

   You will also need to make sure you change this flag yourself 
   to 1 whenever you want to redraw your object
 
   If your extension won't draw to the window, but it still needs 
   to do something every MMF loop use :

        return 0;

   If you don't need to do something every loop, use :

        return REFLAG_ONESHOT;

   This doesn't mean this function can never run again. If you want MMF
   to handle your object again (causing this code to run) use this function:

        callRun-timeFunction(rdPtr, RFUNCTION_REHANDLE, 0, 0);

   At the end of the loop this code will run
*/
	// 若设定了保持锁定，并已启用锁定，且当前窗口为活动窗口，则继续锁定窗口
	//if (rdPtr->Lock && rdPtr->KeepLock && (GetForegroundWindow() == rdPtr->MainWindowHandle)) {
	if (rdPtr->Lock && rdPtr->KeepLock && (GetActiveWindow() == rdPtr->MainWindowHandle)) {
		// 若设定了更新锁定，则更新锁定
		if (rdPtr->UpdateLock) {
			switch (rdPtr->LockType) {
				case LOCK_CURRENTWINDOW: {
					LockMouse(rdPtr, LOCK_CURRENTWINDOW);
					break;
				}
				case LOCK_CLIENTAREA: {
					LockMouse(rdPtr, LOCK_CLIENTAREA);
					break;
				}
				case LOCK_FRAMEAREA: {
					LockMouse(rdPtr, LOCK_FRAMEAREA);
					break;
				}
				case LOCK_BYRECT: {
					// 若设定了相对锁定，且处于相对于屏幕锁定模式，则更新锁定
					//相对于其他模式则是强制开启
					switch (rdPtr->RectOffset_Type) {
						case RELATIVE_SCREEN:{
							if (rdPtr->RectOffset_State) {
								RECT CurrentWindowRect;
								::GetWindowRect(rdPtr->MainWindowHandle, &CurrentWindowRect);
								CurrentWindowRect += rdPtr->RectOffset;
								LockMouse(rdPtr, CurrentWindowRect, RELATIVE_SCREEN);
							}
							else {
								LockMouse(rdPtr, rdPtr->UserSetRect, RELATIVE_SCREEN);
							}
							break;
						}
						case RELATIVE_CURRENTWINDOW: {
							LockMouse(rdPtr, rdPtr->UserSetRect, RELATIVE_CURRENTWINDOW);
							break;
						}
						case RELATIVE_CLIENTAREA: {
							LockMouse(rdPtr, rdPtr->UserSetRect, RELATIVE_CLIENTAREA);
							break;
						}
						case RELATIVE_FRAMEAREA: {
							LockMouse(rdPtr, rdPtr->UserSetRect, RELATIVE_FRAMEAREA);
							break;
						}
						default: {

						}
					}
					break;
				}				
				default: {
					
				}
			}			
		}		
	}

	// 若设定了保持输入法状态，且当前状态与保持状态不一致，则更新状态
	if (rdPtr->KeepIMEState&&(IMEState != (bool)ImmGetOpenStatus(ImmGetContext(rdPtr->MainWindowHandle)))) {
		IMEStateControl(rdPtr->MainWindowHandle,IMEState);
	}

	// 刷新显示器状态
	if (rdPtr->bSecondFrame) {
		RefreshMonitorState(rdPtr);
	}

	rdPtr->bSecondFrame = true;

	// Handle resizing
	windowResizing.TriggerCallback([&] () {
		CallEvent(ONRESIZINGCOMPLETE);
	});

	// Handle mouse
	mouseHandler.TriggerCallback([&] () {
		if (mouseHandler.curCmd == APPCOMMAND_BROWSER_BACKWARD) {
			CallEvent(ONCLICKBACKWARD);
		}

		if (mouseHandler.curCmd == APPCOMMAND_BROWSER_FORWARD) {
			CallEvent(ONCLICKFORWARD);
		}
	});

	//更新显示
	if ((rdPtr->Display) && (rdPtr->rc.rcChanged)) {
		return REFLAG_DISPLAY;
	}
	else {
		return 0;
	}
}

// ----------------
// DisplayRunObject
// ----------------
// Draw the object in the application screen.
// 
short WINAPI DLLExport DisplayRunObject(LPRDATA rdPtr)
{
/*
   If you return REFLAG_DISPLAY in HandleRunObject this routine will run.
*/
	//使用GetRunObjectSurface不会正确处理当对象并非全部位于场景内时的Shader

	if ((rdPtr->Display) && (rdPtr->img->IsValid())) {
		// Begin render process...
		LPSURFACE ps = WinGetSurface((int)rdPtr->rhPtr->rhIdEditWin);
		//int nDrv = ps->GetDriver();
		//bool HWA = nDrv >= SD_3DFX;

		// On-screen coords
		int screenX = rdPtr->rHo.hoX - rdPtr->rhPtr->rhWindowX;
		int screenY = rdPtr->rHo.hoY - rdPtr->rhPtr->rhWindowY;

		// Hot spot (transform center)
		POINT point = { 0, 0 };

		//rdPtr->img.Blit(*ps, (float)screenX, (float)screenY, (rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) ? BMODE_TRANSP : BMODE_OPAQUE, BlitOp(rdPtr->rs.rsEffect & EFFECT_MASK), rdPtr->rs.rsEffectParam, BLTF_ANTIA);
		rdPtr->img->BlitEx(*ps, (float)screenX, (float)screenY,
			rdPtr->rc.rcScaleX, rdPtr->rc.rcScaleY, 0, 0,
			rdPtr->img->GetWidth(), rdPtr->img->GetHeight(), &point, rdPtr->rc.rcAngle,
			(rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) ? BMODE_TRANSP : BMODE_OPAQUE,
			BlitOp(rdPtr->rs.rsEffect & EFFECT_MASK),
			rdPtr->rs.rsEffectParam, BLTF_ANTIA);
	}
		
	// Ok
	return 0;
}

// -------------------
// GetRunObjectSurface
// -------------------
// Implement this function instead of DisplayRunObject if your extension
// supports ink effects and transitions. Note: you can support ink effects
// in DisplayRunObject too, but this is automatically done if you implement
// GetRunObjectSurface (MMF applies the ink effect to the surface).
//
// Note: do not forget to enable the function in the .def file 
// if you remove the comments below.

//cSurface* WINAPI DLLExport GetRunObjectSurface(LPRDATA rdPtr)
//{
//	if (rdPtr->Display) {
//		return &(rdPtr->img);
//	}
//	else {
//		return NULL;
//	}
//	//return NULL;
//}


// -------------------------
// GetRunObjectCollisionMask
// -------------------------
// Implement this function if your extension supports fine collision mode (OEPREFS_FINECOLLISIONS),
// Or if it's a background object and you want Obstacle properties for this object.
//
// Should return NULL if the object is not transparent.
//
// Note: do not forget to enable the function in the .def file 
// if you remove the comments below.
//
/*
LPSMASK WINAPI DLLExport GetRunObjectCollisionMask(LPRDATA rdPtr, LPARAM lParam)
{
	// Typical example for active objects
	// ----------------------------------
	// Opaque? collide with box
	if ( (rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) == 0 )	// Note: only if your object has the OEPREFS_INKEFFECTS option
		return NULL;

	// Transparent? Create mask
	LPSMASK pMask = rdPtr->m_pColMask;
	if ( pMask == NULL )
	{
		if ( rdPtr->m_pSurface != NULL )
		{
			DWORD dwMaskSize = rdPtr->m_pSurface->CreateMask(NULL, lParam);
			if ( dwMaskSize != 0 )
			{
				pMask = (LPSMASK)calloc(dwMaskSize, 1);
				if ( pMask != NULL )
				{
					rdPtr->m_pSurface->CreateMask(pMask, lParam);
					rdPtr->m_pColMask = pMask;
				}
			}
		}
	}

	// Note: for active objects, lParam is always the same.
	// For background objects (OEFLAG_BACKGROUND), lParam maybe be different if the user uses your object
	// as obstacle and as platform. In this case, you should store 2 collision masks
	// in your data: one if lParam is 0 and another one if lParam is different from 0.

	return pMask;
}
*/

// ----------------
// PauseRunObject
// ----------------
// Enters the pause mode
// 
short WINAPI DLLExport PauseRunObject(LPRDATA rdPtr)
{
	// Ok
	return 0;
}


// -----------------
// ContinueRunObject
// -----------------
// Quits the pause mode
//
short WINAPI DLLExport ContinueRunObject(LPRDATA rdPtr)
{
	// Ok
	return 0;
}

// -----------------
// SaveRunObject
// -----------------
// Saves the object to disk
// 

BOOL WINAPI SaveRunObject(LPRDATA rdPtr, HANDLE hf)
{            
	BOOL bOK = FALSE;

#ifndef VITALIZE

	// Save the object's data here

	// OK
	bOK = TRUE;

#endif // VITALIZE

	return bOK;
}

// -----------------
// LoadRunObject
// -----------------
// Loads the object from disk
// 
BOOL WINAPI LoadRunObject(LPRDATA rdPtr, HANDLE hf)
{            
	BOOL bOK=FALSE;

	// Load the object's data here

	// OK
	bOK = TRUE;

	return bOK; 
}




// ============================================================================
//
// START APP / END APP / START FRAME / END FRAME routines
// 
// ============================================================================

// -------------------
// StartApp
// -------------------
// Called when the application starts or restarts.
// Useful for storing global data
// 
void WINAPI DLLExport StartApp(mv _far *mV, CRunApp* pApp)
{
	// Example
	// -------
	// Delete global data (if restarts application)
//	CMyData* pData = (CMyData*)mV->mvGetExtUserData(pApp, hInstLib);
//	if ( pData != NULL )
//	{
//		delete pData;
//		mV->mvSetExtUserData(pApp, hInstLib, NULL);
//	}
}

// -------------------
// EndApp
// -------------------
// Called when the application ends.
// 
void WINAPI DLLExport EndApp(mv _far *mV, CRunApp* pApp)
{
	// Example
	// -------
	// Delete global data
//	CMyData* pData = (CMyData*)mV->mvGetExtUserData(pApp, hInstLib);
//	if ( pData != NULL )
//	{
//		delete pData;
//		mV->mvSetExtUserData(pApp, hInstLib, NULL);
//	}
}

// -------------------
// StartFrame
// -------------------
// Called when the frame starts or restarts.
// 
void WINAPI DLLExport StartFrame(mv _far *mV, DWORD dwReserved, int nFrameIndex)
{
}

// -------------------
// EndFrame
// -------------------
// Called when the frame ends.
// 
void WINAPI DLLExport EndFrame(mv _far *mV, DWORD dwReserved, int nFrameIndex)
{
}

// ============================================================================
//
// TEXT ROUTINES (if OEFLAG_TEXT)
// 
// ============================================================================

// -------------------
// GetRunObjectFont
// -------------------
// Return the font used by the object.
// 
/*

  // Note: do not forget to enable the functions in the .def file 
  // if you remove the comments below.

void WINAPI GetRunObjectFont(LPRDATA rdPtr, LOGFONT* pLf)
{
	// Example
	// -------
	// GetObject(rdPtr->m_hFont, sizeof(LOGFONT), pLf);
}

// -------------------
// SetRunObjectFont
// -------------------
// Change the font used by the object.
// 
void WINAPI SetRunObjectFont(LPRDATA rdPtr, LOGFONT* pLf, RECT* pRc)
{
	// Example
	// -------
//	HFONT hFont = CreateFontIndirect(pLf);
//	if ( hFont != NULL )
//	{
//		if (rdPtr->m_hFont!=0)
//			DeleteObject(rdPtr->m_hFont);
//		rdPtr->m_hFont = hFont;
//		SendMessage(rdPtr->m_hWnd, WM_SETFONT, (WPARAM)rdPtr->m_hFont, FALSE);
//	}

}

// ---------------------
// GetRunObjectTextColor
// ---------------------
// Return the text color of the object.
// 
COLORREF WINAPI GetRunObjectTextColor(LPRDATA rdPtr)
{
	// Example
	// -------
	return 0;	// rdPtr->m_dwColor;
}

// ---------------------
// SetRunObjectTextColor
// ---------------------
// Change the text color of the object.
// 
void WINAPI SetRunObjectTextColor(LPRDATA rdPtr, COLORREF rgb)
{
	// Example
	// -------
	rdPtr->m_dwColor = rgb;
	InvalidateRect(rdPtr->m_hWnd, NULL, TRUE);
}
*/


// ============================================================================
//
// WINDOWPROC (interception of messages sent to hMainWin and hEditWin)
//
// Do not forget to enable the WindowProc function in the .def file if you implement it
// 
// ============================================================================

// Get the pointer to the object's data from its window handle
// Note: the object's window must have been subclassed with a
// callRunTimeFunction(rdPtr, RFUNCTION_SUBCLASSWINDOW, 0, 0);
// See the documentation and the Simple Control example for more info.
LPRDATA GetRdPtr(HWND hwnd, LPRH rhPtr) {
	return (LPRDATA)GetProp(hwnd, (LPCWSTR)rhPtr->rh4.rh4AtomRd);
}

// Called from the window proc of hMainWin and hEditWin.
// You can intercept the messages and/or tell the main proc to ignore them.
LRESULT CALLBACK DLLExport WindowProc(LPRH rhPtr, HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam) {
	if (rhPtr->rh2.rh2PauseCompteur != 0) {
		return 0;
	}

	switch (nMsg) {
	case WM_ENTERSIZEMOVE: {
		windowResizing.EnterResizing(rhPtr->rhHMainWin);

		break;
	}
	case WM_EXITSIZEMOVE: {
		windowResizing.ExitResizing(rhPtr->rhHMainWin);

		break;
	}
	case WM_APPCOMMAND: {
		const auto cmd = GET_APPCOMMAND_LPARAM(lParam);
		const auto uDevice = GET_DEVICE_LPARAM(lParam);
		const auto dwKeys = GET_KEYSTATE_LPARAM(lParam);

		if (FAPPCOMMAND_MOUSE != uDevice) {
			break;
		}

		if (cmd == APPCOMMAND_BROWSER_BACKWARD) {
			mouseHandler.ReceiveCommand(APPCOMMAND_BROWSER_BACKWARD);

			return REFLAG_MSGHANDLED;
		}

		if (cmd == APPCOMMAND_BROWSER_FORWARD) {
			mouseHandler.ReceiveCommand(APPCOMMAND_BROWSER_FORWARD);

			return REFLAG_MSGHANDLED;
		}

		break;
	}

//	case WM_XBUTTONDOWN: {
//		const auto curXButton = GET_XBUTTON_WPARAM(wParam);
//#ifdef _DEBUG
//		OutputDebugStringA(std::format("Down {}",
//			curXButton == XBUTTON1
//			? "XB1"
//			: "XB2").c_str());
//#endif
//
//		return REFLAG_MSGHANDLED;
//	}
//	case WM_XBUTTONUP: {
//		const auto curXButton = GET_XBUTTON_WPARAM(wParam);
//#ifdef _DEBUG
//		OutputDebugStringA(std::format("Up {}",
//			curXButton == XBUTTON1
//			? "XB1"
//			: "XB2").c_str());
//#endif
//
//		return REFLAG_MSGHANDLED;
//	}
	}

	return 0;
}

// ============================================================================
//
// DEBUGGER ROUTINES
// 
// ============================================================================

// -----------------
// GetDebugTree
// -----------------
// This routine returns the address of the debugger tree
//
LPWORD WINAPI DLLExport GetDebugTree(LPRDATA rdPtr)
{
#if !defined(RUN_ONLY)
	return DebugTree;
#else
	return NULL;
#endif // !defined(RUN_ONLY)
}

// -----------------
// GetDebugItem
// -----------------
// This routine returns the text of a given item.
//
void WINAPI DLLExport GetDebugItem(LPTSTR pBuffer, LPRDATA rdPtr, int id)
{
#if !defined(RUN_ONLY)

	// Example
	// -------
/*
	char temp[DB_BUFFERSIZE];

	switch (id)
	{
	case DB_CURRENTSTRING:
		LoadString(hInstLib, IDS_CURRENTSTRING, temp, DB_BUFFERSIZE);
		wsprintf(pBuffer, temp, rdPtr->text);
		break;
	case DB_CURRENTVALUE:
		LoadString(hInstLib, IDS_CURRENTVALUE, temp, DB_BUFFERSIZE);
		wsprintf(pBuffer, temp, rdPtr->value);
		break;
	case DB_CURRENTCHECK:
		LoadString(hInstLib, IDS_CURRENTCHECK, temp, DB_BUFFERSIZE);
		if (rdPtr->check)
			wsprintf(pBuffer, temp, _T("TRUE"));
		else
			wsprintf(pBuffer, temp, _T("FALSE"));
		break;
	case DB_CURRENTCOMBO:
		LoadString(hInstLib, IDS_CURRENTCOMBO, temp, DB_BUFFERSIZE);
		wsprintf(pBuffer, temp, rdPtr->combo);
		break;
	}
*/

#endif // !defined(RUN_ONLY)
}

// -----------------
// EditDebugItem
// -----------------
// This routine allows to edit editable items.
//
void WINAPI DLLExport EditDebugItem(LPRDATA rdPtr, int id)
{
#if !defined(RUN_ONLY)

	// Example
	// -------
/*
	switch (id)
	{
	case DB_CURRENTSTRING:
		{
			EditDebugInfo dbi;
			char buffer[256];

			dbi.pText=buffer;
			dbi.lText=TEXT_MAX;
			dbi.pTitle=NULL;

			strcpy(buffer, rdPtr->text);
			long ret=callRunTimeFunction(rdPtr, RFUNCTION_EDITTEXT, 0, (LPARAM)&dbi);
			if (ret)
				strcpy(rdPtr->text, dbi.pText);
		}
		break;
	case DB_CURRENTVALUE:
		{
			EditDebugInfo dbi;

			dbi.value=rdPtr->value;
			dbi.pTitle=NULL;

			long ret=callRunTimeFunction(rdPtr, RFUNCTION_EDITINT, 0, (LPARAM)&dbi);
			if (ret)
				rdPtr->value=dbi.value;
		}
		break;
	}
*/
#endif // !defined(RUN_ONLY)
}


