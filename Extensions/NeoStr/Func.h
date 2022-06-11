#pragma once

#include <functional>
#include <string_view>

constexpr auto a = L"a";
constexpr auto b = L"b";
constexpr auto c = L"c";
constexpr auto pStr = L"abc";
constexpr auto pStr0 = L"入目的也";
constexpr auto pStr1 = L"入目的也仅仅只有那一片惨白罢了，偶有灰色的什么掠过视野也很快湮没在了白之中。那一刻的她到底在想什么";
constexpr auto pStr2 = L"入目的也仅仅只有那一片惨白罢了，偶有灰色的什么掠过视野也很快湮没在了白之中。那一刻的她到底在想什么入目的也仅仅只有那一片惨白罢了，偶有灰色的什么掠过视野也很快湮没在了白之中。那一刻的她到底在想什么";

inline void ReDisplay(LPRDATA rdPtr) {		
		rdPtr->rc.rcChanged = true;

		rdPtr->rHo.hoImgXSpot = 0;
		rdPtr->rHo.hoImgYSpot = 0;

		rdPtr->rHo.hoImgWidth = rdPtr->swidth;
		rdPtr->rHo.hoImgHeight = rdPtr->sheight;

		callRunTimeFunction(rdPtr, RFUNCTION_REDRAW, 0, 0);
}

inline void Display(LPRDATA rdPtr) {
	LPRH rhPtr = rdPtr->rHo.hoAdRunHeader;
	LPSURFACE ps = WinGetSurface((int)rhPtr->rhIdEditWin);
	
	if (ps != nullptr && rdPtr->pStr != nullptr) {
		RECT rc;

		rc.left = rdPtr->rHo.hoX;
		rc.top = rdPtr->rHo.hoY;
		rc.right = rc.left + rdPtr->rHo.hoImgWidth;
		rc.bottom = rc.top + rdPtr->rHo.hoImgHeight;

		// Ink effects
		BlitMode bm = (rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) ? BMODE_TRANSP : BMODE_OPAQUE;
		BOOL bAntiA = (rdPtr->rs.rsEffect & EFFECTFLAG_ANTIALIAS) ? TRUE : FALSE;
		BlitOp bo = (BlitOp)(rdPtr->rs.rsEffect & EFFECT_MASK);
		int boParam = rdPtr->rs.rsEffectParam;

		// Draw text
		NeoStr neoStr(rdPtr->dwAlignFlags, rdPtr->dwColor, rdPtr->hFont);
		neoStr.Display(ps, rdPtr->pStr->c_str(), &rc
			, bm, bo, boParam, bAntiA);
		
		//auto sz1 = neoStr.GetStrSize(a);
		//auto sz2 = neoStr.GetStrSize(b);
		//auto sz3 = neoStr.GetStrSize(c);
		//auto sz4 = neoStr.GetStrSize(pStr);
		//auto sz5 = neoStr.GetStrSize(pStr0);
		//auto sz6 = neoStr.GetStrSize(pStr1);
		//auto sz7 = neoStr.GetStrSize(pStr2);
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

		// Draw text
		NeoStr neoStr(edPtr->dwAlignFlags, edPtr->dwColor, hFont);
		neoStr.Display(ps, &edPtr->pText, rc
			, bm, bo, boParam, bAntiA);

		// Delete font
		if (hFont != NULL)
			DeleteObject(hFont);
	}
}
