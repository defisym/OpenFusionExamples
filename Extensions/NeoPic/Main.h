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

#define	CND_LAST					5

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

#define	ACT_ACTION_US				8
#define	ACT_ACTION_RC				9

#define	ACT_ACTION_S				10

#define	ACT_ACTION_AB				11
#define	ACT_ACTION_UC				12

#define	ACT_ACTION_SC				13
#define	ACT_ACTION_SQ				14

#define	ACT_ACTION_AT				15
#define	ACT_ACTION_O				16

#define	ACT_ACTION_LFD				17

#define	ACT_ACTION_SPL				18
#define	ACT_ACTION_SPP				19
#define	ACT_ACTION_CC				20

#define	ACT_ACTION_SKL				21

#define	ACT_ACTION_ITRC				22

#define	ACT_ACTION_LFP				23

#define	ACT_ACTION_SB				24

#define	ACT_LAST					25

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

#define	EXP_LAST                    	26

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

	//Collision
	bool collision = false;
	bool autoUpdateCollision = false;

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

	//Lib
	bool isLib = false;
	SurfaceLib* pLib = nullptr;							// kept over frames

	//Collision
	bool collision = false;
	bool autoUpdateCollision = false;

	LPSMASK pColMask = nullptr;
	LPSURFACE pCollisionSf = nullptr;

	//Display

	//Settings
	volatile bool HWA = false;
	volatile bool stretchQuality = false;

	//Source
	bool fromLib = false;
	SurfaceLibValue* pLibValue = nullptr;
	size_t* pRefCount = nullptr;

	std::wstring* FileName = nullptr;
	std::wstring* FilePath = nullptr;
	std::wstring* RelativeFilePath = nullptr;
	std::wstring* Key = nullptr;

	//img->collision & add backdrop
	LPSURFACE img = nullptr;
	
	POINT imgHotSpot = { 0,0 };
	ZoomScale imgZoomScale = { 1.0,1.0 };

	OffsetCoef imgOffset = { 0,0,false };
	
	int imgAngle = 0;
	ATArray imgAT = {};

	//src->display
	LPSURFACE src = nullptr;
	
	POINT hotSpot = { 0,0 };	
	ZoomScale zoomScale = { 1.0,1.0 };

	OffsetCoef offset = { 0,0,false };

	int angle = 0;
	ATArray AT = {};

	//trans->transform
	LPSURFACE trans = nullptr;

	bool changed = false;

	//preload		
	std::vector<std::wstring>* pPreloadList = nullptr;
	SurfaceLib* preloadLib = nullptr;

	volatile HANDLE threadID;
	volatile bool forceExit = false;
	volatile bool preloading = false;
	volatile bool preloadMerge = false;

	size_t memoryLimit;
	size_t sizeLimit;
	bool autoClean;

	RefCount* pCount = nullptr;							// kept over frames
	KeepList* pKeepList = nullptr;						// kept over frames
	RefCountVec* pCountVec = nullptr;					// update when trigger clear

	HotSpotPos defaultHotSpot;

	GlobalData* pData;

	std::wstring* itCountVecStr = nullptr;
	Count* itCountVecCount = nullptr;

	FileListMap* pFileListMap = nullptr;

	bool bCurrentDisplayTransparent = false;
	LPSURFACE pOldSf = nullptr;

#ifdef _USE_DXGI
	D3DUtilities* pD3DU = nullptr;
#endif

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
