#pragma once

#include <functional>
#include <string_view>

inline void ReDisplay(LPRDATA rdPtr) {		
		rdPtr->rc.rcChanged = true;
		rdPtr->bStrChanged = true;
		
		rdPtr->rHo.hoImgXSpot = 0;
		rdPtr->rHo.hoImgYSpot = 0;

		rdPtr->rHo.hoImgWidth = rdPtr->swidth;
		rdPtr->rHo.hoImgHeight = rdPtr->sheight;

		callRunTimeFunction(rdPtr, RFUNCTION_REDRAW, 0, 0);
}

inline void Display(LPRDATA rdPtr) {
	//if (!rdPtr->bStrChanged) {
	//	return;
	//}

	LPRH rhPtr = rdPtr->rHo.hoAdRunHeader;
	LPSURFACE ps = WinGetSurface((int)rhPtr->rhIdEditWin);
	
	if (ps != nullptr && rdPtr->pStr != nullptr) {
		// On-screen coords
		int screenX = rdPtr->rHo.hoX - rhPtr->rhWindowX;
		int screenY = rdPtr->rHo.hoY - rhPtr->rhWindowY;
		
		RECT rc;

		rc.left = screenX;
		rc.top = screenY;
		rc.right = rc.left + rdPtr->rHo.hoImgWidth;
		rc.bottom = rc.top + rdPtr->rHo.hoImgHeight;

		// Ink effects
		BlitMode bm = (rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) ? BMODE_TRANSP : BMODE_OPAQUE;
		BOOL bAntiA = (rdPtr->rs.rsEffect & EFFECTFLAG_ANTIALIAS) ? TRUE : FALSE;
		BlitOp bo = (BlitOp)(rdPtr->rs.rsEffect & EFFECT_MASK);
		int boParam = rdPtr->rs.rsEffectParam;

		// Draw text
		if (rdPtr->bFontChanged) {
			rdPtr->bFontChanged = false;

			delete rdPtr->pNeoStr;
			rdPtr->pNeoStr = new NeoStr(rdPtr->dwAlignFlags, rdPtr->dwColor, rdPtr->hFont);
		}

		rdPtr->pNeoStr->SetOutLine(rdPtr->nOutLinePixel, rdPtr->dwOutLineColor);
		rdPtr->pNeoStr->SetClip(true);
		rdPtr->pNeoStr->DisplayPerChar(ps, rdPtr->pStr->c_str(), &rc
			, rdPtr->nRowSpace, rdPtr->nColSpace
			, bm, bo, boParam, bAntiA);

		rdPtr->bStrChanged = false;
	}
}

inline void Display(mv _far* mV, fpObjInfo oiPtr, fpLevObj loPtr, LPEDATA edPtr, RECT FAR* rc) {
	LPSURFACE ps = WinGetSurface((int)mV->mvIdEditWin);
	
	if (ps != NULL) {	// Do the following if this surface exists
		// Create font
		HFONT hFont = CreateFontIndirect(&edPtr->logFont);

		// Ink effects
		BOOL bTransp = ((oiPtr->oiHdr.oiInkEffect & EFFECTFLAG_TRANSPARENT) != 0);
		BlitMode bm = (bTransp) ? BMODE_TRANSP : BMODE_OPAQUE;
		BOOL bAntiA = (oiPtr->oiHdr.oiInkEffect & EFFECTFLAG_ANTIALIAS) ? TRUE : FALSE;
		BlitOp bo = (BlitOp)(oiPtr->oiHdr.oiInkEffect & EFFECT_MASK);
		LPARAM boParam = oiPtr->oiHdr.oiInkEffectParam;

		//MSGBOX(L"L: "+_itos(rc->left)+ L"T: " + _itos(rc->top), L"RECT");

		// Draw text
		NeoStr neoStr(edPtr->dwAlignFlags, edPtr->dwColor, hFont);
		neoStr.SetOutLine(edPtr->nOutLinePixel, edPtr->dwOutLineColor);
		neoStr.SetClip(false);
		neoStr.DisplayPerChar(ps, &edPtr->pText, rc
			, edPtr->nRowSpace, edPtr->nColSpace
			, bm, bo, boParam, bAntiA);

		// Delete font
		if (hFont != NULL)
			DeleteObject(hFont);
	}
}

inline auto UpdateLastCharPos(LPRDATA rdPtr) {
	RECT rc;

	rc.left = rdPtr->rHo.hoX;
	rc.top = rdPtr->rHo.hoY;
	rc.right = rc.left + rdPtr->rHo.hoImgWidth;
	rc.bottom = rc.top + rdPtr->rHo.hoImgHeight;

	if (rdPtr->bFontChanged) {
		rdPtr->bFontChanged = false;
		
		delete rdPtr->pNeoStr;
		rdPtr->pNeoStr = new NeoStr(rdPtr->dwAlignFlags, rdPtr->dwColor, rdPtr->hFont);
	}

	rdPtr->pNeoStr->SetOutLine(rdPtr->nOutLinePixel, rdPtr->dwOutLineColor);
	rdPtr->pNeoStr->SetClip(false);
	return rdPtr->pNeoStr->DisplayPerChar(nullptr, rdPtr->pStr->c_str(), &rc
		, rdPtr->nRowSpace, rdPtr->nColSpace);
}
