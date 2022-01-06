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

typedef std::map<std::wstring, LPSURFACE> SurfaceLib;

#define SetExtUserData(pData) rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvSetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib,pData)
#define GetExtUserData() rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvGetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib)