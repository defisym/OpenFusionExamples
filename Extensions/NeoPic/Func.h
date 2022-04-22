#pragma once

inline void UpdateHotSpot(LPRDATA rdPtr, HotSpotPos Type, int X = 0, int Y = 0);

inline void UpdateImg(LPRDATA rdPtr, bool ForceLowQuality = false, bool ForceUpdate = false);

inline void GetTransformedSize(int& width, int& height, ZoomScale Scale = { 1.0,1.0 }, int Angle = 0, ATArray AT = { 1,0,0,1 });

//-----------------------------

inline void AddBackdrop(LPRDATA rdPtr, cSurface* pSf, int x, int y, DWORD dwInkEffect, DWORD dwInkEffectParam, int nObstacleType, int nLayer) {
	rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvAddBackdrop(pSf, x, y, dwInkEffect, dwInkEffectParam, nObstacleType, nLayer);
}

inline void ConvertHWA(LPRDATA rdPtr) {
	LPSURFACE srchwa = ConvertHWATexture(rdPtr, rdPtr->src);
	LPSURFACE imghwa = ConvertHWATarget(rdPtr, rdPtr->img);

	if (rdPtr->src != srchwa) {
		if (!rdPtr->FromLib) {
			delete rdPtr->src;
		}
		rdPtr->FromLib = false;

		rdPtr->src = srchwa;
	}

	if (rdPtr->img != imghwa) {
		delete rdPtr->img;
		rdPtr->img = imghwa;
	}
}

inline void ReDisplay(LPRDATA rdPtr) {
	if (!rdPtr->IsLib) {
		//callRunTimeFunction(rdPtr, RFUNCTION_REDRAW, 0, 0);
		rdPtr->rc.rcChanged = true;

		rdPtr->rHo.hoImgXSpot = rdPtr->HotSpot.x;
		rdPtr->rHo.hoImgYSpot = rdPtr->HotSpot.y;

		rdPtr->rHo.hoImgWidth = rdPtr->src->GetWidth();
		rdPtr->rHo.hoImgHeight = rdPtr->src->GetHeight();

		rdPtr->Changed = true;

		FreeColMask(rdPtr->pColMask);		
	}
}

inline void NewImg(LPRDATA rdPtr) {
	delete rdPtr->img;
	rdPtr->img = new cSurface;
	rdPtr->img->Clone(*rdPtr->src);
}

//Set default values
inline void NewPic(LPRDATA rdPtr){
	rdPtr->HotSpot = { 0,0 };
	UpdateHotSpot(rdPtr, rdPtr->DefaultHotSpot);
	
	rdPtr->ZoomScale = { 1.0,1.0 };
	rdPtr->Angle = 0;

	rdPtr->Offset = { 0,0,false };	
	rdPtr->AT = { 1,0,0,1 };

	rdPtr->ImgHotSpot = rdPtr->HotSpot;
	rdPtr->ImgZoomScale = rdPtr->ZoomScale;
	rdPtr->ImgAngle = rdPtr->Angle;

	rdPtr->ImgOffset = rdPtr->Offset;
	rdPtr->ImgAT = rdPtr->AT;

	NewImg(rdPtr);

	ReDisplay(rdPtr);
}

inline void NewPic(LPRDATA rdPtr, LPRDATA Copy) {
	rdPtr->HotSpot = Copy->HotSpot;
	rdPtr->ZoomScale = Copy->ZoomScale;
	rdPtr->Angle = Copy->Angle;

	rdPtr->Offset = Copy->Offset;

	rdPtr->ImgHotSpot = rdPtr->HotSpot;
	rdPtr->ImgZoomScale = rdPtr->ZoomScale;
	rdPtr->ImgAngle = rdPtr->Angle;

	rdPtr->ImgOffset = rdPtr->Offset;

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
	rdPtr->HotSpot.x = X;
	rdPtr->HotSpot.y = Y;
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

	Stretch(Src, Des, rdPtr->StretchQuality);

	if (ReverseX) {
		Des->ReverseX();
	}
	if (ReverseY) {
		Des->ReverseY();
	}
}

inline void Zoom(LPRDATA rdPtr, float XScale, float YScale, bool UpdateCur = false) {
	if (rdPtr->ZoomScale == ZoomScale{ XScale, YScale }) {
		return;
	}

	rdPtr->ZoomScale = { XScale ,YScale };

	ReDisplay(rdPtr);
}

