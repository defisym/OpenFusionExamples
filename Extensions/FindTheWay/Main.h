// Object identifier "SAM3"
#define IDENTIFIER	MAKEID(F,T,W,A)		// REQUIRED: you MUST replace the letters in the MAKEID macro by others
										// and then remove the #pragma message above. If you do not do this, MMF2
										// could confuse your object with another in the event editor.

// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define	CND_CONDITION_SMBS				0
#define	CND_CONDITION_SMBB				1
#define	CND_CONDITION_SMBC				2
#define	CND_CONDITION_OSMBC				3

#define	CND_CONDITION_OPF				4
#define	CND_CONDITION_PA				5

#define	CND_CONDITION_OITP				6

#define	CND_CONDITION_MA				7

#define	CND_LAST						8

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define	ACT_ACTION_SM					0

#define	ACT_ACTION_C					1

#define	ACT_ACTION_ITP					2

#define	ACT_LAST						3

// -------------------------------
// DEFINITION OF EXPRESSIONS CODES
// -------------------------------
#define	EXP_EXPRESSION_GITX				0
#define EXP_EXPRESSION_GITY				1

#define EXP_EXPRESSION_GS				2
#define EXP_EXPRESSION_GSOP				3
#define EXP_EXPRESSION_GSCOP			4

#define EXP_EXPRESSION_GGC				5
#define EXP_EXPRESSION_GRC				6

#define EXP_EXPRESSION_GITI				7

#define EXP_EXPRESSION_GMC				8
#define EXP_EXPRESSION_GMB64			9
#define EXP_EXPRESSION_GMS				10

#define	EXP_LAST                    	11

// ---------------------
// OBJECT DATA STRUCTURE 
// ---------------------
// Used at edit time and saved in the MFA/CCN/EXE files

typedef struct tagEDATA_V1
{
	// Header - required
	extHeader		eHeader;

	// Object's data
//	short			swidth;
//	short			sheight;

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
//	rCom			rc;				// Common structure for movements & animations
//	rMvt			rm;				// Movements
//	rSpr			rs;				// Sprite (displayable objects)
	rVal			rv;				// Alterable values

	// Object's runtime data
	FindTheWayClass* pFTW;

	size_t itIndex;
	Coord itCoord;

	wstring* pOnItCollisionName;
	wstring* pOnItPathName;
	wstring* pMapBase64Str;
	wstring* pMapStr;

} RUNDATA;
typedef	RUNDATA	*			LPRDATA;

// Size when editing the object under level editor
// -----------------------------------------------
#define	MAX_EDITSIZE			sizeof(EDITDATA)

// Default flags - see documentation for more info
// -------------
#define	OEFLAGS      			(OEFLAG_VALUES|OEFLAG_NEVERKILL|OEFLAG_RUNBEFOREFADEIN|OEFLAG_MANUALSLEEP|OEFLAG_NEVERSLEEP)
#define	OEPREFS      			0


// If to handle message, specify the priority of the handling procedure
// 0= low, 255= very high. You should use 100 as normal.                                                
// --------------------------------------------------------------------
#define	WINDOWPROC_PRIORITY		100
