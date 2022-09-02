
// ============================================================================
//
// The following routines are used internally by MMF, and should not need to
// be modified.
//
// 
// ============================================================================

// Common Include
#include	"common.h"

HINSTANCE hInstLib;

// ============================================================================
//
// LIBRARY ENTRY & QUIT POINTS
// 
// ============================================================================

// -----------------
// Entry points
// -----------------
// Usually you do not need to do any initialization here: you will prefer to 
// do them in "Initialize" found in Edittime.cpp
BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
		// DLL is attaching to the address space of the current process.
		case DLL_PROCESS_ATTACH:
			
			hInstLib = hDLL; // Store HINSTANCE
			break;

		// A new thread is being created in the current process.
		case DLL_THREAD_ATTACH:
			break;

		// A thread is exiting cleanly.
		case DLL_THREAD_DETACH:
			break;

		// The calling process is detaching the DLL from its address space.
	    case DLL_PROCESS_DETACH:
			break;
	}
	
	return TRUE;
}

// -----------------
// Initialize
// -----------------
// Where you want to do COLD-START initialization.
// Called when the extension is loaded into memory.
//
extern "C" int WINAPI DLLExport Initialize(mv _far *mV, int quiet)
{
	// No error
	return 0;
}

// -----------------
// Free
// -----------------
// Where you want to kill and initialized data opened in the above routine
// Called just before freeing the DLL.
// 
extern "C" int WINAPI DLLExport Free(mv _far *mV)
{
	// No error
	return 0;
}

// ============================================================================
//
// GENERAL INFO
// 
// ============================================================================

// -----------------
// Get Infos
// -----------------
// 
extern "C" 
{
	DWORD WINAPI DLLExport GetInfos(int info)
	{
		
		switch (info)
		{
			case KGI_VERSION:
				return EXT_VERSION2;				// Do not change
			case KGI_PLUGIN:
				return EXT_PLUGIN_VERSION1;			// Do not change
			case KGI_PRODUCT:
#if defined(PROEXT)
				return PRODUCT_VERSION_DEV;			// Works with MMF Developer 
#else
				return PRODUCT_VERSION_STANDARD;	// Works with MMF Standard or above
#endif
			case KGI_BUILD:
				return MINBUILD;					// Works with build MINBUILD or above
			case KGI_UNICODE:
				#ifdef _UNICODE
					return TRUE;
				#else
					return FALSE;
				#endif
			default:
				return 0;
		}
	}
}

// ----------------------------------------------------------
// GetRunObjectInfos
// ----------------------------------------------------------
// Fills an information structure that tells CC&C everything
// about the object, its actions, conditions and expressions
// 

short WINAPI DLLExport GetRunObjectInfos(mv _far *mV, fpKpxRunInfos infoPtr)
{
	infoPtr->conditions = (LPBYTE)ConditionJumps;
	infoPtr->actions = (LPBYTE)ActionJumps;
	infoPtr->expressions = (LPBYTE)ExpressionJumps;

	infoPtr->numOfConditions = CND_LAST;
	infoPtr->numOfActions = ACT_LAST;
	infoPtr->numOfExpressions = EXP_LAST;

	infoPtr->editDataSize = MAX_EDITSIZE;
	infoPtr->editFlags= OEFLAGS;

	infoPtr->windowProcPriority = WINDOWPROC_PRIORITY;

	// See doc
	infoPtr->editPrefs = OEPREFS;

	// Identifier, for run-time identification
	infoPtr->identifier = IDENTIFIER;
	
	// Current version
	infoPtr->version = KCX_CURRENT_VERSION;
	
	return TRUE;
}

// ----------------------------------------------------------
// GetDependencies
// ----------------------------------------------------------
// Returns the name of the external modules that you wish MMF to include
// with stand-alone applications (these modules must be in the MMF
// Data\Runtime folder).
// 

//LPCTSTR szDep[] = {
//	_T("MyDll.dll"),
//	NULL
//};

LPCTSTR* WINAPI DLLExport GetDependencies()
{
	return NULL;	// szDep;
}

