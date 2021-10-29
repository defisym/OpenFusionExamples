// Object identifier "Func"
#define IDENTIFIER	MAKEID(F,u,n,c)		// REQUIRED: you MUST replace the letters in the MAKEID macro by others
										// and then remove the #pragma message above. If you do not do this, MMF2
										// could confuse your object with another in the event editor.

// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define	CND_CONDITION_OF			0
#define	CND_LAST					1

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

#define	ACT_LAST					7

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

#define	EXP_LAST                    16

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
	LPVEC FuncNameStack;

	LPSTACK FuncParamStack;
	LPTPARAM FuncTempParamStack;

	LPLIDX FuncLoopIndex;
	LPLIDX FuncCurLoopIndex;

	LPLIDX RecursiveIndex;

	LPVEC FuncReturn;

	//LPSTRING OutPut;
	LPTSTR OutPut = nullptr;

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
