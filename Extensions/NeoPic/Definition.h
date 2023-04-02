#pragma once
#include <EffectEx.h>

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
	size_t count;			// total ref times, count objects have used this
	size_t priority;		// lib size when first object ref this
	size_t curRef;			// current ref times, currently curRef objects are using this
							// erase safely if curRef == 0

	inline size_t GetWeight(size_t countWeight) const {
		return this->count * countWeight + this->priority;
	}
};

struct ShaderRef {
	LPRO pObject = nullptr;

	CEffectEx* pEffect = nullptr;
	LPCSTR m_pFxBuf = nullptr;

	std::wstring paramName;

	ShaderRef(LPRO pObject, CEffectEx* pEffect, const std::wstring& paramName) {
		this->pObject = pObject;
		this->pEffect = pEffect;
		this->paramName = paramName;

		this->m_pFxBuf = pEffect->m_pFxBuf;
	}
};

// for flipping
constexpr auto SurfaceLibSfNum = 4;

struct SurfaceLibValue {
	LPSURFACE pSf = nullptr;

	// don't change position, refered in GetEstimateMemUsage
	LPSURFACE pSf_VF = nullptr;
	LPSURFACE pSf_HF = nullptr;
	LPSURFACE pSf_VHF = nullptr;

	std::wstring Hash;
	BOOL isTransparent = -1;		// constexpr BOOL transpTBD = -1;
	bool bUsedInShader = false;
	std::vector<ShaderRef>* pShaderList = nullptr;

	SurfaceLibValue() = default;
	SurfaceLibValue(const LPSURFACE pSf, const std::wstring& hash, const BOOL bTransp) {
		this->pSf = pSf;
		this->Hash = hash;

		this->isTransparent = bTransp;
	}

	inline void RefShader(const ShaderRef& info) {
		bUsedInShader = true;

		if (pShaderList == nullptr) {
			pShaderList = new std::remove_pointer_t<decltype(pShaderList)>;
		}

		pShaderList->emplace_back(info);
	}

	template<typename T>
	static inline void ReleasePointer(T*& p) {
		delete p;
		p = nullptr;
	}

	inline void Release() {
		ReleasePointer(pSf);

		ReleasePointer(pSf_VF);
		ReleasePointer(pSf_HF);
		ReleasePointer(pSf_VHF);

		if (pShaderList != nullptr) {
			ReleasePointer(pShaderList);
		}
	}
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

template<typename T>
constexpr auto MemRange(T X) { return min(MAX_MEMORYLIMIT, max(0, X)); }

struct GlobalData {
	SurfaceLib* pLib = nullptr;
	RefCount* pCount = nullptr;
	KeepList* pKeepList = nullptr;
	FileListMap* pFileListMap = nullptr;

	uint64_t estimateRAMSizeMB = 0;
	uint64_t estimateVRAMSizeMB = 0;

	bool bDX11 = false;
	bool bPreMulAlpha = false;

#ifdef _USE_DXGI
	D3DUtilities* pD3DU = nullptr;
#ifdef _DYNAMIC_LINK
	HINSTANCE DXGI = nullptr;
#endif
#endif
};

inline void DeleteLib(SurfaceLib* pData);

inline void DeleteGlobalData(GlobalData* pData) {
	DeleteLib(pData->pLib);

	delete pData->pCount;
	delete pData->pKeepList;

	for (auto& it : *pData->pFileListMap) {
		delete it.second;
	}
	delete pData->pFileListMap;

#ifdef _USE_DXGI
	delete pData->pD3DU;

#ifdef _DYNAMIC_LINK
	if (pData->DXGI != nullptr) {
		FreeLibrary(pData->DXGI);
		pData->DXGI = nullptr;
	}
#endif

#endif

	delete pData;
}

constexpr auto DEFAULT_FILELISTSIZE = 1000;
constexpr auto Delimiter = L'|';
