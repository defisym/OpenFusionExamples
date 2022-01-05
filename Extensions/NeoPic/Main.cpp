// ============================================================================
//
// This file are where the Conditions/Actions/Expressions are defined.
// You can manually enter these, or use CICK (recommended)
// See the Extension FAQ in this SDK for more info and where to download it
//
// ============================================================================

// Common Include
#include	"common.h"

// Quick memo: content of the eventInformations arrays
// ---------------------------------------------------
// Menu ID
// String ID
// Code
// Flags
// Number_of_parameters
// Parameter_type [Number_of_parameters]
// Parameter_TitleString [Number_of_parameters]

// Definitions of parameters for each condition
short conditionsInfos[]=
		{
		IDMN_CONDITION, M_CONDITION, CND_CONDITION, EVFLAGS_ALWAYS, 3, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_CND_P1, M_CND_P2, M_CND_P3,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_LFF, M_ACTION_LFF,	ACT_ACTION_LFF,	0, 2,PARAM_EXPSTRING,PARAM_EXPSTRING,M_ACTION_FILENAME,M_ACTION_KEY,
		IDMN_ACTION_LFL, M_ACTION_LFL,	ACT_ACTION_LFL,	0, 3,PARAM_OBJECT,PARAM_EXPSTRING,PARAM_EXPSTRING,M_ACTION_OBJECT,M_ACTION_FILENAME,M_ACTION_KEY,
		
		IDMN_ACTION_RL, M_ACTION_RL,	ACT_ACTION_RL,	0, 0,
		IDMN_ACTION_EL, M_ACTION_EL,	ACT_ACTION_EL,	0, 1,PARAM_EXPSTRING,M_ACTION_FILENAME,
		IDMN_ACTION_UL, M_ACTION_UL,	ACT_ACTION_UL,	0, 2,PARAM_EXPSTRING,PARAM_EXPSTRING,M_ACTION_FILENAME,M_ACTION_KEY,
		
		IDMN_ACTION_SH, M_ACTION_SH,	ACT_ACTION_SH,	0, 3,PARAM_EXPRESSION,PARAM_EXPRESSION,PARAM_EXPRESSION,M_ACTION_HOTSPOT,M_ACTION_HOTSPOT_X,M_ACTION_HOTSPOT_Y,

		IDMN_ACTION_Z, M_ACTION_Z,	ACT_ACTION_Z,	0, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,M_ACTION_Z_XSCALE,M_ACTION_Z_YSCALE,

		IDMN_ACTION_R, M_ACTION_R,	ACT_ACTION_R,	0, 1,PARAM_EXPRESSION,M_ACTION_A,

		IDMN_ACTION_US, M_ACTION_US,	ACT_ACTION_US,	0, 0,
		IDMN_ACTION_RC, M_ACTION_RC,	ACT_ACTION_RC,	0, 0,

		IDMN_ACTION_S, M_ACTION_S,	ACT_ACTION_S,	0, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,M_ACTION_S_WIDTH,M_ACTION_S_HEIGHT,

		IDMN_ACTION_AB, M_ACTION_AB,	ACT_ACTION_AB,	0, 1,PARAM_PASTE,0,

		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GHSX, M_EXPRESSION_GHSX, EXP_EXPRESSION_GHSX, 0, 0,
		IDMN_EXPRESSION_GHSY, M_EXPRESSION_GHSY, EXP_EXPRESSION_GHSY, 0, 0,

		IDMN_EXPRESSION_GOW, M_EXPRESSION_GOW, EXP_EXPRESSION_GOW, 0, 0,
		IDMN_EXPRESSION_GOH, M_EXPRESSION_GOH, EXP_EXPRESSION_GOH, 0, 0,

		IDMN_EXPRESSION_GCW, M_EXPRESSION_GCW, EXP_EXPRESSION_GCW, 0, 0,
		IDMN_EXPRESSION_GCH, M_EXPRESSION_GCH, EXP_EXPRESSION_GCH, 0, 0,
		
		IDMN_EXPRESSION_GXZS, M_EXPRESSION_GXZS, EXP_EXPRESSION_GXZS, EXPFLAG_DOUBLE, 0,
		IDMN_EXPRESSION_GYZS, M_EXPRESSION_GYZS, EXP_EXPRESSION_GYZS, EXPFLAG_DOUBLE, 0,

		IDMN_EXPRESSION_GA, M_EXPRESSION_GA, EXP_EXPRESSION_GA, 0, 0,
		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

// -----------------
// Sample Condition
// -----------------
// Returns TRUE when the two values are equal!
// 

long WINAPI DLLExport Condition(LPRDATA rdPtr, long param1, long param2)
{

//  **** Still use this method for 1 or 2 parameters ****	
//	if (param1==param2)	
//		return TRUE;

	long p1 = CNC_GetParameter(rdPtr);
	long p2 = CNC_GetParameter(rdPtr);
	long p3 = CNC_GetParameter(rdPtr);

	if ((p1 + p2)==p3)
		return TRUE;
		 
	return FALSE;
}


// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport LoadFromeFile(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	LoadFromFile(rdPtr, FilePath, Key);

	return 0;
}

