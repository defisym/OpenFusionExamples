
// ============================================================================
//
// This file contains routines that are handled only during the Edittime,
// under the Frame and Event editors.
//
// Including creating, display, and setting up your object.
// 
// ============================================================================

// Common
#include	"common.h"

#if !defined(RUN_ONLY)

// PROPERTIES /////////////////////////////////////////////////////////////////

// Property identifiers
enum {
	PROPID_SETTINGS = PROPID_EXTITEM_CUSTOM_FIRST,

// Example
// -------
//	PROPID_TEXTTITLE,	
//	PROPID_TEXT,	
//	PROPID_CHECK,
//	PROPID_COMBO,
//	PROPID_COLOR,

	PROPID_TEXT_TITLE,
	PROPID_TEXT,

	PROPID_ALLIGN_VERTICALOFFSET,
	PROPID_ALLIGN_ROWSPACE,
	PROPID_ALLIGN_COLSPACE,

	PROPID_OUTLINE_PIXEL,
	PROPID_OUTLINE_COLOR,
	
	PROPID_RENDER_TITLE,
	PROPID_RENDER_CLIP,	
	PROPID_RENDER_CLIPTOOBJECT,	
	PROPID_RENDER_BorderOffsetX,	
	PROPID_RENDER_BorderOffsetY,	
	PROPID_RENDER_TextRenderingHint,
	PROPID_RENDER_SmoothingMode,
	PROPID_RENDER_PixelOffsetMode,

	PROPID_HOTSPOT_TEXTTITLE,
	PROPID_HOTSPOT_DEFAULT,
	PROPID_HOTSPOT_X,
	PROPID_HOTSPOT_Y,

	PROPID_FORMAT_TEXTTITLE,

	PROPID_FORMAT_FILTER_UNKNOWN,
	PROPID_FORMAT_FILTER_INCOMPLETE,

	PROPID_FORMAT_ICONGLOBAL,
	PROPID_FORMAT_ICONGLOBALFORCEUPDATE,
	PROPID_FORMAT_ICONOFFSETX,
	PROPID_FORMAT_ICONOFFSETY,
	PROPID_FORMAT_ICONSCALE,
	PROPID_FORMAT_ICONRESAMPLE,

	PROPID_ALLIGN_TABSIZE,
	PROPID_ALLIGN_TABEMSPACE,

	PROPID_FORMAT_REMARKOFFSETX,
	PROPID_FORMAT_REMARKOFFSETY,
};

// Example of content of the PROPID_COMBO combo box
//LPCSTR ComboList[] = {
//	0,	// reserved
//	MAKEINTRESOURCE(IDS_FIRSTOPTION),	
//	MAKEINTRESOURCE(IDS_SECONDOPTION),	
//	MAKEINTRESOURCE(IDS_THIRDOPTION),	
//	NULL
//};

LPCWSTR TextRenderingHint_ComboList[] = {
	0,	// reserved
	
	MAKEINTRESOURCE(COMBO_TextRenderingHintSystemDefault),
	MAKEINTRESOURCE(COMBO_TextRenderingHintSingleBitPerPixelGridFit),
	MAKEINTRESOURCE(COMBO_TextRenderingHintSingleBitPerPixel),
	MAKEINTRESOURCE(COMBO_TextRenderingHintAntiAliasGridFit),
	MAKEINTRESOURCE(COMBO_TextRenderingHintAntiAlias),
	
	NULL
};

LPCWSTR SmoothingMode_ComboList[] = {
	0,	// reserved
	
	MAKEINTRESOURCE(COMBO_SmoothingModeInvalid),
	MAKEINTRESOURCE(COMBO_SmoothingModeDefault),
	MAKEINTRESOURCE(COMBO_SmoothingModeHighSpeed),
	MAKEINTRESOURCE(COMBO_SmoothingModeHighQuality),
	MAKEINTRESOURCE(COMBO_SmoothingModeNone),
	MAKEINTRESOURCE(COMBO_SmoothingModeAntiAlias),
	
	NULL
};

LPCWSTR PixelOffsetMode_ComboList[] = {
	0,	// reserved

	MAKEINTRESOURCE(COMBO_PixelOffsetModeInvalid),
	MAKEINTRESOURCE(COMBO_PixelOffsetModeDefault),
	MAKEINTRESOURCE(COMBO_PixelOffsetModeHighSpeed),
	MAKEINTRESOURCE(COMBO_PixelOffsetModeHighQuality),
	MAKEINTRESOURCE(COMBO_PixelOffsetModeNone),
	MAKEINTRESOURCE(COMBO_PixelOffsetModeHalf),
	
	NULL
};

LPCWSTR HotSpotComboList[] = {
	0,	// reserved
	MAKEINTRESOURCE(COMBO_HOTSPOT_LT),
	MAKEINTRESOURCE(COMBO_HOTSPOT_LM),
	MAKEINTRESOURCE(COMBO_HOTSPOT_LB),
	MAKEINTRESOURCE(COMBO_HOTSPOT_MT),
	MAKEINTRESOURCE(COMBO_HOTSPOT_MM),
	MAKEINTRESOURCE(COMBO_HOTSPOT_MB),
	MAKEINTRESOURCE(COMBO_HOTSPOT_RT),
	MAKEINTRESOURCE(COMBO_HOTSPOT_RM),
	MAKEINTRESOURCE(COMBO_HOTSPOT_RB),
	MAKEINTRESOURCE(COMBO_HOTSPOT_CUSTOM),
	NULL
};

// Property definitions
//
// Type, ID, Text, Text of Info box [, Options, Init Param]
//
PropData Properties[] = {

// Example
// -------
//	PropData_Group		(PROPID_TEXTTITLE,	IDS_PROP_TEXTTITLE,		IDS_PROP_TEXTTITLE),
//	PropData_EditString	(PROPID_TEXT,		IDS_PROP_TEXT,			IDS_PROP_TEXT_INFO),
//	PropData_CheckBox	(PROPID_CHECK,		IDS_PROP_CHECK,			IDS_PROP_CHECK_INFO),
//	PropData_ComboBox	(PROPID_COMBO,		IDS_PROP_COMBO,			IDS_PROP_COMBO,	ComboList),
//	PropData_Color		(PROPID_COLOR,		IDS_PROP_COLOR,			IDS_PROP_COLOR_INFO),

	PropData_Group		(PROPID_TEXT_TITLE,	IDS_PROP_TEXTTITLE,		IDS_PROP_TEXTTITLE),
	PropData_EditMultiLine(PROPID_TEXT,		IDS_PROP_TEXT,			IDS_PROP_TEXT_INFO),
	
	// End of table (required)
	PropData_End()
};

PropData DisplayProperties[] = {
	PropData_Group(PROPID_HOTSPOT_TEXTTITLE,	IDS_HOTSPOT_TEXTTITLE,		IDS_HOTSPOT_TEXTTITLE),
	PropData_ComboBox(PROPID_HOTSPOT_DEFAULT,		IDS_PROP_HOTSPOT_DEFAULT,			IDS_PROP_HOTSPOT_DEFAULT_INFO,			HotSpotComboList),
	PropData_EditNumber(PROPID_HOTSPOT_X,		IDS_PROP_HOTSPOT_X,			IDS_PROP_HOTSPOT_X_INFO),
	PropData_EditNumber(PROPID_HOTSPOT_Y,		IDS_PROP_HOTSPOT_Y,			IDS_PROP_HOTSPOT_Y_INFO),

	// End of table (required)
	PropData_End()
};

// MinMax: min, max, step
float MinMaxFloat[] = { -2.0f, 2.0f, 0.01f };

