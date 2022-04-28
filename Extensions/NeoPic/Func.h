#pragma once

inline void UpdateHotSpot(LPRDATA rdPtr, HotSpotPos Type, int X = 0, int Y = 0);

inline void UpdateImg(LPRDATA rdPtr, bool ForceLowQuality = false, bool ForceUpdate = false);

inline void GetTransformedSize(int& width, int& height, ZoomScale Scale = { 1.0,1.0 }, int Angle = 0, ATArray AT = { 1,0,0,1 });

inline void GetFileName(LPRDATA rdPtr);

//-----------------------------

inline void AddBackdrop(LPRDATA rdPtr, cSurface* pSf, int x, int y, DWORD dwInkEffect, DWORD dwInkEffectParam, int nObstacleType, int nLayer) {
	rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvAddBackdrop(pSf, x, y, dwInkEffect, dwInkEffectParam, nObstacleType, nLayer);
}

inline void ConvertHWA(LPRDATA rdPtr) {
	LPSURFACE srchwa = ConvertHWATexture(rdPtr, rdPtr->src);
	LPSURFACE imghwa = ConvertHWATarget(rdPtr, rdPtr->img);

	if (rdPtr->src != srchwa) {
		if (!rdPtr->fromLib) {
			delete rdPtr->src;
		}
		rdPtr->fromLib = false;

		rdPtr->src = srchwa;
	}

	if (rdPtr->img != imghwa) {
		delete rdPtr->img;
		rdPtr->img = imghwa;
	}
}

inline void ReDisplay(LPRDATA rdPtr) {
	if (!rdPtr->isLib) {
		//callRunTimeFunction(rdPtr, RFUNCTION_REDRAW, 0, 0);
		rdPtr->rc.rcChanged = true;

		rdPtr->rHo.hoImgXSpot = rdPtr->hotSpot.x;
		rdPtr->rHo.hoImgYSpot = rdPtr->hotSpot.y;

		rdPtr->rHo.hoImgWidth = rdPtr->src->GetWidth();
		rdPtr->rHo.hoImgHeight = rdPtr->src->GetHeight();

		rdPtr->changed = true;

		FreeColMask(rdPtr->pColMask);
	}
}

inline void NewImg(LPRDATA rdPtr) {
	//delete rdPtr->img;
	//rdPtr->img = new cSurface;
	//rdPtr->img->Clone(*rdPtr->src);
	
	rdPtr->img = rdPtr->src;
}

//Set default values
inline void NewPic(LPRDATA rdPtr){
	rdPtr->hotSpot = { 0,0 };
	UpdateHotSpot(rdPtr, rdPtr->defaultHotSpot);
	
	rdPtr->zoomScale = { 1.0,1.0 };
	rdPtr->angle = 0;

	rdPtr->offset = { 0,0,false };	
	rdPtr->AT = { 1,0,0,1 };

	rdPtr->imgHotSpot = rdPtr->hotSpot;
	rdPtr->imgZoomScale = rdPtr->zoomScale;
	rdPtr->imgAngle = rdPtr->angle;

	rdPtr->imgOffset = rdPtr->offset;
	rdPtr->imgAT = rdPtr->AT;

	NewImg(rdPtr);

	ReDisplay(rdPtr);
}

inline void NewPic(LPRDATA rdPtr, LPRDATA Copy) {
	rdPtr->hotSpot = Copy->hotSpot;
	rdPtr->zoomScale = Copy->zoomScale;
	rdPtr->angle = Copy->angle;

	rdPtr->offset = Copy->offset;

	rdPtr->imgHotSpot = rdPtr->hotSpot;
	rdPtr->imgZoomScale = rdPtr->zoomScale;
	rdPtr->imgAngle = rdPtr->angle;

	rdPtr->imgOffset = rdPtr->offset;

	NewImg(rdPtr);

	ReDisplay(rdPtr);
}

