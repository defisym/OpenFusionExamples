// Object identifier "SMIX"
#define IDENTIFIER	MAKEID(S,M,I,X)		// REQUIRED: you MUST replace the letters in the MAKEID macro by others
										// and then remove the #pragma message above. If you do not do this, MMF2
										// could confuse your object with another in the event editor.

// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define	CND_CONDITION				0
#define	CND_LAST					1

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define	ACT_ACTION					0
#define	ACT_LAST					1

// -------------------------------
// DEFINITION OF EXPRESSIONS CODES
// -------------------------------
#define	EXP_EXPRESSION				0
#define EXP_EXPRESSION2				1
#define EXP_EXPRESSION3				2
#define	EXP_LAST                    3

// ---------------------
// OBJECT DATA STRUCTURE 
// ---------------------
// Used at edit time and saved in the MFA/CCN/EXE files

//#define _DISPLAY_OBJECT

#ifndef _DISPLAY_OBJECT
#define _NODISPLAY // for UpdateHoImgInfo
#endif

typedef struct tagEDATA_V1
{
	// Header - required
	extHeader		eHeader;

	// Object's data
#ifdef _DISPLAY_OBJECT
	short			swidth;
	short			sheight;
#endif

	// buffer
	int buffer[52];

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
#ifdef _DISPLAY_OBJECT
	rCom			rc;				// Common structure for movements & animations
	rMvt			rm;				// Movements
	rSpr			rs;				// Sprite (displayable objects)
#endif
	rVal			rv;				// Alterable values

#ifdef _DISPLAY_OBJECT
	short			swidth;
	short			sheight;
#endif

	// Object's runtime data
	GlobalData* pData;

	std::wstring* pRet;
	
} RUNDATA;
typedef	RUNDATA	*			LPRDATA;

// Size when editing the object under level editor
// -----------------------------------------------
#define	MAX_EDITSIZE			sizeof(EDITDATA)

// Default flags - see documentation for more info
// -------------

// Display
#ifdef _DISPLAY_OBJECT
#define	OEFLAGS      			(OEFLAG_VALUES|OEFLAG_SPRITES|OEFLAG_MOVEMENTS|OEFLAG_SCROLLINGINDEPENDANT|OEFLAG_NEVERKILL|OEFLAG_RUNBEFOREFADEIN|OEFLAG_MANUALSLEEP|OEFLAG_NEVERSLEEP|OEFLAG_BACKSAVE)
#define	OEPREFS      			(OEPREFS_SCROLLINGINDEPENDANT|OEPREFS_INKEFFECTS|OEPREFS_BACKSAVE|OEPREFS_BACKEFFECTS)
#else
// Non-Display
#define	OEFLAGS      			(OEFLAG_VALUES|OEFLAG_NEVERKILL|OEFLAG_RUNBEFOREFADEIN|OEFLAG_MANUALSLEEP|OEFLAG_NEVERSLEEP)
#define	OEPREFS      			0
#endif

// If to handle message, specify the priority of the handling procedure
// 0= low, 255= very high. You should use 100 as normal.                                                
// --------------------------------------------------------------------
#define	WINDOWPROC_PRIORITY		100