PropData FontPorperties[] = {
	PropData_CheckBox(PROPID_ALLIGN_VERTICALOFFSET,	IDS_PROP_ALLIGN_VERTICALOFFSET,		IDS_PROP_ALLIGN_VERTICALOFFSET_INFO),
	//PropData_EditNumber_Check(PROPID_ALLIGN_ROWSPACE,		IDS_PROP_ALLIGN_ROWSPACE,			IDS_PROP_ALLIGN_ROWSPACE_INFO),
	//PropData_EditNumber_Check(PROPID_ALLIGN_COLSPACE,		IDS_PROP_ALLIGN_COLSPACE,			IDS_PROP_ALLIGN_COLSPACE_INFO),
	PropData_EditNumber(PROPID_ALLIGN_ROWSPACE,		IDS_PROP_ALLIGN_ROWSPACE,			IDS_PROP_ALLIGN_ROWSPACE_INFO),
	PropData_EditNumber(PROPID_ALLIGN_COLSPACE,		IDS_PROP_ALLIGN_COLSPACE,			IDS_PROP_ALLIGN_COLSPACE_INFO),
	PropData_EditNumber(PROPID_ALLIGN_TABSIZE,		IDS_PROP_ALLIGN_TABSIZE,			IDS_PROP_ALLIGN_TABSIZE_INFO),
	PropData_CheckBox(PROPID_ALLIGN_TABEMSPACE,		IDS_PROP_ALLIGN_TABEMSPACE,			IDS_PROP_ALLIGN_TABEMSPACE_INFO),

	//PropData_EditNumber(PROPID_OUTLINE_PIXEL,		IDS_PROP_OUTLINE_PIXEL,			IDS_PROP_OUTLINE_PIXEL_INFO),
	//PropData_Color(PROPID_OUTLINE_COLOR,		IDS_PROP_OUTLINE_COLOR,			IDS_PROP_OUTLINE_COLOR_INFO),

	PropData_Group(PROPID_RENDER_TITLE,	IDS_PROP_RENDER_TITLE,		IDS_PROP_RENDER_TITLE),
	
	PropData_CheckBox(PROPID_RENDER_CLIP,	IDS_PROP_RENDER_CLIP,		IDS_PROP_RENDER_CLIP_INFO),
	PropData_CheckBox(PROPID_RENDER_CLIPTOOBJECT,	IDS_PROP_RENDER_CLIPTOOBJECT,		IDS_PROP_RENDER_CLIPTOOBJECT_INFO),
	
	PropData_EditNumber(PROPID_RENDER_BorderOffsetX,	IDS_PROP_RENDER_BorderOffsetX,		IDS_PROP_RENDER_BorderOffsetX_INFO),
	PropData_EditNumber(PROPID_RENDER_BorderOffsetY,	IDS_PROP_RENDER_BorderOffsetY,		IDS_PROP_RENDER_BorderOffsetY_INFO),
	
	PropData_ComboBox(PROPID_RENDER_TextRenderingHint,	IDS_PROP_RENDER_TextRenderingHint,		IDS_PROP_RENDER_TextRenderingHint_INFO, TextRenderingHint_ComboList),
	PropData_ComboBox(PROPID_RENDER_SmoothingMode,	IDS_PROP_RENDER_SmoothingMode,		IDS_PROP_RENDER_SmoothingMode_INFO, SmoothingMode_ComboList),
	PropData_ComboBox(PROPID_RENDER_PixelOffsetMode,	IDS_PROP_RENDER_PixelOffsetMode,		IDS_PROP_RENDER_PixelOffsetMode_INFO, PixelOffsetMode_ComboList),
	
	PropData_Group(PROPID_FORMAT_TEXTTITLE,	IDS_PROP_FORMAT_TITLE,		IDS_PROP_FORMAT_TITLE),

	PropData_CheckBox(PROPID_FORMAT_FILTER_UNKNOWN,	IDS_PROP_FORMAT_FILTER_UNKNOWN,		IDS_PROP_FORMAT_FILTER_UNKNOWN_INFO),
	PropData_CheckBox(PROPID_FORMAT_FILTER_INCOMPLETE,	IDS_PROP_FORMAT_FILTER_INCOMPLETE,		IDS_PROP_FORMAT_FILTER_INCOMPLETE_INFO),

	PropData_CheckBox(PROPID_FORMAT_ICONGLOBAL,	IDS_PROP_FORMAT_ICONGLOBAL,		IDS_PROP_FORMAT_ICONGLOBAL_INFO),
	PropData_CheckBox(PROPID_FORMAT_ICONGLOBALFORCEUPDATE,	IDS_PROP_FORMAT_ICONGLOBALFR,		IDS_PROP_FORMAT_ICONGLOBALFR_INFO),

	PropData_CheckBox(PROPID_FORMAT_ICONRESAMPLE,	IDS_PROP_FORMAT_ICONRESAMPLE,		IDS_PROP_FORMAT_ICONRESAMPLE_INFO),

	PropData_SpinEditFloat(PROPID_FORMAT_ICONOFFSETX,	IDS_PROP_FORMAT_ICONOFFSETX,		IDS_PROP_FORMAT_ICONOFFSETX_INFO, &MinMaxFloat),
	PropData_SpinEditFloat(PROPID_FORMAT_ICONOFFSETY,	IDS_PROP_FORMAT_ICONOFFSETY,		IDS_PROP_FORMAT_ICONOFFSETY_INFO, &MinMaxFloat),

	PropData_SpinEditFloat(PROPID_FORMAT_ICONSCALE,	IDS_PROP_FORMAT_ICONSCALE,		IDS_PROP_FORMAT_ICONSCALE_INFO, &MinMaxFloat),

	PropData_SpinEditFloat(PROPID_FORMAT_REMARKOFFSETX,	IDS_PROP_FORMAT_REMARKOFFSETX,		IDS_PROP_FORMAT_REMARKOFFSETX_INFO, &MinMaxFloat),
	PropData_SpinEditFloat(PROPID_FORMAT_REMARKOFFSETY,	IDS_PROP_FORMAT_REMARKOFFSETY,		IDS_PROP_FORMAT_REMARKOFFSETY_INFO, &MinMaxFloat),
		
	// End of table (required)
	PropData_End()
};

//// SETUP PROC /////////////////////////////////////////////////////////////////

//// Prototype of setup procedure
//BOOL CALLBACK DLLExport setupProc(HWND hDlg,uint msgType,WPARAM wParam,LPARAM lParam);

//// Structure defined to pass edptr and mv into setup box
//typedef struct tagSetP
//{
//	EDITDATA _far *	edpt;
//	mv _far	*		kv;
//} setupParams;

#endif // !defined(RUN_ONLY)


// -----------------
// GetObjInfos
// -----------------
// Return object info
//
// Info displayed in the object's About properties
// Note: ObjComment is also displayed in the Quick Description box in the Insert Object dialog box
//
void WINAPI	DLLExport GetObjInfos (mv _far *mV, LPEDATA edPtr, LPTSTR ObjName, LPTSTR ObjAuthor, LPTSTR ObjCopyright, LPTSTR ObjComment, LPTSTR ObjHttp)
{
#ifndef RUN_ONLY
	// Name
	LoadString(hInstLib, IDST_OBJNAME,ObjName, 255);

	// Author
	LoadString(hInstLib, IDST_AUTHOR,ObjAuthor,255);

	// Copyright
	LoadString(hInstLib, IDST_COPYRIGHT,ObjCopyright,255);

	// Comment
	LoadString(hInstLib, IDST_COMMENT,ObjComment,1024);

	// Internet address
	LoadString(hInstLib, IDST_HTTP,ObjHttp,255);
#endif // !defined(RUN_ONLY)
}

// -----------------
// GetHelpFileName
// -----------------
// Returns the help filename of the object.
//
LPCTSTR WINAPI GetHelpFileName()
{
#ifndef RUN_ONLY
	// Return a file without path if your help file can be loaded by the MMF help file.
//	return "MyExt.chm";

	// Or return the path of your file, relatively to the MMF directory
	// if your file is not loaded by the MMF help file.
	return _T("Help\\NeoStr\\NeoStr.md");
#else
	return NULL;
#endif // !defined(RUN_ONLY)
}

// -----------------
// BmpToImg
// -----------------
// Converts an image from the resource to an image displayable under CC&C
// Not used in this template, but it is a good example on how to create
// an image.
//
/*
WORD BmpToImg(int bmID, npAppli idApp, short HotX = 0, short HotY = 0, short ActionX = 0, short ActionY = 0)
{
	Img					ifo;
	WORD				img;
	HRSRC				hs;
	HGLOBAL				hgBuf;
	LPBYTE				adBuf;
	LPBITMAPINFOHEADER	adBmi;

	img = 0;
	if ((hs = FindResource(hInstLib, MAKEINTRESOURCE(bmID), RT_BITMAP)) != NULL)
	{
		if ((hgBuf = LoadResource(hInstLib, hs)) != NULL)
		{
			if ((adBuf = (LPBYTE)LockResource(hgBuf)) != NULL)
			{
				adBmi = (LPBITMAPINFOHEADER)adBuf;
				ifo.imgXSpot = HotX;
				ifo.imgYSpot = HotY;
				ifo.imgXAction = ActionX;
				ifo.imgYAction = ActionY;
				if (adBmi->biBitCount > 4)
					RemapDib((LPBITMAPINFO)adBmi, idApp, NULL);
				img = (WORD)DibToImage(idApp, &ifo, adBmi);
				UnlockResource(hgBuf);
			}
			FreeResource(hgBuf);
		}
	}
	return img;
}
*/

// ============================================================================
//
// ROUTINES USED UNDER FRAME EDITOR
// 
// ============================================================================


