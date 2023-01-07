// ============================================================================
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

	DB_SAPARATOR_START,
	DB_OBJECTTYPE,
	DB_SAPARATOR_1,
	DB_LIBSIZE,
	DB_LIBMEMUSE,
	DB_SAPARATOR_2,
	DB_FROMLIB,
	DB_FILENAME,
	DB_FILEPATH,
	DB_KEY,
	DB_SAPARATOR_3,
	DB_COLLISION,
	DB_AUTOUPDATECOLLISION,
	DB_SAPARATOR_4,
	DB_STRETCHQUALITY,
	DB_HOTSPOT,
	DB_ZOOMSCALE,
	DB_ORIGINSIZE,
	DB_CURRENTSIZE,
	DB_SAPARATOR_END,
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
	DB_SAPARATOR_START,
	DB_OBJECTTYPE,
	DB_SAPARATOR_1,
	DB_LIBSIZE,
	DB_LIBMEMUSE,
	DB_SAPARATOR_2,
	DB_FROMLIB,
	DB_FILENAME,
	DB_FILEPATH,
	DB_KEY,
	DB_SAPARATOR_3,
	//DB_COLLISION,
	//DB_AUTOUPDATECOLLISION,
	//DB_SAPARATOR_4,
	DB_STRETCHQUALITY,
	DB_HOTSPOT,
	DB_ZOOMSCALE,
	DB_ORIGINSIZE,
	DB_CURRENTSIZE,
	DB_SAPARATOR_END,
	
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

	//Settings
	rdPtr->isLib = edPtr->isLib;
	rdPtr->memoryLimit = edPtr->memoryLimit;
	rdPtr->autoClean = edPtr->autoClean;
	rdPtr->hotSpotPos = (HotSpotPos)edPtr->hotSpotComboID;
	
	rdPtr->HWA = edPtr->HWA;

	rdPtr->stretchQuality = edPtr->stretchQuality;

	//rdPtr->collision = edPtr->collision;
	//rdPtr->autoUpdateCollision = edPtr->autoUpdateCollision;

	//Display
	rdPtr->FileName = new std::wstring;
	rdPtr->FilePath = new std::wstring;
	rdPtr->RelativeFilePath = new std::wstring;
	rdPtr->Key = new std::wstring;

	rdPtr->img = nullptr;
	rdPtr->src = nullptr;

	// to create new surface, don't change
	rdPtr->fromLib = true;

	rdPtr->zoomScale = { 1.0,1.0 };
	rdPtr->imgZoomScale = { 1.0,1.0 };

	rdPtr->AT = { 1,0,0,1 };
	rdPtr->imgAT = { 1,0,0,1 };

	//Init global data
	if (GetExtUserData() == nullptr) {
		//init global
		auto pData = new GlobalData;

		//init general
#ifdef _USE_DXGI
#ifdef _DYNAMIC_LINK
		wchar_t rootDir[MAX_PATH] = {};
		GetCurrentDirectory(MAX_PATH - 1, rootDir);

		wchar_t dllPath[2 * MAX_PATH] = {};
		wsprintf(dllPath, L"%s\\%s", rootDir, L"Modules\\DXGI.DLL");

		pData->DXGI = LoadLibrary(dllPath);

		if (pData->DXGI == nullptr) {
			MSGBOX(L"Load Failed");
		}
#endif
		pData->pD3DU = new D3DUtilities;

		//auto& Desc = rdPtr->pD3DU->GetDesc();
		//MSGBOX(Desc.Description);

		//rdPtr->pD3DU->UpdateVideoMemoryInfo();
		//auto& info = rdPtr->pD3DU->GetLocalVideoMemoryInfo();
#endif

		//init specific
		pData->pLib = new SurfaceLib;
		pData->pCount = new RefCount;
		pData->pKeepList = new KeepList;
		pData->pFileListMap = new FileListMap;

		//Update pointer
		SetExtUserData(pData);
	}

	//Get global data
	//Get pointers here and never delete them.
	rdPtr->pData = (GlobalData*)GetExtUserData();

	//Get general
