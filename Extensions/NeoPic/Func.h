#pragma once

#include <algorithm>
#include <functional>
#include <string_view>

//-----------------------------

inline size_t UpdateRef(LPRDATA rdPtr, bool add);
inline void ReDisplay(LPRDATA rdPtr);
inline void UpdateHotSpot(LPRDATA rdPtr, HotSpotPos Type, int X = 0, int Y = 0);
inline void GetFileName(LPRDATA rdPtr);
inline bool ExceedDefaultMemLimit(LPRDATA rdPtr, size_t memLimit);

//-----------------------------

inline bool UpdateShaderUsage(SurfaceLibValue* pData) {
	do {
		if (!pData->bUsedInShader) {
			break;
		}

		const auto removeIt = std::ranges::remove_if(*pData->pShaderList,
			[&] (const ShaderRef& info) {
				if (IsDestroyed(info.pObject)) {
					return true;
				}

				const auto rdPtr = reinterpret_cast<LPRDATA>(info.pObject);

				const auto pEffect = EffectUtilities::GetEffect(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam);

				if (pEffect == nullptr) {
					return true;
				}

				if (pEffect != info.pEffect) {
					return true;
				}

				if (pEffect->m_pFxBuf != info.m_pFxBuf) {
					return true;
				}

				const auto paramIndex = EffectUtilities::GetParamIndex(pEffect, info.paramName);

				if (paramIndex == -1) {
					return true;
				}

				const auto paramType = pEffect->GetParamType(paramIndex);

				if (paramType != EFFECTPARAM_SURFACE) {
					return true;
				}

				const auto pSfImpl = pEffect->GetParamSurfaceValue(paramIndex);

				if (pSfImpl != GetSurfaceImplementation(*pData->pSf)) {
					return true;
				}

				return false;
		});

		pData->pShaderList->erase(removeIt.begin(), removeIt.end());

		pData->bUsedInShader = !pData->pShaderList->empty();
	} while (0);

	return pData->bUsedInShader;
}
inline void UpdateShaderUsage(SurfaceLib* pLib) {
	for (auto& it : *pLib) {
		UpdateShaderUsage(&it.second);
	}
}

inline void NewNonFromLib(LPRDATA rdPtr, LPSURFACE pSrc) {
	rdPtr->src = pSrc;
	rdPtr->pSf_Nor = pSrc;
}
inline void ResetNonFromLib(LPRDATA rdPtr) {
	rdPtr->src = nullptr;

	rdPtr->pSf_Nor = nullptr;
	rdPtr->pSf_HF = nullptr;
	rdPtr->pSf_VF = nullptr;
	rdPtr->pSf_VHF = nullptr;
}
inline void ReleaseNonFromLib(LPRDATA rdPtr) {
	// rdPtr->src must point to the following ones
	delete rdPtr->pSf_Nor;
	delete rdPtr->pSf_HF;
	delete rdPtr->pSf_VF;
	delete rdPtr->pSf_VHF;

	ResetNonFromLib(rdPtr);
}

inline void DetachFromLib(LPRDATA rdPtr) {
	if (rdPtr->fromLib) {
		rdPtr->fromLib = false;
		rdPtr->src = nullptr;

		UpdateRef(rdPtr, false);
		rdPtr->pRefCount = nullptr;

		rdPtr->pLibValue = nullptr;
	}
	else {
		ReleaseNonFromLib(rdPtr);
	}
}
inline void DetachFromLib(LPRDATA rdPtr, LPSURFACE pNewSf) {
	DetachFromLib(rdPtr);

	rdPtr->src = pNewSf;

	if (rdPtr->HWA) {
		ConvertToHWATexture(rdPtr, rdPtr->src);
	}
	
	NewNonFromLib(rdPtr, rdPtr->src);
}
inline void AttachToLib(LPRDATA rdPtr, SurfaceLibValue* pLibValue, size_t* pRefCount) {
	if (!rdPtr->fromLib) {
		ReleaseNonFromLib(rdPtr);
	}

	// Update ref
	UpdateRef(rdPtr, false);

	rdPtr->fromLib = true;

	rdPtr->src = pLibValue->pSf;
	rdPtr->pLibValue = pLibValue;
	rdPtr->pRefCount = pRefCount;

	UpdateRef(rdPtr, true);
}

// Create new surface according to HWA
inline auto CreateNewSurface(LPRDATA rdPtr, bool HWA) {
	return HWA
		? CreateHWASurface(rdPtr, 32, 4, 4, ST_HWA_ROMTEXTURE)
		: CreateSurface(32, 4, 4);
}

inline void UpdateHoImgInfo(LPRDATA rdPtr) {
	UpdateHoImgInfo(rdPtr, rdPtr->src
		, rdPtr->zoomScale.XScale, rdPtr->zoomScale.YScale
		, rdPtr->hotSpotPos
		, rdPtr->hotSpot.x, rdPtr->hotSpot.y
		, rdPtr->angle);
}

