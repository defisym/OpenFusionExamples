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
	DB_DisplayString,
	DB_FilteredString,
};

// Items displayed in the debugger
WORD DebugTree[]=
{
	DB_DisplayString,
	DB_FilteredString,

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

#ifdef _CONSOLE
	AllocConsole();
	
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
#endif
	
	rdPtr->rHo.hoX = cobPtr->cobX;
	rdPtr->rHo.hoY = cobPtr->cobY;

	rdPtr->swidth = edPtr->swidth;
	rdPtr->sheight = edPtr->sheight;

	rdPtr->angle = 0.0;

	rdPtr->xScale = 1.0;
	rdPtr->yScale = 1.0;

	rdPtr->hotSpotPos = edPtr->hotSpotPos;

	int x = edPtr->hotSpotX;
	int y = edPtr->hotSpotY;

	UpdateHotSpot(rdPtr->hotSpotPos, rdPtr->swidth, rdPtr->sheight, x, y);

	rdPtr->hotSpotX = x;
	rdPtr->hotSpotY = y;

	ChangeScale(rdPtr);

	rdPtr->hWnd = rdPtr->rHo.hoAdRunHeader->rhHMainWin;
	
	rdPtr->dwColor = edPtr->dwColor = edPtr->dwColor = edPtr->dwColor;
	rdPtr->dwAlignFlags = edPtr->dwAlignFlags;
	rdPtr->logFont = edPtr->logFont;
	rdPtr->hFont = CreateFontIndirect(&edPtr->logFont);

	rdPtr->nOutLinePixel = edPtr->nOutLinePixel;
	rdPtr->dwOutLineColor = edPtr->dwOutLineColor;

	rdPtr->bRowSpace = edPtr->bRowSpace;
	rdPtr->bColSpace = edPtr->bColSpace;
	rdPtr->nRowSpace = edPtr->nRowSpace;
	rdPtr->nColSpace = edPtr->nColSpace;

	rdPtr->textRenderingHint = edPtr->textRenderingHint;
	rdPtr->smoothingMode = edPtr->smoothingMode;
	rdPtr->pixelOffsetMode = edPtr->pixelOffsetMode;

	rdPtr->bClip = edPtr->bClip;
	rdPtr->bClipToObject = edPtr->bClipToObject;
	rdPtr->borderOffsetX = edPtr->borderOffsetX;
	rdPtr->borderOffsetY = edPtr->borderOffsetY;

	rdPtr->bVerticalAlignOffset = edPtr->bVerticalAlignOffset;

	rdPtr->reRender = true;

	rdPtr->bStrChanged = true;
	rdPtr->pStr = new std::wstring(&edPtr->pText);
	
	rdPtr->bFontChanged = true;

	rdPtr->pIConObject = nullptr;
	rdPtr->pIConParamParser = new NeoStr::IConParamParser;

	rdPtr->pIConItName = new std::wstring;
	rdPtr->pIConParams = nullptr;
	rdPtr->pIConLibValue = nullptr;

	rdPtr->iConOffsetX = edPtr->iConOffsetX;
	rdPtr->iConOffsetY = edPtr->iConOffsetY;
	rdPtr->iConScale = edPtr->iConScale;
	rdPtr->bIConResample = edPtr->bIConResample;
	rdPtr->filterFlags = edPtr->filterFlags;
	rdPtr->bIConGlobal = edPtr->bIConGlobal;
	rdPtr->bIConForceUpdate = edPtr->bIConForceUpdate;

	rdPtr->bIConNeedUpdate = false;

	rdPtr->pRenderOptions = new NeoStr::RenderOptions;
	static_cast<NeoStr::RenderOptions*>(rdPtr->pRenderOptions)->UpdateTagCallback([=] (const std::wstring& callbackName, const std::vector<std::wstring>& callbackParams) {
		*rdPtr->pTagCallbackName = callbackName;
		rdPtr->pTagCallbackParams = &callbackParams;
		CallEvent(ONTAGCB);
		CallEvent(ONTAGCBF);
		rdPtr->pTagCallbackParams = nullptr;
	});

	rdPtr->pBlitOptions = new NeoStr::BlitOptions;

	rdPtr->bTagCallbackIndexManaged = true;
	rdPtr->pTagCallbackName = new std::wstring;
	rdPtr->pTagCallbackParams = nullptr;

	rdPtr->tabSize = edPtr->tabSize;
	rdPtr->bTabEM = edPtr->bTabEM;

	rdPtr->remarkOffsetX = edPtr->remarkOffsetX;
	rdPtr->remarkOffsetY = edPtr->remarkOffsetY;

	rdPtr->currentTriggerIndex = static_cast<size_t>(-1);

	rdPtr->pExpRet = new std::wstring;

	if (GetExtUserData() == nullptr) {
		//Update pointer
		rdPtr->pData = new GlobalData;
		SetExtUserData(rdPtr->pData);
	}else{
		rdPtr->pData = (GlobalData*)GetExtUserData();
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
#ifdef _CONSOLE
	FreeConsole();
#endif
	
	if (rdPtr->hFont != nullptr) {
		DeleteObject(rdPtr->hFont);
	}
	
	delete rdPtr->pStr;
	delete rdPtr->pNeoStr;

	delete static_cast<NeoStr::IConParamParser*>(rdPtr->pIConParamParser);
	delete rdPtr->pIConItName;

	if (rdPtr->pData->pIConData->pCaller == reinterpret_cast<LPRO>(rdPtr)) {
		rdPtr->pData->pIConData->ResetCaller();
	}

	delete static_cast<NeoStr::RenderOptions*>(rdPtr->pRenderOptions);
	delete rdPtr->pTagCallbackName;

	delete static_cast<NeoStr::BlitOptions*>(rdPtr->pBlitOptions);

	delete rdPtr->pFormatByVector;

	delete rdPtr->pExpRet;

	SetExtUserData(rdPtr->pData);

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
	// Will not be called next loop	
	//return REFLAG_ONESHOT;

#ifdef COUNT_GDI_OBJECT
	rdPtr->pData->objectCounter.UpdateObjectCount();
#endif

	// update caller state
	rdPtr->pData->pIConData->CheckCallerValidity();

	if (rdPtr->rc.rcChanged)
		return REFLAG_DISPLAY;
	else
		return 0;
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

	Display(rdPtr);
	
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
	auto pData = (GlobalData*)mV->mvGetExtUserData(pApp, hInstLib);
	if ( pData != NULL ) {
		delete pData;
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
	if ( pData != NULL ) {
		delete pData;
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


// Note: do not forget to enable the functions in the .def file 
// if you remove the comments below.

void WINAPI GetRunObjectFont(LPRDATA rdPtr, LOGFONT* pLf) {
	memcpy(pLf, &rdPtr->logFont, sizeof(LOGFONT));
}

// -------------------
// SetRunObjectFont
// -------------------
// Change the font used by the object.
// 
void WINAPI SetRunObjectFont(LPRDATA rdPtr, LOGFONT* pLf, RECT* pRc) {
	const HFONT hFont = CreateFontIndirect(pLf);

#ifdef _DEBUG
	LOGFONT Lf = { };
	GetObject(hFont, sizeof(LOGFONT), &Lf);
#endif // _DEBUG

	if ( hFont != nullptr) {
		if (rdPtr->hFont != nullptr) {
			DeleteObject(rdPtr->hFont);
		}

		rdPtr->logFont = *pLf;
		rdPtr->hFont = hFont;
		rdPtr->bFontChanged = true;

		//Seems doesn't need it here
		//SendMessage(rdPtr->hWnd, WM_SETFONT, (WPARAM)rdPtr->hFont, FALSE);
		//callRunTimeFunction(rdPtr, RFUNCTION_REDRAW, 0, 0);
	}
}

// ---------------------
// GetRunObjectTextColor
// ---------------------
// Return the text color of the object.
// 
COLORREF WINAPI GetRunObjectTextColor(LPRDATA rdPtr)
{
	return rdPtr->dwColor;
}

// ---------------------
// SetRunObjectTextColor
// ---------------------
// Change the text color of the object.
// 
void WINAPI SetRunObjectTextColor(LPRDATA rdPtr, COLORREF rgb)
{
	if (rdPtr->dwColor != rgb) {
		rdPtr->dwColor = rgb;
		rdPtr->reRender = true;
		InvalidateRect(rdPtr->hWnd, NULL, TRUE);
		//callRunTimeFunction(rdPtr, RFUNCTION_REDRAW, 0, 0);
	}
}


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
	auto getString = [] (const NeoStr* pNeoStr,const std::function<const wchar_t*()>& cb) {
		if (pNeoStr == nullptr) { return L"Object Not Initialized"; }

		const auto pStr = cb();
		return pStr ? pStr : L"No String";
		};
    auto copyString = [&pBuffer] (const std::wstring& result) {
        if (result.length() < DB_BUFFERSIZE) {
            memcpy(pBuffer, result.c_str(), sizeof(wchar_t) * result.length());
            pBuffer[result.length()] = L'\0';

            return;
        }

        constexpr auto turncated = L"(...turncated)";
        const auto turncatedLength = wcslen(turncated);

        const auto allowedLength = DB_BUFFERSIZE - turncatedLength - 1;
        memcpy(pBuffer, result.c_str(), sizeof(wchar_t) * allowedLength);
        memcpy(pBuffer + allowedLength, turncated, sizeof(wchar_t) * turncatedLength);
        pBuffer[DB_BUFFERSIZE] = L'\0';
        };

	switch (id) {
	case DB_DisplayString:
	{
		const auto result = std::format(L"Display String: {}",
			getString(rdPtr->pNeoStr, [&] () {return rdPtr->pNeoStr->GetRawText(); }));
        copyString(result);

		break;
	}
	case DB_FilteredString:
	{
		const auto result = std::format(L"Filtered String: {}",
			getString(rdPtr->pNeoStr, [&] () {return rdPtr->pNeoStr->GetText(); }));
        copyString(result);

		break;
	}
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


