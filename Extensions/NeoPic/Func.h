#pragma once

inline void ReDisplay(LPRDATA rdPtr) {
	if (!rdPtr->IsLib) {
		//callRunTimeFunction(rdPtr, RFUNCTION_REDRAW, 0, 0);
		rdPtr->rc.rcChanged = true;

		rdPtr->rHo.hoImgXSpot = rdPtr->HotSpot.x;
		rdPtr->rHo.hoImgYSpot = rdPtr->HotSpot.y;

		rdPtr->rHo.hoImgWidth = rdPtr->img->GetWidth();
		rdPtr->rHo.hoImgHeight = rdPtr->img->GetHeight();

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
		UpdateHotSpot(rdPtr, 0, (int)(rdPtr->img->GetHeight() / 2));
		break;
	case HotSpotPos::LB:
		UpdateHotSpot(rdPtr, 0, rdPtr->img->GetHeight());
		break;
	case HotSpotPos::MT:
		UpdateHotSpot(rdPtr, (int)(rdPtr->img->GetWidth() / 2), 0);
		break;
	case HotSpotPos::MM:
		UpdateHotSpot(rdPtr, (int)(rdPtr->img->GetWidth() / 2), (int)(rdPtr->img->GetHeight() / 2));
		break;
	case HotSpotPos::MB:
		UpdateHotSpot(rdPtr, (int)(rdPtr->img->GetWidth() / 2), Y);
		break;
	case HotSpotPos::RT:
		UpdateHotSpot(rdPtr, rdPtr->img->GetWidth(), 0);
		break;
	case HotSpotPos::RM:
		UpdateHotSpot(rdPtr, rdPtr->img->GetWidth(), (int)(rdPtr->img->GetHeight() / 2));
		break;
	case HotSpotPos::RB:
		UpdateHotSpot(rdPtr, rdPtr->img->GetWidth(), rdPtr->img->GetHeight());
		break;
	case HotSpotPos::CUSTOM:
		UpdateHotSpot(rdPtr, X, Y);
		break;
	}

	ReDisplay(rdPtr);
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

		//Proto
		LPSURFACE proto = nullptr;
		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);

		//MGR
		CImageFilterMgr* pImgMgr = rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvImgFilterMgr;
		CImageFilter    pFilter(pImgMgr);

		if (NoStretch) {
			ImportImageFromInputFile(pImgMgr, &MemFile, Src, 0, 0);
		}
		else {
			cSurface img;

			if(ImportImageFromInputFile(pImgMgr, &MemFile, &img, 0, 0)){
				Src->Delete();
				Src->Create(width, height, proto);

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
		_LoadFromFile(rdPtr->img, FileName, Key, rdPtr, -1, -1, true, rdPtr->StretchQuality);
		
		if (rdPtr->img->IsValid()) {
			if (rdPtr->FromLib) {
				rdPtr->src = new cSurface;
			}

			rdPtr->FromLib = false;

			rdPtr->src->Delete();
			rdPtr->src->Clone(*rdPtr->img);

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
		rdPtr->img->Delete();
		rdPtr->img->Clone(*it->second);

		if (!rdPtr->FromLib) {
			delete rdPtr->src;
		}

		rdPtr->FromLib = true;
		rdPtr->src = it->second;

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