inline bool CanDisplay(LPRDATA rdPtr) {
	return !rdPtr->isLib && rdPtr->src != nullptr && rdPtr->src->IsValid();
}

inline void ReDisplay(LPRDATA rdPtr) {
	if (!rdPtr->isLib) {
		rdPtr->rc.rcChanged = true;

		UpdateHoImgInfo(rdPtr);
		FreeColMask(rdPtr->pColMask);
	}
}

//Set default values
//NewPic covers ReDisplay
inline void NewPic(LPRDATA rdPtr, LPRDATA Copy = nullptr) {
	if (Copy == nullptr) {
		rdPtr->angle = 0;

		rdPtr->hotSpot = { 0,0 };
		rdPtr->zoomScale = { 1.0,1.0 };

	}else {
		rdPtr->angle = Copy->angle;

		rdPtr->hotSpotPos = Copy->hotSpotPos;
		rdPtr->hotSpot = Copy->hotSpot;
		rdPtr->zoomScale = Copy->zoomScale;
	}

	UpdateHotSpot(rdPtr, rdPtr->hotSpotPos);

	ReDisplay(rdPtr);
}

// Update real hot sopt
inline void UpdateHotSpot(LPRDATA rdPtr, int X, int Y) {
	rdPtr->hotSpot.x = X;
	rdPtr->hotSpot.y = Y;
}
inline void UpdateHotSpot(LPRDATA rdPtr, HotSpotPos Type, int X, int Y) {	
	rdPtr->hotSpotPos = Type;

	if (rdPtr->src == nullptr || !rdPtr->src->IsValid()) {
		if (Type == HotSpotPos::CUSTOM) {
			UpdateHotSpot(rdPtr, X, Y);
		}

		return;
	}

	//auto width = rdPtr->src->GetWidth();
	//auto height = rdPtr->src->GetHeight();

	auto width = int(rdPtr->src->GetWidth() * rdPtr->zoomScale.XScale);
	auto height = int(rdPtr->src->GetHeight() * rdPtr->zoomScale.YScale);

	UpdateHotSpot(Type, width, height, X, Y);
	UpdateHotSpot(rdPtr, X, Y);

	ReDisplay(rdPtr);
}

inline void Zoom(LPRDATA rdPtr, float XScale, float YScale, bool UpdateCur = false) {
	if (rdPtr->zoomScale == ZoomScale{ XScale, YScale }) {
		return;
	}

	rdPtr->zoomScale = { XScale ,YScale };

	ReDisplay(rdPtr);
}
inline void Rotate(LPRDATA rdPtr, int Angle, bool UpdateCur = false) {
	if (rdPtr->angle == Angle) {
		return;
	}

	rdPtr->angle = Angle;

	ReDisplay(rdPtr);
}
inline auto Offset(LPSURFACE Src, LPSURFACE Des, const OffsetCoef& O) {	
	return Offset(Src, Des, O.XOffset, O.YOffset, O.Wrap);
}

// hash
inline std::wstring GetFileHash(LPBYTE pData, DWORD StrLength) {
	Encryption Hash;
	Hash.SetEncryptStr((char*)pData, StrLength);

	auto ret = Hash.GetHash();

	return std::wstring(ret == nullptr ? L"" : ret);
}
inline std::wstring GetFileHash(LPCWSTR filePath) {
	// protection for null file
	if (wcscmp(filePath, Empty_Str) == 0) {
		return Empty_Str;
	}

	Encryption Hash;
	Hash.OpenFile(filePath);

	auto ret = Hash.GetHash();

	return std::wstring(ret == nullptr ? L"" : ret);
}
inline std::wstring GetFileHash(const std::wstring& filePath) {
	return GetFileHash(filePath.c_str());
}

// Load file then convert to Src type
inline bool _LoadFromFile(LPSURFACE& Src, LPCWSTR FilePath, LPCWSTR Key, LPRDATA rdPtr, int width, int height, bool NoStretch, bool HighQuality) {
	// HWA?
	auto srcHWA = IsHWA(Src);
	bool ret = true;

	// Load by bitmap, as HWA texture cannot add alpha channel
	LPSURFACE pBitmap = srcHWA ? CreateNewSurface(rdPtr, false) : Src;
	
	if (StrEmpty(Key)) {
		ret = _LoadFromFile(pBitmap, FilePath, rdPtr, width, height, NoStretch, HighQuality);
	}
	else {
		Encryption E;
		E.GenerateKey(Key);

		E.OpenFile(FilePath);
		E.Decrypt();

		ret = _LoadFromMemFile(pBitmap, E.GetOutputData(), E.GetOutputDataLength(), rdPtr, width, height, NoStretch, HighQuality);
	}

	// Add alpha to avoid Fusion set (0,0,0) to transparent
	_AddAlpha(pBitmap);

	if (srcHWA) {
		delete Src;
		Src = pBitmap;
		
		ConvertToHWATexture(rdPtr, Src);
	}

	return ret;
}

