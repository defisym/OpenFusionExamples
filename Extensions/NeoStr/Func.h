#pragma once

#include <functional>
#include <string_view>

constexpr auto ONITOIC = 0;
constexpr auto ONTAGCB = 1;
constexpr auto ONTAGCBF = 2;

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

inline NeoStr::IConData* GetIConData(LPRDATA rdPtr) {
	return rdPtr->bIConGlobal ? rdPtr->pData->pIConData : nullptr;
}

inline void HandleUpdate(LPRDATA rdPtr, RECT rc) {
#ifdef COUNT_GDI_OBJECT
	rdPtr->pData->objectCounter.UpdateObjectCount();
#endif

	if (rdPtr->bFontChanged) {
		rdPtr->bFontChanged = false;
		rdPtr->bStrChanged = true;

		delete rdPtr->pNeoStr;
		rdPtr->pNeoStr = new NeoStr(rdPtr->dwAlignFlags, rdPtr->dwColor, rdPtr->hFont, false,
			{ rdPtr->pData->pFontCache,
				rdPtr->pData->pCharSzCacheWithFont,
				rdPtr->pData->pRegexHandler,
				rdPtr->pData->pFontCollection },
			GetIConData(rdPtr));

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
		const auto cPos = rdPtr->pNeoStr->CalculateRange(&rc);

		rdPtr->charPos = { cPos.x,cPos.y, cPos.maxWidth,cPos.totalHeight };

		rdPtr->bIConNeedUpdate = false;
		rdPtr->reRender = true;
	}

	// only clip mode needs to redraw
	if (!rdPtr->bScroll && rdPtr->bClip) {		
		const auto pRenderOptions = static_cast<NeoStr::RenderOptions*>(rdPtr->pRenderOptions);
		const auto bCoordChanged = rdPtr->oldX != rc.left || rdPtr->oldY != rc.top;

		if (pRenderOptions->bCharClipped && bCoordChanged) {
			rdPtr->oldX = rc.left;
			rdPtr->oldY = rc.top;

			rdPtr->reRender = true;
		}
	}

	if (rdPtr->bUpdateScroll && rdPtr->bClip) {
		rdPtr->bUpdateScroll = false;
		rdPtr->reRender = true;
	}

	if (rdPtr->pNeoStr != nullptr && rdPtr->pNeoStr->GetShakeUpdateState()) {
		rdPtr->reRender = true;
	}

	if (rdPtr->reRender) {
		rdPtr->reRender = false;

		////App Size
		//rhPtr->rhApp->m_hdr.gaCxWin;
		//rhPtr->rhApp->m_hdr.gaCyWin;

		////Frame Size
		//rhPtr->rhFrame->m_hdr.leWidth;
		//rhPtr->rhFrame->m_hdr.leHeight;

		const LPRH rhPtr = rdPtr->rHo.hoAdRunHeader;

		rdPtr->pNeoStr->SetColor(rdPtr->dwColor);
		rdPtr->pNeoStr->SetBorderOffset(rdPtr->borderOffsetX, rdPtr->borderOffsetY);

		rdPtr->pNeoStr->SetSmooth(
			Gdiplus::TextRenderingHint(rdPtr->textRenderingHint)
			, Gdiplus::SmoothingMode(rdPtr->smoothingMode - 1)
			, Gdiplus::PixelOffsetMode(rdPtr->pixelOffsetMode - 1));

		const auto pRenderOptions = static_cast<NeoStr::RenderOptions*>(rdPtr->pRenderOptions);

		// clip should be disabled if scroll is enabled, to make sure the full size is rendered
		pRenderOptions->SetClip(!rdPtr->bScroll && rdPtr->bClip
			, min(rhPtr->rhApp->m_hdr.gaCxWin, rhPtr->rhFrame->m_hdr.leWidth)
			, min(rhPtr->rhApp->m_hdr.gaCyWin, rhPtr->rhFrame->m_hdr.leHeight));
		pRenderOptions->SetClipToObject(!rdPtr->bScroll && rdPtr->bClipToObject);

		// params like clip will be reset in render routine,
		// but it will be reset to external each call, so it's safe
		rdPtr->pNeoStr->RenderPerChar(&rc, *pRenderOptions);
		if(rdPtr->bTagCallbackIndexManaged) {
			pRenderOptions->UpdateTagCallbackIndex(rdPtr->pNeoStr);
		}
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

// edittime display
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
		//MSGBOX(L"L: "+_itos(rc->left)+ L"T: " + _itos(rc->top), L"RECT");

		// Draw text
		HFONT hFont = CreateFontIndirect(&edPtr->logFont);
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
		neoStr.SetBorderOffset(edPtr->borderOffsetX, edPtr->borderOffsetY);

		neoStr.SetSmooth(
			Gdiplus::TextRenderingHint(edPtr->textRenderingHint)
			, Gdiplus::SmoothingMode(edPtr->smoothingMode - 1)
			, Gdiplus::PixelOffsetMode(edPtr->pixelOffsetMode - 1));

		//MSGBOX(L"Editor Render");
		NeoStr::RenderOptions renderOpt;
		renderOpt.SetClip(false, 65535, 65535);
		renderOpt.SetClipToObject(edPtr->bClipToObject);

		neoStr.RenderPerChar(rc, renderOpt);

		//neoStr.SetHotSpot(edPtr->hotSpotPos, edPtr->hotSpotX, edPtr->hotSpotY);
		neoStr.SetHotSpot(0, 0);
		neoStr.SetScale(1.0, 1.0);
		neoStr.SetAngle(0);

		//MSGBOX(L"Editor Display");

		// Ink effects
		NeoStr::BlitOptions blitOpt;

		const BOOL bTransp = ((oiPtr->oiHdr.oiInkEffect & EFFECTFLAG_TRANSPARENT) != 0);
		blitOpt.bm = (bTransp) ? BMODE_TRANSP : BMODE_OPAQUE;
		blitOpt.bAntiA = (oiPtr->oiHdr.oiInkEffect & EFFECTFLAG_ANTIALIAS) ? TRUE : FALSE;
		blitOpt.bo = (BlitOp)(oiPtr->oiHdr.oiInkEffect & EFFECT_MASK);
		blitOpt.boParam = oiPtr->oiHdr.oiInkEffectParam;

		neoStr.BlitResult(ps, rc, blitOpt);

		// Delete font
		if (hFont != NULL)
			DeleteObject(hFont);
	}
}