inline void RotatePoint(double angle, int* hotX, int* hotY, int sw, int sh) {
	//Rotate hotspot
	float hx = (float)*hotX;
	float hy = (float)*hotY;

	float si = (float)sin(angle);
	float co = (float)cos(angle);

	int trX = (int)(sw * co);
	int trY = (int)(-sw * si);
	int blX = (int)(sh * si);
	int blY = (int)(sh * co);
	int brX = (int)(sw * co + sh * si);
	int brY = (int)(sh * co - sw * si);

	//Update topleft coordinate
	int dx = min(0, min(trX, min(blX, brX)));
	int dy = min(0, min(trY, min(blY, brY)));

	//Update hotspot
	*hotX = (int)(round(hx * co + hy * si)-dx);
	*hotY = (int)(round(hy * co - hx * si)-dy);
}

inline void RotatePoint(int angle, int* hotX, int* hotY, int sw, int sh) {	
	return RotatePoint(RAD(angle), hotX, hotY, sw, sh);
}

inline void UpdateHotSpot(LPRDATA rdPtr, int X, int Y) {
	rdPtr->hotSpot.x = X;
	rdPtr->hotSpot.y = Y;
}

inline void UpdateHotSpot(LPRDATA rdPtr, HotSpotPos Type, int X, int Y ) {
	switch (Type) {
	case HotSpotPos::LT:
		UpdateHotSpot(rdPtr, 0, 0);
		break;
	case HotSpotPos::LM:
		UpdateHotSpot(rdPtr, 0, (int)(rdPtr->src->GetHeight() / 2));
		break;
	case HotSpotPos::LB:
		UpdateHotSpot(rdPtr, 0, rdPtr->src->GetHeight());
		break;
	case HotSpotPos::MT:
		UpdateHotSpot(rdPtr, (int)(rdPtr->src->GetWidth() / 2), 0);
		break;
	case HotSpotPos::MM:
		UpdateHotSpot(rdPtr, (int)(rdPtr->src->GetWidth() / 2), (int)(rdPtr->src->GetHeight() / 2));
		break;
	case HotSpotPos::MB:
		UpdateHotSpot(rdPtr, (int)(rdPtr->src->GetWidth() / 2), Y);
		break;
	case HotSpotPos::RT:
		UpdateHotSpot(rdPtr, rdPtr->src->GetWidth(), 0);
		break;
	case HotSpotPos::RM:
		UpdateHotSpot(rdPtr, rdPtr->src->GetWidth(), (int)(rdPtr->src->GetHeight() / 2));
		break;
	case HotSpotPos::RB:
		UpdateHotSpot(rdPtr, rdPtr->src->GetWidth(), rdPtr->src->GetHeight());
		break;
	case HotSpotPos::CUSTOM:
		UpdateHotSpot(rdPtr, X, Y);
		break;
	}

	ReDisplay(rdPtr);
}

inline void DoZoom(LPRDATA rdPtr, LPSURFACE Src, POINT SrcHotSpot, LPSURFACE& Des, POINT& DesHotSpot, float XScale, float YScale) {
	bool ReverseX = XScale < 0.0;
	bool ReverseY = YScale < 0.0;

	XScale = abs(XScale);
	YScale = abs(YScale);

	DesHotSpot.x = (int)(SrcHotSpot.x * XScale);
	DesHotSpot.y = (int)(SrcHotSpot.y * YScale);

	int width = (int)(Src->GetWidth() * XScale);
	int height = (int)(Src->GetHeight() * YScale);

	delete Des;
	Des = CreateSurface(24, width, height);

	Stretch(Src, Des, rdPtr->stretchQuality);

	if (ReverseX) {
		Des->ReverseX();
	}
	if (ReverseY) {
		Des->ReverseY();
	}
}

inline void Zoom(LPRDATA rdPtr, float XScale, float YScale, bool UpdateCur = false) {
	if (rdPtr->zoomScale == ZoomScale{ XScale, YScale }) {
		return;
	}

	rdPtr->zoomScale = { XScale ,YScale };

	ReDisplay(rdPtr);
}

inline void DoRotate(LPRDATA rdPtr, LPSURFACE Src, POINT SrcHotSpot, LPSURFACE& Des, POINT& DesHotSpot, int Angle) {
	DesHotSpot = SrcHotSpot;
	RotatePoint(Angle, (int*)&DesHotSpot.x, (int*)&DesHotSpot.y, Src->GetWidth(), Src->GetHeight());

	delete Des;
	Des = CreateSurface(24, 4, 4);

	Src->CreateRotatedSurface(*Des, Angle, rdPtr->stretchQuality);
	//Src->CreateRotatedSurface(*rdPtr->img, Angle, rdPtr->stretchQuality, DARK_GREEN);
}