inline size_t UpdateRef(LPRDATA rdPtr, bool add) {
	if (rdPtr->pRefCount != nullptr) {
		return add ? (*rdPtr->pRefCount)++ : (*rdPtr->pRefCount)--;
	}
	return (size_t)-1;
}

inline void LoadFromFile(LPRDATA rdPtr, LPCWSTR FileName, LPCWSTR Key = L"") {
	const auto fullPath = GetFullPathNameStr(FileName);

	auto loadCallback = [&] () {
		if (rdPtr->bLoadCallback) {
			*rdPtr->pCallbackFileName = fullPath;
			CallEvent(ONLOADCALLBACK)
		}
	};
	auto LoadCore = [&] (std::function<void(LPSURFACE pSf)> success) {
		auto pSf = CreateNewSurface(rdPtr, rdPtr->HWA);
		_LoadFromFile(pSf,
			fullPath.c_str(), Key,
			rdPtr, -1, -1,
			true, rdPtr->stretchQuality);

		if (pSf->IsValid()) {
			success(pSf);
			loadCallback();
		}
		else {
			delete pSf;
		}
	};

	if (rdPtr->isLib) {
		if (rdPtr->pLib->find(fullPath) != rdPtr->pLib->end()) {
			return;
		}

		LoadCore([&] (LPSURFACE pSf) {
			(*rdPtr->pLib)[fullPath] = SurfaceLibValue(pSf, GetFileHash(fullPath), GetTransparent(pSf));
		});
	}
	else {
		LoadCore([&] (LPSURFACE pSf) {
			DetachFromLib(rdPtr, pSf);
			NewPic(rdPtr);

			*rdPtr->FilePath = FileName;
			*rdPtr->Key = Key;

			GetFileName(rdPtr);
	});
	}
}

inline bool ObjIsLib(LPRDATA obj) {
	if (!obj || obj->rHo.hoIdentifier != IDENTIFIER) {
		return false;
	}

	if (!obj->isLib) {
		return false;
	}

	return true;
}

// lib load core
inline SurfaceLibIt _LoadLib(LPRDATA rdPtr, LPRDATA obj, LPCWSTR FileName, LPCWSTR Key = L"") {
	if (!ObjIsLib(obj)) {
		return obj->pLib->end();
	}

	auto fullPath = GetFullPathNameStr(FileName);

	auto it = obj->pLib->find(fullPath);

	if (it == obj->pLib->end()) {
		LoadFromFile(obj, FileName, Key);
	}

	it = obj->pLib->find(fullPath);
	if (it == obj->pLib->end()) {
		return obj->pLib->end();
	}

	// convert to HWA if needed
	if (obj->HWA && !IsHWA(it->second.pSf)) {
		ConvertToHWATexture(rdPtr, it->second.pSf);
	}

	return it;
}

inline void LoadFromLib(LPRDATA rdPtr, LPRO object, LPCWSTR FileName, LPCWSTR Key = L"") {
	LPRDATA obj = (LPRDATA)object;

	if (!ObjIsLib(obj)) {
		return;
	}

	auto fullPath = GetFullPathNameStr(FileName);

	// load
	auto it = _LoadLib(rdPtr, obj, FileName, Key);
	if (it == obj->pLib->end()) {
		return;
	}

	// update ref count
	auto countit = obj->pCount->find(fullPath);
	if (countit != obj->pCount->end()) {
		countit->second.count++;
	}
	else {
		(*obj->pCount)[fullPath] = Count{ 1, obj->pLib->size(),0 };
		countit = obj->pCount->find(fullPath);
	}

	// update src
	if(!rdPtr->isLib){		
		AttachToLib(rdPtr, &(it->second), &(countit->second.curRef));
		NewPic(rdPtr);
	}
	else {
		auto thisit = rdPtr->pLib->find(fullPath);
		if (thisit == rdPtr->pLib->end()) {
			(*rdPtr->pLib)[it->first]=it->second;
		}
	}

	*rdPtr->FilePath = FileName;
	*rdPtr->Key = Key;

	GetFileName(rdPtr);
}

inline void LoadFromLib(LPRDATA rdPtr, int Fixed, LPCWSTR FileName, LPCWSTR Key = L"") {
	return LoadFromLib(rdPtr, LproFromFixed(rdPtr, Fixed), FileName, Key);
}