// --------------------
// MakeIcon
// --------------------
// Called once object is created or modified, just after setup.
//
// Note: this function is optional. If it's not defined in your extension,
// MMF2 will load the EXO_ICON bitmap if it's defined in your resource file.
//
// If you need to draw the icon manually, remove the comments around this function and in the .def file.
//
/*
int WINAPI DLLExport MakeIconEx ( mv _far *mV, cSurface* pIconSf, LPTSTR lpName, fpObjInfo oiPtr, LPEDATA edPtr )
{
	int error = -1;
#ifndef RUN_ONLY
	if ( pIconSf->LoadImage(hInstLib, EXO_ICON) != 0 )
		error = 0;
#endif // !defined(RUN_ONLY)
	return error;
}
*/

// --------------------
// SetupProc
// --------------------
// This routine is yours. You may even not need a setup dialog box.
// I have put it as an example...

//#ifndef RUN_ONLY
//
//BOOL CALLBACK DLLExport setupProc(HWND hDlg,uint msgType,WPARAM wParam,LPARAM lParam)
//{
//	setupParams	_far *	spa;
//	EDITDATA _far *		edPtr;
//
//	switch (msgType)
//	{
//	case WM_INITDIALOG: // Init dialog
//		SetWindowLong(hDlg, DWL_USER, lParam);
//		spa = (setupParams far *)lParam;
//		edPtr = spa->edpt;
//
//		/*
//			Insert your code to initalise the dialog!
//			Try the following code snippets:
//
//			** Change an editbox's text:
//
//			SetDlgItemText(hDlg, IDC_YOUR_EDITBOX_ID, edPtr->YourTextVariable);
//
//			** (Un)check a checkbox:
//
//			CheckDlgButton(hDlg, IDC_YOUR_CHECKBOX_ID,
//				edPtr->YourBooleanValue ? BST_CHECKED : BST_UNCHECKED);
//			
//			** If the variable is not of type 'bool' then include a comparison
//			** before the question mark (conditional operator):
//
//			CheckDlgButton(hDlg, IDC_YOUR_CHECKBOX_ID,
//				edPtr->YourLongValue == 1 ? BST_CHECKED : BST_UNCHECKED);
//
//			** Check a radio button, deselecting the others at the same time
//
//			CheckRadioButton(hDlg, IDC_FIRST_RADIO_IN_GROUP, IDC_LAST_RADIO_IN_GROUP, IDC_RADIO_TO_CHECK);
//
//			** You should know how to add radio buttons properly in MSVC++'s dialog editor first...
//			** Make sure to add radiobuttons in order, and use the 'Group' property to signal a new group
//			** of radio buttons.
//
//			** Disable a control. Replace 'FALSE' with 'TRUE' to enable the control:
//
//			EnableWindow(GetDlgItem(hDlg, IDC_YOUR_CONTROL_ID), FALSE);
//		*/
//		
//		return TRUE;
//
//	case WM_COMMAND: // Command
//		spa = (setupParams far *)GetWindowLong(hDlg, DWL_USER);
//		edPtr = spa->edpt;
//
//		switch (wmCommandID)
//		{
//		case IDOK:
//			/*
//				The user has pressed OK! Save our data with the following commands:
//
//				** Get text from an editbox. There is a limit to how much you can retrieve,
//				** make sure this limit is reasonable and your variable can hold this data.
//				** (Replace 'MAXIMUM_TEXT_LENGTH' with a value or defined constant!)
//
//				GetDlgItemText(hDlg, IDC_YOUR_EDITBOX_ID, edPtr->YourTextVariable, MAXIMUM_TEXT_LENGTH);
//
//				** Check if a checkbox or radiobutton is checked. This is the basic code:
//
//				(IsDlgButtonChecked(hDlg, IDC_YOUR_CHECKBOX_ID)==BST_CHECKED)
//
//				** This will return true if checked, false if not.
//				** If your variable is a bool, set it to this code
//				** If not, use an if statement or the conditional operator
//
//				if (IsDlgButtonChecked(hDlg, IDC_YOUR_CHECKBOX_ID)==BST_CHECKED)
//					edPtr->YourLongValue = 100;
//				else
//					edPtr->YourLongValue = 50;
//			*/
//
//			// Close the dialog
//			EndDialog(hDlg, IDOK);
//			return 0;
//
//		case IDCANCEL:
//			// User pressed cancel, don't save anything
//			// Close the dialog
//			EndDialog(hDlg, IDCANCEL);
//			return 0;
//
//		case ID_HELP:
//			{
//				// Call the mvHelp function
//				//
//				spa->kv->mvHelp(GetHelpFileName(), 0 /*HH_DISPLAY_TOPIC*/, NULL /*(LPARAM)"index.html"*/);
//			}
//			return 0;
//
//		/*
//			If you have a button or checkbox which, when clicked, will change
//			something on the dialog, add them like so:
//
//		case IDC_YOUR_CLICKED_CONTROL:
//			// your code here
//			return 0;
//
//			You can use any of the commands added previously, (including the Help code,)
//			but it's a good idea NOT to save data to edPtr until the user presses OK.
//		*/
//
//		default:
//			break;
//		}
//		break;
//
//	default:
//		break;
//	}
//	return FALSE;
//}
//
//#endif // !defined(RUN_ONLY)

// --------------------
// CreateObject
// --------------------
// Called when you choose "Create new object". It should display the setup box 
// and initialize everything in the datazone.

int WINAPI DLLExport CreateObject(mv _far *mV, fpLevObj loPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
	{
		// Set default object settings
		edPtr->swidth = 150;
		edPtr->sheight = 16;

		// Default alignment
		edPtr->dwAlignFlags = DT_LEFT|DT_TOP;

		// Default text color
		edPtr->dwColor = RGB(0, 0, 0);

		edPtr->nOutLinePixel = 0;
		edPtr->dwOutLineColor = RGB(0, 0, 0);

		edPtr->bRowSpace = false;
		edPtr->bColSpace = false;
		edPtr->nRowSpace = 0;
		edPtr->nColSpace = 0;

		// Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias
		edPtr->textRenderingHint = 4;
		// Gdiplus::SmoothingMode::SmoothingModeHighQuality
		edPtr->smoothingMode = 3;
		// Gdiplus::PixelOffsetMode::PixelOffsetModeHalf
		edPtr->pixelOffsetMode = 5;

		edPtr->bClip = true;
		edPtr->bClipToObject = false;

		edPtr->borderOffsetX = DEFAULT_EBORDER_OFFSET;
		edPtr->borderOffsetY = DEFAULT_EBORDER_OFFSET;

		edPtr->bVerticalAlignOffset = false;

		edPtr->hotSpotPos = HotSpotPos::LT;

		edPtr->hotSpotX = 0;
		edPtr->hotSpotY = 0;

		edPtr->iConOffsetX = 0;
		edPtr->iConOffsetY = 0;
		edPtr->iConScale = 1.0;
		edPtr->bIConResample = false;
		edPtr->filterFlags = FORMAT_IGNORE_DEFAULTFLAG;
		edPtr->bIConGlobal = false;
		edPtr->bIConForceUpdate = false;

		edPtr->tabSize = 4;
		edPtr->bTabEM = false;

		edPtr->remarkOffsetX = 0;
		edPtr->remarkOffsetY = 0;

		// Default font
		if (mV->mvGetDefaultFont != NULL) {
			mV->mvGetDefaultFont(&edPtr->logFont, NULL, 0);		// Get default font from frame editor preferences
		}
		else {
			edPtr->logFont.lfWidth = 0;
			edPtr->logFont.lfHeight = 8;
			wcscpy_s(edPtr->logFont.lfFaceName, L"Arial");
		}

		constexpr LPCWSTR default_text = L"Text";

		// Re-allocate EDITDATA structure according to the size of the default text
		auto bfSz = wcslen(default_text) + 1;
		DWORD dwNewSize = sizeof(EDITDATA) + bfSz * sizeof(wchar_t);
		LPEDATA pNewPtr = (LPEDATA)mvReAllocEditData(mV, edPtr, dwNewSize);

		if (pNewPtr != NULL)
		{
			edPtr = pNewPtr;

			// Initialize default text
			wcscpy_s(&edPtr->pText, bfSz, default_text);
		}
		
		return 0;	// No error
	}
#endif // !defined(RUN_ONLY)

	// Error
	return -1;
}

// --------------------
// EditObject
// --------------------
// Called when the user selects the Edit command in the object's popup menu
//
BOOL WINAPI EditObject (mv _far *mV, fpObjInfo oiPtr, fpLevObj loPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Check compatibility
	if (IS_COMPATIBLE(mV)) {
		//setupParams		spa;
		//spa.edpt = edPtr;
		//spa.kv = mV;
		//if (DialogBoxParam(hInstLib, MAKEINTRESOURCE(DB_SETUP), mV->mvHEditWin, setupProc, (LPARAM)(LPBYTE)&spa) == IDOK)
		//{
		//	return TRUE;
		//}
	}
#endif // !defined(RUN_ONLY)
	return FALSE;
}