inline void Rotate(LPRDATA rdPtr, int Angle, bool UpdateCur = false) {
	if (rdPtr->angle == Angle) {
		return;
	}

	rdPtr->angle = Angle;

	ReDisplay(rdPtr);
}

inline LPSURFACE Offset(LPSURFACE Src, OffsetCoef O) {
	return Offset(Src, O.XOffset, O.YOffset, O.Wrap);
}

inline auto OffsetHWA(LPSURFACE Src, LPSURFACE Des, OffsetCoef O) {
	return OffsetHWA(Src, Des, O.XOffset, O.YOffset, O.Wrap);
}

inline void GetTransformedSize(int& width, int& height, ZoomScale Scale, int Angle, ATArray AT) {
	//Affine Transform


	//Zoom
	width *= (int)abs(Scale.XScale);
	height *= (int)abs(Scale.YScale);

	//Rotate
	cSurface Rotate;
	Rotate.GetSizeOfRotatedRect(&width, &height, (float)Angle);
}

inline void AffineTransformation(LPSURFACE& Src,ATArray A, int divide) {
	AffineTransformation(Src, A.a11, A.a12, A.a21, A.a22, divide);
}

inline void UpdateImg(LPRDATA rdPtr, bool ForceLowQuality, bool ForceUpdate) {
	if (ForceUpdate ||
		rdPtr->offset != rdPtr->imgOffset||
		rdPtr->AT != rdPtr->imgAT ||
		rdPtr->zoomScale != rdPtr->imgZoomScale ||
		rdPtr->angle != rdPtr->imgAngle) {
		//Update Coef
		rdPtr->imgOffset = rdPtr->offset;
		rdPtr->imgAT = rdPtr->AT;
		rdPtr->imgZoomScale = rdPtr->zoomScale;
		rdPtr->imgAngle = rdPtr->angle;

		//Fast
		bool OldQuality;

		if (ForceLowQuality) {
			OldQuality = rdPtr->stretchQuality;
			rdPtr->stretchQuality = false;
		}

		//Delete old if it's direct reference from source
		if (rdPtr->img != rdPtr->src) {
			delete rdPtr->img;
		}		

		//Temp
		LPSURFACE Temp = nullptr;

		//AffineTrans(rdPtr->AT)

		//Offset
		//create then return new surface
		rdPtr->img = Offset(rdPtr->src, rdPtr->imgOffset);

		//NewImg(rdPtr);

		//Zoom
		DoZoom(rdPtr, rdPtr->img, rdPtr->hotSpot, Temp, rdPtr->imgHotSpot, rdPtr->zoomScale.XScale, rdPtr->zoomScale.YScale);

		//Rotate
		DoRotate(rdPtr, Temp, rdPtr->imgHotSpot, rdPtr->img, rdPtr->imgHotSpot, rdPtr->angle);

		//Delete temp
		delete Temp;

		//Fast
		if (ForceLowQuality) {			 
			rdPtr->stretchQuality = OldQuality;
		}
	}
}

inline void _LoadFromFile(LPSURFACE& Src, LPCTSTR FilePath, LPCTSTR Key, LPRDATA rdPtr, int width, int height, bool NoStretch, bool HighQuality) {
	if (StrEmpty(Key)) {
		_LoadFromFile(Src, FilePath, rdPtr, width, height, NoStretch, HighQuality);
	}
	else {
		Encryption E;
		E.GenerateKey(Key);

		E.OpenFile(FilePath);
		E.Decrypt();

		CInputMemFile MemFile;
		MemFile.Create(E.GetOutputData(), E.GetOutputDataLength());

		//MGR
		CImageFilterMgr* pImgMgr = rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvImgFilterMgr;
		CImageFilter    pFilter(pImgMgr);

		if (NoStretch) {
			if (!ImportImageFromInputFile(pImgMgr, &MemFile, Src, 0, 0)) {
				CreateBlankSurface(Src);
			}
		}
		else {
			cSurface img;

			if(ImportImageFromInputFile(pImgMgr, &MemFile, &img, 0, 0)){
				delete Src;
				Src = CreateSurface(24, width, height);

				Stretch(&img, Src, HighQuality);
			}
			else {
				CreateBlankSurface(Src);
			}
		}
	}

	_AddAlpha(Src);

	if (rdPtr->HWA) {
		auto pSf = Src;
		Src = ConvertHWATexture(rdPtr, Src);
		
		delete pSf;
	}
}

