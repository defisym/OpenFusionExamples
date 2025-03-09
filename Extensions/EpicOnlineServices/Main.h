// Object identifier "EOSS"
#define IDENTIFIER	MAKEID(E,O,S,S)		// REQUIRED: you MUST replace the letters in the MAKEID macro by others
										// and then remove the #pragma message above. If you do not do this, MMF2
										// could confuse your object with another in the event editor.

// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define	CND_CONDITION_ONLOGIN				0
#define	CND_CONDITION_USERLOGIN				1
#define	CND_CONDITION_QUEARYCOMPLETE		2
#define	CND_CONDITION_ONERROR				3
#define	CND_CONDITION_ONLOGOUT				4
#define	CND_CONDITION_PE					5

#define	CND_LAST							6

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define	ACT_ACTION_ACH_UL					0
#define	ACT_ACTION_STAT_I					1
#define	ACT_ACTION_QUERY					2
#define	ACT_ACTION_PRE_SRT					3
#define	ACT_ACTION_LI						4
#define	ACT_ACTION_LO						5

#define	ACT_LAST							6

// -------------------------------
// DEFINITION OF EXPRESSIONS CODES
// -------------------------------
#define	EXP_EXPRESSION_STATVALUE			0
#define	EXP_EXPRESSION_ACCOUNTID			1
#define	EXP_EXPRESSION_PRODUCTUSERID		2
#define	EXP_EXPRESSION_PRE_GRT				3
#define	EXP_EXPRESSION_GLE					4

#define	EXP_LAST                    		5

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

	// InitializeOptions
	wchar_t pAppName[2 * EOS_IDSZ];
	wchar_t pAppVersion[EOS_IDSZ];

	// PlatformOptions
	wchar_t pProductId[EOS_IDSZ];

	wchar_t pClientId[EOS_IDSZ];
	wchar_t pClientSecret[2 * EOS_IDSZ];

	// Sandbox	
	SandboxComboListEnum sandboxType;	// default

	wchar_t pDevSandboxId[EOS_IDSZ];
	wchar_t pDevDeploymentId[EOS_IDSZ];
	wchar_t pStageSandboxId[EOS_IDSZ];
	wchar_t pStageDeploymentId[EOS_IDSZ];
	wchar_t pLiveSandboxId[EOS_IDSZ];
	wchar_t pLiveDeploymentId[EOS_IDSZ];

	// RuntimeOptions
	AuthTypeComboListEnum authType;

	bool bRequireLauncher;
	bool bRequireBootstrap;

	bool bAuthPremissions_BasicProfile;
	bool bAuthPremissions_FriendsList;
	bool bAuthPremissions_Presence;
	bool bAuthPremissions_FriendsManagement;
	bool bAuthPremissions_Email;
	bool bAuthPremissions_Country;

	bool bAutoLogin;
	bool bAutoLogout;

	// aligning
	bool bUnused_0;
	bool bUnused_1;

	// buffer
	int buffer[48];

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