inline void LoadFromDisplay(LPRDATA rdPtr, LPRO object, bool CopyCoef = false) {
	LPRDATA obj = (LPRDATA)object;

	if (!obj || obj->rHo.hoIdentifier != IDENTIFIER) {
		return;
	}

	if (obj->isLib) {
		return;
	}

	if (!obj->src->IsValid()) {
		return;
	}

	if (obj->fromLib) {
		AttachToLib(rdPtr, obj->pLibValue, obj->pRefCount);		
	}
	else {
		const auto pSf = CreateNewSurface(rdPtr, rdPtr->HWA);
		pSf->Clone(*obj->src);

		DetachFromLib(rdPtr, pSf);
	}

	*rdPtr->FileName = *obj->FileName;
	*rdPtr->FilePath = *obj->FilePath;
	*rdPtr->Key = *obj->Key;

	NewPic(rdPtr, CopyCoef ? obj : nullptr);
}

inline void LoadFromDisplay(LPRDATA rdPtr, int Fixed, bool CopyCoef = false) {
	return LoadFromDisplay(rdPtr, LproFromFixed(rdPtr, Fixed), CopyCoef);
}

constexpr auto LoadFromPointerFlags_NoFullPath = 0b0000'0000'0000'0000'0000'0000'0000'0001;

inline void LoadFromPointer(LPRDATA rdPtr, LPCWSTR pFileName, LPSURFACE pSf
	, DWORD dwFlags = 0) {
	if (pSf == nullptr) {
		return;
	}

	LPSURFACE pSave = new cSurface;

	ProcessBitmap(rdPtr, pSf, [&] (const LPSURFACE pBitmap) {
		pSave->Clone(*pBitmap);

		if (rdPtr->HWA) {
			ConvertToHWATexture(rdPtr, pSave);
		}
		});

	if(!pSave->IsValid()) {
		delete pSave;

		return ;
	}

	auto bNoFullPath = dwFlags & LoadFromPointerFlags_NoFullPath;
	auto fullPath = bNoFullPath
		? std::wstring(pFileName)
		: GetFullPathNameStr(pFileName);

	if (rdPtr->isLib) {
		const auto it = rdPtr->pLib->find(fullPath);
		if (it != rdPtr->pLib->end()) {
			it->second.Release();
		}

		(*rdPtr->pLib)[fullPath] = SurfaceLibValue(pSave, fullPath, GetTransparent(pSave));
	}
	else {
		DetachFromLib(rdPtr, pSave);
		NewPic(rdPtr);

		*rdPtr->FilePath = fullPath + L"\\" + L"LoadFromPointer";
		*rdPtr->Key = L"";

		GetFileName(rdPtr);
	}
}

inline LPSURFACE _GetSurfacePointer(LPRDATA rdPtr, const std::wstring& FilePath, const std::wstring& Key) {
	cSurface* ret = nullptr;

	if (!rdPtr->isLib) {
		ret = rdPtr->src;
	}
	else {
		const auto it = _LoadLib(rdPtr, rdPtr, FilePath.c_str(), Key.c_str());

		if (it == rdPtr->pLib->end()) {
			ret = nullptr;
		}
		else {
			ret = it->second.pSf;
		}
	}

	return ret;
}

inline void ResetLib(LPRDATA rdPtr, SurfaceLib*& pData) {
	SurfaceLib* kept = new SurfaceLib;

	if (pData != NULL) {
		for (auto& it : *pData) {
			auto itCount = rdPtr->pCount->find(it.first);

			if (itCount->second.curRef != 0 || UpdateShaderUsage(&it.second)) {
				kept->emplace(it);

				continue;
			}

			it.second.Release();
		}

		//pData->clear();

		delete pData;
		pData = kept;
	}
}

inline void EraseLib(SurfaceLib* pData, LPCWSTR Item) {
	const auto it = pData->find(GetFullPathNameStr(Item));
	if (it != pData->end() && !UpdateShaderUsage(&it->second)) {
		it->second.Release();

		pData->erase(it);
	}
}

inline void DeleteLib(SurfaceLib* pData) {
	if (pData != NULL) {
		for (auto& it : *pData) {
			it.second.Release();
		}

		delete pData;
	}
}

inline bool NeedUpdateLib(SurfaceLib* pData, LPCWSTR Item) {
	auto fullPath = GetFullPathNameStr(Item);
	auto it = pData->find(fullPath);
	if (it != pData->end()
		&& it->second.Hash != GetFileHash(fullPath)) {
		return true;
	}

	return false;
}

// filter duplicate items
inline void GetSingleList(FileList* pList) {
	FileList singleList;
	singleList.reserve(pList->size());

	for (auto& it : *pList) {
		if (std::find(singleList.begin(), singleList.end(), it) == singleList.end()) {
			singleList.emplace_back(it);
		}
	}
	
	*pList = std::move(singleList);
}

inline auto GetFileList(LPRDATA rdPtr, const std::wstring& basePath) {
	FileList* pFileList = nullptr;

	auto fullPath = GetFullPathNameStr(basePath);
	bool removeSlash = fullPath.find_last_of(L'\\') == fullPath.size() - 1;
	auto searchPath = fullPath.substr(0, fullPath.size() - removeSlash);
	
	auto fileListMapIt = rdPtr->pFileListMap->find(fullPath);

	if (fileListMapIt == rdPtr->pFileListMap->end()) {
		pFileList = new FileList;
		pFileList->reserve(DEFAULT_FILELISTSIZE);

		GetFileList(pFileList, searchPath);

		(*rdPtr->pFileListMap)[searchPath] = pFileList;
	}
	else {
		pFileList = fileListMapIt->second;
	}

	return pFileList;
}