inline RECT GetRuntimeObjectRect(LPRDATA rdPtr) {
	// On-screen coords
	const LPRH rhPtr = rdPtr->rHo.hoAdRunHeader;

	const int screenX = rdPtr->rHo.hoX - rhPtr->rhWindowX;
	const int screenY = rdPtr->rHo.hoY - rhPtr->rhWindowY;

	RECT rc = {};

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

	return rc;
}

// runtime display
inline void Display(LPRDATA rdPtr) {
	const LPRH rhPtr = rdPtr->rHo.hoAdRunHeader;
	const LPSURFACE ps = WinGetSurface((int)rhPtr->rhIdEditWin);

	if (ps != nullptr && rdPtr->pStr != nullptr) {
		// On-screen coords
		const int screenX = rdPtr->rHo.hoX - rhPtr->rhWindowX;
		const int screenY = rdPtr->rHo.hoY - rhPtr->rhWindowY;

		auto rc = GetRuntimeObjectRect(rdPtr);

		const int hotSpotX = screenX - rc.left;
		const int hotSpotY = screenY - rc.top;

		// Draw text
		HandleUpdate(rdPtr, rc);

		rdPtr->pNeoStr->SetHotSpot(hotSpotX, hotSpotY);
		rdPtr->pNeoStr->SetScale(rdPtr->xScale, rdPtr->yScale);
		rdPtr->pNeoStr->SetAngle(rdPtr->angle);

		const auto pBlitOptions = static_cast<NeoStr::BlitOptions*>(rdPtr->pBlitOptions);

		// Ink effects
		pBlitOptions->bm = (rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) ? BMODE_TRANSP : BMODE_OPAQUE;
		pBlitOptions->bAntiA = (rdPtr->rs.rsEffect & EFFECTFLAG_ANTIALIAS) ? TRUE : FALSE;
		pBlitOptions->bo = (BlitOp)(rdPtr->rs.rsEffect & EFFECT_MASK);
		pBlitOptions->boParam = rdPtr->rs.rsEffectParam;

		// scroll
		pBlitOptions->bScroll = rdPtr->bScroll;
		pBlitOptions->scrollCoefX = rdPtr->scrollCoefX;
		pBlitOptions->scrollCoefY = rdPtr->scrollCoefY;

		rdPtr->pNeoStr->BlitResult(ps, &rc, *pBlitOptions);

		rdPtr->scrollCoefX = pBlitOptions->scrollCoefX;
		rdPtr->scrollCoefY = pBlitOptions->scrollCoefY;
	}
}

inline CharPos UpdateLastCharPos(LPRDATA rdPtr) {
	auto rc = GetRuntimeObjectRect(rdPtr);

	HandleUpdate(rdPtr, rc);

	const auto pBlitOptions = static_cast<NeoStr::BlitOptions*>(rdPtr->pBlitOptions);

	pBlitOptions->bNoBlit = true;
	pBlitOptions->bScroll = rdPtr->bScroll;
	pBlitOptions->scrollCoefX = rdPtr->scrollCoefX;
	pBlitOptions->scrollCoefY = rdPtr->scrollCoefY;

	rdPtr->pNeoStr->BlitResult(nullptr, &rc, *pBlitOptions);

	pBlitOptions->bNoBlit = false;
	rdPtr->scrollCoefX = pBlitOptions->scrollCoefX;
	rdPtr->scrollCoefY = pBlitOptions->scrollCoefY;

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
	if (!rdPtr->bIConGlobal) { return; }

	if (rdPtr->pData->pIConData->NeedUpdateICon(rdPtr->pIConObject)) {
		SetIConUpdate(rdPtr);
	}

	rdPtr->pData->pIConData->pCaller = (LPRO)rdPtr;
	rdPtr->pData->pIConData->UpdateICon(rdPtr->pIConObject, GIPP(rdPtr->pIConParamParser));
}