#ifdef _USE_DXGI
	rdPtr->pD3DU = rdPtr->pData->pD3DU;
#endif

	//Get specific
	if (rdPtr->isLib) {
		//Load Lib
		rdPtr->pLib = rdPtr->pData->pLib;
		rdPtr->pCount = rdPtr->pData->pCount;
		rdPtr->pKeepList = rdPtr->pData->pKeepList;
		rdPtr->pFileListMap = rdPtr->pData->pFileListMap;
	}
	
	rdPtr->pCountVec = new RefCountVec;
	rdPtr->pPreloadList = nullptr;

	rdPtr->itCountVecStr = new std::wstring;
	rdPtr->itCountVecCount = new Count;
	
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
	//Display
	delete rdPtr->FileName;
	delete rdPtr->FilePath;
	delete rdPtr->RelativeFilePath;
	delete rdPtr->Key;

	if (!rdPtr->isLib) {
		if (rdPtr->src != rdPtr->img) {
			delete rdPtr->img;
		}

		if (!rdPtr->fromLib) {
			ReleaseNonFromLib(rdPtr);
		}

		delete rdPtr->trans;

		if (!rdPtr->isLib) {
			FreeColMask(rdPtr->pColMask);
		}

		UpdateRef(rdPtr, false);
		rdPtr->pRefCount = nullptr;
	}

	if (rdPtr->isLib) {
		if (rdPtr->threadID) {
			rdPtr->forceExit = true;

			DWORD ret;
			while (GetExitCodeThread(rdPtr->threadID, &ret)) {
				if (ret == 0) {
					break;
				}
			}
		}
		
		// Clear ref
		ClearCurRef(rdPtr);
	}

	delete rdPtr->pCountVec;
	delete rdPtr->pPreloadList;

	delete rdPtr->itCountVecStr;
	delete rdPtr->itCountVecCount;

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

	CleanCache(rdPtr, false);
	MergeLib(rdPtr);

	if (!rdPtr->isLib && rdPtr->rc.rcChanged) {
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
	if (!rdPtr->isLib
		&& rdPtr->src != nullptr
		&& rdPtr->src->IsValid()) {
		// Begin render process...
		LPSURFACE ps = WinGetSurface((int)rdPtr->rHo.hoAdRunHeader->rhIdEditWin);

		// On-screen coords
		int screenX = rdPtr->rHo.hoX - rdPtr->rHo.hoAdRunHeader->rhWindowX;
		int screenY = rdPtr->rHo.hoY - rdPtr->rHo.hoAdRunHeader->rhWindowY;

		HandleFlip(rdPtr);

		LPSURFACE pDisplay = rdPtr->src;
		
		std::unique_ptr<cSurface> pOffset = nullptr;
		
		if (rdPtr->offset.XOffset != 0 || rdPtr->offset.YOffset != 0) {
			pOffset.reset(GetSurface(rdPtr, rdPtr->src->GetWidth(), rdPtr->src->GetHeight()));
			OffsetHWA(rdPtr->src, pOffset.get(), rdPtr->offset);

			pDisplay = pOffset.get();
		}

		DWORD flags = 0;

		if (rdPtr->stretchQuality) {
			flags |= BLTF_ANTIA;
		}

		pDisplay->BlitEx(*ps, (float)screenX, (float)screenY,
			abs(rdPtr->zoomScale.XScale), abs(rdPtr->zoomScale.YScale), 0, 0,
			rdPtr->src->GetWidth(), rdPtr->src->GetHeight(), &rdPtr->hotSpot, (float)rdPtr->angle,
			(rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) ? BMODE_TRANSP : BMODE_OPAQUE,
			BlitOp(rdPtr->rs.rsEffect & EFFECT_MASK),
			rdPtr->rs.rsEffectParam, flags);
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
/*
cSurface* WINAPI DLLExport GetRunObjectSurface(LPRDATA rdPtr)
{
	return NULL;
}
*/

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

LPSMASK WINAPI DLLExport GetRunObjectCollisionMask(LPRDATA rdPtr, LPARAM lParam)
{
	// Typical example for active objects
	// ----------------------------------
	if (!rdPtr->isLib) {
		// Opaque? collide with box	
		// Note: only if your object has the OEPREFS_INKEFFECTS option
		if ((rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) == 0) {
			return NULL;
		}

		// Transparent? Create mask
		LPSMASK pMask = rdPtr->pColMask;
		if (pMask == nullptr) {
			if (rdPtr->src != nullptr){
				GetTransfromedBitmap(rdPtr, [&](LPSURFACE pCollideBitmap) {
					//_SavetoClipBoard(pCollideBitmap, false);
					DWORD dwMaskSize = pCollideBitmap->CreateMask(NULL, lParam);

					if (dwMaskSize != 0) {
						pMask = (LPSMASK)calloc(dwMaskSize, 1);

						if (pMask != nullptr) {
							pCollideBitmap->CreateMask(pMask, lParam);
							rdPtr->pColMask = pMask;
						}
					}
					});
			}
		}

		// Note: for active objects, lParam is always the same.
		// For background objects (OEFLAG_BACKGROUND), lParam maybe be different if the user uses your object
		// as obstacle and as platform. In this case, you should store 2 collision masks
		// in your data: one if lParam is 0 and another one if lParam is different from 0.

		return pMask;
	}
	else {
		return NULL;
	}
}

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
	auto pData = (GlobalData*)mV->mvGetExtUserData(pApp, hInstLib);
	if (pData != NULL) {
		DeleteGlobalData(pData);
		mV->mvSetExtUserData(pApp, hInstLib, NULL);
	}
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
	auto pData = (GlobalData*)mV->mvGetExtUserData(pApp, hInstLib);
	if (pData != NULL) {
		DeleteGlobalData(pData);
		mV->mvSetExtUserData(pApp, hInstLib, NULL);
	}
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
/*
// Get the pointer to the object's data from its window handle
// Note: the object's window must have been subclassed with a
// callRunTimeFunction(rdPtr, RFUNCTION_SUBCLASSWINDOW, 0, 0);
// See the documentation and the Simple Control example for more info.
//
LPRDATA GetRdPtr(HWND hwnd, LPRH rhPtr)
{
	return (LPRDATA)GetProp(hwnd, (LPCSTR)rhPtr->rh4.rh4AtomRd);
}

// Called from the window proc of hMainWin and hEditWin.
// You can intercept the messages and/or tell the main proc to ignore them.
//
LRESULT CALLBACK DLLExport WindowProc(LPRH rhPtr, HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	LPRDATA rdPtr = NULL;

	switch (nMsg) {

	// Example
	case WM_CTLCOLORSTATIC:
		{
			// Get the handle of the control
			HWND hWndControl = (HWND)lParam;

			// Get a pointer to the RUNDATA structure (see GetRdptr function above for more info)
			rdPtr = GetRdPtr(hWndControl, rhPtr);

			// Check if the rdPtr pointer is valid and points to an object created with this extension
			if ( rdPtr == NULL || rdPtr->rHo.hoIdentifier != IDENTIFIER )
				break;

			// OK, intercept the message
			HDC hDC = (HDC)wParam;
			SetBkColor(hDC, rdPtr->backColor);
			SetTextColor(hDC, rdPtr->fontColor);
			rhPtr->rh4.rh4KpxReturn = (long)rdPtr->hBackBrush;
			return REFLAG_MSGRETURNVALUE;
		}
		break;
	}

	return 0;
}
*/

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

	//wchar_t temp[DB_BUFFERSIZE];

	auto libFilter = [&](LPCWSTR pattern, LPCWSTR negativePattern, LPCWSTR negative, std::function<void(LPCWSTR)> f) {
		if (rdPtr->isLib) {
			f(pattern);
		}
		else {
			swprintf_s(pBuffer, DB_BUFFERSIZE, negativePattern, negative);
		}
	};

	auto displayFilter = [&](LPCWSTR pattern, LPCWSTR negativePattern, LPCWSTR negative, std::function<void(LPCWSTR)> f) {
		if (!rdPtr->isLib) {
			f(pattern);
		}
		else {
			swprintf_s(pBuffer, DB_BUFFERSIZE, negativePattern, negative);
		}
	};

	auto libNegative = L"Not Lib";
	auto displayNegative = L"Not Display";

	switch (id)
	{
	case DB_SAPARATOR_START:
	case DB_SAPARATOR_1:
	case DB_SAPARATOR_2:
	case DB_SAPARATOR_3:
	case DB_SAPARATOR_4:
	case DB_SAPARATOR_END:
		swprintf_s(pBuffer, DB_BUFFERSIZE, L"=====================");
		break;
	case DB_OBJECTTYPE:
		swprintf_s(pBuffer, DB_BUFFERSIZE, L"Object Type: %s", rdPtr->isLib ? L"Lib" : L"Display");
		break;
	case DB_LIBSIZE:
		libFilter(L"Lib Size: %d", L"Lib Size: %s", libNegative, [&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->pLib->size());
			});
		break;
	case DB_LIBMEMUSE:
		libFilter(L"Auto Clean Cache: %s", L"Auto Clean Cache: %s", libNegative, [&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->autoClean ? L"True" : L"False");
			});
		break;
	case DB_FROMLIB:
		displayFilter(L"From Lib: %s", L"From Lib: %s", displayNegative, [&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->fromLib ? L"True" : L"False");
			});
		break;
	case DB_FILENAME:
		displayFilter(L"FileName: %s", L"FileName: %s", displayNegative, [&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->FileName->c_str());
			});
		break;
	case DB_FILEPATH:
		displayFilter(L"FilePath: %s", L"FilePath: %s", displayNegative, [&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->FilePath->c_str());
			});
		break;
	case DB_KEY:
		displayFilter(L"Key: %s", L"Key: %s", displayNegative, [&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, (*rdPtr->Key)==L""? L"Not Encrypted" : rdPtr->Key->c_str());
			});
		break;
	//case DB_COLLISION:
	//	displayFilter(L"Has Collision: %s", L"Has Collision: %s", displayNegative, [&](LPCWSTR pattern) {
	//		swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->collision ? L"True" : L"False");
	//		});
	//	break;
	//case DB_AUTOUPDATECOLLISION:
	//	displayFilter(L"Auto Update Collision: %s", L"Auto Update Collision: %s", displayNegative, [&](LPCWSTR pattern) {
	//		swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->autoUpdateCollision ? L"True" : L"False");
	//		});
	//	break;
	case DB_STRETCHQUALITY:
		displayFilter(L"Stretch Quality: %s", L"Stretch Quality: %s", displayNegative, [&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->stretchQuality ? L"Resample" : L"Fast");
			});
		break;
	case DB_HOTSPOT:
		displayFilter(L"HotSpot: [ %d, %d ]", L"HotSpot: %s", displayNegative, [&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, GetHotSpotX(rdPtr), GetHotSpotY(rdPtr));
			});
		break;
	case DB_ZOOMSCALE:
		displayFilter(L"Zoom Scale: [ %f, %f ]", L"Zoom Scale: %s", displayNegative, [&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, GetXZoomScale(rdPtr), GetYZoomScale(rdPtr));
			});
		break;
	case DB_ORIGINSIZE:
		displayFilter(L"Origin Size: [ %d ¡Á %d ]", L"Origin Size: %s", displayNegative, [&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, GetOriginalWidth(rdPtr), GetOriginalHeight(rdPtr));
			});
		break;
	case DB_CURRENTSIZE:
		displayFilter(L"Current Size: [ %d ¡Á %d ]", L"Current Size: %s", displayNegative, [&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, GetCurrentWidth(rdPtr), GetCurrentHeight(rdPtr));
			});
		break;
	}

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