// --------------------
// SetEditSize
// --------------------
// Called when the object has been resized
//
// Note: remove the comments if your object can be resized (and remove the comments in the .def file)

BOOL WINAPI SetEditSize(LPMV mv, LPEDATA edPtr, int cx, int cy)
{
#ifndef RUN_ONLY
	edPtr->swidth = cx;
	edPtr->sheight = cy;
#endif // !defined(RUN_ONLY)
	return TRUE;	// OK
}


// --------------------
// PutObject
// --------------------
// Called when each individual object is dropped in the frame.
//
void WINAPI	DLLExport PutObject(mv _far *mV, fpLevObj loPtr, LPEDATA edPtr, ushort cpt)
{
#ifndef RUN_ONLY
#endif // !defined(RUN_ONLY)
}

// --------------------
// RemoveObject
// --------------------
// Called when each individual object is removed from the frame.
//
void WINAPI	DLLExport RemoveObject(mv _far *mV, fpLevObj loPtr, LPEDATA edPtr, ushort cpt)
{
#ifndef RUN_ONLY
	// Is the last object removed?
    if (0 == cpt)
	{
		// Do whatever necessary to remove our data
	}
#endif // !defined(RUN_ONLY)
}

// --------------------
// DuplicateObject
// --------------------
// Called when an object is created from another one (note: should be called CloneObject instead...)
//
void WINAPI DLLExport DuplicateObject(mv __far *mV, fpObjInfo oiPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
#endif // !defined(RUN_ONLY)
}

// --------------------
// GetObjectRect
// --------------------
// Returns the size of the rectangle of the object in the frame editor.
//
void WINAPI DLLExport GetObjectRect(mv _far *mV, RECT FAR *rc, fpLevObj loPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	//rc->right = rc->left + edPtr->swidth;
	//rc->bottom = rc->top + edPtr->sheight;

	UpdateRectByHotSpot(edPtr->hotSpotPos
		, edPtr->swidth, edPtr->sheight
		, edPtr->hotSpotX, edPtr->hotSpotY
		, rc);

#endif // !defined(RUN_ONLY)
	return;
}


// --------------------
// EditorDisplay
// --------------------
// Displays the object under the frame editor
//
// Note: this function is optional. If it's not defined in your extension,
// MMF2 will load and display the EXO_IMAGE bitmap if it's defined in your resource file.
//
// If you need to draw the icon manually, remove the comments around this function and in the .def file.
//

void WINAPI DLLExport EditorDisplay(mv _far *mV, fpObjInfo oiPtr, fpLevObj loPtr, LPEDATA edPtr, RECT FAR *rc)
{
#ifndef RUN_ONLY

	// This is a simple case of drawing an image onto MMF's frame editor window
	// First, we must get a pointer to the surface used by the frame editor

	//LPSURFACE ps = WinGetSurface((int)mV->mvIdEditWin);
	//if ( ps != NULL )		// Do the following if this surface exists
	//{
	//	int x = rc->left;	// get our boundaries
	//	int y = rc->top;
	//	int w = rc->right-rc->left;
	//	int h = rc->bottom-rc->top;

	//	cSurface is;			// New surface variable for us to use
	//	is.Create(4, 4, ps);	// Create a surface implementation from a prototype (frame editor win)
	//	is.LoadImage(hInstLib, EXO_IMAGE, LI_REMAP);	// Load our bitmap from the resource,
	//													// and remap palette if necessary
	//	is.Blit(*ps, x, y, BMODE_TRANSP, BOP_COPY, 0);	// Blit the image to the frame editor surface!
	//	// This actually blits (or copies) the whole of our surface onto the frame editor's surface
	//	// at a specified position.
	//	// We could use different image effects when we copy, e.g. invert, AND, OR, XOR,
	//	// blend (semi-transparent, the 6th param is amount of transparency)
	//	// You can 'anti-alias' with the 7th param (default=0 or off)
	//}
	Display(mV, oiPtr, loPtr, edPtr, rc);

#endif // !defined(RUN_ONLY)
}


// --------------------
// IsTransparent
// --------------------
// This routine tells CC&C if the mouse pointer is over a transparent zone of the object.
// 

extern "C" BOOL WINAPI DLLExport IsTransparent(mv _far *mV, fpLevObj loPtr, LPEDATA edPtr, int dx, int dy)
{
#ifndef RUN_ONLY
	// Write your code here
#endif // !defined(RUN_ONLY)
	return FALSE;
}

// --------------------
// PrepareToWriteObject
// --------------------
// Just before writing the datazone when saving the application, CC&C calls this routine.
// 
void WINAPI	DLLExport PrepareToWriteObject(mv _far *mV, LPEDATA edPtr, fpObjInfo adoi)
{
#ifndef RUN_ONLY
	// Write your code here
#endif // !defined(RUN_ONLY)
}

// --------------------
// GetFilters
// --------------------

BOOL WINAPI GetFilters(LPMV mV, LPEDATA edPtr, DWORD dwFlags, LPVOID pReserved)
{
#ifndef RUN_ONLY
	// If your extension uses image filters
//	if ( (dwFlags & GETFILTERS_IMAGES) != 0 )
//		return TRUE;

	// If your extension uses sound filters
//	if ( (dwFlags & GETFILTERS_SOUNDS) != 0 )
//		return TRUE;
#endif // RUN_ONLY
	return FALSE;
}

// --------------------
// UsesFile
// --------------------
// Triggers when a file is dropped onto the frame
// Return TRUE if you can create an object from the given file
//
BOOL WINAPI	DLLExport UsesFile (LPMV mV, LPTSTR fileName)
{
	BOOL r = FALSE;
#ifndef RUN_ONLY

	// Example: return TRUE if file extension is ".txt"
/*	
	LPTSTR	ext, npath;

	if ( fileName != NULL )
	{
		if ( (ext=(LPTSTR)calloc(_MAX_EXT, sizeof(TCHAR))) != NULL )
		{
			if ( (npath=(LPTSTR)calloc(_MAX_PATH, sizeof(TCHAR))) != NULL )
			{
				_tcscpy(npath, fileName);
				_tsplitpath(npath, NULL, NULL, NULL, ext);
				if ( _tcsicmp(ext, _T(".txt")) == 0 )
					r = TRUE;
				free(npath);
			}
			free(ext);
		}
	} */
#endif // !defined(RUN_ONLY)
	return r;
}


// --------------------
// CreateFromFile
// --------------------
// Creates a new object from file
//
void WINAPI	DLLExport CreateFromFile (LPMV mV, LPTSTR fileName, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Initialize your extension data from the given file
//	edPtr->swidth = 32;
//	edPtr->sheight = 32;

	// Example: store the filename
	// _tcscpy(edPtr->myFileName, fileName);
#endif // !defined(RUN_ONLY)
}

// ============================================================================
//
// PROPERTIES
// 
// ============================================================================

// --------------------
// GetProperties
// --------------------
// Inserts properties into the properties of the object.
//
BOOL WINAPI DLLExport GetProperties(LPMV mV, LPEDATA edPtr, BOOL bMasterItem)
{
#ifndef RUN_ONLY
	mvInsertProps(mV, edPtr, Properties, PROPID_TAB_GENERAL, TRUE);
	mvInsertProps(mV, edPtr, DisplayProperties, PROPID_TAB_DISPLAY, TRUE);
	mvInsertProps(mV, edPtr, FontPorperties, PROPID_TAB_TEXTOPT, TRUE);
#endif // !defined(RUN_ONLY)

	// OK
	return TRUE;
}

// --------------------
// ReleaseProperties
// --------------------
// Called when the properties are removed from the property window.
//
void WINAPI DLLExport ReleaseProperties(LPMV mV, LPEDATA edPtr, BOOL bMasterItem)
{
#ifndef RUN_ONLY
	// Write your code here
#endif // !defined(RUN_ONLY)
}

// --------------------
// GetPropCreateParam
// --------------------
// Called when a property is initialized and its creation parameter is NULL (in the PropData).
// Allows you, for example, to change the content of a combobox property according to specific settings in the EDITDATA structure.
//
LPARAM WINAPI DLLExport GetPropCreateParam(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
#ifndef RUN_ONLY
	// Example
	// -------
//	if ( nPropID == PROPID_COMBO )
//	{
//		switch (edPtr->sType)
//		{
//		case TYPE1:
//			return (LPARAM)ComboList1;
//		case TYPE2:
//			return (LPARAM)ComboList2;
//		}
//	}
#endif // !defined(RUN_ONLY)
	return NULL;
}

