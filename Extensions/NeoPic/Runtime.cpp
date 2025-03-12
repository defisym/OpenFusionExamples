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
	DB_LOADCALLBACK,
	DB_SAPARATOR_4,
	DB_ANGLE,
	DB_SAPARATOR_5,
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
	DB_LOADCALLBACK,
	DB_SAPARATOR_4,
	DB_ANGLE,
	DB_SAPARATOR_5,
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

	//------------
	// Lib
	//------------

	rdPtr->isLib = edPtr->isLib;

	rdPtr->bLoadCallback = edPtr->bLoadCallback;
	rdPtr->pLoadCallbackInfo = new LoadCallbackInfo();

	rdPtr->bLoadKeepAngle = edPtr->bLoadKeepAngle;

	rdPtr->itCountVecStr = new std::wstring;
	rdPtr->itCountVecCount = new Count;

	//------------
	// Display
	//------------

	rdPtr->HWA = edPtr->HWA;
	rdPtr->stretchQuality = edPtr->stretchQuality;
	
	// to create new surface, don't change
	rdPtr->fromLib = true;

	rdPtr->FileName = new std::wstring;
	rdPtr->FilePath = new std::wstring;
	rdPtr->RelativeFilePath = new std::wstring;
	rdPtr->Key = new std::wstring;

	rdPtr->src = nullptr;

	rdPtr->isTransparent = transpTBD;

	rdPtr->hotSpotPos = static_cast<HotSpotPos>(edPtr->hotSpotComboID);
	rdPtr->zoomScale = { 1.0,1.0 };

	rdPtr->pFrameCapture = new FrameCapture(rdPtr->rHo.hoAdRunHeader->rhHEditWin);

	rdPtr->pAI = new AnimationInterface(rdPtr);
	rdPtr->pNS = new NineSliceInterface(rdPtr);

	//Init global data
	if (GetExtUserData() == nullptr) {
		//init global
		auto pData = new GlobalData;

		pData->bDX11 = D3D11(rdPtr);
		pData->bPreMulAlpha = PreMulAlpha(rdPtr);

		//Get specific
		if (rdPtr->isLib) {
			pData->SetClean(edPtr->autoClean, edPtr->memoryLimit, edPtr->sizeLimit);
		}

		//Update pointer
		SetExtUserData(pData);
	}

	//Get global data
	//Get pointers here and never delete them.
	rdPtr->pData = static_cast<GlobalData*>(GetExtUserData());

	//Get general
#ifdef QUERY_VRAM
	rdPtr->pVRAMU = rdPtr->pData->pVRAMU;
