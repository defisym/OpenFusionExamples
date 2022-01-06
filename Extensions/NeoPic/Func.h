#pragma once

inline void UpdateImg(LPRDATA rdPtr, bool ForceUpdate = false);

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

		rdPtr->rHo.hoRect;

		FreeColMask(rdPtr->pColMask);		
	}
}

inline void NewImg(LPRDATA rdPtr) {
	delete rdPtr->img;
	rdPtr->img = new cSurface;
	rdPtr->img->Clone(*rdPtr->src);
}

inline void NewPic(LPRDATA rdPtr){
	rdPtr->HotSpot = { 0,0 };
	rdPtr->ZoomScale = { 1.0,1.0 };
	rdPtr->Angle = 0;

	rdPtr->Offset = { 0,0,false };	

	rdPtr->ImgHotSpot = rdPtr->HotSpot;
	rdPtr->ImgZoomScale = rdPtr->ZoomScale;
	rdPtr->ImgAngle = rdPtr->Angle;

	rdPtr->ImgOffset = rdPtr->Offset;

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

inline void UpdateHotSpot(LPRDATA rdPtr, HotSpotPos Type, int X, int Y) {
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
}

inline void UpdateImg(LPRDATA rdPtr, bool ForceUpdate) {
	if (ForceUpdate ||
		rdPtr->ZoomScale != rdPtr->ImgZoomScale ||
		rdPtr->Angle != rdPtr->ImgAngle ||
		rdPtr->Offset != rdPtr->ImgOffset) {
		//Update Coef
		rdPtr->ImgZoomScale = rdPtr->ZoomScale;
		rdPtr->ImgAngle = rdPtr->Angle;
		rdPtr->ImgOffset = rdPtr->Offset;

		//Delete old
		delete rdPtr->img;

		//Temp
		LPSURFACE Temp = nullptr;

		//Offset
		rdPtr->img = Offset(rdPtr->src, rdPtr->ImgOffset);

		//NewImg(rdPtr);

		//Zoom
		DoZoom(rdPtr, rdPtr->img, rdPtr->HotSpot, Temp, rdPtr->ImgHotSpot, rdPtr->ZoomScale.XScale, rdPtr->ZoomScale.YScale);

		//Rotate
		DoRotate(rdPtr, Temp, rdPtr->ImgHotSpot, rdPtr->img, rdPtr->ImgHotSpot, rdPtr->Angle);

		//Delete temo
		delete Temp;
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
			rdPtr->Lib->emplace(FileName, img);
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

	auto& it = obj->Lib->find(FileName);
	if (it == obj->Lib->end()) {
		LoadFromFile(obj, FileName, Key);
	}

	it = obj->Lib->find(FileName);
	if (it == obj->Lib->end()) {
		return;
	}

	if(!rdPtr->IsLib){
		if (!rdPtr->FromLib) {
			delete rdPtr->src;
		}

		rdPtr->FromLib = true;
		rdPtr->src = it->second;

		NewPic(rdPtr);
	}
	else {
		auto& thisit = rdPtr->Lib->find(FileName);
		if (thisit == rdPtr->Lib->end()) {
			rdPtr->Lib->emplace(it->first, it->second);
		}
	}
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
	auto& it = pData->find(Item);
	if (it != pData->end()) {
		delete it->second;
		pData->erase(it);
	}
}