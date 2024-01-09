#pragma once

#include <functional>
#include <string_view>

constexpr auto ONITOIC = 0;

#define GIPP(p) (*(NeoStr::IConParamParser*)(p))

void WINAPI SetRunObjectFont(LPRDATA rdPtr, LOGFONT* pLf, RECT* pRc);

inline void NoClip(LPRDATA rdPtr) {
	if (rdPtr->hotSpotPos != HotSpotPos::LT && rdPtr->hotSpotPos != HotSpotPos::CUSTOM
		|| rdPtr->hotSpotPos == HotSpotPos::CUSTOM && (rdPtr->hotSpotX != 0 || rdPtr->hotSpotY != 0)
		|| rdPtr->xScale != 1.0 && rdPtr->yScale != 1.0
		|| rdPtr->angle != 0.0) {
		rdPtr->bClip = false;
	}
}

inline void ChangeScale(LPRDATA rdPtr) {
	UpdateHoImgInfo(rdPtr
		, rdPtr->swidth, rdPtr->sheight
		, rdPtr->xScale, rdPtr->yScale
		, rdPtr->hotSpotPos
		, rdPtr->hotSpotX, rdPtr->hotSpotY
		, rdPtr->angle);

	NoClip(rdPtr);
}

inline void ReDisplay(LPRDATA rdPtr) {
	rdPtr->rc.rcChanged = true;
	rdPtr->bStrChanged = true;

	ChangeScale(rdPtr);

	//callRunTimeFunction(rdPtr, RFUNCTION_REDRAW, 0, 0);
}

