// Object identifier "FFMG"
#define IDENTIFIER	MAKEID(F,F,M,G)		// REQUIRED: you MUST replace the letters in the MAKEID macro by others
										// and then remove the #pragma message above. If you do not do this, MMF2
										// could confuse your object with another in the event editor.

// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define CND_CONDITION_VO				0
#define CND_CONDITION_VP				1
#define CND_CONDITION_VL				2
#define CND_CONDITION_VF				3

#define CND_CONDITION_OVF				4
#define CND_CONDITION_OVOF				5

#define CND_CONDITION_VHD				6
#define CND_CONDITION_VWHD				7

#define	CND_LAST						8

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define	ACT_ACTION_OV					0
#define	ACT_ACTION_CV					1
#define	ACT_ACTION_PLAYV				2
#define	ACT_ACTION_PAUSEV				3

#define	ACT_ACTION_SV					4
#define	ACT_ACTION_SL					5
#define	ACT_ACTION_SP					6
#define	ACT_ACTION_SPWF					7

#define	ACT_ACTION_SQS					8

#define	ACT_ACTION_SAS					9

#define	ACT_ACTION_CACHEV				10
#define	ACT_ACTION_ERASEV				11

#define	ACT_ACTION_OVT					12

#define	ACT_ACTION_SHDE					13

#define	ACT_ACTION_STRETCH				14

#define	ACT_ACTION_SAT					15

#define	ACT_ACTION_SFNA					16

#define	ACT_ACTION_SOC					17

#define	ACT_ACTION_RD					18

#define	ACT_LAST						19

// -------------------------------
// DEFINITION OF EXPRESSIONS CODES
// -------------------------------
#define	EXP_EXPRESSION_GVN				0
#define EXP_EXPRESSION_GVP				1
#define EXP_EXPRESSION_GVD				2

#define EXP_EXPRESSION_GV				3

#define EXP_EXPRESSION_GCVFP			4
#define EXP_EXPRESSION_GGVFP			5

#define EXP_EXPRESSION_GVO				6
#define EXP_EXPRESSION_GVPLAY			7
#define EXP_EXPRESSION_GVL				8
#define EXP_EXPRESSION_GVF				9

#define EXP_EXPRESSION_GHDS 			10
#define EXP_EXPRESSION_GAHDE 			11
#define EXP_EXPRESSION_GWHDE 			12

#define EXP_EXPRESSION_GAT	 			13

#define EXP_EXPRESSION_GVOCN	 		14
#define EXP_EXPRESSION_GAOCN	 		15

#define	EXP_LAST                    	16

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

	bool bHwa;

	bool bStretch;
	bool bPlayAfterLoad;

	bool bLoop;

	int audioQSize;
	int videoQSize;

	bool bAccurateSeek;

	bool bCache;

	AVHWDeviceType hwDeviceType;

	bool bForceNoAudio;

	bool bBuffer[2];
	int buffer[47];

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

	// MemSf
	LPSURFACE pMemSf;	
	LPSURFACE pGrabbedFrame;

	// HwaSf
	LPSURFACE pHwaSf;

	bool bHwa;

	bool bStretch;
	bool bPlayAfterLoad;

	std::wstring* pFilePath;

	bool bLoop;

	int audioQSize;
	int videoQSize;

	bool bAccurateSeek;

	bool bOpen;
	bool bPlay;	
	bool bPlayStateUpdated;	

	int volume;

	FFMpeg* pFFMpeg;
	Timer* pPreviousTimer;

	Encryption* pEncrypt;
	
	bool bPm;
	bool bChanged;

	double scaleX;
	double scaleY;

	float atempo;

	bool bCache;
	GlobalData* pData;

	AVHWDeviceType hwDeviceType;
	bool bForceNoAudio;

	std::string* pVideoOverrideCodecName = nullptr;
	std::string* pAudioOverrideCodecName = nullptr;

	std::wstring* pRetStr;

} RUNDATA;
typedef	RUNDATA	*			LPRDATA;

// Size when editing the object under level editor
// -----------------------------------------------
#define	MAX_EDITSIZE			sizeof(EDITDATA)

// Default flags - see documentation for more info
// -------------
#define	OEFLAGS      			(OEFLAG_VALUES|OEFLAG_SPRITES|OEFLAG_MOVEMENTS|OEFLAG_SCROLLINGINDEPENDANT|OEFLAG_NEVERKILL|OEFLAG_RUNBEFOREFADEIN|OEFLAG_MANUALSLEEP|OEFLAG_NEVERSLEEP|OEFLAG_BACKSAVE)
#define	OEPREFS      			(OEPREFS_SCROLLINGINDEPENDANT|OEPREFS_INKEFFECTS|OEPREFS_BACKSAVE|OEPREFS_BACKEFFECTS)


// If to handle message, specify the priority of the handling procedure
// 0= low, 255= very high. You should use 100 as normal.                                                
// --------------------------------------------------------------------
#define	WINDOWPROC_PRIORITY		100
