#pragma once

inline bool CheckCompatibility(LPRDATA rdPtr) {
	const auto ver = rdPtr->rHo.hoAdRunHeader->rhApp->m_miniHdr.gaPrdBuild;

	return ver < 295;
}

inline void FindPath(LPRDATA rdPtr, Coord start, Coord destination, size_t ignoreFlag, bool diagonal, bool checkDiagonalCorner, bool forceFind, bool useRealCoord, const std::wstring& saveName) {
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

	if (!saveName.empty()) {
		rdPtr->pFTW->SaveLastPath(saveName);
	}
}

inline void UpdateMapCallBackFunc(void* pData) {
	const auto rdPtr = (LPRDATA)pData;

	if (!CheckCompatibility(rdPtr)) {
#ifndef RUN_ONLY
		//MSGBOX(L"Map Update Callback is not supported in 295 or later");
#endif
		return;
	}

	CallEvent(ONMAPCHANGE);
}

inline bool OverlapUnit(LPRDATA rdPtr,const Coord& c) {
	return std::ranges::find(*rdPtr->pUnit, c) != rdPtr->pUnit->end();
}

// `gridSize` & `gridOffset` are used to find the proper pixel of picture
inline bool SetMapBySurface(LPRDATA rdPtr, cSurface* pSf,
	size_t gridSize, size_t gridOffsetX, size_t gridOffsetY) {
	if (!pSf->IsValid()) {
		return FALSE;
	}

	bool blited = false;

	if (pSf->GetDepth() != 24	// blit active to 24 bit depth
		|| IsHWA(pSf)) {		// blit HWA texture
		LPSURFACE proto = nullptr;
		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);
		auto* sf = new cSurface;

		sf->Create(pSf->GetWidth(), pSf->GetHeight(), proto);
		pSf->Blit(*sf);

		pSf = sf;
		blited = true;
	}

	const size_t picWidth = pSf->GetWidth();
	const size_t picHeight = pSf->GetHeight();

	const auto [width, height]
		= FindTheWayClass::CalcMapSize(pSf->GetWidth(), pSf->GetHeight(),
		gridSize, false);

	try {
		rdPtr->pFTW = new FindTheWayClass(width, height);
		rdPtr->pFTW->SetUpdateMapCallBack(UpdateMapCallBackFunc, rdPtr);
		rdPtr->pFTW->SetIsometric(false);
	}
	catch (Exception) {
		return FALSE;
	}

	rdPtr->pFTW->SetGridSize(gridSize, gridOffsetX, gridOffsetY);

	BYTE* buff = pSf->LockBuffer();
	if (!buff) {
		return FALSE;
	}

	int pitch = pSf->GetPitch();
	bool reverse = false;

	if (pitch < 0) {
		pitch *= -1;
		buff -= pitch * (picHeight - 1);

		reverse = true;
	}

	const size_t size = pitch * picHeight;
	const size_t byte = pSf->GetDepth() >> 3;

	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			auto [realX, realY] = rdPtr->pFTW->GetRealCoord({ x ,y });
			const auto offset = realY * pitch + realX * byte;

			if (offset >= size) {
				continue;
			}

			const auto costT = buff[offset + 2];		// R value
			const auto costD = buff[offset + 1];		// G valu

			//auto B = buff[offset];
			//auto G = buff[offset + 1];
			//auto R = buff[offset + 2];
			//auto U = buff[offset + 3];

			const auto revY = reverse
								  ? (height - 1) - y				// reverse y if pitch < 0
								  : y;

			rdPtr->pFTW->SetMap(x, revY, costT, MapType::TERRAIN);
			rdPtr->pFTW->SetMap(x, revY, costD, MapType::DYNAMIC);
		}
	}

	pSf->UnlockBuffer(buff);

	if (blited) {
		delete pSf;
	}

	return TRUE;
}