short WINAPI DLLExport LoadFromeLib(LPRDATA rdPtr, long param1, long param2) {
	LPRO object = (LPRO)CNC_GetParameter(rdPtr);

	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	LoadFromLib(rdPtr, object, FilePath, Key);

	return 0;
}

short WINAPI DLLExport ResetLib(LPRDATA rdPtr, long param1, long param2) {
	if(rdPtr->IsLib){
		ResetLib(rdPtr->Lib);
	}

	return 0;
}

short WINAPI DLLExport EraseLib(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	if (rdPtr->IsLib) {
		EraseLib(rdPtr->Lib, FilePath);		
	}

	return 0;
}

short WINAPI DLLExport UpdateLib(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	
	if (rdPtr->IsLib) {
		EraseLib(rdPtr->Lib, FilePath);
		LoadFromFile(rdPtr, FilePath, Key);
	}

	return 0;
}

short WINAPI DLLExport UpdateSrc(LPRDATA rdPtr, long param1, long param2) {
	if (!rdPtr->IsLib && rdPtr->img->IsValid()) {
		rdPtr->src->Delete();
		rdPtr->src->Clone(*rdPtr->img);

		rdPtr->SrcHotSpot = rdPtr->HotSpot;
	}

	return 0;
}
short WINAPI DLLExport RestoreCur(LPRDATA rdPtr, long param1, long param2) {
	if (!rdPtr->IsLib && rdPtr->src->IsValid()) {
		rdPtr->HotSpot = rdPtr->SrcHotSpot;

		rdPtr->img->Delete();
		rdPtr->img->Clone(*rdPtr->src);
	}

	return 0;
}

short WINAPI DLLExport SetHotSpot(LPRDATA rdPtr, long param1, long param2) {
	HotSpotPos Pos = (HotSpotPos)CNC_GetIntParameter(rdPtr);

	int X = (int)CNC_GetIntParameter(rdPtr);
	int Y = (int)CNC_GetIntParameter(rdPtr);
	
	UpdateHotSpot(rdPtr, Pos, X, Y);
	
	rdPtr->SrcHotSpot = rdPtr->HotSpot;

	return 0;
}

short WINAPI DLLExport Zoom(LPRDATA rdPtr, long param1, long param2) {
	float XScale = GetFloatParam(rdPtr);
	float YScale = GetFloatParam(rdPtr);
	
	if (!rdPtr->IsLib && rdPtr->src->IsValid()) {
		Zoom(rdPtr, XScale, YScale);

		ReDisplay(rdPtr);
	}	

	return 0;
}

