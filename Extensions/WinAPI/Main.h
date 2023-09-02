﻿// Object identifier "DIO0"

#define IDENTIFIER	MAKEID(D,I,O,0)		// REQUIRED: you MUST replace the letters in the MAKEID macro by others
										// and then remove the #pragma message above. If you do not do this, MMF2
										// could confuse your object with another in the event editor.

// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define	CND_CONDITION_INAP			0
#define	CND_CONDITION_IML			1
#define	CND_CONDITION_RKS			2
#define	CND_CONDITION_IFS			3
#define	CND_CONDITION_ICA			4
#define	CND_CONDITION_IDHA			5
#define	CND_CONDITION_IAIR			6
#define	CND_CONDITION_IWF			7
#define	CND_CONDITION_ICLIHP		8
#define	CND_CONDITION_OD			9
#define	CND_CONDITION_IDHA_S		10
#define	CND_CONDITION_IRIE			11
#define	CND_CONDITION_IAAT			12
#define	CND_CONDITION_OMC			13
#define	CND_CONDITION_IOHA			14
#define	CND_CONDITION_IOHA_S		15
#define	CND_CONDITION_ORC			16
#define	CND_CONDITION_RMCX			17
#define	CND_CONDITION_OCB			18
#define	CND_CONDITION_OCF			19

#define	CND_LAST					20

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define ACT_ACTION_RUN				0
#define ACT_ACTION_RUN_16			1
#define ACT_ACTION_STOPBYNAME		2
#define ACT_ACTION_STOPBYID			3

#define ACT_ACTION_LOCKMOUSE		4
#define ACT_ACTION_LOCKMOUSEBR		5
#define ACT_ACTION_UNLOCKMOUSE		6

#define ACT_ACTION_SETMOUSE			7

#define ACT_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_ON			8
#define ACT_ACTION_LOCKMOUSESETTINGS_KEEPLOCK_OFF			9
#define ACT_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_ON			10
#define ACT_ACTION_LOCKMOUSESETTINGS_UPDATELOCK_OFF			11
#define ACT_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_ON			12
#define ACT_ACTION_LOCKMOUSESETTINGS_RECTOFFSET_OFF			13

#define ACT_ACTION_IME_DISABLE		14
#define ACT_ACTION_IME_ENABLE		15

#define ACT_ACTION_WINDOW_GF		16
#define ACT_ACTION_WINDOW_GW		17

#define ACT_ACTION_WINDOW_BFA		18
#define ACT_ACTION_WINDOW_LFC		19
#define ACT_ACTION_WINDOW_LFF		20
#define	ACT_ACTION_WINDOW_RGB		21
#define	ACT_ACTION_WINDOW_MTGB		22
#define	ACT_ACTION_WINDOW_LBG		23
#define ACT_ACTION_WINDOW_MTSB		24

#define ACT_ACTION_WINDOW_STF		25
#define ACT_ACTION_WINDOW_STC		26
#define ACT_ACTION_WINDOW_STT		27
#define ACT_ACTION_WINDOW_LFT		28

#define ACT_ACTION_SQ_F				29
#define ACT_ACTION_SQ_H				30

#define ACT_ACTION_SMS_ON			31
#define ACT_ACTION_SMS_OFF			32

#define ACT_ACTION_SDFN				33

#define ACT_ACTION_GFL				34
#define ACT_ACTION_SB642I			35

#define ACT_ACTION_SWS				36

#define ACT_ACTION_EF				37

#define ACT_ACTION_SAOT				38

#define ACT_ACTION_GVF				39

#define ACT_ACTION_RMS				40

#define	ACT_LAST					41

// -------------------------------
// DEFINITION OF EXPRESSIONS CODES
// -------------------------------
#define	EXP_EXPRESSION_GPIDBN		0

#define	EXP_EXPRESSION_GCLR_L		1
#define	EXP_EXPRESSION_GCLR_R		2
#define	EXP_EXPRESSION_GCLR_T		3
#define	EXP_EXPRESSION_GCLR_B		4

#define	EXP_EXPRESSION_RXO			5
#define	EXP_EXPRESSION_RYO			6

#define	EXP_EXPRESSION_RXS			7
#define	EXP_EXPRESSION_RYS			8

#define	EXP_EXPRESSION_GCWR_L		9
#define	EXP_EXPRESSION_GCWR_R		10
#define	EXP_EXPRESSION_GCWR_T		11
#define	EXP_EXPRESSION_GCWR_B		12

#define	EXP_EXPRESSION_GCLT			13

#define	EXP_EXPRESSION_IME_STATE	14

#define	EXP_EXPRESSION_DPISCALING	15

#define	EXP_EXPRESSION_IFS			16

#define	EXP_EXPRESSION_GFN			17

#define	EXP_EXPRESSION_GT			18
#define	EXP_EXPRESSION_GTPT			19

#define	EXP_EXPRESSION_GFLS			20
#define	EXP_EXPRESSION_GFLA			21

#define	EXP_EXPRESSION_GFH			22

#define	EXP_EXPRESSION_GAD			23

#define	EXP_EXPRESSION_GVWS			24

#define	EXP_EXPRESSION_HEX2RGB		25

#define	EXP_EXPRESSION_GSON			26

#define	EXP_EXPRESSION_W2L			27
#define	EXP_EXPRESSION_L2W			28

#define	EXP_EXPRESSION_GCLI			29