inline void LoadFromFile(LPRDATA rdPtr, LPCWSTR FileName, LPCTSTR Key = _T("")) {
	if (rdPtr->isLib) {
		if (rdPtr->lib->find(FileName) != rdPtr->lib->end()) {
			return;
		}

		LPSURFACE img = new cSurface;
		_LoadFromFile(img, FileName, Key, rdPtr, -1, -1, true, rdPtr->stretchQuality);
		
		if (img->IsValid()) {
			(*rdPtr->lib)[FileName] = img;
		}
		else {
			delete img;
		}
	}
	else {
		if (rdPtr->fromLib) {			
			rdPtr->src = new cSurface;
		}

		rdPtr->fromLib = false;

		_LoadFromFile(rdPtr->src, FileName, Key, rdPtr, -1, -1, true, rdPtr->stretchQuality);
		
		if (rdPtr->src->IsValid()) {
			NewPic(rdPtr);

			*rdPtr->FilePath = FileName;
			*rdPtr->Key = Key;

			GetFileName(rdPtr);			
		}
	}	
}

inline void LoadFromLib(LPRDATA rdPtr, LPRO object, LPCWSTR FileName, LPCTSTR Key = _T("")) {
	LPRDATA obj = (LPRDATA)object;

	if (!obj || obj->rHo.hoIdentifier != IDENTIFIER) {
		return;
	}

	if (!obj->isLib) {
		return;
	}

	auto it = obj->lib->find(FileName);
	
	if (it == obj->lib->end()) {
		LoadFromFile(obj, FileName, Key);
	}

	it = obj->lib->find(FileName);
	if (it == obj->lib->end()) {
		return;
	}
	
	auto countit = obj->pCount->find(FileName);	
	if (countit != obj->pCount->end()) {
		auto count = countit->second;
	}
	auto curCount = countit != obj->pCount->end()
		? countit->second + 1
		: 1;
	
	(*obj->pCount)[FileName] = curCount;
	
	if(!rdPtr->isLib){
		if (!rdPtr->fromLib) {
			delete rdPtr->src;
		}

		rdPtr->fromLib = true;
		rdPtr->src = it->second;

		NewPic(rdPtr);
	}
	else {
		auto thisit = rdPtr->lib->find(FileName);
		if (thisit == rdPtr->lib->end()) {
			rdPtr->lib->emplace(it->first, it->second);
		}
	}

	*rdPtr->FilePath = FileName;
	*rdPtr->Key = Key;

	GetFileName(rdPtr);
}

inline void LoadFromLib(LPRDATA rdPtr, int Fixed, LPCWSTR FileName, LPCTSTR Key = _T("")) {
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
		if (!rdPtr->fromLib) {
			delete rdPtr->src;
		}

		rdPtr->fromLib = true;
		rdPtr->src = obj->src;
	}
	else {
		if (rdPtr->fromLib) {
			rdPtr->src = nullptr;
		}

		rdPtr->fromLib = false;
		
		delete rdPtr->src;
		rdPtr->src = new cSurface;
		rdPtr->src->Clone(*obj->src);
	}

	*rdPtr->FilePath = *obj->FileName;
	*rdPtr->Key = *obj->Key;

	GetFileName(rdPtr);

	if (CopyCoef) {
		NewPic(rdPtr, obj);
	}
	else {
		NewPic(rdPtr);
	}	
}

inline void LoadFromDisplay(LPRDATA rdPtr, int Fixed, bool CopyCoef = false) {
	return LoadFromDisplay(rdPtr, LproFromFixed(rdPtr, Fixed), CopyCoef);
}

inline void ResetLib(SurfaceLib* pData) {
	if (pData != NULL) {
		for (auto& it : *pData) {
			delete it.second;
		}

		pData->clear();
	}
}

inline void DeleteLib(SurfaceLib* pData) {
	if (pData != NULL) {
		for (auto& it : *pData) {
			delete it.second;
		}

		delete pData;
	}
}

