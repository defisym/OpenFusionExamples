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

inline void HandleUpdate(LPRDATA rdPtr, RECT rc) {
	if (rdPtr->bFontChanged) {
		rdPtr->bFontChanged = false;
		rdPtr->bStrChanged = true;

		delete rdPtr->pNeoStr;
		rdPtr->pNeoStr = new NeoStr(rdPtr->dwAlignFlags, rdPtr->dwColor, rdPtr->hFont);

#ifdef _USE_HWA
		LPSURFACE wSurf = WinGetSurface((int)rdPtr->rHo.hoAdRunHeader->rhIdEditWin);
		int sfDrv = wSurf->GetDriver();

		rdPtr->pNeoStr->SetHWA(ST_HWA_ROMTEXTURE, sfDrv, PreMulAlpha(rdPtr));
#endif

		rdPtr->reRender = true;
	}

	if (rdPtr->bStrChanged) {
		rdPtr->bStrChanged = false;

		rdPtr->pNeoStr->SetAlign(rdPtr->dwAlignFlags, rdPtr->bVerticalAlignOffset);
		rdPtr->pNeoStr->SetSpace(rdPtr->nRowSpace, rdPtr->nColSpace);

		auto cPos = rdPtr->pNeoStr->CalculateRange(rdPtr->pStr->c_str(), &rc);

		rdPtr->charPos = { cPos.x,cPos.y, cPos.maxWidth,cPos.totalHeight };

		rdPtr->reRender = true;
	}

	if (rdPtr->bClip		// only clip mode needs to redraw
		&& (rdPtr->oldX != rc.left
			|| rdPtr->oldY != rc.top)) {
		rdPtr->oldX = rc.left;
		rdPtr->oldY = rc.top;

		rdPtr->reRender = true;
	}

	if (rdPtr->reRender) {
		////App Size
		//rhPtr->rhApp->m_hdr.gaCxWin;
		//rhPtr->rhApp->m_hdr.gaCyWin;

		////Frame Size
		//rhPtr->rhFrame->m_hdr.leWidth;
		//rhPtr->rhFrame->m_hdr.leHeight;

		LPRH rhPtr = rdPtr->rHo.hoAdRunHeader;

		rdPtr->pNeoStr->SetColor(rdPtr->dwColor);

		rdPtr->pNeoStr->SetClip(rdPtr->bClip
			, min(rhPtr->rhApp->m_hdr.gaCxWin, rhPtr->rhFrame->m_hdr.leWidth)
			, min(rhPtr->rhApp->m_hdr.gaCyWin, rhPtr->rhFrame->m_hdr.leHeight));
		rdPtr->pNeoStr->SetBorderOffset(5, 5);

		//rdPtr->pNeoStr->SetClip(false
		//	, 65535
		//	, 65535);

		rdPtr->pNeoStr->SetSmooth(
			Gdiplus::TextRenderingHint(rdPtr->textRenderingHint)
			, Gdiplus::SmoothingMode(rdPtr->smoothingMode - 1)
			, Gdiplus::PixelOffsetMode(rdPtr->pixelOffsetMode - 1));

		rdPtr->pNeoStr->RenderPerChar(rdPtr->pStr->c_str(), &rc);

		rdPtr->reRender = false;
	}
}

inline void Display(mv _far* mV, fpObjInfo oiPtr, fpLevObj loPtr, LPEDATA edPtr, RECT FAR* rc) {
	//MSGBOX(L"Editor Display");
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

		neoStr.SetHWA(ST_HWA_ROMTEXTURE, sfDrv, false);
#endif

#ifdef _PATH
		neoStr.SetOutLine(edPtr->nOutLinePixel, edPtr->dwOutLineColor);
#endif
		//MSGBOX(L"Editor Calc");
		neoStr.SetAlign(edPtr->dwAlignFlags, edPtr->bVerticalAlignOffset);
		neoStr.SetSpace(edPtr->nRowSpace, edPtr->nColSpace);

		neoStr.CalculateRange(&edPtr->pText, rc);

		neoStr.SetColor(edPtr->dwColor);

		neoStr.SetClip(false, 65535, 65535);
		neoStr.SetBorderOffset(5, 5);

		neoStr.SetSmooth(
			Gdiplus::TextRenderingHint(edPtr->textRenderingHint)
			, Gdiplus::SmoothingMode(edPtr->smoothingMode - 1)
			, Gdiplus::PixelOffsetMode(edPtr->pixelOffsetMode - 1));

		//MSGBOX(L"Editor Render");
		neoStr.RenderPerChar(&edPtr->pText, rc);

		//neoStr.SetHotSpot(edPtr->hotSpotPos, edPtr->hotSpotX, edPtr->hotSpotY);
		neoStr.SetHotSpot(HotSpotPos::LT, 0, 0);
		neoStr.SetScale(1.0, 1.0);
		neoStr.SetAngle(0);

		//MSGBOX(L"Editor Display PerChar");
		neoStr.DisplayPerChar(ps, &edPtr->pText, rc
			, bm, bo, boParam, bAntiA);

		// Delete font
		if (hFont != NULL)
			DeleteObject(hFont);
	}
}

inline void Display(LPRDATA rdPtr) {
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
		HandleUpdate(rdPtr, rc);

#ifdef _PATH
		rdPtr->pNeoStr->SetOutLine(rdPtr->nOutLinePixel, rdPtr->dwOutLineColor);
#endif

		rdPtr->pNeoStr->SetHotSpot(rdPtr->hotSpotPos, rdPtr->hotSpotX, rdPtr->hotSpotY);
		rdPtr->pNeoStr->SetScale(rdPtr->xScale, rdPtr->yScale);
		rdPtr->pNeoStr->SetAngle(rdPtr->angle);

		rdPtr->pNeoStr->DisplayPerChar(ps, rdPtr->pStr->c_str(), &rc
			, bm, bo, boParam, bAntiA);

		rdPtr->bStrChanged = false;
	}
}

inline CharPos UpdateLastCharPos(LPRDATA rdPtr) {
	LPRH rhPtr = rdPtr->rHo.hoAdRunHeader;

	// On-screen coords
	int screenX = rdPtr->rHo.hoX - rhPtr->rhWindowX;
	int screenY = rdPtr->rHo.hoY - rhPtr->rhWindowY;

	RECT rc;

	rc.left = screenX;
	rc.top = screenY;
	rc.right = rc.left + rdPtr->rHo.hoImgWidth;
	rc.bottom = rc.top + rdPtr->rHo.hoImgHeight;

	HandleUpdate(rdPtr, rc);

	return rdPtr->charPos;
}
