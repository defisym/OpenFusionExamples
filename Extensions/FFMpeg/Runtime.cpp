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
#ifdef _CONSOLE
	AllocConsole();

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
#endif

	rdPtr->swidth = edPtr->swidth;
	rdPtr->sheight = edPtr->sheight;
	
	//rdPtr->pSf = new cSurface;
	//rdPtr->pFrame = new cSurface;
		
	rdPtr->pDisplaySf = nullptr;
	rdPtr->pGrabbedFrame = nullptr;	
	
	rdPtr->pReturnSf = nullptr;

	rdPtr->bHwa = edPtr->bHwa;

	rdPtr->bStretch = edPtr->bStretch;
	rdPtr->bPlayAfterLoad = edPtr->bPlayAfterLoad;

	rdPtr->pFilePath = new std::wstring;

	rdPtr->bLoop = edPtr->bLoop;
	rdPtr->bAccurateSeek = edPtr->bAccurateSeek;
	
	rdPtr->bOpen = false;
	rdPtr->bPlay = false;
	rdPtr->bPlayStateUpdated = false;

    rdPtr->threadCount = edPtr->threadCount;

	rdPtr->volume = 100;

	rdPtr->pFFMpeg = nullptr;
	rdPtr->pPreviousTimer = new Timer;

	rdPtr->pEncrypt = nullptr;

    rdPtr->bPm = PreMulAlpha(rdPtr);
    rdPtr->bChanged = true;
    rdPtr->bPositionSet = false;
    rdPtr->bResetDisplay = false;

	rdPtr->pRetStr = new std::wstring;

	rdPtr->bCache = edPtr->bCache;

	rdPtr->hwDeviceType = edPtr->hwDeviceType;
	rdPtr->bForceNoAudio = edPtr->bForceNoAudio;
    rdPtr->bCopyToTexture = edPtr->bCopyToTexture;
    rdPtr->bSharedHardWareDevice = edPtr->bSharedHardWareDevice;
    rdPtr->pD3DLocalHandler = new D3DLocalHandler{ (ID3D11Device*)GetD3DDevice(rdPtr) };

	rdPtr->pVideoOverrideCodecName = new std::string;
	rdPtr->pAudioOverrideCodecName = new std::string;

	rdPtr->atempo = ATEMPO_DEFAULT;

	if (GetExtUserData() == nullptr) {
		rdPtr->pData = new GlobalData;
		SetExtUserData(rdPtr->pData);

        // create it here instead of constructor to solve dependency
        rdPtr->pData->pD3DSharedHandler = new D3DSharedHandler{ (ID3D11Device*)GetD3DDevice(rdPtr), hInstLib };
	}
	else {
		rdPtr->pData = (GlobalData*)GetExtUserData();
	}	

	rdPtr->pData->Create(&rdPtr->pFFMpeg, &rdPtr->bForceNoAudio);

//#define LIST_DECODERS

#ifdef LIST_DECODERS
	auto ListDecoders = [] (AVMediaType type) {
		void* iter = nullptr;
		const AVCodec* codec = nullptr;

		while (codec = av_codec_iterate(&iter)) {
			if (av_codec_is_decoder(codec)) {
				if (codec->type == type) {
					OutputDebugStringA(codec->name);
					OutputDebugStringA("\n");
				}
			}
		}
	};

	OutputDebugStringA("Video\n\n");
	ListDecoders(AVMEDIA_TYPE_VIDEO);
	OutputDebugStringA("\n\n");

	OutputDebugStringA("Audio\n\n");
	ListDecoders(AVMEDIA_TYPE_AUDIO);
	OutputDebugStringA("\n\n");
#endif

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

	delete rdPtr->pDisplaySf;	
	delete rdPtr->pGrabbedFrame;

	delete rdPtr->pReturnSf;
		
	delete rdPtr->pPreviousTimer;

	rdPtr->pData->Destroy(&rdPtr->pFFMpeg);
	CloseGeneral(rdPtr);

	delete rdPtr->pFilePath;
    
    delete rdPtr->pD3DLocalHandler;

	delete rdPtr->pVideoOverrideCodecName;
	delete rdPtr->pAudioOverrideCodecName;

	delete rdPtr->pRetStr;	

	SetExtUserData(rdPtr->pData);

#ifdef _USE_CRTDBG
	_CrtDumpMemoryLeaks();
#endif

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

#ifdef _LOOPBENCH
	using namespace std::literals;

	auto now = std::chrono::steady_clock::now();
	auto duration = (now - *rdPtr->pPreviousTimer) / 1ms;

	*rdPtr->pPreviousTimer = now;

	std::wstring outPut = L"Duration: ";
	outPut += _itos((int)duration);

	OutputDebugString(outPut.c_str());
	OutputDebugString(L"\n");