inline void HandleUpdate(LPRDATA rdPtr, RECT rc) {
#ifdef COUNT_GDI_OBJECT
	rdPtr->pData->objectCounter.UpdateObjectCount();
#endif

	if (rdPtr->bFontChanged) {
		rdPtr->bFontChanged = false;
		rdPtr->bStrChanged = true;

		delete rdPtr->pNeoStr;
		rdPtr->pNeoStr = new NeoStr(rdPtr->dwAlignFlags, rdPtr->dwColor
			, rdPtr->hFont
			, rdPtr->pData->pFontCache
			, rdPtr->pData->pCharSzCacheWithFont
			, rdPtr->pData->pRegexHandler
			, rdPtr->pData->pIConData
			, rdPtr->pData->pFontCollection
			, false);

		LPSURFACE wSurf = WinGetSurface((int)rdPtr->rHo.hoAdRunHeader->rhIdEditWin);
		int sfDrv = wSurf->GetDriver();

		rdPtr->pNeoStr->SetHWA(sfDrv, PreMulAlpha(rdPtr));

		rdPtr->reRender = true;
	}

	if (rdPtr->bStrChanged) {
		rdPtr->bStrChanged = false;

		rdPtr->pNeoStr->SetAlign(rdPtr->dwAlignFlags, rdPtr->bVerticalAlignOffset);
		rdPtr->pNeoStr->SetSpace(rdPtr->nRowSpace, rdPtr->nColSpace);
		rdPtr->pNeoStr->SetTabProperties(rdPtr->tabSize, rdPtr->bTabEM);

		rdPtr->pNeoStr->LinkObject(rdPtr->pIConObject, GIPP(rdPtr->pIConParamParser));
		rdPtr->pNeoStr->SetIConOffset(rdPtr->iConOffsetX, rdPtr->iConOffsetY);
		rdPtr->pNeoStr->SetIConScale(rdPtr->iConScale);
		rdPtr->pNeoStr->SetIConResample(rdPtr->bIConResample);

		rdPtr->pNeoStr->SetRemarkOffset(rdPtr->remarkOffsetX, rdPtr->remarkOffsetY);

		rdPtr->pNeoStr->GetFormat(rdPtr->pStr->c_str(), rdPtr->filterFlags, rdPtr->bIConNeedUpdate);
		auto cPos = rdPtr->pNeoStr->CalculateRange(&rc);

		rdPtr->charPos = { cPos.x,cPos.y, cPos.maxWidth,cPos.totalHeight };

		rdPtr->bIConNeedUpdate = false;
		rdPtr->reRender = true;
	}

	if (rdPtr->bClip		// only clip mode needs to redraw
		&& (rdPtr->oldX != rc.left
			|| rdPtr->oldY != rc.top)) {
		rdPtr->oldX = rc.left;
		rdPtr->oldY = rc.top;

		rdPtr->reRender = true;
	}

	if (rdPtr->pNeoStr != nullptr && rdPtr->pNeoStr->GetShakeUpdateState()) {
		rdPtr->reRender = true;
	}

	if (rdPtr->reRender) {
		////App Size
		//rhPtr->rhApp->m_hdr.gaCxWin;
		//rhPtr->rhApp->m_hdr.gaCyWin;

		////Frame Size
		//rhPtr->rhFrame->m_hdr.leWidth;
		//rhPtr->rhFrame->m_hdr.leHeight;

		const LPRH rhPtr = rdPtr->rHo.hoAdRunHeader;

		rdPtr->pNeoStr->SetColor(rdPtr->dwColor);

		rdPtr->pNeoStr->SetClip(rdPtr->bClip
			, min(rhPtr->rhApp->m_hdr.gaCxWin, rhPtr->rhFrame->m_hdr.leWidth)
			, min(rhPtr->rhApp->m_hdr.gaCyWin, rhPtr->rhFrame->m_hdr.leHeight));
		//rdPtr->pNeoStr->SetBorderOffset(5, 5);
		rdPtr->pNeoStr->SetBorderOffset(rdPtr->borderOffsetX, rdPtr->borderOffsetY);

		//rdPtr->pNeoStr->SetClip(false
		//	, 65535
		//	, 65535);

		rdPtr->pNeoStr->SetSmooth(
			Gdiplus::TextRenderingHint(rdPtr->textRenderingHint)
			, Gdiplus::SmoothingMode(rdPtr->smoothingMode - 1)
			, Gdiplus::PixelOffsetMode(rdPtr->pixelOffsetMode - 1));

		rdPtr->pNeoStr->RenderPerChar(&rc,
			*static_cast<NeoStr::RenderOptions*>(rdPtr->pRenderOptions));

		rdPtr->reRender = false;
	}

#ifdef COUNT_GDI_OBJECT
	const auto count = rdPtr->pData->objectCounter.objectCount;

	rdPtr->pData->objectCounter.UpdateObjectCount();
	if (count < rdPtr->pData->objectCounter.objectCount) {
		const auto info = std::format(L"Add when render {}\n", rdPtr->rHo.hoOiList->oilName);
		OutputDebugStringW(info.c_str());
	}
#endif
}