#define	EXP_EXPRESSION_C2B			30

#define	EXP_EXPRESSION_GVFP			31

#define	EXP_EXPRESSION_GD			32

#define	EXP_EXPRESSION_GCMW			33
#define	EXP_EXPRESSION_GCMH			34

#define	EXP_EXPRESSION_GFFN			35

#define	EXP_EXPRESSION_GDT			36

#define	EXP_EXPRESSION_GTPM			37
#define	EXP_EXPRESSION_GFPM			38

#define	EXP_EXPRESSION_GAID			39

#define	EXP_EXPRESSION_GCFC			40
#define	EXP_EXPRESSION_GADFC		41

#define	EXP_LAST                    42

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

	bool Display = false;
	
	//切换窗口后保持锁定
	bool KeepLock;
	//拖拽窗口后更新锁定
	bool UpdateLock;
	//区域锁定相对窗口坐标
	bool RectOffset_State;

	//持续保持输入法状态
	bool KeepIMEState;

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

	bool Display = false;

	//显示Surface
	LPSURFACE img;
	//备份Surface
	LPSURFACE temp;

	bool StretchQuality = false;
	bool MultiThreadSave = true;

	//用于保存的RunHeader
	fprh rhPtr = NULL;

	//主窗口句柄
	HWND MainWindowHandle = NULL;
	//场景区域窗口句柄
	HWND FrameWindowHandle = NULL;

	//APP分辨率
	int AppW;
	int AppH;

	//场景大小
	int FrameW;
	int FrameH;
		
	//窗口是否锁定
	bool Lock = false;
	//窗口锁定类别
	int LockType;
	//切换窗口后保持锁定
	bool KeepLock;
	//拖拽窗口后更新锁定
	bool UpdateLock;
	//相对于场景区域以矩形区域锁定时的缩放比例
	DPOINT FrameScale = { 1,1 };

	//区域锁定类型
	int RectOffset_Type;
	//区域锁定相对窗口坐标
	bool RectOffset_State;
	//当前鼠标锁定的矩形区域相对于窗口的偏移
	RECT RectOffset = { 0,0,0,0 };
	//指定的矩形区域
	RECT UserSetRect = { 0,0,0,0 };

	//当前鼠标锁定的矩形区域
	RECT CurrentLockRect;

	//持续保持输入法状态
	bool KeepIMEState;

	//程序是否已经缩放
	bool AppScaled;

	//图像的默认存储格式
	//指向常量，因此无需释放
	LPCWSTR DefaultFilterName;

	//当前时间字符串
	LPWSTR CurrentTime;

	//总游玩时间字符串
	LPWSTR TotalPlayTime;

	//文件列表
	std::vector<std::wstring>* FileList;
	LPWSTR FileListOutPut;

	LPWSTR HashOutput;

	LPWSTR ValWithSignOutput;

	ObjectSelection* pSelect;

	int argc;
	char** argv;
	
	LPSURFACE pHwaSf_Video;

	bool bSecondFrame;

	HMONITOR curMonitorHandle;

	LONG curMonitorWidth;
	LONG curMonitorHeight;
} RUNDATA;
typedef	RUNDATA	*			LPRDATA;

// Size when editing the object under level editor
// -----------------------------------------------
#define	MAX_EDITSIZE			sizeof(EDITDATA)

// Default flags - see documentation for more info
// -------------
//#define	OEFLAGS      			(OEFLAG_VALUES|OEFLAG_SCROLLINGINDEPENDANT|OEFLAG_NEVERKILL|OEFLAG_RUNBEFOREFADEIN|OEFLAG_MANUALSLEEP|OEFLAG_SPRITES|OEFLAG_QUICKDISPLAY|OEFLAG_BACKSAVE|OEFLAG_MOVEMENTS)
//#define	OEPREFS      			(OEPREFS_SCROLLINGINDEPENDANT|OEPREFS_KILL|OEPREFS_SLEEP|OEPREFS_INKEFFECTS|OEPREFS_BACKSAVE|OEPREFS_BACKEFFECTS)

//#define	OEFLAGS      			OEFLAG_VALUES|OEFLAG_SPRITES|OEFLAG_QUICKDISPLAY|OEFLAG_BACKSAVE|OEFLAG_MOVEMENTS
//#define	OEPREFS      			OEPREFS_SCROLLINGINDEPENDANT|OEPREFS_KILL|OEPREFS_SLEEP|OEPREFS_INKEFFECTS|OEPREFS_BACKSAVE

#define	OEFLAGS      			(OEFLAG_VALUES|OEFLAG_SCROLLINGINDEPENDANT|OEFLAG_NEVERKILL|OEFLAG_RUNBEFOREFADEIN|OEFLAG_MANUALSLEEP|OEFLAG_NEVERSLEEP|OEFLAG_SPRITES|OEFLAG_QUICKDISPLAY|OEFLAG_BACKSAVE|OEFLAG_MOVEMENTS|OEFLAG_WINDOWPROC)
#define	OEPREFS      			(OEPREFS_SCROLLINGINDEPENDANT|OEPREFS_INKEFFECTS|OEPREFS_BACKSAVE|OEPREFS_BACKEFFECTS)

// If to handle message, specify the priority of the handling procedure
// 0= low, 255= very high. You should use 100 as normal.                                                
// --------------------------------------------------------------------
#define	WINDOWPROC_PRIORITY		100