#endif

	do {
		if (!rdPtr->bOpen) { break; }

		// update audio pause
		// only update state, pts is not updated
		rdPtr->pFFMpeg->set_pause(!rdPtr->bPlay, rdPtr->bPlayStateUpdated);

        // flag should be cancel when pause
        if (!rdPtr->bPlay && rdPtr->bPlayStateUpdated) {
            rdPtr->bPositionSet = false;
        }

		rdPtr->bPlayStateUpdated = false;

		if (!rdPtr->bPlay) { break; }
#ifdef _LOOPBENCH
		auto beforeDecode = std::chrono::steady_clock::now();
#endif
        // only one frame, do not need to get next frame, as there's no next frame
        // and finish state won't be updated
        if (VideoSingleFrame(rdPtr)) { break; }
        if (rdPtr->bPositionSet) { rdPtr->bPositionSet = false; break; }

		rdPtr->pFFMpeg->get_nextFrame([&] (const unsigned char* pData, const int stride, const int height) {
            CopyData(rdPtr, rdPtr->pDisplaySf, pData, stride, height);
			ReDisplay(rdPtr);
			});

		if (rdPtr->pFFMpeg->get_finishState()) {
			CallEvent(ON_FINISH);
		}

#ifdef _LOOPBENCH
			auto decodeDuration = (std::chrono::steady_clock::now() - beforeDecode) / 1ms;

			std::wstring outPut2 = L"Decode Duration: ";
			outPut2 += _itos((int)decodeDuration);

			OutputDebugString(outPut2.c_str());
			OutputDebugString(L"\n");

			if (rdPtr->pFFMpeg->get_finishState()) {
				OutputDebugString(L"======FINISH======\n");
			}
#endif

#ifdef _DEBUG		
		auto bFinish = rdPtr->pFFMpeg->get_finishState();

		auto totalTime = rdPtr->pFFMpeg->get_videoDuration();
		auto curTime = rdPtr->pFFMpeg->get_videoPosition();

		//assert(totalTime >= curTime);
#endif // _DEBUG
	} while (false);

	CleanCache(rdPtr, false);

	if (rdPtr->pDisplaySf != nullptr
		&& rdPtr->pDisplaySf->IsValid()
		&& rdPtr->rc.rcChanged) {
		return REFLAG_DISPLAY;
	}

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

	if (rdPtr->pDisplaySf != nullptr && rdPtr->pDisplaySf->IsValid()) {
        // handle ResetDisplay
        if (!GetVideoPlayState(rdPtr) && rdPtr->bResetDisplay) { return 0; }
        rdPtr->bResetDisplay = false;

		// Begin render process...
		LPSURFACE ps = WinGetSurface((int)rdPtr->rHo.hoAdRunHeader->rhIdEditWin);
		//int nDrv = ps->GetDriver();
		//bool HWA = nDrv >= SD_3DFX;

		// On-screen coords
		int screenX = rdPtr->rHo.hoX - rdPtr->rHo.hoAdRunHeader->rhWindowX;
		int screenY = rdPtr->rHo.hoY - rdPtr->rHo.hoAdRunHeader->rhWindowY;

		// Hot spot (transform center)
		POINT point = { 0, 0 };

		rdPtr->pDisplaySf->BlitEx(*ps, (float)screenX, (float)screenY,
			rdPtr->rc.rcScaleX, rdPtr->rc.rcScaleY, 0, 0,
			rdPtr->pDisplaySf->GetWidth(), rdPtr->pDisplaySf->GetHeight(), &point, rdPtr->rc.rcAngle,
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
short WINAPI DLLExport PauseRunObject(LPRDATA rdPtr) {
	if (rdPtr->bPlay && rdPtr->pFFMpeg != nullptr) {
		rdPtr->pFFMpeg->set_pause(true);
	}

	// Ok
	return 0;
}


// -----------------
// ContinueRunObject
// -----------------
// Quits the pause mode
//
short WINAPI DLLExport ContinueRunObject(LPRDATA rdPtr) {
	if (rdPtr->bPlay && rdPtr->pFFMpeg != nullptr) {
		rdPtr->pFFMpeg->set_pause(false);
	}

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
        // delete it here instead of destructor to solve dependency
        delete pData->pD3DSharedHandler;
        
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
	if (pData != NULL) {
        // delete it here instead of destructor to solve dependency
        delete pData->pD3DSharedHandler; 
        
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


