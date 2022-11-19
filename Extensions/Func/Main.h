// Object identifier "Func"
#define IDENTIFIER	MAKEID(F,u,n,c)		// REQUIRED: you MUST replace the letters in the MAKEID macro by others
										// and then remove the #pragma message above. If you do not do this, MMF2
										// could confuse your object with another in the event editor.

// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define	CND_CONDITION_OF			0

#define	CND_CONDITION_FHPA			1
#define	CND_CONDITION_FHRA			2

#define	CND_CONDITION_FHTP			3
#define	CND_CONDITION_CFHTP			4

#define	CND_CONDITION_FPAIN			5
#define	CND_CONDITION_FRAIN			6

#define	CND_CONDITION_FTPIN			7
#define	CND_CONDITION_CFTPIN		8

#define	CND_CONDITION_OITO			9
#define	CND_CONDITION_SA			10

#define	CND_CONDITION_IE			11
#define	CND_CONDITION_IS			12

#define	CND_LAST					13

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define	ACT_ACTION_SR				0
#define	ACT_ACTION_PR				1

#define	ACT_ACTION_CF				2

#define	ACT_ACTION_T				3

#define	ACT_ACTION_STV				4
#define	ACT_ACTION_SCTV				5

#define	ACT_ACTION_SAR				6

#define	ACT_ACTION_SLI				7
#define	ACT_ACTION_SL				8

#define	ACT_ACTION_PASSRET			9
#define	ACT_ACTION_PASSRETV			10

#define	ACT_ACTION_STVV				11
#define	ACT_ACTION_SCTVV			12

#define	ACT_ACTION_SRV				13
#define	ACT_ACTION_PRV				14

#define	ACT_ACTION_ITO				15

#define	ACT_ACTION_SSGP				16
#define	ACT_ACTION_SVGP				17

#define	ACT_ACTION_ASSERT			18
#define	ACT_ACTION_MSGBOX			19

#define	ACT_LAST					20

// -------------------------------
// DEFINITION OF EXPRESSIONS CODES
// -------------------------------
#define	EXP_EXPRESSION_RV			0
#define EXP_EXPRESSION_RS			1

#define EXP_EXPRESSION_GVP			2
#define EXP_EXPRESSION_GSP			3

#define EXP_EXPRESSION_GVR			4
#define EXP_EXPRESSION_GSR			5

#define EXP_EXPRESSION_GRI			6

#define EXP_EXPRESSION_GPS			7
#define EXP_EXPRESSION_GRS			8

#define EXP_EXPRESSION_TRV			9
#define EXP_EXPRESSION_TRS			10

#define EXP_EXPRESSION_GVTP			11
#define EXP_EXPRESSION_GSTP			12

#define EXP_EXPRESSION_GCFN			13

#define EXP_EXPRESSION_GVCTP		14
#define EXP_EXPRESSION_GSCTP		15

#define EXP_EXPRESSION_GLI			16

#define EXP_EXPRESSION_GVGP			17
#define EXP_EXPRESSION_GSGP			18

#define EXP_EXPRESSION_ES			19
#define EXP_EXPRESSION_EV			20

#define EXP_EXPRESSION_CTB			21
#define EXP_EXPRESSION_NEG			22
#define EXP_EXPRESSION_AND			23
#define EXP_EXPRESSION_OR			24
#define EXP_EXPRESSION_XOR			25

#define EXP_EXPRESSION_GV			26
#define EXP_EXPRESSION_GOEV			27
#define EXP_EXPRESSION_LV			28
#define EXP_EXPRESSION_LOEV			29

#define	EXP_LAST                    30

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

	bool CompatibleMode;

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
	bool CompatibleMode;

	LPVEC FuncNameStack;

	LPPARAMSTACK FuncParamStack;
	LPTPARAM FuncTempParam;

	LPPARAMMAP GlobalTempParam;

	LPPARAMVEC FuncReturn;

	LPLIDX FuncLoopIndex;
	LPLIDX FuncCurLoopIndex;

	LPLIDX RecursiveIndex;	

	//LPSTRING OutPut;
	LPTSTR OutPut = nullptr;

	std::wstring* pPreviousFuncName = nullptr;

	ObjectSelection* pSelect;
	std::wstring* pOnItObjName;
	LPRO pObject;

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