inline void DoRotate(LPRDATA rdPtr, LPSURFACE Src, POINT SrcHotSpot, LPSURFACE& Des, POINT& DesHotSpot, int Angle) {
	DesHotSpot = SrcHotSpot;
	RotatePoint(Angle, (int*)&DesHotSpot.x, (int*)&DesHotSpot.y, Src->GetWidth(), Src->GetHeight());

	delete Des;
	Des = CreateSurface(24, 4, 4);

	Src->CreateRotatedSurface(*Des, Angle, rdPtr->StretchQuality);
	//Src->CreateRotatedSurface(*rdPtr->img, Angle, rdPtr->StretchQuality, DARK_GREEN);
}

inline void Rotate(LPRDATA rdPtr, int Angle, bool UpdateCur = false) {
	if (rdPtr->Angle == Angle) {
		return;
	}

	rdPtr->Angle = Angle;

	ReDisplay(rdPtr);
}

inline LPSURFACE Offset(LPSURFACE Src, OffsetCoef O) {
	return Offset(Src, O.XOffset, O.YOffset, O.Wrap);
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
		rdPtr->Offset != rdPtr->ImgOffset||
		rdPtr->AT != rdPtr->ImgAT ||
		rdPtr->ZoomScale != rdPtr->ImgZoomScale ||
		rdPtr->Angle != rdPtr->ImgAngle) {
		//Update Coef
		rdPtr->ImgOffset = rdPtr->Offset;
		rdPtr->ImgAT = rdPtr->AT;
		rdPtr->ImgZoomScale = rdPtr->ZoomScale;
		rdPtr->ImgAngle = rdPtr->Angle;

		//Fast
		bool OldQuality;

		if (ForceLowQuality) {
			OldQuality = rdPtr->StretchQuality;
			rdPtr->StretchQuality = false;
		}

		//Delete old
		delete rdPtr->img;

		//Temp
		LPSURFACE Temp = nullptr;

		//AffineTrans(rdPtr->AT)

		//Offset
		rdPtr->img = Offset(rdPtr->src, rdPtr->ImgOffset);

		//NewImg(rdPtr);

		//Zoom
		DoZoom(rdPtr, rdPtr->img, rdPtr->HotSpot, Temp, rdPtr->ImgHotSpot, rdPtr->ZoomScale.XScale, rdPtr->ZoomScale.YScale);

		//Rotate
		DoRotate(rdPtr, Temp, rdPtr->ImgHotSpot, rdPtr->img, rdPtr->ImgHotSpot, rdPtr->Angle);

		//Delete temp
		delete Temp;

		//Fast
		if (ForceLowQuality) {			 
			rdPtr->StretchQuality = OldQuality;
		}
	}
}

inline void _LoadFromFile(LPSURFACE Src, LPCTSTR FilePath, LPCTSTR Key, LPRDATA rdPtr, int width, int height, bool NoStretch, bool HighQuality){
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
			ImportImageFromInputFile(pImgMgr, &MemFile, Src, 0, 0);
		}
		else {
			cSurface img;

			if(ImportImageFromInputFile(pImgMgr, &MemFile, &img, 0, 0)){
				delete Src;
				Src = CreateSurface(24, width, height);

				Stretch(&img, Src, HighQuality);
			}
		}	
	}
}

inline void LoadFromFile(LPRDATA rdPtr, LPCWSTR FileName, LPCTSTR Key = _T("")) {
	if (rdPtr->IsLib) {
		if (rdPtr->Lib->find(FileName) != rdPtr->Lib->end()) {
			return;
		}

		LPSURFACE img = new cSurface;
		_LoadFromFile(img, FileName, Key, rdPtr, -1, -1, true, rdPtr->StretchQuality);

		if (img->IsValid()) {
			(*rdPtr->Lib)[FileName] = img;
		}
		else {
			delete img;
		}
	}
	else {
		if (rdPtr->FromLib) {
			rdPtr->src = new cSurface;
		}

		rdPtr->FromLib = false;

		_LoadFromFile(rdPtr->src, FileName, Key, rdPtr, -1, -1, true, rdPtr->StretchQuality);
		
		if (rdPtr->src->IsValid()) {
			NewPic(rdPtr);

			*rdPtr->FileName = FileName;
			*rdPtr->Key = Key;
		}
	}	
}

