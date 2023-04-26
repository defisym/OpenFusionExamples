#pragma once

#include <chrono>
#include <algorithm>

#include "EffectEx.h"

struct tagRDATA;
typedef tagRDATA RUNDATA;
typedef RUNDATA* LPRDATA;

constexpr auto ONPRELOADCOMPLETE = 0;
constexpr auto ONITREFCOUNT = 1;
constexpr auto ONANIMATIONFINISHED = 5;
constexpr auto ONLOADCALLBACK = 8;

struct ZoomScale {
	float XScale;
	float YScale;

	inline bool operator ==(const ZoomScale& B) const {
		return (this->XScale == B.XScale) && (this->YScale == B.YScale);
	}
};

struct OffsetCoef {
	int XOffset;
	int YOffset;
	bool Wrap;

	inline bool operator ==(const OffsetCoef& B) const {
		return (this->XOffset == B.XOffset) && (this->YOffset == B.YOffset) && (this->Wrap == B.Wrap);
	}
};

struct LoadCallbackInfo {
	std::wstring callbackFileName;
	LPSURFACE* ppSf;

	inline LPSURFACE GetSurfacePointer() const {
		return ppSf == nullptr ? nullptr : *ppSf;
	}
};

using List = std::vector<std::wstring>;
using KeepList = List;
using pPreLoadList = List*;
using PreLoadList = List;

struct Count {
	size_t totalRef;			// total ref times, count objects have used this
	size_t priority;		// lib size when first object ref this
	size_t curRef;			// current ref times, currently curRef objects are using this
							// erase safely if curRef == 0
	std::vector<tagRDATA*> pRefObj;

	Count() = default;
	~Count() = default;

