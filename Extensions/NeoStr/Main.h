// Object identifier "NStr"
#define IDENTIFIER	MAKEID(S,A,M,3)		// REQUIRED: you MUST replace the letters in the MAKEID macro by others
										// and then remove the #pragma message above. If you do not do this, MMF2
										// could confuse your object with another in the event editor.

// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define	CND_CONDITION					0

#define	CND_LAST						1

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define	ACT_ACTION_CS					0
#define	ACT_ACTION_CSTR					1
#define	ACT_ACTION_CRS					2
#define	ACT_ACTION_CCS					3

#define	ACT_ACTION_CHA					4
#define	ACT_ACTION_CVA					5

#define	ACT_ACTION_ASTR			    	6
#define	ACT_ACTION_ASTRNL		    	7

#define	ACT_LAST						8

// -------------------------------
// DEFINITION OF EXPRESSIONS CODES
// -------------------------------
#define	EXP_EXPRESSION_GSTR					0
#define EXP_EXPRESSION_GRS					1
#define EXP_EXPRESSION_GCS					2

#define EXP_EXPRESSION_GLCX					3
#define EXP_EXPRESSION_GLCY					4
#define EXP_EXPRESSION_GMW					5

#define	EXP_LAST                    		6

// ---------------------
// OBJECT DATA STRUCTURE 
// ---------------------
// Used at edit time and saved in the MFA/CCN/EXE files

typedef struct tagEDATA_V1
{
	// Header - required
	extHeader		eHeader;

	// Object's data
	short			swidth;
	short			sheight;
	
	COLORREF		dwColor;		// text color
	DWORD			dwAlignFlags;	// alignment flags
	LOGFONT			logFont;		// text font

	BYTE nOutLinePixel;
	COLORREF dwOutLineColor;

	bool bShadow;
	BYTE nShadowOffsetX;
	BYTE nShadowOffsetY;
	
	bool bRowSpace;
	bool bColSpace;
	size_t nRowSpace;
	size_t nColSpace;
	
	wchar_t	pText;		// Text

} EDITDATA;
typedef EDITDATA *			LPEDATA;

// Object versions
#define	KCX_CURRENT_VERSION			1

// --------------------------------
// RUNNING OBJECT DATA STRUCTURE
// --------------------------------
// Used at runtime. Initialize it in the CreateRunObject function.
// Free any allocated memory or object in the DestroyRunObject function.
//
// Note: if you store C++ objects in this structure and do not store 
// them as pointers, you must call yourself their constructor in the
// CreateRunObject function and their destructor in the DestroyRunObject
// function. As the RUNDATA structure is a simple C structure and not a C++ object.

typedef struct tagRDATA
{
	// Main header - required
	headerObject	rHo;					// Header

	// Optional headers - depend on the OEFLAGS value, see documentation and examples for more info
	rCom			rc;				// Common structure for movements & animations
	rMvt			rm;				// Movements
	rSpr			rs;				// Sprite (displayable objects)
	rVal			rv;				// Alterable values

	// Object's runtime data
	short			swidth;
	short			sheight;

	HWND			hWnd;
	
	COLORREF		dwColor;		// text color
	DWORD			dwAlignFlags;	// alignment flags
	HFONT			hFont;			// text font

	BYTE nOutLinePixel;
	COLORREF dwOutLineColor;

	bool bRowSpace;
	bool bColSpace;
	size_t nRowSpace;
	size_t nColSpace;

	bool bStrChanged;
	std::wstring* pStr;

	bool bFontChanged;
	NeoStr* pNeoStr;

	CharPos charPos;

} RUNDATA;
typedef	RUNDATA	*			LPRDATA;

// Size when editing the object under level editor
// -----------------------------------------------
#define	MAX_EDITSIZE			sizeof(EDITDATA)

// Default flags - see documentation for more info
// -------------
#define	OEFLAGS      			(OEFLAG_VALUES|OEFLAG_SPRITES|OEFLAG_MOVEMENTS|OEFLAG_SCROLLINGINDEPENDANT|OEFLAG_NEVERKILL|OEFLAG_RUNBEFOREFADEIN|OEFLAG_MANUALSLEEP|OEFLAG_NEVERSLEEP|OEFLAG_BACKSAVE|OEFLAG_TEXT)
#define	OEPREFS      			(OEPREFS_SCROLLINGINDEPENDANT|OEPREFS_INKEFFECTS|OEPREFS_BACKSAVE|OEPREFS_BACKEFFECTS)


// If to handle message, specify the priority of the handling procedure
// 0= low, 255= very high. You should use 100 as normal.                                                
// --------------------------------------------------------------------
#define	WINDOWPROC_PRIORITY		100