inline void LoadFromLib(LPRDATA rdPtr, LPRO object, LPCWSTR FileName, LPCTSTR Key = _T("")) {
	LPRDATA obj = (LPRDATA)object;

	if (!obj || obj->rHo.hoIdentifier != IDENTIFIER) {
		return;
	}

	if (!obj->IsLib) {
		return;
	}

	auto it = obj->Lib->find(FileName);
	
	if (it == obj->Lib->end()) {
		LoadFromFile(obj, FileName, Key);
	}

	it = obj->Lib->find(FileName);
	if (it == obj->Lib->end()) {
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
	
	if(!rdPtr->IsLib){
		if (!rdPtr->FromLib) {
			delete rdPtr->src;
		}

		rdPtr->FromLib = true;
		rdPtr->src = it->second;

		NewPic(rdPtr);
	}
	else {
		auto thisit = rdPtr->Lib->find(FileName);
		if (thisit == rdPtr->Lib->end()) {
			rdPtr->Lib->emplace(it->first, it->second);
		}
	}

	*rdPtr->FileName = FileName;
	*rdPtr->Key = Key;
}

inline void LoadFromLib(LPRDATA rdPtr, int Fixed, LPCWSTR FileName, LPCTSTR Key = _T("")) {
	return LoadFromLib(rdPtr, LproFromFixed(rdPtr, Fixed), FileName, Key);
}

inline void LoadFromDisplay(LPRDATA rdPtr, LPRO object, bool CopyCoef = false) {
	LPRDATA obj = (LPRDATA)object;

	if (!obj || obj->rHo.hoIdentifier != IDENTIFIER) {
		return;
	}

	if (obj->IsLib) {
		return;
	}

	if (!obj->src->IsValid()) {
		return;
	}

	if (obj->FromLib) {
		if (!rdPtr->FromLib) {
			delete rdPtr->src;
		}

		rdPtr->FromLib = true;
		rdPtr->src = obj->src;
	}
	else {
		if (rdPtr->FromLib) {
			rdPtr->src = nullptr;
		}

		rdPtr->FromLib = false;
		
		delete rdPtr->src;
		rdPtr->src = new cSurface;
		rdPtr->src->Clone(*obj->src);
	}

	*rdPtr->FileName = *obj->FileName;
	*rdPtr->Key = *obj->Key;

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

using LoadLibCallBack = std::function<void(SurfaceLib&)>;

inline void PreloadLibFromVec(LPRDATA rdPtr, const std::vector<std::wstring> PreloadList, std::wstring BasePath, std::wstring Key, LoadLibCallBack callBack) {
	if (PreloadList.empty()) {
		return;
	}
	
	std::vector<std::wstring> tempList;
	tempList.reserve(PreloadList.size());
	
	std::vector<std::wstring> fileList;
	GetFileList(&fileList, BasePath);

	for (auto& it : PreloadList) {
		for (auto fileIt = fileList.begin(); fileIt != fileList.end();) {
			fileIt = std::find_if(fileIt+1, fileList.end(), [&](std::wstring& file) {
				auto pos = file.find_last_of(L"\\")+1;
				auto fileName = file.substr(pos, file.size() - pos);

				if (fileName == it) {
					return true;
				}

				return false;
				});
			
			if (fileIt != fileList.end()) {
				tempList.emplace_back(*fileIt);
			}
		}
	}

	SurfaceLib tempLib;

	for (auto& it : tempList) {
		if (min(rdPtr->memoryLimit + CLEAR_MEMRANGE, MAX_MEMORYLIMIT) <= (GetProcessMemoryUsage() >> 20)) {
			break;
		}

		LPSURFACE img = new cSurface;
		_LoadFromFile(img, it.c_str(), Key.c_str(), rdPtr, -1, -1, true, rdPtr->StretchQuality);

		if (img->IsValid()) {
			tempLib.emplace(it, img);
		}
		else {
			delete img;
		}
	}

	if (!tempLib.empty()) {
		callBack(tempLib);
	}
}

inline void PreloadLibFromPath(LPRDATA rdPtr, std::wstring BasePath, std::wstring Key, LoadLibCallBack callBack) {	
	std::vector<std::wstring> fileList;
	GetFileList(&fileList, BasePath);

	SurfaceLib tempLib;

	for (auto& it : fileList) {
		if (min(rdPtr->memoryLimit + CLEAR_MEMRANGE, MAX_MEMORYLIMIT) <= (GetProcessMemoryUsage() >> 20)) {
			break;
		}

		LPSURFACE img = new cSurface;
		_LoadFromFile(img, it.c_str(), Key.c_str(), rdPtr, -1, -1, true, rdPtr->StretchQuality);

		if (img->IsValid()) {
			tempLib.emplace(it, img);
		}
		else {
			delete img;
		}
	}

	if (!tempLib.empty()) {
		callBack(tempLib);
	}
}