#endif

	//Get specific
	if (rdPtr->isLib) {		
		rdPtr->pLib = rdPtr->pData->pLib;
		rdPtr->pData->pPreloadHandler->ResumePreload(rdPtr);
	}
	
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
	delete rdPtr->pLoadCallbackInfo;

	delete rdPtr->FileName;
	delete rdPtr->FilePath;
	delete rdPtr->RelativeFilePath;
	delete rdPtr->Key;

	delete rdPtr->itCountVecStr;
	delete rdPtr->itCountVecCount;

	delete rdPtr->pFrameCapture;

	delete rdPtr->pAI;
	delete rdPtr->pNS;

	if (rdPtr->isLib) {
		rdPtr->pData->pPreloadHandler->PausePreload();
	}
	else {
		DetachFromLib(rdPtr);
		FreeColMask(rdPtr->pColMask);
	}

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

	rdPtr->pAI->UpdateAnimation();
	rdPtr->pNS->Render();

	if (rdPtr->isLib) {
		rdPtr->pData->AutoClean();
		rdPtr->pData->MergeLib(rdPtr);
	}

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
	if (CanDisplay(rdPtr)) {
		// Begin render process...
		const LPSURFACE ps = WinGetSurface((int)rdPtr->rHo.hoAdRunHeader->rhIdEditWin);

		// On-screen coords
		const int screenX = rdPtr->rHo.hoX - rdPtr->rHo.hoAdRunHeader->rhWindowX;
		const int screenY = rdPtr->rHo.hoY - rdPtr->rHo.hoAdRunHeader->rhWindowY;

		HandleFlip(rdPtr);
		
		DWORD flags = 0;

		if (rdPtr->stretchQuality) {
			flags |= STRF_RESAMPLE;
		}

		rdPtr->src->BlitEx(*ps, (float)screenX, (float)screenY,
			abs(rdPtr->zoomScale.XScale), abs(rdPtr->zoomScale.YScale), 0, 0,
			rdPtr->src->GetWidth(), rdPtr->src->GetHeight(), &rdPtr->hotSpot, rdPtr->angle,
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

LPSMASK WINAPI DLLExport GetRunObjectCollisionMask(LPRDATA rdPtr, LPARAM lParam) {
	if (CanDisplay(rdPtr)) {
		// Opaque? collide with box	
		// Note: only if your object has the OEPREFS_INKEFFECTS option
		if ((rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) == 0) {
			return nullptr;
		}

		// Transparent? Create mask
		LPSMASK pMask = rdPtr->pColMask;

		if (pMask == nullptr) {
			GetTransformedBitmap(rdPtr, rdPtr->src, [&] (const LPSURFACE pBitmap) {				
				DWORD dwMaskSize = pBitmap->CreateMask(nullptr, lParam);

				if (dwMaskSize != 0) {
					pMask = (LPSMASK)calloc(dwMaskSize, 1);

					if (pMask != nullptr) {
						pBitmap->CreateMask(pMask, lParam);
						rdPtr->pColMask = pMask;
					}
				}});
		}

		// Note: for active objects, lParam is always the same.
		// For background objects (OEFLAG_BACKGROUND), lParam maybe be different if the user uses your object
		// as obstacle and as platform. In this case, you should store 2 collision masks
		// in your data: one if lParam is 0 and another one if lParam is different from 0.

		return pMask;
	}
	else {
		return nullptr;
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
void WINAPI DLLExport StartApp(mv _far *mV, CRunApp* pApp) {
	auto pData = (GlobalData*)mV->mvGetExtUserData(pApp, hInstLib);
	if (pData != nullptr) {
		delete pData;
		mV->mvSetExtUserData(pApp, hInstLib, nullptr);
	}
}

// -------------------
// EndApp
// -------------------
// Called when the application ends.
// 
void WINAPI DLLExport EndApp(mv _far *mV, CRunApp* pApp) {
	auto pData = (GlobalData*)mV->mvGetExtUserData(pApp, hInstLib);
	if (pData != nullptr) {
		delete pData;
		mV->mvSetExtUserData(pApp, hInstLib, nullptr);
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
	auto libFilter = [&](LPCWSTR pattern, LPCWSTR negativePattern, LPCWSTR negative, const std::function<void(LPCWSTR)>& f) {
		if (rdPtr->isLib) {
			f(pattern);
		}
		else {
			swprintf_s(pBuffer, DB_BUFFERSIZE, negativePattern, negative);
		}
	};

	auto displayFilter = [&](LPCWSTR pattern, LPCWSTR negativePattern, LPCWSTR negative, const std::function<void(LPCWSTR)>& f) {
		if (!rdPtr->isLib) {
			f(pattern);
		}
		else {
			swprintf_s(pBuffer, DB_BUFFERSIZE, negativePattern, negative);
		}
	};
	
	const auto libNegative = L"Not Lib";
	const auto displayNegative = L"Not Display";

	switch (id)
	{
	case DB_SAPARATOR_START:
	case DB_SAPARATOR_1:
	case DB_SAPARATOR_2:
	case DB_SAPARATOR_3:
	case DB_SAPARATOR_4:
	case DB_SAPARATOR_5:
	case DB_SAPARATOR_END:
		swprintf_s(pBuffer, DB_BUFFERSIZE, L"=====================");
		break;
	case DB_OBJECTTYPE:
		swprintf_s(pBuffer, DB_BUFFERSIZE, L"Object Type: %s", rdPtr->isLib ? L"Lib" : L"Display");
		break;
	case DB_LIBSIZE:
		libFilter(L"Lib Size: %d", L"Lib Size: %s", libNegative, 
			[&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->pLib->size());
			});
		break;
	case DB_LIBMEMUSE:
		libFilter(L"Auto Clean Cache: %s", L"Auto Clean Cache: %s", libNegative,
			[&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->pData->autoClean ? L"True" : L"False");
			});
		break;
	case DB_FROMLIB:
		displayFilter(L"From Lib: %s", L"From Lib: %s", displayNegative,
			[&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->fromLib ? L"True" : L"False");
			});
		break;
	case DB_FILENAME:
		displayFilter(L"FileName: %s", L"FileName: %s", displayNegative,
			[&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->FileName->c_str());
			});
		break;
	case DB_FILEPATH:
		displayFilter(L"FilePath: %s", L"FilePath: %s", displayNegative,
			[&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->FilePath->c_str());
			});
		break;
	case DB_KEY:
		displayFilter(L"Key: %s", L"Key: %s", displayNegative, 
			[&] (LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, *rdPtr->Key == L""
				? L"Not Encrypted"
				: rdPtr->Key->c_str());
			});
		break;
	case DB_LOADCALLBACK:
		displayFilter(L"Load Callback: %s", L"Load Callback: %s", displayNegative,
			[&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->bLoadCallback 
				? L"True"
				: L"False");
			});
		break;
	case DB_ANGLE:
		displayFilter(L"Angle: %f", L"Angle: %s", displayNegative,
			[&] (LPCWSTR pattern) {
				swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->angle);
			});
		break;
	case DB_STRETCHQUALITY:
		displayFilter(L"Stretch Quality: %s", L"Stretch Quality: %s", displayNegative,
			[&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, rdPtr->stretchQuality 
				? L"Resample"
				: L"Fast");
			});
		break;
	case DB_HOTSPOT:
		displayFilter(L"HotSpot: [ %d, %d ]", L"HotSpot: %s", displayNegative, 
			[&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, GetHotSpotX(rdPtr), GetHotSpotY(rdPtr));
			});
		break;
	case DB_ZOOMSCALE:
		displayFilter(L"Zoom Scale: [ %f, %f ]", L"Zoom Scale: %s", displayNegative, 
			[&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, GetXZoomScale(rdPtr), GetYZoomScale(rdPtr));
			});
		break;
	case DB_ORIGINSIZE:
		displayFilter(L"Origin Size: [ %d x %d ]", L"Origin Size: %s", displayNegative, 
			[&](LPCWSTR pattern) {
			swprintf_s(pBuffer, DB_BUFFERSIZE, pattern, GetOriginalWidth(rdPtr), GetOriginalHeight(rdPtr));
			});
		break;
	case DB_CURRENTSIZE:
		displayFilter(L"Current Size: [ %d x %d ]", L"Current Size: %s", displayNegative, 
			[&](LPCWSTR pattern) {
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