// -----------------
// LoadObject
// -----------------
// Routine called for each object when the object is read from the MFA file (edit time)
// or from the CCN or EXE file (run time).
// You can load data here, reserve memory etc...
//
int	WINAPI DLLExport LoadObject(mv _far *mV, LPCSTR fileName, LPEDATA edPtr, int reserved)
{
	return 0;
}

// -----------------
// UnloadObject
// -----------------
// The counterpart of the above routine: called just before the object is
// deleted from the frame.
//
void WINAPI DLLExport UnloadObject(mv _far *mV, LPEDATA edPtr, int reserved)
{
}

// --------------------
// UpdateEditStructure
// --------------------
// For you to update your object structure to newer versions
// Called at both edit time and run time
// 
HGLOBAL WINAPI DLLExport UpdateEditStructure(mv __far *mV, void __far * OldEdPtr)
{
	//if (LPEDATA(OldEdPtr)->hotSpotPos<HotSpotPos::LT || LPEDATA(OldEdPtr)->hotSpotPos > HotSpotPos::CUSTOM) {
	//	LPEDATA(OldEdPtr)->hotSpotPos = HotSpotPos::LT;
	//	LPEDATA(OldEdPtr)->hotSpotX = 0;
	//	LPEDATA(OldEdPtr)->hotSpotY = 0;
	//}

	//MSGBOX(L"Check Update", L"UPDATE");

	//auto oldPtr = LPEDATA(OldEdPtr);

	//HGLOBAL hgNew = NULL;

	//auto bfSz = wcslen(&oldPtr->pText) + 1;
	//DWORD dwNewSize = sizeof(EDITDATA) + bfSz * sizeof(wchar_t);

	////MSGBOX(L"SZ: " + _itos(dwNewSize), L"UPDATE");

	//if ((hgNew = GlobalAlloc(GPTR, dwNewSize)) != NULL) {
	//	tagEDATA_V1* newEdPtr = (tagEDATA_V1*)GlobalLock(hgNew);

	//	if (newEdPtr != nullptr) {
	//		MSGBOX(L"Copy", L"UPDATE");

	//		memcpy(newEdPtr, oldPtr, dwNewSize);

	//		newEdPtr->borderOffsetX = DEFAULEBORDEROFFSET;
	//		newEdPtr->borderOffsetY = DEFAULEBORDEROFFSET;
	//	}

	//	GlobalUnlock(hgNew);
	//}

	////MSGBOX(L"Done", L"UPDATE");

	//return hgNew;

	//MSGBOX(L"Check Update", L"UPDATE");

	//auto oldPtr = LPEDATA(OldEdPtr);

	//if (oldPtr->hotSpotPos < HotSpotPos::LT || oldPtr->hotSpotPos > HotSpotPos::CUSTOM) {
	//	MSGBOX(L"Need Update", L"UPDATE");

	//	HGLOBAL hgNew = NULL;

	//	auto bfSz = wcslen(&oldPtr->pText) + 1;
	//	DWORD dwNewSize = sizeof(EDITDATA) + bfSz * sizeof(wchar_t);

	//	MSGBOX(L"SZ: "+_itos(dwNewSize), L"UPDATE");

	//	if ((hgNew = GlobalAlloc(GPTR, dwNewSize)) != NULL) {
	//		tagEDATA_V1* newEdPtr = (tagEDATA_V1*)GlobalLock(hgNew);

	//		if (newEdPtr != nullptr) {
	//			MSGBOX(L"Copy", L"UPDATE");

	//			memcpy(newEdPtr, oldPtr, dwNewSize);

	//			newEdPtr->hotSpotPos = HotSpotPos::LT;
	//			newEdPtr->hotSpotX = 0;
	//			newEdPtr->hotSpotY = 0;
	//		}

	//		GlobalUnlock(hgNew);
	//	}

	//	MSGBOX(L"Done", L"UPDATE");

	//	return hgNew;
	//}

	//return 0;

	//HGLOBAL hgNew = NULL;
	//
	//if ((hgNew = GlobalAlloc(GPTR, sizeof(tagEDATA_V1))) != NULL) {
	//	tagEDATA_V1* newEdPtr = (tagEDATA_V1*)GlobalLock(hgNew);

	//	if (newEdPtr != nullptr) {
	//		memcpy(&newEdPtr->eHeader, &((tagEDATA_V1*)OldEdPtr)->eHeader, sizeof(extHeader));
	//		newEdPtr->eHeader.extSize = sizeof(tagEDATA_V1);				// Update the EDITDATA structure size

	//		newEdPtr->swidth = ((tagEDATA_V1*)OldEdPtr)->swidth;
	//		newEdPtr->sheight = ((tagEDATA_V1*)OldEdPtr)->sheight;

	//		newEdPtr->dwColor = ((tagEDATA_V1*)OldEdPtr)->dwColor;
	//		newEdPtr->dwAlignFlags = ((tagEDATA_V1*)OldEdPtr)->dwAlignFlags;
	//		newEdPtr->logFont = ((tagEDATA_V1*)OldEdPtr)->logFont;

	//		newEdPtr->nOutLinePixel = ((tagEDATA_V1*)OldEdPtr)->nOutLinePixel;
	//		newEdPtr->dwOutLineColor = ((tagEDATA_V1*)OldEdPtr)->dwOutLineColor;

	//		newEdPtr->bShadow = ((tagEDATA_V1*)OldEdPtr)->bShadow;
	//		newEdPtr->nShadowOffsetX = ((tagEDATA_V1*)OldEdPtr)->nShadowOffsetX;
	//		newEdPtr->nShadowOffsetY = ((tagEDATA_V1*)OldEdPtr)->nShadowOffsetY;

	//		newEdPtr->bRowSpace = ((tagEDATA_V1*)OldEdPtr)->bRowSpace;
	//		newEdPtr->bColSpace = ((tagEDATA_V1*)OldEdPtr)->bColSpace;
	//		newEdPtr->nRowSpace = ((tagEDATA_V1*)OldEdPtr)->nRowSpace;
	//		newEdPtr->nColSpace = ((tagEDATA_V1*)OldEdPtr)->nColSpace;

	//		newEdPtr->textRenderingHint = 0;
	//		newEdPtr->smoothingMode = 0;
	//		newEdPtr->pixelOffsetMode = 0;

	//		newEdPtr->bClip = true;

	//		memset(&newEdPtr->buffer, 0, 24 * sizeof(int));

	//		newEdPtr->pText = L'\0';

	//	}

	//	GlobalUnlock(hgNew);
	//}
	//
	//return hgNew;

	// We do nothing here
	return 0;
}