// ----------------------
// ReleasePropCreateParam
// ----------------------
// Called after a property has been initialized.
// Allows you, for example, to free memory allocated in GetPropCreateParam.
//
void WINAPI DLLExport ReleasePropCreateParam(LPMV mV, LPEDATA edPtr, UINT nPropID, LPARAM lParam)
{
#ifndef RUN_ONLY
#endif // !defined(RUN_ONLY)
}

// --------------------
// GetPropValue
// --------------------
// Returns the value of properties that have a value.
// Note: see GetPropCheck for checkbox properties
//
LPVOID WINAPI DLLExport GetPropValue(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
#ifndef RUN_ONLY
	// Example
	// -------
	switch (nPropID) {
		//
		//	// Returns a color.
		//	case PROPID_COLOR:
		//		return new CPropDWordValue(edPtr->dwColor);
		//
		//	// Returns a string
		//	case PROPID_TEXT:
		//		return new CPropStringValue(&edPtr->szText[0]);
		//
		//	// Returns the value of the combo box
		//	case PROPID_COMBO:
		//		return new CPropDWordValue(edPtr->nComboIndex);

	// Text
	case PROPID_TEXT:
		return new CPropStringValue(&edPtr->pText);

	case PROPID_ALLIGN_ROWSPACE:
		return new CPropDWordValue(edPtr->nRowSpace);
	case PROPID_ALLIGN_COLSPACE:
		return new CPropDWordValue(edPtr->nColSpace);
	case PROPID_ALLIGN_TABSIZE:
		return new CPropDWordValue(edPtr->tabSize);

	//case PROPID_OUTLINE_PIXEL:
	//	return new CPropDWordValue(edPtr->nOutLinePixel);
	//case PROPID_OUTLINE_COLOR:
	//	return new CPropDWordValue(edPtr->dwOutLineColor);

	case PROPID_RENDER_BorderOffsetX:
		return new CPropDWordValue(edPtr->borderOffsetX);
	case PROPID_RENDER_BorderOffsetY:
		return new CPropDWordValue(edPtr->borderOffsetY);

	case PROPID_RENDER_TextRenderingHint:
		return new CPropDWordValue(edPtr->textRenderingHint);
	case PROPID_RENDER_SmoothingMode:
		return new CPropDWordValue(edPtr->smoothingMode);
	case PROPID_RENDER_PixelOffsetMode:
		return new CPropDWordValue(edPtr->pixelOffsetMode);

	case PROPID_HOTSPOT_DEFAULT:
		return new CPropDWordValue((size_t)edPtr->hotSpotPos);
	case PROPID_HOTSPOT_X:
		return new CPropDWordValue(edPtr->hotSpotX);
	case PROPID_HOTSPOT_Y:
		return new CPropDWordValue(edPtr->hotSpotY);
	case PROPID_FORMAT_ICONOFFSETX:
		return new CPropFloatValue(edPtr->iConOffsetX);
	case PROPID_FORMAT_ICONOFFSETY:
		return new CPropFloatValue(edPtr->iConOffsetY);
	case PROPID_FORMAT_ICONSCALE:
		return new CPropFloatValue(edPtr->iConScale);
	case PROPID_FORMAT_REMARKOFFSETX:
		return new CPropFloatValue(edPtr->remarkOffsetX);
	case PROPID_FORMAT_REMARKOFFSETY:
		return new CPropFloatValue(edPtr->remarkOffsetY);
	}
#endif // !defined(RUN_ONLY)
	return NULL;
}

// --------------------
// GetPropCheck
// --------------------
// Returns the checked state of properties that have a check box.
//
BOOL WINAPI DLLExport GetPropCheck(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
#ifndef RUN_ONLY
	// Example
	// -------
	switch (nPropID) {

	// Return 0 (unchecked) or 1 (checked)
	case PROPID_ALLIGN_VERTICALOFFSET:
		return edPtr->bVerticalAlignOffset;
	case PROPID_ALLIGN_ROWSPACE:
		return edPtr->bRowSpace;
	case PROPID_ALLIGN_COLSPACE:
		return edPtr->bColSpace;
	case PROPID_ALLIGN_TABEMSPACE:
		return edPtr->bTabEM;
	case PROPID_RENDER_CLIP:
		return edPtr->bClip;
	case PROPID_RENDER_CLIPTOOBJECT:
		return edPtr->bClipToObject;	
	case PROPID_FORMAT_ICONGLOBAL:
		return edPtr->bIConGlobal;
	case PROPID_FORMAT_ICONGLOBALFORCEUPDATE:
		return edPtr->bIConForceUpdate;
	case PROPID_FORMAT_ICONRESAMPLE:
		return edPtr->bIConResample;
	case PROPID_FORMAT_FILTER_UNKNOWN:
		return bool(edPtr->filterFlags & FORMAT_IGNORE_UNKNOWN);
	case PROPID_FORMAT_FILTER_INCOMPLETE:
		return bool(edPtr->filterFlags & FORMAT_IGNORE_INCOMPLETE);
	}

#endif // !defined(RUN_ONLY)
	return 0;		// Unchecked
}

// --------------------
// SetPropValue
// --------------------
// This routine is called by MMF after a property has been modified.
//
void WINAPI DLLExport SetPropValue(LPMV mV, LPEDATA edPtr, UINT nPropID, LPVOID lParam)
{
#ifndef RUN_ONLY
	// Gets the pointer to the CPropValue structure
	CPropValue* pValue = (CPropValue*)lParam;

	// Example
	// -------
	switch (nPropID) {
//
//	case PROPID_COMBO:
//		// Simply grab the value
//		edPtr->nComboIndex = ((CPropDWordValue*)pValue)->m_dwValue;
//		break;

//	case PROPID_COLOR:
//		// Here too, gets the value
//		edPtr->dwColor = ((CPropDWordValue*)pValue)->m_dwValue;
//		break;

	case PROPID_TEXT:
	{
		//MSGBOX(L"Text change");
		// Gets the string
		LPWSTR pStr = (LPWSTR)((CPropStringValue*)pValue)->GetString();

		// You can simply poke the string if your EDITDATA structure has a fixed size,
		// or have an adaptive size of structure like below

		// If the length is different
		if (wcslen(pStr) != wcslen(&edPtr->pText))
		{
			//MSGBOX(L"Text buffer allocate");

			// Asks MMF to reallocate the structure with the new size
			auto bfSz = (wcslen(pStr) + 1);
			DWORD dwNewSize = sizeof(EDITDATA) + bfSz * sizeof(wchar_t);
			LPEDATA pNewPtr = (LPEDATA)mvReAllocEditData(mV, edPtr, dwNewSize);
			
			//MSGBOX(L"Text copy");
			// If reallocation worked
			if (pNewPtr != NULL)
			{
				// Copy the string
				edPtr = pNewPtr;
				wcscpy_s(&edPtr->pText, bfSz, pStr);
			}
		}
		else
		{
			//MSGBOX(L"Text copy directly");
			// Same size : simply copy
			wcscpy_s(&edPtr->pText, wcslen(pStr) + 1, pStr);
		}
	}
	mvInvalidateObject(mV, edPtr);
	break;

	case PROPID_ALLIGN_ROWSPACE:
		edPtr->nRowSpace = ((CPropDWordValue*)pValue)->m_dwValue;
		mvInvalidateObject(mV, edPtr);
		break;
	case PROPID_ALLIGN_COLSPACE:
		edPtr->nColSpace = ((CPropDWordValue*)pValue)->m_dwValue;
		mvInvalidateObject(mV, edPtr);
		break;
	case PROPID_ALLIGN_TABSIZE:
		edPtr->tabSize = static_cast<unsigned char>(((CPropDWordValue*)pValue)->m_dwValue);
		mvInvalidateObject(mV, edPtr);
		break;

	//case PROPID_OUTLINE_PIXEL:
	//	edPtr->nOutLinePixel = (BYTE)((CPropDWordValue*)pValue)->m_dwValue;
	//	mvInvalidateObject(mV, edPtr);
	//	break;
	//case PROPID_OUTLINE_COLOR:
	//	edPtr->dwOutLineColor = ((CPropDWordValue*)pValue)->m_dwValue;
	//	mvInvalidateObject(mV, edPtr);
	//	break;
		
	case PROPID_RENDER_BorderOffsetX:
		edPtr->borderOffsetX = (unsigned short)(std::max)((DWORD)0, ((CPropDWordValue*)pValue)->m_dwValue);
		mvInvalidateObject(mV, edPtr);
		break;
	case PROPID_RENDER_BorderOffsetY:
        edPtr->borderOffsetY = (unsigned short)(std::max)((DWORD)0, ((CPropDWordValue*)pValue)->m_dwValue);
		mvInvalidateObject(mV, edPtr);
		break;

	case PROPID_RENDER_TextRenderingHint:
		edPtr->textRenderingHint = (BYTE)((CPropDWordValue*)pValue)->m_dwValue;
		mvInvalidateObject(mV, edPtr);
		break;
	case PROPID_RENDER_SmoothingMode:
		edPtr->smoothingMode = (BYTE)((CPropDWordValue*)pValue)->m_dwValue;
		mvInvalidateObject(mV, edPtr);
		break;
	case PROPID_RENDER_PixelOffsetMode:
		edPtr->pixelOffsetMode = (BYTE)((CPropDWordValue*)pValue)->m_dwValue;
		mvInvalidateObject(mV, edPtr);
		break;

	case PROPID_HOTSPOT_DEFAULT:
		edPtr->hotSpotPos = (HotSpotPos)((CPropDWordValue*)pValue)->m_dwValue;
		mvInvalidateObject(mV, edPtr);
		mvRefreshProp(mV, edPtr, PROPID_HOTSPOT_X, false);
		mvRefreshProp(mV, edPtr, PROPID_HOTSPOT_Y, false);
		break;
	case PROPID_HOTSPOT_X:
		edPtr->hotSpotX = ((CPropDWordValue*)pValue)->m_dwValue;
		mvInvalidateObject(mV, edPtr);
		break;
	case PROPID_HOTSPOT_Y:
		edPtr->hotSpotY = ((CPropDWordValue*)pValue)->m_dwValue;
		mvInvalidateObject(mV, edPtr);
		break;
	case PROPID_FORMAT_ICONOFFSETX:
		edPtr->iConOffsetX = ((CPropFloatValue*)pValue)->m_fValue;
		mvInvalidateObject(mV, edPtr);
		break;
	case PROPID_FORMAT_ICONOFFSETY:
		edPtr->iConOffsetY = ((CPropFloatValue*)pValue)->m_fValue;
		mvInvalidateObject(mV, edPtr);
		break;
	case PROPID_FORMAT_ICONSCALE:
		edPtr->iConScale = ((CPropFloatValue*)pValue)->m_fValue;
		mvInvalidateObject(mV, edPtr);
		break;

	case PROPID_FORMAT_REMARKOFFSETX:
		edPtr->remarkOffsetX = ((CPropFloatValue*)pValue)->m_fValue;
		mvInvalidateObject(mV, edPtr);
		break;
	case PROPID_FORMAT_REMARKOFFSETY:
		edPtr->remarkOffsetY = ((CPropFloatValue*)pValue)->m_fValue;
		mvInvalidateObject(mV, edPtr);
		break;
	}

	// You may want to have your object redrawn in the frame editor after the modifications,
	// in this case, just call this function
	// mvInvalidateObject(mV, edPtr);

#endif // !defined(RUN_ONLY)
}

