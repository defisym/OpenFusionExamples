#pragma once

inline void ReDisplay(LPRDATA rdPtr) {
	//callRunTimeFunction(rdPtr, RFUNCTION_REDRAW, 0, 0);
	rdPtr->rc.rcChanged = true;
	rdPtr->rHo.hoImgWidth = rdPtr->img->GetWidth();
	rdPtr->rHo.hoImgHeight = rdPtr->img->GetHeight();
	return;
}

#define ReDisplay() ReDisplay(rdPtr)

inline void LoadFromFile(LPRDATA rdPtr, LPCWSTR FileName) {
	if (rdPtr->IsLib) {
		LPSURFACE img = new cSurface;
		_LoadFromFile(img, FileName, rdPtr, -1, -1, true, rdPtr->StretchQuality);
		rdPtr->Lib->emplace(FileName, img);
	}
	else {
		_LoadFromFile(rdPtr->img, FileName, rdPtr, -1, -1, true, rdPtr->StretchQuality);
	}
}

inline void LoadFromLib(LPRDATA rdPtr, LPRO object, LPCWSTR FileName) {
	LPRDATA obj = (LPRDATA)object;

	if (!obj || obj->rHo.hoIdentifier != IDENTIFIER) {
		return;
	}

	if (!obj->IsLib) {
		return;
	}

	auto& it = obj->Lib->find(FileName);
	if (it == obj->Lib->end()) {
		LoadFromFile(obj, FileName);
	}

	it = obj->Lib->find(FileName);
	if (it == obj->Lib->end()) {
		return;
	}

	if(!rdPtr->IsLib){
		rdPtr->img->Delete();
		rdPtr->img->Clone(*it->second);
	}
	else {
		auto& thisit = rdPtr->Lib->find(FileName);
		if (thisit == rdPtr->Lib->end()) {
			rdPtr->Lib->emplace(it->first, it->second);
		}
	}

	ReDisplay();
}

inline void LoadFromLib(LPRDATA rdPtr, int Fixed, LPCWSTR FileName) {
	return LoadFromLib(rdPtr, LproFromFixed(rdPtr, Fixed), FileName);
}

inline void DeleteLib(SurfaceLib* pData) {
	if (pData != NULL) {
		for (auto& it : *pData) {
			delete it.second;
		}

		delete pData;
	}
}