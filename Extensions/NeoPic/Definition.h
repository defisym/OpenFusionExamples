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

using List = std::vector<std::wstring>;
using KeepList = List;
using pPreLoadList = List*;
using PreLoadList = List;

struct Count {
	size_t count;			// total ref times
	size_t priority;		// lib size when first time ref
	size_t curRef;			// current ref times

	inline size_t GetWeight(size_t countWeight) {
		return this->count * countWeight + this->priority;
	}
};

struct SurfaceLibValue {
	LPSURFACE pSf;
	std::wstring Hash;
	BOOL isTransparent;
};

using SurfaceLib = std::map<std::wstring, SurfaceLibValue>;
using SurfaceLibIt = SurfaceLib::iterator;
using RefCount = std::map<std::wstring, Count>;
using RefCountPair = std::pair<std::wstring, Count>;
using RefCountVec = std::vector<RefCountPair>;
using FileList = std::vector<std::wstring>;
using FileListMap = std::map<std::wstring, FileList*>;

constexpr auto ONPRELOADCOMPLETE = 0;
constexpr auto ONITREFCOUNT = 1;

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
	FileListMap* pFileListMap;
};

inline void DeleteLib(SurfaceLib* pData);

inline void DeleteGlobalData(GlobalData* pData) {
	DeleteLib(pData->pLib);
	delete pData->pCount;
	delete pData->pKeepList;
	
	for(auto& it : *pData->pFileListMap) {
		delete it.second;
	}
	delete pData->pFileListMap;

	delete pData;
}

constexpr auto DEFAULT_FILELISTSIZE = 1000;