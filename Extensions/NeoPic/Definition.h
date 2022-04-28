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

using pPreLoadList = std::vector<std::wstring>*;
using PreLoadList = std::remove_pointer_t<pPreLoadList>;

using SurfaceLib = std::map<std::wstring, LPSURFACE>;
using RefCount = std::map<std::wstring, size_t>;
using MapPair = std::pair<std::wstring, size_t>;
using KeepList = std::vector<std::wstring>;

#define ONPRELOADCOMPLETE 0

constexpr auto CLEAR_MEMRANGE = 100;
constexpr auto CLEAR_NUMTHRESHOLD = 50;

constexpr auto MAX_MEMORYLIMIT = 2048;
constexpr auto DEFAULT_MEMORYLIMIT = 1800;

template<typename T>
constexpr auto MemRange(T X) { return min(MAX_MEMORYLIMIT, max(0, X)); }

struct GlobalData {
	SurfaceLib* pLib;
	RefCount* pCount;
	KeepList* pKeepList;
};

inline void DeleteLib(SurfaceLib* pData);

inline void DeleteGlobalData(GlobalData* pData) {
	DeleteLib(pData->pLib);
	delete pData->pCount;
	delete pData;
}