// --------------------
// SetPropCheck
// --------------------
// This routine is called by MMF when the user modifies a checkbox in the properties.
//
void WINAPI DLLExport SetPropCheck(LPMV mV, LPEDATA edPtr, UINT nPropID, BOOL nCheck)
{
#ifndef RUN_ONLY
	// Example
	// -------
	switch (nPropID)
	{
	case PROPID_ALLIGN_VERTICALOFFSET:
		edPtr->bVerticalAlignOffset = nCheck;
		mvInvalidateObject(mV, edPtr);
		mvRefreshProp(mV, edPtr, PROPID_ALLIGN_VERTICALOFFSET, TRUE);
		break;
	case PROPID_ALLIGN_ROWSPACE:
		edPtr->bRowSpace = nCheck;
		mvInvalidateObject(mV, edPtr);
		mvRefreshProp(mV, edPtr, PROPID_ALLIGN_ROWSPACE, TRUE);
		break;
	case PROPID_ALLIGN_COLSPACE:
		edPtr->bColSpace = nCheck;
		mvInvalidateObject(mV, edPtr);
		mvRefreshProp(mV, edPtr, PROPID_ALLIGN_COLSPACE, TRUE);
		break;
	case PROPID_ALLIGN_TABEMSPACE:
		edPtr->bTabEM = nCheck;
		mvInvalidateObject(mV, edPtr);
		mvRefreshProp(mV, edPtr, PROPID_ALLIGN_TABEMSPACE, TRUE);
		break;
	case PROPID_RENDER_CLIP:
		edPtr->bClip = nCheck;
		mvInvalidateObject(mV, edPtr);
		mvRefreshProp(mV, edPtr, PROPID_RENDER_CLIP, TRUE);
		break;
	case PROPID_RENDER_CLIPTOOBJECT:
		edPtr->bClipToObject = nCheck;
		mvInvalidateObject(mV, edPtr);
		mvRefreshProp(mV, edPtr, PROPID_RENDER_CLIPTOOBJECT, TRUE);
		break;
	case PROPID_FORMAT_ICONGLOBAL:
		edPtr->bIConGlobal = nCheck;
		mvInvalidateObject(mV, edPtr);
		mvRefreshProp(mV, edPtr, PROPID_FORMAT_ICONGLOBAL, TRUE);
		mvRefreshProp(mV, edPtr, PROPID_FORMAT_ICONGLOBALFORCEUPDATE, TRUE);
		break;
	case PROPID_FORMAT_ICONGLOBALFORCEUPDATE:
		edPtr->bIConForceUpdate = nCheck;
		mvInvalidateObject(mV, edPtr);
		mvRefreshProp(mV, edPtr, PROPID_FORMAT_ICONGLOBALFORCEUPDATE, TRUE);
		break;
	case PROPID_FORMAT_ICONRESAMPLE:
		edPtr->bIConResample = nCheck;
		mvInvalidateObject(mV, edPtr);
		mvRefreshProp(mV, edPtr, PROPID_FORMAT_ICONRESAMPLE, TRUE);
		break;
	case PROPID_FORMAT_FILTER_UNKNOWN:
		UpdateEditFlag(edPtr->filterFlags, (size_t)FORMAT_IGNORE_UNKNOWN, nCheck);		
		mvInvalidateObject(mV, edPtr);
		mvRefreshProp(mV, edPtr, PROPID_FORMAT_FILTER_UNKNOWN, TRUE);
		break;
	case PROPID_FORMAT_FILTER_INCOMPLETE:
		UpdateEditFlag(edPtr->filterFlags, (size_t)FORMAT_IGNORE_INCOMPLETE, nCheck);
		mvInvalidateObject(mV, edPtr);
		mvRefreshProp(mV, edPtr, PROPID_FORMAT_FILTER_INCOMPLETE, TRUE);
		break;
	}
#endif // !defined(RUN_ONLY)
}

// --------------------
// EditProp
// --------------------
// This routine is called when the user clicks the button of a Button or EditButton property.
//
BOOL WINAPI DLLExport EditProp(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
#ifndef RUN_ONLY

	// Example
	// -------
/*
	if (nPropID==PROPID_EDITCONTENT)
	{
		if ( EditObject(mV, NULL, NULL, edPtr) )
			return TRUE;
	}
*/

#endif // !defined(RUN_ONLY)
	return FALSE;
}

// --------------------
// IsPropEnabled
// --------------------
// This routine returns the enabled state of a property.
//
BOOL WINAPI IsPropEnabled(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
#ifndef RUN_ONLY
	// Example
	// -------
/*
	switch (nPropID) {

	case PROPID_CHECK:
		return (edPtr->nComboIndex != 0);
	}
*/
	switch (nPropID) {
	case PROPID_FORMAT_ICONGLOBALFORCEUPDATE:
		return edPtr->bIConGlobal;
	case PROPID_HOTSPOT_X:
		return (edPtr->hotSpotPos == HotSpotPos::CUSTOM);
	case PROPID_HOTSPOT_Y:
		return (edPtr->hotSpotPos == HotSpotPos::CUSTOM);
	}

#endif // !defined(RUN_ONLY)
	return TRUE;
}


// ============================================================================
//
// TEXT PROPERTIES
// 
// ============================================================================

// --------------------
// GetTextCaps
// --------------------
// Return the text capabilities of the object under the frame editor.
//
DWORD WINAPI DLLExport GetTextCaps(mv _far *mV, LPEDATA edPtr)
{
	return (TEXT_ALIGN_LEFT|TEXT_ALIGN_HCENTER|TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP|TEXT_ALIGN_VCENTER|TEXT_ALIGN_BOTTOM|TEXT_FONT|TEXT_COLOR);
}

// --------------------
// GetTextFont
// --------------------
// Return the font used the object.
// Note: the pStyle and cbSize parameters are obsolete and passed for compatibility reasons only.
//
BOOL WINAPI DLLExport GetTextFont(mv _far *mV, LPEDATA edPtr, LPLOGFONT plf, LPTSTR pStyle, UINT cbSize)
{
#if !defined(RUN_ONLY)
	// Example: copy LOGFONT structure from EDITDATA
	memcpy(plf, &edPtr->logFont, sizeof(LOGFONT));
#endif // !defined(RUN_ONLY)

	return TRUE;
}

// --------------------
// SetTextFont
// --------------------
// Change the font used the object.
// Note: the pStyle parameter is obsolete and passed for compatibility reasons only.
//
BOOL WINAPI DLLExport SetTextFont(mv _far *mV, LPEDATA edPtr, LPLOGFONT plf, LPCTSTR pStyle)
{
#if !defined(RUN_ONLY)
	// Example: copy LOGFONT structure to EDITDATA
	memcpy(&edPtr->logFont, plf, sizeof(LOGFONT));
#endif // !defined(RUN_ONLY)

	return TRUE;
}

// --------------------
// GetTextClr
// --------------------
// Get the text color of the object.
//
COLORREF WINAPI DLLExport GetTextClr(mv _far *mV, LPEDATA edPtr)
{
	// Example
	return edPtr->dwColor;
}

// --------------------
// SetTextClr
// --------------------
// Set the text color of the object.
//
void WINAPI DLLExport SetTextClr(mv _far *mV, LPEDATA edPtr, COLORREF color)
{
	// Example
	edPtr->dwColor = color;
}

// --------------------
// GetTextAlignment
// --------------------
// Get the text alignment of the object.
//
DWORD WINAPI DLLExport GetTextAlignment(mv _far *mV, LPEDATA edPtr)
{
	DWORD dw = 0;
#if !defined(RUN_ONLY)
	// Example
	// -------
	if ( (edPtr->dwAlignFlags & DT_LEFT) != 0 )
		dw |= TEXT_ALIGN_LEFT;
	if ( (edPtr->dwAlignFlags & DT_CENTER) != 0 )
		dw |= TEXT_ALIGN_HCENTER;
	if ( (edPtr->dwAlignFlags & DT_RIGHT) != 0 )
		dw |= TEXT_ALIGN_RIGHT;
	if ( (edPtr->dwAlignFlags & DT_TOP) != 0 )
		dw |= TEXT_ALIGN_TOP;
	if ( (edPtr->dwAlignFlags & DT_VCENTER) != 0 )
		dw |= TEXT_ALIGN_VCENTER;
	if ( (edPtr->dwAlignFlags & DT_BOTTOM) != 0 )
		dw |= TEXT_ALIGN_BOTTOM;

#endif // !defined(RUN_ONLY)
	return dw;
}

// --------------------
// SetTextAlignment
// --------------------
// Set the text alignment of the object.
//
void WINAPI DLLExport SetTextAlignment(mv _far *mV, LPEDATA edPtr, DWORD dwAlignFlags)
{
#if !defined(RUN_ONLY)
	// Example
	// -------
	DWORD dw = edPtr->dwAlignFlags;

	if ((dwAlignFlags & TEXT_ALIGN_LEFT) != 0)
		dw = (dw & ~(DT_LEFT | DT_CENTER | DT_RIGHT)) | DT_LEFT;
	if ((dwAlignFlags & TEXT_ALIGN_HCENTER) != 0)
		dw = (dw & ~(DT_LEFT | DT_CENTER | DT_RIGHT)) | DT_CENTER;
	if ((dwAlignFlags & TEXT_ALIGN_RIGHT) != 0)
		dw = (dw & ~(DT_LEFT | DT_CENTER | DT_RIGHT)) | DT_RIGHT;

	if ((dwAlignFlags & TEXT_ALIGN_TOP) != 0)
		dw = (dw & ~(DT_TOP | DT_VCENTER | DT_BOTTOM)) | DT_TOP;
	if ((dwAlignFlags & TEXT_ALIGN_VCENTER) != 0)
		dw = (dw & ~(DT_TOP | DT_VCENTER | DT_BOTTOM)) | DT_VCENTER;
	if ((dwAlignFlags & TEXT_ALIGN_BOTTOM) != 0)
		dw = (dw & ~(DT_TOP | DT_VCENTER | DT_BOTTOM)) | DT_BOTTOM;

	edPtr->dwAlignFlags = dw;

#endif // !defined(RUN_ONLY)
}


// ============================================================================
//
// ROUTINES USED UNDER EVENT / TIME / STEP-THROUGH EDITOR
// You should not need to change these routines
// 
// ============================================================================

// -----------------
// menucpy
// -----------------
// Internal routine used later, copy one menu onto another
// 
#ifndef RUN_ONLY
void menucpy(HMENU hTargetMenu, HMENU hSourceMenu)
{
#ifndef RUN_ONLY
	int			n, id, nMn;
	LPTSTR		strBuf;
	HMENU		hSubMenu;
	
	nMn = GetMenuItemCount(hSourceMenu);
	strBuf = (LPTSTR)calloc(80, sizeof(TCHAR));
	for (n = 0; n < nMn; n++)
	{
		if (0 == (id = GetMenuItemID(hSourceMenu, n)))
			AppendMenu(hTargetMenu, MF_SEPARATOR, 0, 0L);
		else
		{
			GetMenuString(hSourceMenu, n, strBuf, 80, MF_BYPOSITION);
			if (id != -1)
				AppendMenu(hTargetMenu, GetMenuState(hSourceMenu, n, MF_BYPOSITION), id, strBuf);
			else
			{
				hSubMenu = CreatePopupMenu();
				AppendMenu(hTargetMenu, MF_POPUP | MF_STRING, (uint)hSubMenu, strBuf);
				menucpy(hSubMenu, GetSubMenu(hSourceMenu, n));
			}
		}
	}
	free(strBuf);
#endif
}

// -----------------
// GetPopupMenu
// -----------------
// Internal routine used later. Returns the first popup from a menu
// 
HMENU GetPopupMenu(short mn)
{
	HMENU	hMn, hSubMenu, hPopup = NULL;

	if ((hMn = LoadMenu(hInstLib, MAKEINTRESOURCE(mn))) != NULL)
	{
		if ((hSubMenu = GetSubMenu(hMn, 0)) != NULL)
		{
			if ((hPopup = CreatePopupMenu()) != NULL)
				menucpy(hPopup, hSubMenu);
		}
		DestroyMenu(hMn);
	}
	return hPopup;
}

// --------------------
// GetEventInformations
// --------------------
// Internal routine used later. Look for one event in one of the eventInfos array...
// No protection to go faster: you must properly enter the conditions/actions!
// 
static LPEVENTINFOS2 GetEventInformations(LPEVENTINFOS2 eiPtr, short code)

{
	while(eiPtr->infos.code != code)
		eiPtr = EVINFO2_NEXT(eiPtr);
	
	return eiPtr;
}
#endif // !defined(RUN_ONLY)


// ----------------------------------------------------
// GetConditionMenu / GetActionMenu / GetExpressionMenu
// ----------------------------------------------------
// Load the condition/action/expression menu from the resource, eventually
// enable or disable some options, and returns it to CC&C.
//
HMENU WINAPI DLLExport GetConditionMenu(mv _far *mV, fpObjInfo oiPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
		return GetPopupMenu(MN_CONDITIONS);
#endif // !defined(RUN_ONLY)
	return NULL;
}

HMENU WINAPI DLLExport GetActionMenu(mv _far *mV, fpObjInfo oiPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
		return GetPopupMenu(MN_ACTIONS);
#endif // !defined(RUN_ONLY)
	return NULL;
}

HMENU WINAPI DLLExport GetExpressionMenu(mv _far *mV, fpObjInfo oiPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
		return GetPopupMenu(MN_EXPRESSIONS);
#endif // !defined(RUN_ONLY)
	return NULL;
}


// -------------------------------------------------------
// GetConditionTitle / GetActionTitle / GetExpressionTitle
// -------------------------------------------------------
// Returns the title of the dialog box displayed when entering
// parameters for the condition, action or expressions, if any.
// Here, we simply return the title of the menu option
//

#ifndef RUN_ONLY
void GetCodeTitle(LPEVENTINFOS2 eiPtr, short code, short param, short mn, LPTSTR strBuf, WORD maxLen)
{
	HMENU		hMn;

	// Finds event in array
	eiPtr=GetEventInformations(eiPtr, code);

	// If a special string is to be returned
	short strID = EVINFO2_PARAMTITLE(eiPtr, param);

	if ( strID != 0 )
		LoadString(hInstLib, strID, strBuf, maxLen);
	else
	{
		// Otherwise, returns the menu option 
		if ((hMn = LoadMenu(hInstLib, MAKEINTRESOURCE(mn))) != NULL )
		{
			GetMenuString(hMn, eiPtr->menu, strBuf, maxLen, MF_BYCOMMAND);
			DestroyMenu(hMn);
		}
	}
}
#else
#define GetCodeTitle(a,b,c,d,e,f)
#endif // !defined(RUN_ONLY)

void WINAPI DLLExport GetConditionTitle(mv _far *mV, short code, short param, LPTSTR strBuf, short maxLen)
{
	GetCodeTitle((LPEVENTINFOS2)conditionsInfos, code, param, MN_CONDITIONS, strBuf, maxLen);
}
void WINAPI DLLExport GetActionTitle(mv _far *mV, short code, short param, LPTSTR strBuf, short maxLen)
{
	GetCodeTitle((LPEVENTINFOS2)actionsInfos, code, param, MN_ACTIONS, strBuf, maxLen);
}
void WINAPI DLLExport GetExpressionTitle(mv _far *mV, short code, LPTSTR strBuf, short maxLen)
{
	GetCodeTitle((LPEVENTINFOS2)expressionsInfos, code, 0, MN_EXPRESSIONS, strBuf, maxLen);
}

// -------------------------------------------------------
// GetConditionTitle / GetActionTitle / GetExpressionTitle
// -------------------------------------------------------
// From a menu ID, these routines returns the code of the condition,
// action or expression, as defined in the .H file
//

short WINAPI DLLExport GetConditionCodeFromMenu(mv _far *mV, short menuId)
{
#ifndef RUN_ONLY
	LPEVENTINFOS2	eiPtr;
	int				n;

	for (n=CND_LAST, eiPtr=(LPEVENTINFOS2)conditionsInfos; n>0 && eiPtr->menu!=menuId; n--)
		eiPtr = EVINFO2_NEXT(eiPtr);
	if (n>0) 
		return eiPtr->infos.code;
#endif // !defined(RUN_ONLY)
	return -1;
}

short WINAPI DLLExport GetActionCodeFromMenu(mv _far *mV, short menuId)
{
#ifndef RUN_ONLY
	LPEVENTINFOS2	eiPtr;
	int				n;

	for (n=ACT_LAST, eiPtr=(LPEVENTINFOS2)actionsInfos; n>0 && eiPtr->menu!=menuId; n--)
		eiPtr = EVINFO2_NEXT(eiPtr);
	if (n>0) 
		return eiPtr->infos.code;
#endif // !defined(RUN_ONLY)
	return -1;
}

short WINAPI DLLExport GetExpressionCodeFromMenu(mv _far *mV, short menuId)
{
#ifndef RUN_ONLY
	LPEVENTINFOS2	eiPtr;
	int				n;

	for (n=EXP_LAST, eiPtr=(LPEVENTINFOS2)expressionsInfos; n>0 && eiPtr->menu!=menuId; n--)
		eiPtr = EVINFO2_NEXT(eiPtr);
	if (n>0) 
		return eiPtr->infos.code;
#endif // !defined(RUN_ONLY)
	return -1;
}


// -------------------------------------------------------
// GetConditionInfos / GetActionInfos / GetExpressionInfos
// -------------------------------------------------------
// From a action / condition / expression code, returns 
// an infosEvents structure. 
//

LPINFOEVENTSV2 WINAPI DLLExport GetConditionInfos(mv _far *mV, short code)
{
#ifndef RUN_ONLY
	return &GetEventInformations((LPEVENTINFOS2)conditionsInfos, code)->infos;
#else
	return NULL;
#endif // !defined(RUN_ONLY)
}

LPINFOEVENTSV2 WINAPI DLLExport GetActionInfos(mv _far *mV, short code)
{
#ifndef RUN_ONLY
	return &GetEventInformations((LPEVENTINFOS2)actionsInfos, code)->infos;
#else
	return NULL;
#endif // !defined(RUN_ONLY)
}

LPINFOEVENTSV2 WINAPI DLLExport GetExpressionInfos(mv _far *mV, short code)
{
#ifndef RUN_ONLY
	return &GetEventInformations((LPEVENTINFOS2)expressionsInfos, code)->infos;
#else
	return NULL;
#endif // !defined(RUN_ONLY)
}


// ----------------------------------------------------------
// GetConditionString / GetActionString / GetExpressionString
// ----------------------------------------------------------
// From a action / condition / expression code, returns 
// the string to use for displaying it under the event editor
//

void WINAPI DLLExport GetConditionString(mv _far *mV, short code, LPTSTR strPtr, short maxLen)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
		LoadString(hInstLib, GetEventInformations((LPEVENTINFOS2)conditionsInfos, code)->string, strPtr, maxLen);
#endif // !defined(RUN_ONLY)
}

void WINAPI DLLExport GetActionString(mv _far *mV, short code, LPTSTR strPtr, short maxLen)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
		LoadString(hInstLib, GetEventInformations((LPEVENTINFOS2)actionsInfos, code)->string, strPtr, maxLen);
#endif // !defined(RUN_ONLY)
}

void WINAPI DLLExport GetExpressionString(mv _far *mV, short code, LPTSTR strPtr, short maxLen)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
		LoadString(hInstLib, GetEventInformations((LPEVENTINFOS2)expressionsInfos, code)->string, strPtr, maxLen);
#endif // !defined(RUN_ONLY)
}

// ----------------------------------------------------------
// GetExpressionParam
// ----------------------------------------------------------
// Returns the parameter name to display in the expression editor
//
void WINAPI DLLExport GetExpressionParam(mv _far *mV, short code, short param, LPTSTR strBuf, short maxLen)
{
#if !defined(RUN_ONLY)
	short		strID;

	// Finds event in array
	LPEVENTINFOS2 eiPtr=GetEventInformations((LPEVENTINFOS2)expressionsInfos, code);

	// If a special string is to be returned
	strID = EVINFO2_PARAMTITLE(eiPtr, param);
	if ( strID != 0 )
		LoadString(hInstLib, strID, strBuf, maxLen);
	else
		*strBuf=0;
#endif // !defined(RUN_ONLY)
}

// ----------------------------------------------------------
// Custom Parameters
// ----------------------------------------------------------

// --------------------
// InitParameter
// --------------------
// Initialize the parameter.
//
void WINAPI InitParameter(mv _far *mV, short code, paramExt* pExt)
{
#if !defined(RUN_ONLY)
	// Example
	// -------
	// _tcscpy(&pExt->pextData[0], _T("Parameter Test"));
	// pExt->pextSize = sizeof(paramExt) + (_tcslen(pExt->pextData)+1)*sizeof(TCHAR);
#endif // !defined(RUN_ONLY)
}

// Example of custom parameter setup proc
// --------------------------------------
/*
#if !defined(RUN_ONLY)
BOOL CALLBACK DLLExport SetupProc(HWND hDlg, UINT msgType, WPARAM wParam, LPARAM lParam)
{
	paramExt*			pExt;

	switch (msgType)
	{
		case WM_INITDIALOG: // Init dialog

			// Save edptr
			SetWindowLong(hDlg, DWL_USER, lParam);
			pExt=(paramExt*)lParam;

			SetDlgItemText(hDlg, IDC_EDIT, pExt->pextData);
			return TRUE;

		case WM_COMMAND: // Command

			// Retrieve edptr
			pExt = (paramExt *)GetWindowLong(hDlg, DWL_USER);

			switch (wmCommandID)
			{
			case IDOK:	// Exit
				GetDlgItemText(hDlg, IDC_EDIT, pExt->pextData, 500);
				pExt->pextSize=sizeof(paramExt)+(_tcslen(pExt->pextData)+1)*sizeof(TCHAR);
				EndDialog(hDlg, TRUE);
				return TRUE;

				default:
					break;
			}
			break;

		default:
			break;
	}
	return FALSE;
}
#endif // !defined(RUN_ONLY)
*/

// --------------------
// EditParameter
// --------------------
// Edit the parameter.
//
void WINAPI EditParameter(mv _far *mV, short code, paramExt* pExt)
{
#if !defined(RUN_ONLY)

	// Example
	// -------
	// DialogBoxParam(hInstLib, MAKEINTRESOURCE(DB_TRYPARAM), mV->mvHEditWin, SetupProc, (LPARAM)(LPBYTE)pExt);

#endif // !defined(RUN_ONLY)
}

// --------------------
// GetParameterString
// --------------------
// Initialize the parameter.
//
void WINAPI GetParameterString(mv _far *mV, short code, paramExt* pExt, LPTSTR pDest, short size)
{
#if !defined(RUN_ONLY)

	// Example
	// -------
	// wsprintf(pDest, "Super parameter %s", pExt->pextData);

#endif // !defined(RUN_ONLY)
}