inline void GetFullPathFromName(LPRDATA rdPtr, FileList& outList, const FileList& inList, const std::wstring& basePath) {
	outList.reserve(inList.size());
	
	FileList* pFileList = GetFileList(rdPtr, basePath);

	for (auto& it : inList) {
		// fix first element miss
		bool initFinish = false;

		for (auto fileIt = pFileList->begin(); fileIt != pFileList->end();) {
			fileIt = std::find_if(fileIt + initFinish, pFileList->end(), [&](std::wstring& file) {
				auto pos = file.find_last_of(L"\\") + 1;
				std::wstring_view fileName(file.c_str() + pos, file.size() - pos);
				
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

	return;
}

// do not ref PreloadList as this function is for multithread
using LoadLibCallBack = std::function<void(SurfaceLib*)>;

inline int PreloadLibFromVec(volatile LPRDATA rdPtr, FileList PreloadList, const std::wstring& BasePath, const std::wstring& Key, const LoadLibCallBack& callBack) {
	if (PreloadList.empty()) {
		callBack(nullptr);
		return 0;
	}

	SurfaceLib* tempLib = new SurfaceLib;

	for (auto& it : PreloadList) {
		// Stop loading when exceed limit		
		if (ExceedDefaultMemLimit(rdPtr, rdPtr->memoryLimit)) {
			break;
		}

		// can only load bitmap in sub thread, cannot load HWA here
		LPSURFACE pBitmap = CreateNewSurface(rdPtr, false);
		_LoadFromFile(pBitmap, it.c_str(), Key.c_str(), rdPtr, -1, -1, true, rdPtr->stretchQuality);

		if (pBitmap->IsValid()) {
			(*tempLib)[it] = SurfaceLibValue(pBitmap, GetFileHash(it), GetTransparent(pBitmap));
		}
		else {
			delete pBitmap;
		}

		if (rdPtr->forceExit) {
			for (auto& it : *tempLib) {
				it.second.Release();
			}

			delete tempLib;
			tempLib = nullptr;

			break;
		}
	}

	if (tempLib != nullptr
		&& !tempLib->empty()) {
		callBack(tempLib);
	}

	return 0;
}

inline void CreatePreloadProcess(LPRDATA rdPtr, FileList* pList, bool fullPath, const std::wstring& BasePath, const std::wstring& Key) {
	// filter duplicate items
	GetSingleList(pList);

	// Get real path
	FileList* list;
	FileList fullPathList;

	if (!fullPath) {
		GetFullPathFromName(rdPtr, fullPathList, *pList, BasePath);
		list = &fullPathList;
	}
	else {
		list = pList;
	}
	
	// filter exist items
	delete rdPtr->pPreloadList;
	rdPtr->pPreloadList = new PreLoadList;
	rdPtr->pPreloadList->reserve(pList->size());

	std::wstring fullPathStr;
	for (auto& it : *list) {
		fullPathStr = GetFullPathNameStr(it);
		
		if (std::find_if(rdPtr->pLib->begin(), rdPtr->pLib->end(), [fullPathStr](auto& p) {
				return p.first == fullPathStr;
				}) == rdPtr->pLib->end()) {
			rdPtr->pPreloadList->emplace_back(fullPathStr);
		}
	}

	rdPtr->preloading = true;
	std::thread pl(PreloadLibFromVec, rdPtr, *rdPtr->pPreloadList, BasePath, Key
		, [rdPtr](SurfaceLib* pLib) {
			rdPtr->forceExit = false;
			rdPtr->threadID = 0;
			
			rdPtr->preloading = false;
			rdPtr->preloadMerge = true;
			rdPtr->preloadLib = pLib;
		});

	HANDLE handle;
	DuplicateHandle(GetCurrentProcess(), pl.native_handle(), GetCurrentProcess(), &handle, THREAD_QUERY_INFORMATION | THREAD_TERMINATE, NULL, NULL);

	rdPtr->threadID = handle;
	pl.detach();
}

inline void MergeLib(LPRDATA rdPtr) {
	if (!rdPtr->preloading
		&& rdPtr->preloadMerge
		&& rdPtr->preloadLib != nullptr) {
		for (auto& it : *rdPtr->preloadLib) {
			auto& [name, key] = it;
			if (rdPtr->pLib->find(name) == rdPtr->pLib->end()) {
				(*rdPtr->pLib)[name] = key;
			}
		}

		delete rdPtr->preloadLib;
		rdPtr->preloadLib = nullptr;
		
		rdPtr->preloading = false;
		rdPtr->preloadMerge = false;

		CallEvent(ONPRELOADCOMPLETE);
	}
}

inline void GetKeepList(LPRDATA rdPtr, const FileList& keepList, const std::wstring& basePath) {
	//rdPtr->pKeepList->clear();
	//GetFullPathFromName(rdPtr, *rdPtr->pKeepList, keepList, basePath);

	auto pAppendKeepList = new FileList;	
	GetFullPathFromName(rdPtr, *pAppendKeepList, keepList, basePath);

	for (auto& it : *pAppendKeepList) {
		if (std::find(rdPtr->pKeepList->begin(), rdPtr->pKeepList->end(), it) == rdPtr->pKeepList->end()) {
			rdPtr->pKeepList->emplace_back(it);
		}
	}

	delete pAppendKeepList;
}

inline void UpdateCleanVec(LPRDATA rdPtr) {
	auto mapSz = rdPtr->pLib->size();

	rdPtr->pCountVec->clear();
	rdPtr->pCountVec->reserve(mapSz);

	for (auto& it : *rdPtr->pLib) {
		auto pCountIt = rdPtr->pCount->find(it.first);
		auto pCountContain = pCountIt != rdPtr->pCount->end();

		Count count = pCountContain
			? pCountIt->second
			: Count{ 0,0,0 };			// lowest weight

		if ((!pCountContain
			|| pCountIt->second.curRef == 0
			|| !UpdateShaderUsage(&it.second)) // only release assets that currently is not used
			&& std::find(rdPtr->pKeepList->begin(), rdPtr->pKeepList->end(), it.first) == rdPtr->pKeepList->end()) {
			rdPtr->pCountVec->emplace_back(RefCountPair{ it.first,count });
		}
	}

	auto countWeight = mapSz;		// weight of ref count
	std::sort(rdPtr->pCountVec->begin(), rdPtr->pCountVec->end(), [&] (RefCountPair& l, RefCountPair& r) {
		return l.second.GetWeight(countWeight) > r.second.GetWeight(countWeight);	// decending
		});
}

inline void UpdateRefCountVec(LPRDATA rdPtr) {
	auto mapSz = rdPtr->pCount->size();

	rdPtr->pCountVec->clear();
	rdPtr->pCountVec->reserve(mapSz);

	for (auto& it : *rdPtr->pCount) {
		if (it.second.curRef == 0
			&& std::find(rdPtr->pKeepList->begin(), rdPtr->pKeepList->end(), it.first) == rdPtr->pKeepList->end()) {
			rdPtr->pCountVec->emplace_back(it);
		}
	}

	auto countWeight = mapSz;		// weight of ref count
	std::sort(rdPtr->pCountVec->begin(), rdPtr->pCountVec->end(), [&](RefCountPair& l, RefCountPair& r) {
		return l.second.GetWeight(countWeight) > r.second.GetWeight(countWeight);	// decending
		});
}

inline void ClearCurRef(LPRDATA rdPtr) {
	for (auto& it : *rdPtr->pCount) {
		it.second.curRef = 0;
	}
}

inline void GetEstimateMemUsage(GlobalData* pData) {
	// optimize it by add/sub value when modifing lib instead of iterate if has performance issue (unlikely)
	pData->estimateRAMSizeMB = 0;
	pData->estimateVRAMSizeMB = 0;

	auto pLib = pData->pLib;

	for (auto& item : *pLib) {
		LPSURFACE* pSfArrary = &item.second.pSf;

		for (int i = 0; i < SurfaceLibSfNum; i++) {
			auto pSf = *(pSfArrary + i);

			if (pSf == nullptr) {
				continue;
			}

			bool bHWA = IsHWA(pSf);

			auto estimateSizeMB = GetEstimateSizeMB(pSf);

			if (!bHWA) {
				pData->estimateRAMSizeMB += estimateSizeMB;
			}
			else {
				pData->estimateVRAMSizeMB += estimateSizeMB;
			}
		}
	}
}

inline SIZE_T GetMemoryUsageMB(LPRDATA rdPtr) {
#ifdef _USE_DXGI
	rdPtr->pD3DU->UpdateVideoMemoryInfo();

	return max(GetProcessMemoryUsageMB(), (SIZE_T)rdPtr->pD3DU->GetLocalCurrentUsageMB());
#else
	GetEstimateMemUsage(rdPtr->pData);

	return max(GetProcessMemoryUsageMB()
		, (SIZE_T)max(rdPtr->pData->estimateRAMSizeMB, rdPtr->pData->estimateVRAMSizeMB));
#endif
}

inline bool ExceedDefaultMemLimit(LPRDATA rdPtr, size_t memLimit) {
#ifdef _USE_DXGI
	auto totalVRAM = rdPtr->pD3DU->GetLocalBudgetMB();

	return min(totalVRAM, min(memLimit + CLEAR_MEMRANGE, MAX_MEMORYLIMIT)) <= GetMemoryUsageMB(rdPtr);
#else
	return min(memLimit + CLEAR_MEMRANGE, MAX_MEMORYLIMIT) <= GetMemoryUsageMB(rdPtr);
#endif
}

inline void CleanCache(LPRDATA rdPtr, bool forceClean = false, size_t memLimit = -1) {
	if (!rdPtr->isLib) {
		return;
	}

	UpdateShaderUsage(rdPtr->pLib);

	if (rdPtr->preloading) {
		return;
	}

	forceClean |= SystemMemoryNotEnough();

	if (forceClean
		|| (rdPtr->autoClean
		&& rdPtr->pLib->size() > CLEAR_NUMTHRESHOLD
		&& ExceedDefaultMemLimit(rdPtr, rdPtr->memoryLimit))) {
		const auto tarMemLimit = forceClean && (memLimit != static_cast<size_t>(-1))
			? memLimit
			: rdPtr->memoryLimit / 2;

		UpdateCleanVec(rdPtr);

		while (!rdPtr->pCountVec->empty()
			&& tarMemLimit <= GetMemoryUsageMB(rdPtr)) {

			auto& fileName = rdPtr->pCountVec->back().first;

			(*rdPtr->pLib)[fileName].Release();

			rdPtr->pLib->erase(fileName);
			rdPtr->pCount->erase(fileName);

			rdPtr->pCountVec->pop_back();
		}
	}
}

// Get information
inline long GetHotSpotX(LPRDATA rdPtr) {
	return CanDisplay(rdPtr) ? rdPtr->hotSpot.x : -1;
}

inline long GetHotSpotY(LPRDATA rdPtr) {
	return CanDisplay(rdPtr) ? rdPtr->hotSpot.y : -1;
}

inline int GetOriginalWidth(LPRDATA rdPtr) {
	return CanDisplay(rdPtr) ? rdPtr->src->GetWidth() : -1;
}

inline int GetOriginalHeight(LPRDATA rdPtr) {
	return CanDisplay(rdPtr) ? rdPtr->src->GetHeight() : -1;
}

inline int GetCurrentWidth(LPRDATA rdPtr) {
	return CanDisplay(rdPtr) ? (int)(rdPtr->src->GetWidth() * abs(rdPtr->zoomScale.XScale)) : -1;
}

inline int GetCurrentHeight(LPRDATA rdPtr) {
	return CanDisplay(rdPtr) ? (int)(rdPtr->src->GetHeight() * abs(rdPtr->zoomScale.YScale)) : -1;
}

inline float GetXZoomScale(LPRDATA rdPtr) {
	return CanDisplay(rdPtr) ? rdPtr->zoomScale.XScale : -1;
}

inline float GetYZoomScale(LPRDATA rdPtr) {
	return CanDisplay(rdPtr) ? rdPtr->zoomScale.YScale : -1;
}

inline int GetAngle(LPRDATA rdPtr) {
	return CanDisplay(rdPtr) ? rdPtr->angle : -1;
}

inline std::wstring GetFileName(std::wstring& FilePath) {
	auto pos = FilePath.find_last_of(L"\\") + 1;

	try {
		return FilePath.substr(pos, FilePath.size() - pos);
	}
	catch (...) {
		//return L"Invalid";
		return FilePath;
	}
}

inline void GetFileName(LPRDATA rdPtr) {
	*rdPtr->FileName = GetFileName(*rdPtr->FilePath);
}

inline std::wstring GetRelativeFilePath(std::wstring& FilePath, std::wstring& BasePath) {
	auto pos = BasePath.size() + 1;

	try {
		return FilePath.substr(pos, FilePath.size() - pos);
	}
	catch (...) {
		//return L"Invalid";
		return FilePath;
	}
}

// for display
inline auto GetSurface(LPRDATA rdPtr, int width, int height) {
	if (rdPtr->HWA) {
		return CreateHWASurface(rdPtr, rdPtr->src->GetDepth(), width, height, ST_HWA_RTTEXTURE);
	}
	else {
		return CreateSurface(rdPtr->src->GetDepth(), width, height);
	}
}

inline void GetTransfromedBitmap(LPRDATA rdPtr, LPSURFACE pSrc, 
	const std::function<void(LPSURFACE)>& callback) {
	// get size
	auto width = GetCurrentWidth(rdPtr);
	auto height = GetCurrentHeight(rdPtr);

	cSurface::GetSizeOfRotatedRect(&width, &height, static_cast<float>(GetAngle(rdPtr)));

	const auto pRTT = GetSurface(rdPtr, width, height);
	pRTT->CreateAlpha();
	
	ProcessHWA(rdPtr, pSrc, [&] (const LPSURFACE pHWA) {
		pRTT->BeginRendering(TRUE, 0);
		const auto ds = pHWA->GetRenderTargetSurface();

		// ignoreShader
		//const auto bo = BOP_COPY;
		//const auto param = 0;
		// keepShader
		const auto bo = static_cast<BlitOp>(rdPtr->rs.rsEffect & EFFECT_MASK);
		const auto param = rdPtr->rs.rsEffectParam;
		DWORD flags = 0;

		if (rdPtr->stretchQuality) {
			flags |= STRF_RESAMPLE | STRF_COPYALPHA;
		}

		const auto xScale = abs(rdPtr->zoomScale.XScale);
		const auto yScale = abs(rdPtr->zoomScale.YScale);

		auto x = static_cast<int>(static_cast<float>(rdPtr->hotSpot.x) * xScale);
		auto y = static_cast<int>(static_cast<float>(rdPtr->hotSpot.y) * yScale);

		RotatePoint(rdPtr->angle, &x, &y,
			static_cast<int>(static_cast<float>(pHWA->GetWidth()) * xScale),
			static_cast<int>(static_cast<float>(pHWA->GetHeight()) * yScale));

		const auto blitResult = pHWA->BlitEx(*pRTT
			, static_cast<float>(x), static_cast<float>(y)
			, xScale, yScale
			, 0, 0, pHWA->GetWidth(), pHWA->GetHeight()
			, &rdPtr->hotSpot, static_cast<float>(rdPtr->angle)
			, BMODE_TRANSP
			, bo, param, flags);

		pHWA->ReleaseRenderTargetSurface(ds);
		pRTT->EndRendering();

		if(!blitResult) {
			return;
		}

		ProcessBitmap(rdPtr, pRTT, callback);
		});

	delete pRTT;
}

inline void HandleFlip(LPRDATA rdPtr
	,LPSURFACE& pDisplay
	,const LPSURFACE pBase, LPSURFACE& pHF, LPSURFACE& pVF, LPSURFACE& pVHF) {
	bool bFlipH = rdPtr->zoomScale.XScale < 0;
	bool bFlipV = rdPtr->zoomScale.YScale < 0;

	auto FlipCore = [rdPtr](const LPSURFACE pBase, LPSURFACE& pResult
		, BOOL(cSurface::* pFlipFunc)()) {
		if (pResult != nullptr) {
			return pResult;
		}

		LPSURFACE pFlip = new cSurface;

		auto DoFlip = [&] (const LPSURFACE pSf) {
			pFlip->Clone(*pSf);
			(pFlip->*pFlipFunc)();
		};

		if (rdPtr->pData->bDX11) {
			DoFlip(pBase);
		}
		// bug of DX9 runtime, the source was changed if use DoFlip directly
		else {
			auto bBaseHwa = IsHWA(pBase);

			ProcessBitmap(pBase, [&] (const LPSURFACE pBitmap) {
				DoFlip(pBitmap);
				});

			if (bBaseHwa) {
				ConvertToHWATexture(rdPtr, pFlip);
			}
		}
		
		pResult = pFlip;

		return pResult;
	};

	auto FlipX = [&]() {
		return FlipCore(pBase, pHF, &cSurface::ReverseX);
	};
	auto FlipY = [&]() {
		return FlipCore(pBase, pVF, &cSurface::ReverseY);
	};
	std::function<LPSURFACE()> FlipXY;

	FlipXY = [&]() {
		if (pVHF != nullptr) {
			return pVHF;
		}

		bool bXFlipped = pHF != nullptr;
		bool bYFlipped = pVF != nullptr;

		if (bXFlipped) {
			return FlipCore(pHF, pVHF, &cSurface::ReverseY);
		}

		if (bYFlipped) {
			return FlipCore(pVF, pVHF, &cSurface::ReverseX);
		}

		// if not flipped, flip
		FlipX();

		// call again
		return FlipXY();
	};

	do {
		if (!bFlipH && !bFlipV) {
			pDisplay = pBase;

			break;
		}

		if (bFlipH && bFlipV) {
			pDisplay = FlipXY();

			break;
		}

		if (bFlipH) {
			pDisplay = FlipX();

			break;
		}

		if (bFlipV) {
			pDisplay = FlipY();

			break;
		}
	} while (0);
}

inline void HandleFlip(LPRDATA rdPtr) {
#ifdef _DEBUG
	//__SavetoClipBoard(rdPtr->pSf_Nor);
#endif

	if (rdPtr->fromLib) {
		auto pLibItem = rdPtr->pLibValue;

		HandleFlip(rdPtr
			, rdPtr->src
			, pLibItem->pSf, pLibItem->pSf_HF, pLibItem->pSf_VF, pLibItem->pSf_VHF);
	}
	else {
		HandleFlip(rdPtr
			, rdPtr->src
			, rdPtr->pSf_Nor, rdPtr->pSf_HF, rdPtr->pSf_VF, rdPtr->pSf_VHF);
	}

#ifdef _DEBUG
	//__SavetoClipBoard(rdPtr->pSf_Nor);
#endif
}
