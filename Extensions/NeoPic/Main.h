// Object identifier "NPIC"
#define IDENTIFIER	MAKEID(N,P,I,C)		// REQUIRED: you MUST replace the letters in the MAKEID macro by others
										// and then remove the #pragma message above. If you do not do this, MMF2
										// could confuse your object with another in the event editor.

// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define	CND_CONDITION_OPLC			0
#define	CND_CONDITION_OITRC			1

#define	CND_CONDITION_CD			2

#define	CND_CONDITION_CDT			3
#define	CND_CONDITION_LHI			4

#define	CND_CONDITION_OAF			5
#define	CND_CONDITION_IAP			6
#define	CND_CONDITION_IAPAUSED		7

#define	CND_CONDITION_OLC		    8

#define	CND_CONDITION_IAF		    9

#define	CND_LAST					10

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define	ACT_ACTION_LFF				0
#define	ACT_ACTION_LFL				1

#define	ACT_ACTION_RL				2
#define	ACT_ACTION_EL				3
#define	ACT_ACTION_UL				4

#define	ACT_ACTION_SH				5
#define	ACT_ACTION_Z				6
#define	ACT_ACTION_R				7

#define	ACT_ACTION_AOTL				8
#define	ACT_ACTION_DOFL				9

#define	ACT_ACTION_S				10

#define	ACT_ACTION_AB				11
#define	ACT_ACTION_ULCSF				12

#define	ACT_ACTION_SLC				13
#define	ACT_ACTION_SQ				14

#define	ACT_ACTION_PT				15
#define	ACT_ACTION_O				16

#define	ACT_ACTION_LFD				17

#define	ACT_ACTION_SPLBP			18
#define	ACT_ACTION_SPP				19
#define	ACT_ACTION_CC				20

#define	ACT_ACTION_SKL				21

#define	ACT_ACTION_ITRC				22

#define	ACT_ACTION_LFP				23

#define	ACT_ACTION_SB				24

#define	ACT_ACTION_SESP				25

#define	ACT_ACTION_SPL				26
#define	ACT_ACTION_SKLBP			27

#define	ACT_ACTION_FM				28

#define	ACT_ACTION_STF				29

#define	ACT_ACTION_CF				30

#define	ACT_ACTION_STFWS			31

#define	ACT_ACTION_STC				32

#define	ACT_ACTION_SAS				33
#define	ACT_ACTION_LA				34
#define	ACT_ACTION_SA				35

#define	ACT_ACTION_SASPEED			36
#define	ACT_ACTION_PA				37
#define	ACT_ACTION_RA				38
#define	ACT_ACTION_SAFID			39
#define	ACT_ACTION_SAFINDEX			40
#define	ACT_ACTION_SASTEP			41

#define	ACT_ACTION_SNSS				42
#define	ACT_ACTION_LNS				43
#define	ACT_ACTION_RNS				44
#define	ACT_ACTION_RENDERNS			45

#define	ACT_LAST					46

// -------------------------------
// DEFINITION OF EXPRESSIONS CODES
// -------------------------------
#define	EXP_EXPRESSION_GHSX				0
#define EXP_EXPRESSION_GHSY				1

#define EXP_EXPRESSION_GOW				2
#define EXP_EXPRESSION_GOH				3

#define EXP_EXPRESSION_GCW				4
#define EXP_EXPRESSION_GCH				5

#define EXP_EXPRESSION_GXZS				6
#define EXP_EXPRESSION_GYZS				7

#define EXP_EXPRESSION_GA				8

#define EXP_EXPRESSION_GFN				9
#define EXP_EXPRESSION_GK				10

#define EXP_EXPRESSION_GSP				11

#define EXP_EXPRESSION_GAVGCX			12
#define EXP_EXPRESSION_GAVGCY			13

#define EXP_EXPRESSION_GFP				14
#define EXP_EXPRESSION_GRFP				15

#define EXP_EXPRESSION_GITRCK			16
#define EXP_EXPRESSION_GITRCVC			17
#define EXP_EXPRESSION_GITRCVP			18

#define EXP_EXPRESSION_GFFN				19
#define EXP_EXPRESSION_GFRFP			20

#define EXP_EXPRESSION_GRUMB           	21

#define EXP_EXPRESSION_GGPUN           	22