short WINAPI DLLExport Rotate(LPRDATA rdPtr, long param1, long param2) {
	int Angle = (int)CNC_GetIntParameter(rdPtr);
	Angle = Angle % 360;
	
	if (!rdPtr->IsLib && rdPtr->src->IsValid()) {
		if (rdPtr->Angle == Angle) {
			return 0;
		}

		rdPtr->Angle = Angle;

		rdPtr->HotSpot = rdPtr->SrcHotSpot;
		RotatePoint(Angle, (int*)&rdPtr->HotSpot.x, (int*)&rdPtr->HotSpot.y, rdPtr->src->GetWidth(), rdPtr->src->GetHeight());

		//int width = rdPtr->src->GetWidth();
		//int height = rdPtr->src->GetHeight();

		//rdPtr->src->GetSizeOfRotatedRect(&width, &height, (float)Angle);

		delete rdPtr->img;
		//rdPtr->img = CreateSurface(24, width, height);
		rdPtr->img = CreateSurface(24, 4, 4);
		
		rdPtr->src->CreateRotatedSurface(*rdPtr->img, Angle, rdPtr->StretchQuality);
		//rdPtr->src->CreateRotatedSurface(*rdPtr->img, Angle, rdPtr->StretchQuality, DARK_GREEN);

		//if (rdPtr->HWA) {
		//	ConvertHWA(rdPtr);
		//	
		//	POINT Center = { 0,0 };			

		//	//RotatePoint(Angle, (int*)&rdPtr->HotSpot.x, (int*)&rdPtr->HotSpot.y);

		//	rdPtr->src->BlitEx(*rdPtr->img, 0, 0,
		//		1.0, 1.0, 0, 0,
		//		rdPtr->src->GetWidth(), rdPtr->src->GetHeight(), &Center, (float)Angle,
		//		(rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) ? BMODE_TRANSP : BMODE_OPAQUE,
		//		BlitOp(rdPtr->rs.rsEffect & EFFECT_MASK),
		//		rdPtr->rs.rsEffectParam, BLTF_ANTIA);
		//}
		//else {
		//	RotatePoint(Angle, (int*)&rdPtr->HotSpot.x, (int*)&rdPtr->HotSpot.y, rdPtr->src->GetWidth(), rdPtr->src->GetHeight());

		//	rdPtr->src->Rotate(*rdPtr->img, Angle, rdPtr->StretchQuality);
		//	//rdPtr->src->Rotate(*rdPtr->img, Angle, rdPtr->StretchQuality, DARK_GREEN);
		//}

		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport Stretch(LPRDATA rdPtr, long param1, long param2) {
	int Width = (int)CNC_GetIntParameter(rdPtr);
	int Height = (int)CNC_GetIntParameter(rdPtr);

	float XScale = (1.0f * Width/ rdPtr->src->GetWidth());
	float YScale = (1.0f * Height / rdPtr->src->GetHeight());

	if (!rdPtr->IsLib && rdPtr->src->IsValid()) {
		Zoom(rdPtr, XScale, YScale);

		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport AddBackdrop(LPRDATA rdPtr, long param1, long param2) {
	int nObstacleType = ((LPEVP)param1)->evp.evpW.evpW0;

	if (!rdPtr->IsLib && rdPtr->src->IsValid()) {
		AddBackdrop(rdPtr, rdPtr->img,
			rdPtr->rHo.hoX - rdPtr->rHo.hoAdRunHeader->rhWindowX - rdPtr->HotSpot.x, 
			rdPtr->rHo.hoY - rdPtr->rHo.hoAdRunHeader->rhWindowY - rdPtr->HotSpot.y,
			rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam, nObstacleType, rdPtr->rs.rsLayer);
	}

	return 0;
}
// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport GetHotSpotX(LPRDATA rdPtr, long param1) {
	return rdPtr->HotSpot.x;
}
long WINAPI DLLExport GetHotSpotY(LPRDATA rdPtr, long param1) {
	return rdPtr->HotSpot.y;
}

long WINAPI DLLExport GetOriginalWidth(LPRDATA rdPtr, long param1) {
	return rdPtr->src->IsValid() ? rdPtr->src->GetWidth() : -1;
}
long WINAPI DLLExport GetOriginalHeight(LPRDATA rdPtr, long param1) {
	return rdPtr->src->IsValid() ? rdPtr->src->GetHeight() : -1;
}

long WINAPI DLLExport GetCurrentWidth(LPRDATA rdPtr, long param1) {
	return rdPtr->img->IsValid() ? rdPtr->img->GetWidth() : -1;
}
long WINAPI DLLExport GetCurrentHeight(LPRDATA rdPtr, long param1) {
	return rdPtr->img->IsValid() ? rdPtr->img->GetHeight() : -1;
}

long WINAPI DLLExport GetXZoomScale(LPRDATA rdPtr, long param1) {
	return ReturnFloat(rdPtr->img->IsValid() ? rdPtr->ZoomScale.XScale : -1);
}
long WINAPI DLLExport GetYZoomScale(LPRDATA rdPtr, long param1) {
	return ReturnFloat(rdPtr->img->IsValid() ? rdPtr->ZoomScale.YScale : -1);
}

long WINAPI DLLExport GetAngle(LPRDATA rdPtr, long param1) {
	return rdPtr->img->IsValid() ? rdPtr->Angle : -1;
}

// ----------------------------------------------------------
// Condition / Action / Expression jump table
// ----------------------------------------------------------
// Contains the address inside the extension of the different
// routines that handle the action, conditions and expressions.
// Located at the end of the source for convinience
// Must finish with a 0
//
long (WINAPI * ConditionJumps[])(LPRDATA rdPtr, long param1, long param2) = 
			{ 
			Condition,

			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			LoadFromeFile,
			LoadFromeLib,

			ResetLib,
			EraseLib,
			UpdateLib,

			SetHotSpot,
			
			Zoom,
			Rotate,

			UpdateSrc,
			RestoreCur,

			Stretch,

			AddBackdrop,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			GetHotSpotX,
			GetHotSpotY,

			GetOriginalWidth,
			GetOriginalHeight,

			GetCurrentWidth,
			GetCurrentHeight,

			GetXZoomScale,
			GetYZoomScale,

			GetAngle,

			0
			};