	inline size_t GetWeight(size_t countWeight) const {
		return this->totalRef * countWeight + this->priority;
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

struct SurfaceLibValue;

inline void UpdateRefObject(const SurfaceLibValue* pData);
inline bool UpdateShaderUsage(SurfaceLibValue* pData);
inline BOOL GetTransparent(LPSURFACE pSf);

struct SurfaceLibValue {
	LPSURFACE pSf = nullptr;

	// don't change position, refered in GetEstimateMemUsage
	LPSURFACE pSf_VF = nullptr;
	LPSURFACE pSf_HF = nullptr;
	LPSURFACE pSf_VHF = nullptr;

	std::wstring Hash;
	BOOL isTransparent = -1;		// constexpr BOOL transpTBD = -1;

	Count refCount = Count();

	bool bUsedInShader = false;
	std::vector<ShaderRef>* pShaderList = nullptr;

	SurfaceLibValue() = default;
	SurfaceLibValue(const LPSURFACE pSf, const std::wstring& hash, const BOOL bTransp) {
		UpdateSurface(pSf, hash, bTransp);
	}

	inline void UpdateSurface(const LPSURFACE pSf, const std::wstring& hash, const BOOL bTransp) {
		this->pSf = pSf;
		this->Hash = hash;

		this->isTransparent = bTransp;

		UpdateRefObject(this);
	}

	inline void RefImage(LPRDATA rdPtr) {
		refCount.curRef++;
		refCount.pRefObj.emplace_back(rdPtr);

		refCount.totalRef++;
	}
	inline void UnrefImage(LPRDATA rdPtr) {
		refCount.curRef--;

		auto toRemove = std::ranges::remove_if(refCount.pRefObj,
			[&] (const LPRDATA pObject) {
				return rdPtr == pObject;
		});
		refCount.pRefObj.erase(toRemove.begin(), toRemove.end());
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

	inline bool NotUsed() {
		return refCount.curRef == 0 && !UpdateShaderUsage(this);
	}
};

using SurfaceLib = std::map<std::wstring, SurfaceLibValue>;
using SurfaceLibIt = SurfaceLib::iterator;

using RefCount = std::map<std::wstring, Count>;
using RefCountIt = RefCount::iterator;

using RefCountPair = std::pair<std::wstring, Count>;
using RefCountVec = std::vector<RefCountPair>;

using FileList = std::vector<std::wstring>;
using FileListMap = std::map<std::wstring, FileList*>;

template<typename T>
constexpr auto MemRange(T X) { return min(MAX_MEMORYLIMIT, max(0, X)); }

struct GlobalData {
	SurfaceLib* pLib = nullptr;
	KeepList* pKeepList = nullptr;
	FileListMap* pFileListMap = nullptr;

	uint64_t estimateRAMSizeMB = 0;
	uint64_t estimateVRAMSizeMB = 0;

	bool bDX11 = false;
	bool bPreMulAlpha = false;

	const decltype(std::chrono::steady_clock::now()) startTime = std::chrono::steady_clock::now();

#ifdef _USE_DXGI
	D3DUtilities* pD3DU = nullptr;
#ifdef _DYNAMIC_LINK
	HINSTANCE DXGI = nullptr;
#endif
#endif

	GlobalData() {
		//init general
#ifdef _USE_DXGI
#ifdef _DYNAMIC_LINK
		wchar_t rootDir[MAX_PATH] = {};
		GetCurrentDirectory(MAX_PATH - 1, rootDir);

		wchar_t dllPath[2 * MAX_PATH] = {};
		wsprintf(dllPath, L"%s\\%s", rootDir, L"Modules\\DXGI.DLL");

		DXGI = LoadLibrary(dllPath);

		if (DXGI == nullptr) {
			MSGBOX(L"Load Failed");
		}
#endif
		pD3DU = new D3DUtilities;

		//auto& Desc = pD3DU->GetDesc();
		//MSGBOX(Desc.Description);

		//pD3DU->UpdateVideoMemoryInfo();
		//auto& info = pD3DU->GetLocalVideoMemoryInfo();
#endif

		//init specific
		pLib = new SurfaceLib;
		pKeepList = new KeepList;
		pFileListMap = new FileListMap;
	}

	~GlobalData() {
		DeleteLib();

		delete pKeepList;

		for (auto& it : *pFileListMap) {
			delete it.second;
		}
		delete pFileListMap;

#ifdef _USE_DXGI
		delete pD3DU;

#ifdef _DYNAMIC_LINK
		if (DXGI != nullptr) {
			FreeLibrary(pData->DXGI);
			DXGI = nullptr;
		}
#endif

#endif
	}
	
	inline void UpdateLib(const std::wstring& fileName, const std::wstring& hash, const LPSURFACE pSf) const {
		const auto it = pLib->find(fileName);
		if (it != pLib->end()) {
			// refered by shader, move
			if(UpdateShaderUsage(&it->second)) {
				SurfaceLibValue keep = it->second;

				// reset normal ref
				keep.refCount = Count();
				
				const auto newName = std::format(L"{}", std::chrono::steady_clock::now() - startTime);
				(*pLib)[newName] = keep;
								
				// reset shader ref
				it->second.bUsedInShader = false;
				SurfaceLibValue::ReleasePointer(it->second.pShaderList);

				// surface moved to shader item, so reset it to avoid release
				it->second.pSf = nullptr;
				it->second.pSf_VF = nullptr;
				it->second.pSf_HF = nullptr;
				it->second.pSf_VHF = nullptr;
			}

			it->second.Release();
			it->second.UpdateSurface(pSf, hash, GetTransparent(pSf));
		}
		else {
			auto newValue = SurfaceLibValue(pSf, hash, GetTransparent(pSf));
			newValue.refCount.priority = pLib->size();

			(*pLib)[fileName] = newValue;
		}
	}

	inline void DeleteLib() const {
		for (auto& it : *pLib) {
			it.second.Release();
		}

		delete pLib;
	}
};

constexpr auto DEFAULT_FILELISTSIZE = 1000;
constexpr auto Delimiter = L'|';
