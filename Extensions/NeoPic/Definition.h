#pragma once

//X : L M R
//Y : T M B
enum class HotSpotPos {
	LT,
	LM,
	LB,
	MT,
	MM,
	MB,
	RT,
	RM,
	RB,
	CUSTOM
};

struct Size {
	int width;
	int height;
};

struct ZoomScale {
	float XScale;
	float YScale;
};

inline bool operator ==(ZoomScale A, ZoomScale B) {
	return (A.XScale == B.XScale) && (A.YScale == B.YScale);
}

inline bool operator !=(ZoomScale A, ZoomScale B) {
	return (A.XScale != B.XScale) || (A.YScale != B.YScale);
}

struct OffsetCoef {
	int XOffset;
	int YOffset;
	bool Wrap;
};

inline bool operator ==(OffsetCoef A, OffsetCoef B) {
	return (A.XOffset == B.XOffset) && (A.YOffset == B.YOffset) && (A.Wrap == B.Wrap);
}

inline bool operator !=(OffsetCoef A, OffsetCoef B) {
	return (A.XOffset != B.XOffset) || (A.YOffset != B.YOffset) || (A.Wrap != B.Wrap);
}

inline bool DoOffset(OffsetCoef A) {
	return (A.XOffset != 0) || (A.YOffset != 0);
}

//AT array
struct ATArray {
	double a11;
	double a12;
	double a21;
	double a22;
};

inline bool operator ==(ATArray A, ATArray B) {
	return (A.a11 == B.a11) && (A.a12 == B.a12) && (A.a21 == B.a21) && (A.a22 == B.a22);
}

inline bool operator !=(ATArray A, ATArray B) {
	return !(A == B);
}

inline bool DoAffineTrans(ATArray A) {
	return A != ATArray{ 1,0,0,1 };
}


typedef std::map<std::wstring, LPSURFACE> SurfaceLib;

#define SetExtUserData(pData) rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvSetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib, pData)
#define GetExtUserData() rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvGetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib)