#pragma once

#include <mutex>
#include <condition_variable>

#include <chrono>
#include <algorithm>
#include <ranges>

#include "EffectEx.h"
#include "Encryption.h"
#include "WindowsCommon.h"

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

		UpdateRefObject();
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

	inline void UpdateRefObject() const;

	inline void RefShader(const ShaderRef& info) {
		bUsedInShader = true;

		if (pShaderList == nullptr) {
			pShaderList = new std::remove_pointer_t<decltype(pShaderList)>;
		}

		pShaderList->emplace_back(info);
	}
	inline bool UpdateShaderUsage();

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
		return refCount.curRef == 0 && !UpdateShaderUsage();
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

constexpr auto DEFAULT_FILELISTSIZE = 1000;
constexpr auto Delimiter = L'|';

struct GlobalData {
	SurfaceLib* pLib = nullptr;
	FileListMap* pFileListMap = nullptr;

	bool bDX11 = false;
	bool bPreMulAlpha = false;

	const decltype(std::chrono::steady_clock::now()) startTime = std::chrono::steady_clock::now();

	//------------
	// Cache
	//------------

	bool autoClean = false;
	size_t memoryLimit = 0;
	size_t sizeLimit = 0;

	KeepList* pKeepList = nullptr;
	RefCountVec* pCleanVec = nullptr;					// update when trigger clear

	uint64_t estimateRAMSizeMB = 0;
	uint64_t estimateVRAMSizeMB = 0;

#ifdef _USE_DXGI
	D3DUtilities* pD3DU = nullptr;
#ifdef _DYNAMIC_LINK
	HINSTANCE DXGI = nullptr;
#endif
#endif

	//------------
	// Preload
	//------------

	struct PreloadHandler;

