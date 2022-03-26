#pragma once

inline void FindPath(LPRDATA rdPtr, Coord start, Coord destination, bool diagonal, bool forceFind, bool useRealCoord, const wstring& saveName) {
	if (useRealCoord) {
		start = rdPtr->pFTW->GetGirdCoord(start);
		destination = rdPtr->pFTW->GetGirdCoord(destination);
	}

	if (!forceFind) {
		rdPtr->pFTW->Find(start, destination, diagonal);
	}
	else {
		rdPtr->pFTW->ForceFind(start, destination, diagonal);
	}

	if (saveName != L"") {
		rdPtr->pFTW->SaveLastPath(saveName);
	}
}