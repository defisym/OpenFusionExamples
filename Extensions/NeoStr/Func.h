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

	//callRunTimeFunction(rdPtr, RFUNCTION_REDRAW, 0, 0);
}

inline void HandleUpate(LPRDATA rdPtr, RECT rc) {
	bool reRender = false;

	if (rdPtr->bFontChanged) {
		rdPtr->bFontChanged = false;

		delete rdPtr->pNeoStr;
		rdPtr->pNeoStr = new NeoStr(rdPtr->dwAlignFlags, rdPtr->dwColor, rdPtr->hFont);

#ifdef _USE_HWA
		LPSURFACE wSurf = WinGetSurface((int)rdPtr->rHo.hoAdRunHeader->rhIdEditWin);
		int sfDrv = wSurf->GetDriver();
		
		rdPtr->pNeoStr->SetHWA(ST_HWA_ROMTEXTURE, sfDrv);
#endif

		reRender = true;
	}

	if (rdPtr->bStrChanged) {
		rdPtr->bStrChanged = false;
		
		rdPtr->pNeoStr->SetAlign(rdPtr->dwAlignFlags);

		auto cPos = rdPtr->pNeoStr->CalculateRange(rdPtr->pStr->c_str(), &rc
			, rdPtr->nRowSpace, rdPtr->nColSpace);

		rdPtr->charPos = { cPos.x,cPos.y, cPos.maxWidth };

		reRender = true;
	}

	if (reRender) {
		////App Size
		//rhPtr->rhApp->m_hdr.gaCxWin;
		//rhPtr->rhApp->m_hdr.gaCyWin;

		////Frame Size
		//rhPtr->rhFrame->m_hdr.leWidth;
		//rhPtr->rhFrame->m_hdr.leHeight;

		LPRH rhPtr = rdPtr->rHo.hoAdRunHeader;

		rdPtr->pNeoStr->SetClip(true
			, min(rhPtr->rhApp->m_hdr.gaCxWin, rhPtr->rhFrame->m_hdr.leWidth)
			, min(rhPtr->rhApp->m_hdr.gaCyWin, rhPtr->rhFrame->m_hdr.leHeight));

		//rdPtr->pNeoStr->SetClip(false
		//	, 65535
		//	, 65535);
		
		rdPtr->pNeoStr->RenderPerChar(rdPtr->pStr->c_str(), &rc
			, rdPtr->nRowSpace, rdPtr->nColSpace);

		reRender = false;
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

#ifdef _USE_HWA
		int sfDrv = ps->GetDriver();

		neoStr.SetHWA(ST_HWA_ROMTEXTURE, sfDrv);
#endif

#ifdef _PATH
		neoStr.SetOutLine(edPtr->nOutLinePixel, edPtr->dwOutLineColor);
#endif
		neoStr.CalculateRange(&edPtr->pText, rc
			, edPtr->nRowSpace, edPtr->nColSpace);

		neoStr.SetClip(false, 65535, 65535);

		neoStr.RenderPerChar(&edPtr->pText, rc
			, edPtr->nRowSpace, edPtr->nColSpace);

		neoStr.DisplayPerChar(ps, &edPtr->pText, rc
			, edPtr->nRowSpace, edPtr->nColSpace
			, bm, bo, boParam, bAntiA);

		// Delete font
		if (hFont != NULL)
			DeleteObject(hFont);
	}
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
		HandleUpate(rdPtr, rc);

#ifdef _PATH
		rdPtr->pNeoStr->SetOutLine(rdPtr->nOutLinePixel, rdPtr->dwOutLineColor);
#endif

		rdPtr->pNeoStr->DisplayPerChar(ps, rdPtr->pStr->c_str(), &rc
			, rdPtr->nRowSpace, rdPtr->nColSpace
			, bm, bo, boParam, bAntiA);

		rdPtr->bStrChanged = false;
	}
}

inline CharPos UpdateLastCharPos(LPRDATA rdPtr) {
	RECT rc;

	rc.left = rdPtr->rHo.hoX;
	rc.top = rdPtr->rHo.hoY;
	rc.right = rc.left + rdPtr->rHo.hoImgWidth;
	rc.bottom = rc.top + rdPtr->rHo.hoImgHeight;

	HandleUpate(rdPtr, rc);

	return rdPtr->charPos;
}
