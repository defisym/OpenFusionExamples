// Object identifier "SAM3"
#define IDENTIFIER	MAKEID(F,I,N,I)		// REQUIRED: you MUST replace the letters in the MAKEID macro by others
										// and then remove the #pragma message above. If you do not do this, MMF2
										// could confuse your object with another in the event editor.

// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define	CND_CONDITION_ITS			0
#define	CND_CONDITION_ITI			1

#define	CND_CONDITION_SIHV			2

#define	CND_LAST					3

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define	ACT_ACTION_NF				0
#define	ACT_ACTION_DF				1
#define	ACT_ACTION_LF				2
#define	ACT_ACTION_SF				3
#define	ACT_ACTION_SSIV				4
#define	ACT_ACTION_SSIS				5
#define	ACT_ACTION_DSI				6
#define	ACT_ACTION_ITS				7
#define	ACT_ACTION_ITI				8
#define	ACT_ACTION_LS				9
#define	ACT_ACTION_SAON				10
#define	ACT_ACTION_SAOFF			11
#define	ACT_ACTION_CS				12
#define	ACT_ACTION_LAV				13
#define	ACT_ACTION_LP				14
#define	ACT_ACTION_LB64				15
#define	ACT_ACTION_LL				16
#define	ACT_ACTION_SLC				17

#define	ACT_LAST					18

// -------------------------------
// DEFINITION OF EXPRESSIONS CODES
// -------------------------------
#define	EXP_EXPRESSION_GSIV			0
#define	EXP_EXPRESSION_GSIS			1
#define	EXP_EXPRESSION_GCS			2
#define	EXP_EXPRESSION_GCI			3
#define	EXP_EXPRESSION_SS			4
#define	EXP_EXPRESSION_SAV			5
#define	EXP_EXPRESSION_SP			6
#define	EXP_EXPRESSION_SB64			7

#define	EXP_LAST                    8

// ---------------------
// OBJECT DATA STRUCTURE 
// ---------------------
// Used at edit time and saved in the MFA/CCN/EXE files

#define _NODISPLAY

typedef struct tagEDATA_V1
{
	// Header - required
	extHeader		eHeader;

	// Object's data
//	short			swidth;
//	short			sheight;

} EDITDATA_V1;

typedef struct tagEDATA_V2
{
	// Header - required
	extHeader		eHeader;

	// Object's data
//	short			swidth;
//	short			sheight;

	bool cf25p;
	bool allowRVforCS;

} EDITDATA;
typedef EDITDATA *			LPEDATA;

// Object versions
#define	KCX_VERSION_V1				1
#define	KCX_VERSION_V2				2

#define	KCX_CURRENT_VERSION			2

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
	//Auto Save
	bool AutoSave = false;
	bool Modified = false;

	LPTSTR AutoSaveFilePath = nullptr;
	LPTSTR AutoSaveKey = nullptr;
	
	//ReturnStr
	LPTSTR Str = nullptr;
	
	//ini
	LPINI ini = nullptr;

	LPINI pLocalization = nullptr;
	std::wstring* pLanguageCode = nullptr;

	LPTSTR SecLoopName = nullptr;
	LPTSTR ItemLoopName = nullptr;

	LPTSTR CurrentSec = nullptr;
	LPTSTR CurrentItem = nullptr;

	Base64<std::wstring>* pB64 = nullptr;
	std::wstring* b64Str = nullptr;

	bool cf25p;
	bool allowRVforCS;

	int (*CompressToBuffer)(const std::string& srcStr, char*& buf);
	std::string(*DeCompressToString)(const char* srcBuf, const unsigned int bufSz);

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