#define EXP_EXPRESSION_GVRUMB          	23
#define EXP_EXPRESSION_GVRBMB          	24
#define EXP_EXPRESSION_GVRAMB          	25

#define EXP_EXPRESSION_GAN          	26
#define EXP_EXPRESSION_GAFID          	27
#define EXP_EXPRESSION_GAFINDEX         28
#define EXP_EXPRESSION_GAS              29
#define EXP_EXPRESSION_GASTEP           30

#define EXP_EXPRESSION_GLCFN            31
#define EXP_EXPRESSION_GLCSF            32

#define EXP_EXPRESSION_GAFN             33

#define EXP_EXPRESSION_GGIC             34

#define	EXP_LAST                    	35

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

	//Lib
	bool isLib = false;

	bool bLoadCallback = false;
	bool bLoadKeepAngle = false;

	//Display

	//Settings
	bool HWA = false;
	bool stretchQuality = false;

	size_t memoryLimit;
	size_t sizeLimit;
	bool autoClean;

	size_t hotSpotComboID;

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

	//------------
	// Global
	//------------

	GlobalData* pData;

#ifdef QUERY_VRAM
    VRAMUtilities* pVRAMU = nullptr;
#endif

	//------------
	// Lib
	//------------

	bool isLib = false;
	SurfaceLib* pLib = nullptr;							// kept over frames
	
	bool bLoadCallback = false;
	LoadCallbackInfo* pLoadCallbackInfo = nullptr;

	bool bLoadKeepAngle = false;

	// for iterate
	std::wstring* itCountVecStr = nullptr;
	Count* itCountVecCount = nullptr;

	//------------
	// Display
	//------------

	// Settings
	volatile bool HWA = false;
	volatile bool stretchQuality = false;

	// Source
	bool fromLib = false;
	SurfaceLibValue* pLibValue = nullptr;

	std::wstring* FileName = nullptr;
	std::wstring* FilePath = nullptr;
	std::wstring* RelativeFilePath = nullptr;
	std::wstring* Key = nullptr;
		
	// src->display
	LPSURFACE src = nullptr;

	// flip cache
	LPSURFACE pSf_Nor = nullptr;
	LPSURFACE pSf_HF = nullptr;
	LPSURFACE pSf_VF = nullptr;
	LPSURFACE pSf_VHF = nullptr;

	BOOL isTransparent = -1;
	bool bCurrentDisplayTransparent = false;

	float angle = 0;
	HotSpotPos hotSpotPos;
	POINT hotSpot = { 0,0 };
	ZoomScale zoomScale = { 1.0,1.0 };

	LPSMASK pColMask = nullptr;

	// capture
	FrameCapture* pFrameCapture = nullptr;

	// animation
	AnimationInterface* pAI = nullptr;
	// nine slice
	NineSliceInterface* pNS = nullptr;
} RUNDATA;
typedef	RUNDATA	*			LPRDATA;

// Size when editing the object under level editor
// -----------------------------------------------
#define	MAX_EDITSIZE			sizeof(EDITDATA)

// Default flags - see documentation for more info
// -------------
//#define	OEFLAGS      			(OEFLAG_VALUES|OEFLAG_SPRITES|OEFLAG_MOVEMENTS|OEFLAG_SCROLLINGINDEPENDANT|OEFLAG_NEVERKILL|OEFLAG_RUNBEFOREFADEIN|OEFLAG_MANUALSLEEP|OEFLAG_NEVERSLEEP|OEFLAG_QUICKDISPLAY|OEFLAG_BACKSAVE)
#define	OEFLAGS      			(OEFLAG_VALUES|OEFLAG_SPRITES|OEFLAG_MOVEMENTS|OEFLAG_SCROLLINGINDEPENDANT|OEFLAG_NEVERKILL|OEFLAG_RUNBEFOREFADEIN|OEFLAG_MANUALSLEEP|OEFLAG_NEVERSLEEP|OEFLAG_BACKSAVE)
#define	OEPREFS      			(OEPREFS_SCROLLINGINDEPENDANT|OEPREFS_INKEFFECTS|OEPREFS_BACKSAVE|OEPREFS_BACKEFFECTS|OEPREFS_FINECOLLISIONS)


// If to handle message, specify the priority of the handling procedure
// 0= low, 255= very high. You should use 100 as normal.                                                
// --------------------------------------------------------------------
#define	WINDOWPROC_PRIORITY		100
