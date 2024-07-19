#pragma warning(disable:4819)
#pragma warning(disable:4996)

#define IDENTIFIER	MAKEID(S,W,A,1)		// REQUIRED: you MUST replace the letters in the MAKEID macro by others
										// and then remove the #pragma message above. If you do not do this, MMF2
										// could confuse your object with another in the event editor.

// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define	CND_CONDITION_RPO				0

#define	CND_CONDITION_OMTE				1
#define	CND_CONDITION_OMTF				2

#define	CND_CONDITION_ROSD				3

#define	CND_CONDITION_OSS				4

#define	CND_CONDITION_OID				5
#define	CND_CONDITION_SUBMITTED			6

#define	CND_CONDITION_ODLCIC			7

#define	CND_CONDITION_PE				8

#define	CND_CONDITION_OIFU				9
#define	CND_CONDITION_OIRR				10

#define	CND_LAST						11

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define	ACT_ACTION_UA					0
#define	ACT_ACTION_AS					1

#define	ACT_ACTION_SRP					2
#define	ACT_ACTION_CRP					3

#define	ACT_ACTION_MT_SI				4
#define	ACT_ACTION_MT_GUI				5
#define	ACT_ACTION_MT_SR				6
#define	ACT_ACTION_MT_F 				7

#define	ACT_ACTION_SSSL 				8
#define	ACT_ACTION_TSSU 				9
#define	ACT_ACTION_TSSPF 				10
#define	ACT_ACTION_TSS 					11

#define	ACT_ACTION_SGTI					12

#define	ACT_ACTION_IAP					13
#define	ACT_ACTION_SARS					14
#define	ACT_ACTION_AGOTS				15
#define	ACT_ACTION_ID					16
#define	ACT_ACTION_TID					17
#define	ACT_ACTION_CI					18
#define	ACT_ACTION_GTI					19
#define	ACT_ACTION_GAI					20
#define	ACT_ACTION_UFL					21

#define	ACT_LAST						22

// -------------------------------
// DEFINITION OF EXPRESSIONS CODES
// -------------------------------
#define	EXP_EXPRESSION_GCGL				0
#define	EXP_EXPRESSION_GSCL				1

#define	EXP_EXPRESSION_MT_GS			2
#define	EXP_EXPRESSION_MT_GHTMLEC		3
#define	EXP_EXPRESSION_MT_GED			4
#define	EXP_EXPRESSION_MT_GTID			5

#define	EXP_EXPRESSION_GCBP				6

#define	EXP_EXPRESSION_GGIT				7

#define	EXP_EXPRESSION_GDDPP			8

#define	EXP_EXPRESSION_I_GPIC			9
#define	EXP_EXPRESSION_I_GPIIID			10
#define	EXP_EXPRESSION_I_GPID			11
#define	EXP_EXPRESSION_I_GPIQ 			12
#define	EXP_EXPRESSION_I_GPIF 			13
#define	EXP_EXPRESSION_I_GIP			14

#define	EXP_EXPRESSION_F_GFLS			15
#define	EXP_EXPRESSION_F_GFN			16
#define	EXP_EXPRESSION_F_GFA			17

#define	EXP_LAST						18

// ---------------------
// OBJECT DATA STRUCTURE 
// ---------------------
// Used at edit time and saved in the MFA/CCN/EXE files

#define _NODISPLAY // for UpdateHoImgInfo

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
	bool bReportError = false;

	ENotificationPosition NotificationPosition;

	bool unused_0;
	bool unused_1;
	bool unused_2;

	int buffer[50];

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
	AppId_t callBackAppID;
	bool bCallbackSuccess;
	bool bPm;

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