	PreloadHandler* pPreloadHandler = nullptr;

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
		pCleanVec = new RefCountVec;
		pFileListMap = new FileListMap;
		pPreloadHandler = new PreloadHandler;
	}

	~GlobalData() {
#ifdef _USE_DXGI
		delete pD3DU;

#ifdef _DYNAMIC_LINK
		if (DXGI != nullptr) {
			FreeLibrary(pData->DXGI);
			DXGI = nullptr;
		}
#endif
#endif

		DeleteLib();

		delete pKeepList;
		delete pCleanVec;

		for (const auto& pFileList : *pFileListMap | std::views::values) {
			delete pFileList;
		}

		delete pFileListMap;
		delete pPreloadHandler;
	}
	
	inline void UpdateLib(const std::wstring& fileName, const std::wstring& hash, const LPSURFACE pSf) const {
		const auto it = pLib->find(fileName);
		if (it != pLib->end()) {
			// refered by shader, move
			if(it->second.UpdateShaderUsage()) {
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
		for (auto& libValue : *pLib | std::views::values) {
			libValue.Release();
		}

		delete pLib;
	}

	inline void UpdateShaderUsage() const {
		for (auto& libValue : *pLib | std::views::values) {
			libValue.UpdateShaderUsage();
		}
	}

	inline bool NeedUpdateLib(const LPCWSTR Item) const {
		const auto fullPath = GetFullPathNameStr(Item);
		const auto it = pLib->find(fullPath);

		if (it != pLib->end()
			&& it->second.Hash != GetFileHash(fullPath)) {
			return true;
		}

		return false;
	}

	inline void EraseLib(const LPCWSTR Item) const {
		const auto fullPath = GetFullPathNameStr(Item);
		const auto it = pLib->find(fullPath);

		if (it != pLib->end() && it->second.NotUsed()) {
			it->second.Release();

			pLib->erase(it);
		}
	}

	inline void ResetLib() const {
		std::erase_if(*pLib, [] (std::pair<const std::wstring, SurfaceLibValue>& item) {
			auto& [fileName, libValue] = item;
			if (libValue.NotUsed()) {
				libValue.Release();
				return true;
			}

			return false;
		});
	}

	//------------
	// Cache
	//------------

	inline void GetEstimateMemUsage();

	inline SIZE_T GetMemoryUsageMB() {
#ifdef _USE_DXGI
		pD3DU->UpdateVideoMemoryInfo();

		return max(GetProcessMemoryUsageMB(), (SIZE_T)pD3DU->GetLocalCurrentUsageMB());
#else
		GetEstimateMemUsage();

		return max(GetProcessMemoryUsageMB()
			, (SIZE_T)max(estimateRAMSizeMB, estimateVRAMSizeMB));
#endif
	}

	inline bool ExceedMemLimit(size_t memLimit) {
#ifdef _USE_DXGI
		auto totalVRAM = pD3DU->GetLocalBudgetMB();

		return (std::min)(totalVRAM, (std::min)(memLimit + CLEAR_MEMRANGE, static_cast<size_t>(MAX_MEMORYLIMIT))) <= GetMemoryUsageMB();
#else
		return (std::min)(memLimit + CLEAR_MEMRANGE, static_cast<size_t>(MAX_MEMORYLIMIT)) <= GetMemoryUsageMB();
#endif		
	}

	inline FileList* GetFileList(const std::wstring& basePath) const {
		FileList* pFileList = nullptr;

		const auto fullPath = GetFullPathNameStr(basePath);
		const bool removeSlash = fullPath.find_last_of(L'\\') == fullPath.size() - 1;
		const auto searchPath = fullPath.substr(0, fullPath.size() - removeSlash);

		const auto fileListMapIt = pFileListMap->find(fullPath);

		if (fileListMapIt == pFileListMap->end()) {
			pFileList = new FileList;
			pFileList->reserve(DEFAULT_FILELISTSIZE);

			::GetFileList(pFileList, searchPath);

			(*pFileListMap)[searchPath] = pFileList;
		}
		else {
			pFileList = fileListMapIt->second;
		}

		return pFileList;
	}

	// search files in base path, if contains, update in list to full path
	inline void GetFullPathByFileName(FileList& outList, const FileList& inList, const std::wstring& basePath) const {
		outList.reserve(inList.size());

		const auto pFileList = GetFileList(basePath);

		for (auto& it : inList) {
			// fix first element miss
			bool initFinish = false;

			for (auto fileIt = pFileList->begin(); fileIt != pFileList->end();) {
				fileIt = std::find_if(fileIt + initFinish, pFileList->end(), [&] (const std::wstring& file) {
					const auto pos = file.find_last_of(L'\\') + 1;
					const std::wstring_view fileName(file.c_str() + pos, file.size() - pos);

					if (fileName == it) {
						return true;
					}

					return false;
					});

				if (fileIt != pFileList->end()) {
					outList.emplace_back(GetFullPathNameStr(*fileIt));
				}

				initFinish = true;
			}
		}
	}

	inline void ClearKeepList() const {
		pKeepList->clear();
	}
	inline void AppendKeepList(const FileList& keepList, const std::wstring& basePath) const {
		const auto pAppendKeepList = new FileList;
		GetFullPathByFileName(*pAppendKeepList, keepList, basePath);

		for (auto& it : *pAppendKeepList) {
			if (std::ranges::find(*pKeepList, it) == pKeepList->end()) {
				pKeepList->emplace_back(it);
			}
		}

		delete pAppendKeepList;
	}

	inline void SetClean(const bool autoClean, const size_t memoryLimit, const size_t sizeLimit) {
		this->autoClean = autoClean;
		this->memoryLimit = memoryLimit;
		this->sizeLimit = sizeLimit;
	}

	inline void UpdateCleanVec() const {
		const auto mapSz = pLib->size();

		pCleanVec->clear();
		pCleanVec->reserve(mapSz);

		for (auto& it : *pLib) {
			if (it.second.NotUsed() // only release assets that currently is not used
				&& std::ranges::find(*pKeepList, it.first) == pKeepList->end()) {
				pCleanVec->emplace_back(RefCountPair(it.first, it.second.refCount));
			}
		}

		const auto countWeight = mapSz;		// weight of ref count
		std::ranges::sort(*pCleanVec, [&] (const RefCountPair& l, const RefCountPair& r) {
			return l.second.GetWeight(countWeight) > r.second.GetWeight(countWeight);	// decending
			});
	}

	inline void CleanCache(bool forceClean = false, size_t memLimit = -1) {
		if (pPreloadHandler->IsPreloading()) {
			return;
		}

		forceClean |= SystemMemoryNotEnough();

		if (forceClean
			|| (autoClean
			&& pLib->size() > CLEAR_NUMTHRESHOLD
			&& ExceedMemLimit(memoryLimit))) {
			const auto tarMemLimit = forceClean && (memLimit != static_cast<size_t>(-1))
				? memLimit
				: memoryLimit / 2;

			UpdateCleanVec();

			while (!pCleanVec->empty()
				&& tarMemLimit <= GetMemoryUsageMB()) {
				auto& fileName = pCleanVec->back().first;

				(*pLib)[fileName].Release();
				pLib->erase(fileName);

				pCleanVec->pop_back();
			}
		}
	}

	//------------
	// Preload
	//------------

	struct PreloadHandler {
		PreLoadList* pPreloadList = nullptr;
		SurfaceLib* pPreloadLib = nullptr;

		LPRDATA rdPtr = nullptr;
		std::atomic<HANDLE> threadID = nullptr;

		std::atomic<bool> bForceExit = false;
		std::atomic<bool> bPreloading = false;
		std::atomic<bool> bPreloadMerge = false;

		std::mutex mtx;
		std::condition_variable cv;
		std::atomic<bool> bPaused = false;

		PreloadHandler() {
			pPreloadList = new PreLoadList;
			pPreloadLib = new SurfaceLib;
		}
		~PreloadHandler() {
			StopPreload();

			delete pPreloadList;
			delete pPreloadLib;
		}
		
		//------------
		// Control
		//------------

		inline void UpdateRdPtr(LPRDATA rdPtr) {
			std::unique_lock<std::mutex> lock(mtx);
			this->rdPtr = rdPtr;
		}

		inline void StartPreload(LPRDATA rdPtr, HANDLE handle) {
			this->rdPtr = rdPtr;
			this->threadID = handle;

			this->bForceExit = false;
			this->bPreloading = true;
			this->bPreloadMerge = false;

			this->bPaused = false;

			this->pPreloadLib->clear();
		}

		inline void FinishPreload() {
			this->rdPtr = nullptr;
			this->threadID = nullptr;

			this->bForceExit = false;
			this->bPreloading = false;
			this->bPreloadMerge = true;

			this->bPaused = false;
		}

		inline void PausePreload() {
			if (!PreloadValid()) {
				return;
			}

			UpdateRdPtr(nullptr);

			while (true) {
				if (this->bPaused) {
					break;
				}
			}
		}

		inline void ResumePreload(LPRDATA rdPtr) {
			if (PreloadValid()) {
				return;
			}

			UpdateRdPtr(rdPtr);
			cv.notify_all();
		}
		
		inline void StopPreload() {
			if (IsPreloading()) {
				this->bForceExit = true;
				ResumePreload(nullptr);

				DWORD ret;
				while (GetExitCodeThread(this->threadID, &ret)) {
					if (ret == 0) {
						break;
					}
				}
			}
		}
		
		//------------
		// State
		//------------

		inline bool PreloadValid() const {
			return IsPreloading() && this->rdPtr != nullptr;
		}

		inline bool IsPreloading() const {
			return this->bPreloading;
		}

		inline bool AbleToMerge() const {
			return !IsPreloading() && this->bPreloadMerge;
		}
	};	

	inline void MergeLib(LPRDATA rdPtr) const;

	inline void PreloadLib(PreloadHandler* pPreloadHandler, const std::wstring& Key,
		const std::function<void()>& callback);

	inline void StartPreloadProcess(LPRDATA rdPtr, FileList* pList,
		bool fullPath, const std::wstring& BasePath,
		const std::wstring& Key) {
		if(pPreloadHandler->IsPreloading()) {
			return;
		}

		// filter duplicate items
		auto roRemove = std::ranges::unique(*pList);
		pList->erase(roRemove.begin(), roRemove.end());

		// Get real path
		FileList* list;
		FileList fullPathList;

		if (!fullPath) {
			GetFullPathByFileName(fullPathList, *pList, BasePath);
			list = &fullPathList;
		}
		else {
			list = pList;
		}

		// filter exist items
		pPreloadHandler->pPreloadList->clear();
		pPreloadHandler->pPreloadList->reserve(pList->size());

		for (auto& it : *list) {
			std::wstring fullPathStr = GetFullPathNameStr(it);

			if (std::ranges::find_if(*pLib, [fullPathStr] (auto& p) {
				return p.first == fullPathStr;
				}) == pLib->end()) {
				pPreloadHandler->pPreloadList->emplace_back(fullPathStr);
			}
		}
		
		std::thread pl(&GlobalData::PreloadLib, this,
			pPreloadHandler, Key,
			[&] () {
				pPreloadHandler->FinishPreload();
		});

		HANDLE handle;
		DuplicateHandle(GetCurrentProcess(), pl.native_handle(), GetCurrentProcess(), &handle, THREAD_QUERY_INFORMATION | THREAD_TERMINATE, NULL, NULL);

		pPreloadHandler->StartPreload(rdPtr, handle);

		pl.detach();
	}

};