// --------------------
// UpdateFileNames
// --------------------
// If you store file names in your datazone, they have to be relocated when the
// application is moved to a different directory: this routine does it.
// Called at edit time and run time.
//
// Call lpfnUpdate to update your file pathname (refer to the documentation)
// 
void WINAPI DLLExport UpdateFileNames(mv _far *mV, LPTSTR appName, LPEDATA edPtr, void (WINAPI * lpfnUpdate)(LPTSTR, LPTSTR))
{
}

// ---------------------
// EnumElts
// ---------------------
//
// Uncomment this function if you need to store an image in the image bank.
//
// Note: do not forget to enable the function in the .def file 
// if you remove the comments below.
//
/*
int WINAPI DLLExport EnumElts (mv __far *mV, LPEDATA edPtr, ENUMELTPROC enumProc, ENUMELTPROC undoProc, LPARAM lp1, LPARAM lp2)
{  
	int error = 0;

	// Replace wImgIdx with the name of the WORD variable you create within the edit structure
  
	// Enum images  
	if ( (error = enumProc(&edPtr->wImgIdx, IMG_TAB, lp1, lp2)) != 0 )
	{
		// Undo enum images      
		undoProc (&edPtr->wImgIdx, IMG_TAB, lp1, lp2);    
	}  

	return error;
}
*/

