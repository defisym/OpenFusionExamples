#pragma once

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

		//int width = rdPtr->src->GetWidth();
		//int height = rdPtr->src->GetHeight();

		//width *= abs(rdPtr->ZoomScale.XScale);
		//height *= abs(rdPtr->ZoomScale.YScale);

		//rdPtr->src->GetSizeOfRotatedRect(&width, &height, rdPtr->Angle);

		//rdPtr->rHo.hoImgWidth = width;
		//rdPtr->rHo.hoImgHeight = height;

		rdPtr->rHo.hoImgWidth = rdPtr->src->GetWidth();
		rdPtr->rHo.hoImgHeight = rdPtr->src->GetHeight();

		FreeColMask(rdPtr->pColMask);		
	}
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

inline void DoZoom(LPRDATA rdPtr, LPSURFACE Src, POINT HotSpot,float XScale, float YScale) {
	XScale = abs(XScale);
	YScale = abs(YScale);

	rdPtr->ImgHotSpot = HotSpot;

	rdPtr->ImgHotSpot.x = (int)(rdPtr->ImgHotSpot.x * XScale);
	rdPtr->ImgHotSpot.y = (int)(rdPtr->ImgHotSpot.y * YScale);

	int width = (int)(Src->GetWidth() * XScale);
	int height = (int)(Src->GetHeight() * YScale);

	delete rdPtr->img;
	rdPtr->img = CreateSurface(24, width, height);

	Stretch(Src, rdPtr->img, rdPtr->StretchQuality);

	//if (rdPtr->HWA) {
	//	ConvertHWA(rdPtr);

	//	POINT Center = { 0,0 };

	//	rdPtr->src->BlitEx(*rdPtr->img, 0, 0,
	//		XScale, YScale, 0, 0,
	//		rdPtr->src->GetWidth(), rdPtr->src->GetHeight(), &Center, 0,
	//		(rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) ? BMODE_TRANSP : BMODE_OPAQUE,
	//		BlitOp(rdPtr->rs.rsEffect & EFFECT_MASK),
	//		rdPtr->rs.rsEffectParam, BLTF_ANTIA);
	//}
	//else{
	//	Stretch(rdPtr->src, rdPtr->img, rdPtr->StretchQuality);
	//}

	if (rdPtr->ZoomScale.XScale < 0.0) {
		rdPtr->img->ReverseX();
	}
	if (rdPtr->ZoomScale.YScale < 0.0) {
		rdPtr->img->ReverseY();
	}
}

inline void Zoom(LPRDATA rdPtr, float XScale, float YScale, bool UpdateCur = false) {
	if (rdPtr->ZoomScale == ZoomScale{ XScale, YScale }) {
		return;
	}

	rdPtr->ZoomScale = { XScale ,YScale };

	if (UpdateCur) {
		DoZoom(rdPtr, rdPtr->src, rdPtr->HotSpot,XScale, YScale);
	}
	else {
		return;
	}
}

inline void DoRotate(LPRDATA rdPtr, LPSURFACE Src, POINT HotSpot,int Angle) {
	rdPtr->ImgHotSpot = HotSpot;
	RotatePoint(Angle, (int*)&rdPtr->ImgHotSpot.x, (int*)&rdPtr->ImgHotSpot.y, Src->GetWidth(), Src->GetHeight());

	//int width = rdPtr->src->GetWidth();
	//int height = rdPtr->src->GetHeight();

	//rdPtr->src->GetSizeOfRotatedRect(&width, &height, (float)Angle);

	delete rdPtr->img;
	//rdPtr->img = CreateSurface(24, width, height);
	rdPtr->img = CreateSurface(24, 4, 4);

	Src->CreateRotatedSurface(*rdPtr->img, Angle, rdPtr->StretchQuality);
	//rdPtr->src->CreateRotatedSurface(*rdPtr->img, Angle, rdPtr->StretchQuality, DARK_GREEN);

	//if (rdPtr->HWA) {
	//	ConvertHWA(rdPtr);
	//	
	//	POINT Center = { 0,0 };			

	//	//RotatePoint(Angle, (int*)&rdPtr->HotSpot.x, (int*)&rdPtr->HotSpot.y);

	//	rdPtr->src->BlitEx(*rdPtr->img, 0, 0,
	//		1.0, 1.0, 0, 0,
	//		rdPtr->src->GetWidth(), rdPtr->src->GetHeight(), &Center, (float)Angle,
	//		(rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) ? BMODE_TRANSP : BMODE_OPAQUE,
	//		BlitOp(rdPtr->rs.rsEffect & EFFECT_MASK),
	//		rdPtr->rs.rsEffectParam, BLTF_ANTIA);
	//}
	//else {
	//	RotatePoint(Angle, (int*)&rdPtr->HotSpot.x, (int*)&rdPtr->HotSpot.y, rdPtr->src->GetWidth(), rdPtr->src->GetHeight());

	//	rdPtr->src->Rotate(*rdPtr->img, Angle, rdPtr->StretchQuality);
	//	//rdPtr->src->Rotate(*rdPtr->img, Angle, rdPtr->StretchQuality, DARK_GREEN);
	//}
}

inline void Rotate(LPRDATA rdPtr, int Angle, bool UpdateCur = false) {
	if (rdPtr->Angle == Angle) {
		return;
	}

	rdPtr->Angle = Angle;

	if (UpdateCur) {
		DoRotate(rdPtr, rdPtr->src, rdPtr->HotSpot,Angle);
	}
	else {
		return;
	}
}

inline void UpdateImg(LPRDATA rdPtr, bool ForceUpdate = false) {
	if (ForceUpdate||rdPtr->ZoomScale != rdPtr->ImgZoomScale) {
		rdPtr->ImgZoomScale = rdPtr->ZoomScale;
		DoZoom(rdPtr, rdPtr->src, rdPtr->HotSpot, rdPtr->ZoomScale.XScale, rdPtr->ZoomScale.YScale);
	}

	if (ForceUpdate || rdPtr->Angle != rdPtr->ImgAngle) {
		rdPtr->ImgAngle = rdPtr->Angle;

		LPSURFACE Rot = new cSurface;

		Rot->Clone(*rdPtr->img);
		DoRotate(rdPtr, Rot, rdPtr->ImgHotSpot, rdPtr->Angle);

		delete Rot;
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
			delete rdPtr->img;
			rdPtr->img = new cSurface;
			rdPtr->img->Clone(*rdPtr->src);

			ReDisplay(rdPtr);
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

		delete rdPtr->img;
		rdPtr->img = new cSurface;
		rdPtr->img->Clone(*rdPtr->src);

		ReDisplay(rdPtr);
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