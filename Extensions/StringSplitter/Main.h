// Object identifier "SSPL"
#define IDENTIFIER	MAKEID(S,S,P,L)		// REQUIRED: you MUST replace the letters in the MAKEID macro by others
										// and then remove the #pragma message above. If you do not do this, MMF2
										// could confuse your object with another in the event editor.

// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define	CND_CONDITION_OISSV			0
#define	CND_CONDITION_OIKWPV		1
#define	CND_CONDITION_OIMSS			2

#define	CND_CONDITION_SMR			3
#define	CND_CONDITION_SHR			4

#define	CND_CONDITION_OIRPE			5

#define	CND_LAST					6

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define	ACT_ACTION_RP				0

#define	ACT_ACTION_LFF				1
#define	ACT_ACTION_LFS				2

#define	ACT_ACTION_RRF				3
#define	ACT_ACTION_SRF				4
#define	ACT_ACTION_SCI				5

#define	ACT_ACTION_SR				6
#define	ACT_ACTION_SR_S				7
#define	ACT_ACTION_SR_EL			8
#define	ACT_ACTION_SR_C				9
#define	ACT_ACTION_SR_ID			10
#define	ACT_ACTION_SR_K				11

#define	ACT_ACTION_SD				12

#define	ACT_ACTION_ITSSV			13
#define	ACT_ACTION_ITKWPV			14

#define	ACT_ACTION_GASSIS			15
#define	ACT_ACTION_ITMSS			16

#define	ACT_ACTION_AS				17

#define	ACT_ACTION_ITRE				18
#define	ACT_ACTION_ITRE_SRS			19
#define	ACT_ACTION_ITREA			20

#define	ACT_LAST					21

// -------------------------------
// DEFINITION OF EXPRESSIONS CODES
// -------------------------------
#define	EXP_EXPRESSION_RS				0

#define EXP_EXPRESSION_GMSSP			1

#define EXP_EXPRESSION_GLMSS			2
#define	EXP_EXPRESSION_GMSSPIS			3

#define EXP_EXPRESSION_GMSSS			4
#define EXP_EXPRESSION_GMSSIMSSV		5

#define	EXP_EXPRESSION_ITGCMSS			6

#define EXP_EXPRESSION_GSSVS			7
#define EXP_EXPRESSION_GSSISSV			8

#define	EXP_EXPRESSION_ITGCSS			9

#define EXP_EXPRESSION_GNKW				10
#define EXP_EXPRESSION_GNKWP			11

#define EXP_EXPRESSION_GKWPVS			12
#define EXP_EXPRESSION_GKWIKWPV			13
#define EXP_EXPRESSION_GKWPIKWPV		14

#define EXP_EXPRESSION_ITGCKW			15
#define EXP_EXPRESSION_ITGCKWP			16

#define EXP_EXPRESSION_GSD				17

#define EXP_EXPRESSION_GRECM			18
#define EXP_EXPRESSION_GRES				19

#define EXP_EXPRESSION_GSSVP			20

#define	EXP_LAST                    	21

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
	//Split
	Split* S;
	
	//Auto Split
	bool AutoSplit = false;

	//LoopName
	LPTSTR SplitStrVecLoopName = nullptr;
	LPCTSTR CurrentSplitStr = nullptr;

	LPTSTR KeyWordPairVecLoopName = nullptr;
	LPCTSTR CurrentKeyWord = nullptr;
	int CurrentKeyWordPos = 0;

	LPTSTR SubStringVecLoopName = nullptr;
	LPCTSTR CurrentSubString = nullptr;

	//ReplaceEach
	LPTSTR ReplaceEachLoopName = nullptr;

	LPCTSTR CurrentMatchString = nullptr;
	LPTSTR CurrentReplaceString = nullptr;

	LPTSTR ReplacEachResult = nullptr;

	//Work with other ext
	//std::vector<std::wstring>* pSubVec = nullptr;

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
