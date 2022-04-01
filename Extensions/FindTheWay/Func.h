#pragma once

inline void FindPath(LPRDATA rdPtr, Coord start, Coord destination, bool diagonal, bool checkDiagonalCorner, bool forceFind, bool useRealCoord, const wstring& saveName) {
	if (useRealCoord) {
		start = rdPtr->pFTW->GetGirdCoord(start);
		destination = rdPtr->pFTW->GetGirdCoord(destination);
	}

	if (!forceFind) {
		rdPtr->pFTW->Find(start, destination, diagonal, checkDiagonalCorner);
	}
	else {
		rdPtr->pFTW->ForceFind(start, destination, diagonal, checkDiagonalCorner);
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