inline void Display(mv _far* mV, fpObjInfo oiPtr, fpLevObj loPtr, LPEDATA edPtr, RECT FAR* rc) {
	//MSGBOX(L"Editor Display");
	LPSURFACE ps = WinGetSurface((int)mV->mvIdEditWin);

	//if (edPtr->borderOffsetX != DEFAULT_EBORDER_OFFSET || edPtr->borderOffsetY != DEFAULT_EBORDER_OFFSET) {
	//	MSGBOX(L"Update");
	//	edPtr->borderOffsetX = DEFAULT_EBORDER_OFFSET;
	//	edPtr->borderOffsetY = DEFAULT_EBORDER_OFFSET;
	//}

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

		int sfDrv = ps->GetDriver();

		neoStr.SetHWA(sfDrv, false);

		//MSGBOX(L"Editor Calc");
		neoStr.SetAlign(edPtr->dwAlignFlags, edPtr->bVerticalAlignOffset);
		neoStr.SetSpace(edPtr->nRowSpace, edPtr->nColSpace);
		neoStr.SetTabProperties(edPtr->tabSize, edPtr->bTabEM);

		//MSGBOX(L"Editor Calc");
		neoStr.LinkObject(nullptr, nullptr);
		neoStr.SetIConOffset(edPtr->iConOffsetX, edPtr->iConOffsetY);
		neoStr.SetIConScale(edPtr->iConScale);
		neoStr.SetIConResample(edPtr->bIConResample);

		neoStr.SetRemarkOffset(edPtr->remarkOffsetX, edPtr->remarkOffsetY);

		neoStr.GetFormat(&edPtr->pText, edPtr->filterFlags);
		neoStr.CalculateRange(rc);

		neoStr.SetColor(edPtr->dwColor);

		neoStr.SetClip(false, 65535, 65535);
		//neoStr.SetBorderOffset(5, 5);
		neoStr.SetBorderOffset(edPtr->borderOffsetX, edPtr->borderOffsetY);

		neoStr.SetSmooth(
			Gdiplus::TextRenderingHint(edPtr->textRenderingHint)
			, Gdiplus::SmoothingMode(edPtr->smoothingMode - 1)
			, Gdiplus::PixelOffsetMode(edPtr->pixelOffsetMode - 1));

		//MSGBOX(L"Editor Render");
		neoStr.RenderPerChar(rc);

		//neoStr.SetHotSpot(edPtr->hotSpotPos, edPtr->hotSpotX, edPtr->hotSpotY);
		neoStr.SetHotSpot(0, 0);
		neoStr.SetScale(1.0, 1.0);
		neoStr.SetAngle(0);

		//MSGBOX(L"Editor Display PerChar");
		neoStr.DisplayPerChar(ps, rc
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
		//rc.right = rc.left + rdPtr->rHo.hoImgWidth;
		//rc.bottom = rc.top + rdPtr->rHo.hoImgHeight;

		rc.right = rc.left + rdPtr->swidth;
		rc.bottom = rc.top + rdPtr->sheight;

		UpdateRectByHotSpot(rdPtr->hotSpotPos
			//, rdPtr->rHo.hoImgWidth, rdPtr->rHo.hoImgHeight
			, rdPtr->swidth, rdPtr->sheight
			, rdPtr->hotSpotX, rdPtr->hotSpotY
			, &rc);

		int hotSpotX = screenX - rc.left;
		int hotSpotY = screenY - rc.top;

		// Ink effects
		BlitMode bm = (rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) ? BMODE_TRANSP : BMODE_OPAQUE;
		BOOL bAntiA = (rdPtr->rs.rsEffect & EFFECTFLAG_ANTIALIAS) ? TRUE : FALSE;
		BlitOp bo = (BlitOp)(rdPtr->rs.rsEffect & EFFECT_MASK);
		int boParam = rdPtr->rs.rsEffectParam;

		// Draw text
		HandleUpdate(rdPtr, rc);

		rdPtr->pNeoStr->SetHotSpot(hotSpotX, hotSpotY);
		rdPtr->pNeoStr->SetScale(rdPtr->xScale, rdPtr->yScale);
		rdPtr->pNeoStr->SetAngle(rdPtr->angle);

		rdPtr->pNeoStr->DisplayPerChar(ps, &rc
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

inline void SetIConUpdate(LPRDATA rdPtr) {
	ObjectSelection Oc(rdPtr->rHo.hoAdRunHeader);
	Oc.IterateObjectWithIdentifier(rdPtr, rdPtr->rHo.hoIdentifier, [](LPRO pObject) {
		LPRDATA pObj = (LPRDATA)pObject;

		if (pObj->bIConGlobal && pObj->bIConForceUpdate) {
			pObj->bStrChanged = true;
			pObj->bIConNeedUpdate = true;
		}
		});
}

inline void GlobalIConUpdater(LPRDATA rdPtr) {
	if (rdPtr->bIConGlobal) {
		if (rdPtr->pData->pIConData->NeedUpdateICon(rdPtr->pIConObject)) {
			SetIConUpdate(rdPtr);
		}

		rdPtr->pData->pIConData->pCaller = (LPRO)rdPtr;
		rdPtr->pData->pIConData->UpdateICon(rdPtr->pIConObject, GIPP(rdPtr->pIConParamParser));
	}
}