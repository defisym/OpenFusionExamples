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
#define	CND_CONDITION_CMCAC				8

#define	CND_CONDITION_OAG				9
#define	CND_CONDITION_OAO				10
#define	CND_CONDITION_OAC				11

#define	CND_CONDITION_SMBP				12

#define	CND_CONDITION_CA				13
#define	CND_CONDITION_OITA				14
#define	CND_CONDITION_OITAA				15
#define	CND_CONDITION_AITA				16

#define	CND_CONDITION_OMC				17

#define	CND_CONDITION_OCOZ				18
#define	CND_CONDITION_ZV				19
#define	CND_CONDITION_ZAA				20
#define	CND_CONDITION_SA				21
#define	CND_CONDITION_OAOBJ				22
#define	CND_CONDITION_POAO				23

#define	CND_CONDITION_NOAG				24
#define	CND_CONDITION_NOAC				25

#define	CND_LAST						26

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define	ACT_ACTION_SM					0
#define	ACT_ACTION_C					1
#define	ACT_ACTION_ITP					2
#define	ACT_ACTION_SMBO					3
#define	ACT_ACTION_CM					4
#define	ACT_ACTION_SZBO					5
#define	ACT_ACTION_CZ					6
#define	ACT_ACTION_ITA					7
#define	ACT_ACTION_SUBO					8
#define	ACT_ACTION_CU					9
#define	ACT_ACTION_COZ					10
#define	ACT_ACTION_SS					11
#define	ACT_ACTION_COZBE				12
#define	ACT_ACTION_COZBN				13
#define	ACT_ACTION_CAO					14
#define	ACT_ACTION_CABNO				15
#define	ACT_ACTION_CAOE					16
#define	ACT_ACTION_CAOEBN				17

#define	ACT_LAST						18

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

#define EXP_EXPRESSION_GALR				11

#define EXP_EXPRESSION_GIF				12

#define EXP_EXPRESSION_GMTM				13
#define EXP_EXPRESSION_GMTT				14
#define EXP_EXPRESSION_GMTD				15
#define EXP_EXPRESSION_GMCP				16
#define EXP_EXPRESSION_GMCO				17
#define EXP_EXPRESSION_GMCV				18

#define EXP_EXPRESSION_GIGS				19

#define	EXP_LAST                    	20

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

	bool isometric;

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
	bool isometric;

	FindTheWayClass* pFTW;

	size_t itIndex;
	Coord itCoord;

	wstring* pOnItCollisionName;
	wstring* pOnItPathName;
	wstring* pMapBase64Str;
	wstring* pMapStr;

	ObjectSelection* pSelect;

	bool isAttack;
	size_t areaSize;
	size_t areaPos;
	size_t extraRangeStartPos;
	CoordSet* pZoc;
	CoordSet* pAlly;
	CoordSet* pEnemy;
	CoordSet* pUnit;

	wstring* pOnItAreaName;

	LPRO pObject;
	ObjectCreation* pOc;
	CoordSet* pObjZoc;
	wstring* pOnItZocName;

	AOEClass* pAOE;
	vector<AOEClass::coord>* pAOECoord;

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
