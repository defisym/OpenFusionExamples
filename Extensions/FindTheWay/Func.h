#pragma once

inline void FindPath(LPRDATA rdPtr, Coord start, Coord destination, size_t ignoreFlag, bool diagonal, bool checkDiagonalCorner, bool forceFind, bool useRealCoord, const wstring& saveName) {
	if (useRealCoord) {
		start = rdPtr->pFTW->GetGridCoord(start);
		destination = rdPtr->pFTW->GetGridCoord(destination);
	}

	if (!forceFind) {
		rdPtr->pFTW->Find(start, destination, diagonal, checkDiagonalCorner, rdPtr->pAlly, rdPtr->pEnemy, rdPtr->pZoc, ignoreFlag);
	}
	else {
		rdPtr->pFTW->ForceFind(start, destination, diagonal, checkDiagonalCorner, rdPtr->pAlly, rdPtr->pEnemy, rdPtr->pZoc, ignoreFlag);
	}

	if (saveName != L"") {
		rdPtr->pFTW->SaveLastPath(saveName);
	}
}

inline void UpdateMapCallBackFunc(void* rdPtr) {
	LPRDATA pCast = (LPRDATA)rdPtr;
	callRunTimeFunction(pCast, RFUNCTION_GENERATEEVENT, ONMAPCHANGE, 0);
}

inline bool OverlapUnit(LPRDATA rdPtr,const Coord& c) {
	return std::find(rdPtr->pUnit->begin(), rdPtr->pUnit->end(), c) != rdPtr->pUnit->end();
}

inline bool LPROValid(LPRO object, unsigned long identifier = 0) {
	if (object == nullptr) {
		return false;
	}

	if (identifier) {
		return object->roHo.hoIdentifier == identifier;
	}

	return true;		// need not to check indntifier
}

inline bool SetMapBySurface(LPRDATA rdPtr, cSurface* pSf, size_t gridSize, size_t gridOffsetX, size_t gridOffsetY) {
	if (!pSf->IsValid()) {
		return FALSE;
	}

	bool blited = false;

	if (pSf->GetDepth() != 24) {
		// blit active to 24 bit depth
		LPSURFACE proto = nullptr;
		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);
		cSurface* sf = new cSurface;

		sf->Create(pSf->GetWidth(), pSf->GetHeight(), proto);
		pSf->Blit(*sf);

		pSf = sf;
		blited = true;
	}

	size_t picWidth = pSf->GetWidth();
	size_t picHeight = pSf->GetHeight();

	size_t width = FindTheWayClass::CalcMapWidth(pSf->GetWidth(), gridSize, rdPtr->isometric);
	size_t height = FindTheWayClass::CalcMapHeight(pSf->GetHeight(), gridSize, rdPtr->isometric);

	try {
		rdPtr->pFTW = new FindTheWayClass(width, height);
		rdPtr->pFTW->SetUpdateMapCallBack(UpdateMapCallBackFunc, rdPtr);
		rdPtr->pFTW->SetIsometric(rdPtr->isometric);
	}
	catch (Exception) {
		return FALSE;
	}

	rdPtr->pFTW->SetGridSize(gridSize, gridOffsetX, gridOffsetY);

	BYTE* buff = pSf->LockBuffer();
	if (!buff) {
		return 0;
	}

	int pitch = pSf->GetPitch();
	if (pitch < 0) {
		pitch *= -1;
		buff -= pitch * (picHeight - 1);
	}

	size_t size = pitch * picHeight;
	size_t byte = pSf->GetDepth() >> 3;

	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			auto [realX, realY] = rdPtr->pFTW->GetRealCoord(Coord{ x ,y });
			auto offset = realY * pitch + realX * byte;

			if (offset >= size) {
				continue;
			}

			auto costT = buff[offset + 2];		// R value
			auto costD = buff[offset + 1];		// G value

			//auto B = buff[offset];
			//auto G = buff[offset + 1];
			//auto R = buff[offset + 2];
			//auto U = buff[offset + 3];

			rdPtr->pFTW->SetMap(x, y, costT, MapType::TERRAIN);
			rdPtr->pFTW->SetMap(x, y, costD, MapType::DYNAMIC);
		}
	}

	pSf->UnlockBuffer(buff);

	if (blited) {
		delete pSf;
	}

	return TRUE;
}

template<typename T>
constexpr inline long ConvertToLong(T t) {
	return *((long*)&t);
}

template<typename T>
constexpr inline T ConvertToType(long l) {
	return *((T*)&l);
}