inline void EraseLib(SurfaceLib* pData, LPCTSTR Item) {
	auto it = pData->find(Item);
	if (it != pData->end()) {
		delete it->second;
		pData->erase(it);
	}
}

#include <functional>

using LoadLibCallBack = std::function<void(SurfaceLib*)>;
using FileList = std::vector<std::wstring>;

inline void GetFullPathFromName(FileList& outList, const FileList inList, const std::wstring basePath) {
	outList.reserve(inList.size());

	FileList fileList;
	GetFileList(&fileList, basePath);

	for (auto& it : inList) {
		for (auto fileIt = fileList.begin(); fileIt != fileList.end();) {
			fileIt = std::find_if(fileIt + 1, fileList.end(), [&](std::wstring& file) {
				auto pos = file.find_last_of(L"\\") + 1;
				auto fileName = file.substr(pos, file.size() - pos);

				if (fileName == it) {
					return true;
				}

				return false;
				});

			if (fileIt != fileList.end()) {
				outList.emplace_back(*fileIt);
			}
		}
	}
}

constexpr auto PRELOAD_TERMIANTE = 1;

// do not ref PreloadList as this function is for multithread
inline int PreloadLibFromVec(LPRDATA rdPtr, FileList PreloadList, bool fullPath, std::wstring BasePath, std::wstring Key, LoadLibCallBack callBack) {
	if (PreloadList.empty()) {
		rdPtr->forceExit = false;
		rdPtr->threadID = 0;

		return 0;
	}

	FileList* list;
	
	FileList fullPathList;	
	
	if (!fullPath) {
		GetFullPathFromName(fullPathList, PreloadList, BasePath);
		list = &fullPathList;
	}
	else {
		list = &PreloadList;
	}

	SurfaceLib* tempLib = new SurfaceLib;

	for (auto& it : *list) {
		if (min(rdPtr->memoryLimit + CLEAR_MEMRANGE, MAX_MEMORYLIMIT) <= (GetProcessMemoryUsage() >> 20)) {
			break;
		}

		LPSURFACE img = new cSurface;
		_LoadFromFile(img, it.c_str(), Key.c_str(), rdPtr, -1, -1, true, rdPtr->stretchQuality);

		if (img->IsValid()) {
			tempLib->emplace(it, img);
		}
		else {
			delete img;
		}

		if (rdPtr->forceExit) {
			for (auto& it : *tempLib) {
				delete it.second;
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

	rdPtr->forceExit = false;
	rdPtr->threadID = 0;

	return 0;
}

inline void CreatePreloadProcess(LPRDATA rdPtr, FileList* pList, bool fullPath, std::wstring BasePath, std::wstring Key) {
	delete rdPtr->pPreloadList;
	rdPtr->pPreloadList = new PreLoadList;
	rdPtr->pPreloadList->reserve(pList->size());

	// filter duplicate
	for (auto& it : *pList) {
		if (std::find(rdPtr->pPreloadList->begin(), rdPtr->pPreloadList->end(), it) == rdPtr->pPreloadList->end()
			&& std::find_if(rdPtr->lib->begin(), rdPtr->lib->end(), [it](auto& p) {
				return p.first == it;
				}) == rdPtr->lib->end()) {
			rdPtr->pPreloadList->emplace_back(it);
		}
	}

	rdPtr->preloading = true;
	std::thread pl(PreloadLibFromVec, rdPtr, *rdPtr->pPreloadList, fullPath, BasePath, Key
		, [rdPtr](const SurfaceLib* lib) {
			rdPtr->preloading = false;
			rdPtr->preloadMerge = true;
			rdPtr->preloadLib = lib;
		});

	HANDLE handle;
	DuplicateHandle(GetCurrentProcess(), pl.native_handle(), GetCurrentProcess(), &handle, THREAD_QUERY_INFORMATION | THREAD_TERMINATE, NULL, NULL);

	rdPtr->threadID = handle;
	pl.detach();
}

inline void MergeLib(LPRDATA rdPtr) {
	if (!rdPtr->preloading
		&& rdPtr->preloadMerge) {
		for (auto& it : *rdPtr->preloadLib) {
			auto& [name, pSf] = it;

			if (rdPtr->lib->find(name) == rdPtr->lib->end()) {
				rdPtr->lib->emplace(name, pSf);
			}
		}

		delete rdPtr->preloadLib;
		rdPtr->preloading = false;
		rdPtr->preloadMerge = false;
		CallEvent(ONPRELOADCOMPLETE);
	}
}

inline void GetKeepList(LPRDATA rdPtr, const FileList& keepList, std::wstring basePath) {
	rdPtr->pKeepList->clear();
	GetFullPathFromName(*rdPtr->pKeepList, keepList, basePath);
}

inline void CleanCache(LPRDATA rdPtr, bool forceClean = false) {
	if (!rdPtr->preloading
		&& rdPtr->isLib) {
		if (forceClean
			||(rdPtr->pCount->size() > CLEAR_NUMTHRESHOLD
			&& min(rdPtr->memoryLimit + CLEAR_MEMRANGE, MAX_MEMORYLIMIT) <= (GetProcessMemoryUsage() >> 20))
			) {
			rdPtr->pCountVec->clear();
			rdPtr->pCountVec->reserve(rdPtr->pCount->size());

			for (auto& it : *rdPtr->pCount) {
				if (std::find(rdPtr->pKeepList->begin(), rdPtr->pKeepList->end(), it.first) == rdPtr->pKeepList->end()) {
					rdPtr->pCountVec->emplace_back(it);
				}
			}

			// reverse it to keep newest if no ref
			std::reverse(rdPtr->pCountVec->begin(), rdPtr->pCountVec->end());

			std::sort(rdPtr->pCountVec->begin(), rdPtr->pCountVec->end(), [](MapPair& l, MapPair& r) {
				return l.second < r.second;
				});

			while (!rdPtr->pCount->empty()
				&& rdPtr->memoryLimit / 2 <= (GetProcessMemoryUsage() >> 20)) {
				auto& fileName = rdPtr->pCountVec->back().first;

				auto pSf = (*rdPtr->lib)[fileName];
				delete pSf;

				rdPtr->lib->erase(fileName);
				rdPtr->pCount->erase(fileName);

				rdPtr->pCountVec->pop_back();
			}
		}
	}
}

// Get information

inline long GetHotSpotX(LPRDATA rdPtr) {
	return rdPtr->src != nullptr && rdPtr->src->IsValid() ? rdPtr->hotSpot.x : -1;
}

inline long GetHotSpotY(LPRDATA rdPtr) {
	return rdPtr->src != nullptr && rdPtr->src->IsValid() ? rdPtr->hotSpot.y : -1;
}

inline int GetOriginalWidth(LPRDATA rdPtr) {
	return rdPtr->src != nullptr && rdPtr->src->IsValid() ? rdPtr->src->GetWidth() : -1;
}

inline int GetOriginalHeight(LPRDATA rdPtr) {
	return rdPtr->src != nullptr && rdPtr->src->IsValid() ? rdPtr->src->GetHeight() : -1;
}

inline int GetCurrentWidth(LPRDATA rdPtr) {
	return rdPtr->src != nullptr && rdPtr->src->IsValid() ? (int)(rdPtr->src->GetWidth() * abs(rdPtr->zoomScale.XScale)) : -1;
}

inline int GetCurrentHeight(LPRDATA rdPtr) {
	return rdPtr->src != nullptr && rdPtr->src->IsValid() ? (int)(rdPtr->src->GetHeight() * abs(rdPtr->zoomScale.YScale)) : -1;
}

inline float GetXZoomScale(LPRDATA rdPtr) {
	return rdPtr->src != nullptr && rdPtr->src->IsValid() ? rdPtr->zoomScale.XScale : -1;
}

inline float GetYZoomScale(LPRDATA rdPtr) {
	return rdPtr->src != nullptr && rdPtr->src->IsValid() ? rdPtr->zoomScale.YScale : -1;
}

inline int GetAngle(LPRDATA rdPtr) {
	return rdPtr->src != nullptr && rdPtr->src->IsValid() ? rdPtr->angle : -1;
}

inline void GetFileName(LPRDATA rdPtr) {	
	auto pos = rdPtr->FilePath->find_last_of(L"\\") + 1;
	*rdPtr->FileName = rdPtr->FilePath->substr(pos, rdPtr->FilePath->size() - pos);
}

inline auto GetSurface(LPRDATA rdPtr, int width, int height) {
	if (rdPtr->HWA) {
		return CreateHWASurface(rdPtr, rdPtr->src->GetDepth(), width, height, ST_HWA_RTTEXTURE);
	}
	else {
		return CreateSurface(rdPtr->src->GetDepth(), width, height);
	}
}

inline void GetTransfromedBitmap(LPRDATA rdPtr, std::function<void(LPSURFACE)> callback) {
	// decl
	LPSURFACE pDisplay = rdPtr->src;

	std::unique_ptr<cSurface> pOffset = nullptr;
	LPSURFACE pTransform = nullptr;
	LPSURFACE pTransformBitmap = nullptr;

	bool blitResult = false;

	// get size
	auto width = GetCurrentWidth(rdPtr);
	auto height = GetCurrentHeight(rdPtr);

	rdPtr->src->GetSizeOfRotatedRect(&width, &height, (float)GetAngle(rdPtr));

	if (rdPtr->offset.XOffset != 0 || rdPtr->offset.YOffset != 0) {
		pOffset.reset(GetSurface(rdPtr, rdPtr->src->GetWidth(), rdPtr->src->GetHeight()));
		OffsetHWA(rdPtr->src, pOffset.get(), rdPtr->offset);

		pDisplay = pOffset.get();
		
#ifdef _DEBUG
		_SavetoClipBoard(pOffset.get(), false);
#endif
	}

	pTransform = GetSurface(rdPtr, width, height);

	pTransformBitmap = CreateSurface(rdPtr->src->GetDepth(), max(width, rdPtr->src->GetWidth())
		, max(height, rdPtr->src->GetHeight()));

	DWORD flags = 0;

	if (rdPtr->stretchQuality) {
		flags |= BLTF_ANTIA;
	}

	POINT hotSpot = { 0,0 };

	blitResult = pDisplay->BlitEx(*pTransform
		, rdPtr->hotSpot.x * abs(rdPtr->zoomScale.XScale), rdPtr->hotSpot.y * abs(rdPtr->zoomScale.YScale)
		, abs(rdPtr->zoomScale.XScale), abs(rdPtr->zoomScale.YScale), 0, 0
		, rdPtr->src->GetWidth(), rdPtr->src->GetHeight(), &rdPtr->hotSpot, (float)rdPtr->angle
		, (rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) ? BMODE_TRANSP : BMODE_OPAQUE
		, BlitOp(rdPtr->rs.rsEffect & EFFECT_MASK)
		, rdPtr->rs.rsEffectParam, flags);

#ifdef _DEBUG	
	if (pOffset.get() != nullptr) {
		auto blitExTest = GetSurface(rdPtr, width, height);
		blitResult = pDisplay->BlitEx(*blitExTest
			, rdPtr->hotSpot.x * abs(rdPtr->zoomScale.XScale), rdPtr->hotSpot.y * abs(rdPtr->zoomScale.YScale)
			, abs(rdPtr->zoomScale.XScale), abs(rdPtr->zoomScale.YScale), 0, 0
			, rdPtr->src->GetWidth(), rdPtr->src->GetHeight(), &rdPtr->hotSpot, (float)rdPtr->angle
			, (rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) ? BMODE_TRANSP : BMODE_OPAQUE
			, BlitOp(rdPtr->rs.rsEffect & EFFECT_MASK)
			, rdPtr->rs.rsEffectParam, flags);

		_SavetoClipBoard(pDisplay, false);
		_SavetoClipBoard(blitExTest, false);		

		delete blitExTest;
	}
#endif

	int desX = int(rdPtr->hotSpot.x * (1 - abs(rdPtr->zoomScale.XScale)));
	int desY = int(rdPtr->hotSpot.y * (1 - abs(rdPtr->zoomScale.YScale)));

	blitResult = pTransform->Blit(*pTransformBitmap, desX, desY);

	callback(pTransformBitmap);

#ifdef _DEBUG	
	_SavetoClipBoard(rdPtr->src, false);
	_SavetoClipBoard(pTransform, false);
	_SavetoClipBoard(pTransformBitmap, false);
#endif

	// delete
	delete pTransform;
	delete pTransformBitmap;
}