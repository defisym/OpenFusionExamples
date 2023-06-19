#pragma once

#include <algorithm>
#include <functional>
#include <ranges>
#include <string_view>

// ------------
// Forward declaration
// ------------

inline void UpdateHotSpot(LPRDATA rdPtr, HotSpotPos Type, int X = 0, int Y = 0);

// ------------
// Lib object
// ------------

inline bool ObjIsLib(LPRDATA obj) {
	if (!obj || obj->rHo.hoIdentifier != IDENTIFIER) {
		return false;
	}

	if (!obj->isLib) {
		return false;
	}

	return true;
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
inline void ReleaseNonFromLibFlip(LPRDATA rdPtr) {
	delete rdPtr->pSf_HF;
	delete rdPtr->pSf_VF;
	delete rdPtr->pSf_VHF;

	rdPtr->pSf_HF = nullptr;
	rdPtr->pSf_VF = nullptr;
	rdPtr->pSf_VHF = nullptr;
}

inline size_t UpdateRef(LPRDATA rdPtr, bool add) {
	if (rdPtr->pLibValue == nullptr) {
		return static_cast<size_t>(-1);
	}

	if (add) {
		rdPtr->pLibValue->RefImage(rdPtr);
	}
	else {
		rdPtr->pLibValue->UnrefImage(rdPtr);
	}

	return rdPtr->pLibValue->refCount.curRef;

}

inline void DetachFromLib(LPRDATA rdPtr) {
	if (rdPtr->fromLib) {
		UpdateRef(rdPtr, false);

		rdPtr->fromLib = false;

		rdPtr->src = nullptr;
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
inline void AttachToLib(LPRDATA rdPtr, SurfaceLibValue* pLibValue) {
	if (!rdPtr->fromLib) {
		ReleaseNonFromLib(rdPtr);
	}

	// Update ref
	UpdateRef(rdPtr, false);

	rdPtr->fromLib = true;

	rdPtr->src = pLibValue->pSf;
	rdPtr->pLibValue = pLibValue;

	UpdateRef(rdPtr, true);
}

// ------------
// Display object
// ------------

// ------------
// Get properties
// ------------

inline bool CanDisplay(LPRDATA rdPtr) {
	return !rdPtr->isLib && rdPtr->src != nullptr && rdPtr->src->IsValid();
}

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
	auto pos = BasePath.size();

	if (!(BasePath.ends_with(L'\\') || BasePath.ends_with(L'/'))) {
		pos += 1;
	}

	try {
		return FilePath.substr(pos, FilePath.size() - pos);
	}
	catch (...) {
		//return L"Invalid";
		return FilePath;
	}
}

// ------------
// Handle new picture
// ------------

inline void UpdateHoImgInfo(LPRDATA rdPtr) {
	UpdateHoImgInfo(rdPtr, rdPtr->src
		, rdPtr->zoomScale.XScale, rdPtr->zoomScale.YScale
		, rdPtr->hotSpotPos
		, rdPtr->hotSpot.x, rdPtr->hotSpot.y
		, rdPtr->angle);
}

inline void ReDisplay(LPRDATA rdPtr) {
	if (!rdPtr->isLib) {
		rdPtr->rc.rcChanged = true;

		UpdateHoImgInfo(rdPtr);
		FreeColMask(rdPtr->pColMask);
	}
}

// reset default values, then call ReDisplay
inline void NewPic(LPRDATA rdPtr, LPRDATA Copy = nullptr) {
	if (Copy == nullptr) {
		rdPtr->angle = 0;

		rdPtr->hotSpot = { 0,0 };
		rdPtr->zoomScale = { 1.0,1.0 };

	}
	else {
		rdPtr->angle = Copy->angle;

		rdPtr->hotSpotPos = Copy->hotSpotPos;
		rdPtr->hotSpot = Copy->hotSpot;
		rdPtr->zoomScale = Copy->zoomScale;
	}

	UpdateHotSpot(rdPtr, rdPtr->hotSpotPos);

	ReDisplay(rdPtr);
}

// ------------
// Handle transfrom
// ------------

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

inline void Zoom(LPRDATA rdPtr, float XScale, float YScale) {
	if (rdPtr->zoomScale == ZoomScale{ XScale, YScale }) {
		return;
	}

	rdPtr->zoomScale = { XScale ,YScale };

	ReDisplay(rdPtr);
}

inline void Rotate(LPRDATA rdPtr, int Angle) {
	if (rdPtr->angle == Angle) {
		return;
	}

	rdPtr->angle = Angle;

	ReDisplay(rdPtr);
}

inline auto Offset(LPSURFACE Src, LPSURFACE Des, const OffsetCoef& offsetCoef) {
	return Offset(Src, Des, offsetCoef.XOffset, offsetCoef.YOffset, offsetCoef.Wrap);
}

inline auto GetRenderTargetSurface(LPRDATA rdPtr, int width, int height) {
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

	const auto pRTT = GetRenderTargetSurface(rdPtr, width, height);
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

		LPSURFACE pFlip = nullptr;

		//auto DoFlip = [&] (const LPSURFACE pSf) {
		//	pFlip->Clone(*pSf);
		//	(pFlip->*pFlipFunc)();
		//};

		if (rdPtr->pData->bDX11) {
			pFlip = CreateCloneSurface(rdPtr, pBase);
			(pFlip->*pFlipFunc)();
		}
		// bug of DX9 runtime, the source was changed if use DoFlip directly
		else {
			const auto bBaseHwa = IsHWA(pBase);

			pFlip = new cSurface;

			ProcessBitmap(pBase, [&] (const LPSURFACE pBitmap) {
				pFlip->Clone(*pBitmap);
				(pFlip->*pFlipFunc)();
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
		const auto pLibItem = rdPtr->pLibValue;

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

// ------------
// Load functions
// ------------

// Create new surface according to HWA
inline auto CreateNewSurface(LPRDATA rdPtr, bool HWA) {
	return HWA
		? CreateHWASurface(rdPtr, 32, 4, 4, ST_HWA_ROMTEXTURE)
		: CreateSurface(32, 4, 4);
}

// Load file then convert to Src type
inline bool LoadFromFile(LPSURFACE& Src, LPCWSTR FilePath, LPCWSTR Key, LPRDATA rdPtr, int width, int height, bool NoStretch, bool bHighQuality) {
	// HWA?
	auto srcHWA = IsHWA(Src);
	bool ret = true;

	// Load by bitmap, as HWA texture cannot add alpha channel
	LPSURFACE pBitmap = srcHWA ? CreateNewSurface(rdPtr, false) : Src;

	if (StrEmpty(Key)) {
		ret = _LoadFromFile(pBitmap, FilePath, rdPtr, width, height, NoStretch, bHighQuality);
	}
	else {
		Encryption E;
		E.GenerateKey(Key);

		E.OpenFile(FilePath);
		E.Decrypt();

		ret = _LoadFromMemFile(pBitmap, E.GetOutputData(), E.GetOutputDataLength(), rdPtr, width, height, NoStretch, bHighQuality);
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

inline void LoadFromFile(LPRDATA rdPtr, LPCWSTR FileName, LPCWSTR Key = L"") {
	const auto fullPath = GetFullPathNameStr(FileName);

	auto loadCallback = [&] (LPSURFACE* ppSf,
		const std::function<void(LPSURFACE pSf)>& changed) {
			if (rdPtr->bLoadCallback) {
				rdPtr->pLoadCallbackInfo->callbackFileName = FileName;
				rdPtr->pLoadCallbackInfo->ppSf = ppSf;

				// process will do clone first
				// so update back must change the address
				const auto pOldSf = *ppSf;

				CallEvent(ONLOADCALLBACK)

					if (pOldSf != *ppSf) {
						changed(*ppSf);
					}
			}
	};

	auto loadCore = [&] (const std::function<void(LPSURFACE pSf)>& success) {
		auto pSf = CreateNewSurface(rdPtr, rdPtr->HWA);
		const auto bRet = LoadFromFile(pSf,
			fullPath.c_str(), Key,
			rdPtr, -1, -1,
			true, rdPtr->stretchQuality);

		if (bRet && pSf->IsValid()) {
			success(pSf);
		}
		else {
			delete pSf;
		}
	};

	if (rdPtr->isLib) {
		//if (rdPtr->pLib->contains(fullPath)) {
		//	return;
		//}

		loadCore([&] (LPSURFACE pSf) {
			rdPtr->pData->UpdateLib(fullPath, GetFileHash(fullPath), pSf);

			//auto newValue = SurfaceLibValue(pSf, GetFileHash(fullPath), GetTransparent(pSf));
			//newValue.refCount.priority = rdPtr->pLib->size();

			//(*rdPtr->pLib)[fullPath] = newValue;

			loadCallback(&pSf, [&] (const LPSURFACE pCallbackSf) {
				const auto it = rdPtr->pLib->find(fullPath);
				it->second.pSf = pCallbackSf;
				it->second.isTransparent = GetTransparent(pCallbackSf);
			});
		});
	}
	else {
		loadCore([&] (LPSURFACE pSf) {
			DetachFromLib(rdPtr, pSf);

			loadCallback(&pSf, [&] (const LPSURFACE pCallbackSf) {
				DetachFromLib(rdPtr, pCallbackSf);
			});

			NewPic(rdPtr);

			*rdPtr->FilePath = FileName;
			*rdPtr->Key = Key;

			GetFileName(rdPtr);
		});
	}
}

// lib load core
inline SurfaceLibIt LoadLib(LPRDATA rdPtr, LPRDATA obj, LPCWSTR FileName, LPCWSTR Key = L"") {
	if (!ObjIsLib(obj)) {
		return obj->pLib->end();
	}

	const auto fullPath = GetFullPathNameStr(FileName);

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
	auto it = LoadLib(rdPtr, obj, FileName, Key);
	if (it == obj->pLib->end()) {
		return;
	}

	// update src
	if (!rdPtr->isLib) {
		AttachToLib(rdPtr, &(it->second));
		NewPic(rdPtr);
	}
	else {
		if (!rdPtr->pLib->contains(fullPath)) {
			(*rdPtr->pLib)[it->first] = it->second;
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
		AttachToLib(rdPtr, obj->pLibValue);
	}
	else {
		//const auto pSf = CreateNewSurface(rdPtr, rdPtr->HWA);
		//pSf->Clone(*obj->src);

		const auto pSf = CreateCloneSurface(rdPtr, obj->src);

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

	//auto pSave = new cSurface;

	//ProcessBitmap(rdPtr, pSf, [&] (const LPSURFACE pBitmap) {
	//	pSave->Clone(*pBitmap);

	//	if (rdPtr->HWA) {
	//		ConvertToHWATexture(rdPtr, pSave);
	//	}
	//	});

	const auto pSave = CreateCloneSurface(rdPtr, pSf);

	if (!pSave->IsValid()) {
		delete pSave;

		return;
	}

	const auto bNoFullPath = dwFlags & LoadFromPointerFlags_NoFullPath;
	const auto fullPath = bNoFullPath
		? std::wstring(pFileName)
		: GetFullPathNameStr(pFileName);

	if (rdPtr->isLib) {
		rdPtr->pData->UpdateLib(fullPath, fullPath, pSave);
	}
	else {
		DetachFromLib(rdPtr, pSave);
		NewPic(rdPtr);

		*rdPtr->FilePath = fullPath + L"\\" + L"LoadFromPointer";
		*rdPtr->Key = L"";

		GetFileName(rdPtr);
	}
}

inline LPSURFACE GetSurfacePointer(LPRDATA rdPtr, const std::wstring& FilePath, const std::wstring& Key) {
	cSurface* ret = nullptr;

	if (!rdPtr->isLib) {
		ret = rdPtr->src;
	}
	else {
		const auto it = LoadLib(rdPtr, rdPtr, FilePath.c_str(), Key.c_str());

		if (it == rdPtr->pLib->end()) {
			ret = nullptr;
		}
		else {
			ret = it->second.pSf;
		}
	}

	return ret;
}

// ------------
// Member Functions
// ------------

// ------------
// SurfaceLibValue
// ------------

inline void SurfaceLibValue::UpdateRefObject() const {
	for (const auto& it : refCount.pRefObj) {
		it->src = pSf;
	}
}

inline bool SurfaceLibValue::UpdateShaderUsage() {
	do {
		if (!bUsedInShader) {
			break;
		}

		const auto removeIt = std::ranges::remove_if(*pShaderList,
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

				if (pSfImpl != GetSurfaceImplementation(*pSf)) {
					return true;
				}

				return false;
		});

		pShaderList->erase(removeIt.begin(), removeIt.end());

		bUsedInShader = !pShaderList->empty();
	} while (false);

	return bUsedInShader;
}

// ------------
// GlobalData
// ------------

inline void GlobalData::GetEstimateMemUsage() {
	// optimize it by add/sub value when modifing lib instead of iterate if has performance issue (unlikely)
	estimateRAMSizeMB = 0;
	estimateVRAMSizeMB = 0;

	for (auto& libValue : *pLib | std::views::values) {
		const LPSURFACE* pSfArrary = &libValue.pSf;

		for (int i = 0; i < SurfaceLibSfNum; i++) {
			const auto pSf = *(pSfArrary + i);

			if (pSf == nullptr) {
				continue;
			}

			const bool bHWA = IsHWA(pSf);
			const auto estimateSizeMB = GetEstimateSizeMB(pSf);

			if (!bHWA) {
				estimateRAMSizeMB += estimateSizeMB;
			}
			else {
				estimateVRAMSizeMB += estimateSizeMB;
			}
		}
	}
}

inline void GlobalData::MergeLib(LPRDATA rdPtr) const {
	if (pPreloadHandler->AbleToMerge()) {
		for (auto& it : *pPreloadHandler->pPreloadLib) {
			auto& [name, key] = it;
			if (!pLib->contains(name)) {
				(*pLib)[name] = key;
			}
		}
				
		pPreloadHandler->bPreloadMerge = false;

		CallEvent(ONPRELOADCOMPLETE);
	}
}

inline void GlobalData::PreloadLib(PreloadHandler* pPreloadHandler, const std::wstring& Key,
	const std::function<void()>& callback) {
	if (pPreloadHandler->pPreloadList->empty()) {
		callback();

		return;
	}

	for (auto& it : *pPreloadHandler->pPreloadList) {
		// Stop loading when exceed limit
		if (ExceedMemLimit(memoryLimit)) {
			break;
		}

		std::unique_lock<std::mutex> lock(pPreloadHandler->mtx);
		if (pPreloadHandler->rdPtr == nullptr) {
			pPreloadHandler->bPaused = true;
			pPreloadHandler->cv.wait(lock);
		}
		else {
			pPreloadHandler->bPaused = false;

			// can only load bitmap in sub thread, cannot load HWA here
			LPSURFACE pBitmap = CreateSurface(32, 4, 4);
			LoadFromFile(pBitmap,
				it.c_str(), Key.c_str(),
				pPreloadHandler->rdPtr, -1, -1,
				true, pPreloadHandler->rdPtr->stretchQuality);

			if (pBitmap->IsValid()) {
				(*pPreloadHandler->pPreloadLib)[it] = SurfaceLibValue(pBitmap, GetFileHash(it), GetTransparent(pBitmap));
			}
			else {
				delete pBitmap;
			}
		}

		// Stop loading when main object is destroyed
		if (pPreloadHandler->bForceExit) {
			for (auto& libValue : *pPreloadHandler->pPreloadLib | std::views::values) {
				libValue.Release();
			}

			break;
		}
	}

	callback();
}