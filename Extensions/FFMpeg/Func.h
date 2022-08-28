#pragma once

#include <functional>

inline void ReDisplay(LPRDATA rdPtr) {
	if (rdPtr->pDisplay != nullptr && rdPtr->pDisplay->IsValid()) {
		//callRunTimeFunction(rdPtr, RFUNCTION_REDRAW, 0, 0);
		rdPtr->rc.rcChanged = true;

		//rdPtr->rHo.hoImgXSpot = rdPtr->hotSpot.x;
		//rdPtr->rHo.hoImgYSpot = rdPtr->hotSpot.y;

		rdPtr->rHo.hoImgXSpot = 0;
		rdPtr->rHo.hoImgYSpot = 0;

		rdPtr->rHo.hoImgWidth = rdPtr->pDisplay->GetWidth();
		rdPtr->rHo.hoImgHeight = rdPtr->pDisplay->GetHeight();

		rdPtr->bChanged = true;
	}
}

// pMemSf
using blitCallBack = std::function<void(LPSURFACE&)>;

inline void BlitVideoFrame(LPRDATA rdPtr, size_t ms, blitCallBack callBack) {
	if (rdPtr->pFFMpeg == nullptr) {
		return;
	}

	rdPtr->pFFMpeg->get_videoFrame(ms, [&](const unsigned char* pData, const int width, const int height) {
		bool bUpdateAlpha = false;

		if (rdPtr->pMemSf == nullptr) {
			rdPtr->pMemSf = CreateSurface(24, width, height);
			rdPtr->pDisplay = rdPtr->pMemSf;

			bUpdateAlpha = true;
		}

		auto sfCoef = GetSfCoef(rdPtr->pMemSf);

		auto lineSz = sfCoef.pitch;
		auto alphaSz = sfCoef.sz / sfCoef.byte;

		for (int y = 0; y < height; y++) {
			auto pMemData = sfCoef.pData + y * sfCoef.pitch;
			auto pVideo = pData + (height - 1 - y) * sfCoef.pitch;

			memcpy(pMemData, pVideo, lineSz);
		}

		if (bUpdateAlpha) {
			auto pAlpha = new BYTE[alphaSz];
			memset(pAlpha, 255, alphaSz);

			rdPtr->pMemSf->SetAlpha(pAlpha, rdPtr->pMemSf->GetWidth());

			delete[] pAlpha;
			pAlpha = nullptr;
		}

		ReleaseSfCoef(rdPtr->pMemSf, sfCoef);

//#ifdef _DEBUG
//		_SavetoClipBoard(rdPtr->pMemSf, false);
//#endif // _DEBUG

		if (rdPtr->bPm) {
			rdPtr->pMemSf->PremultiplyAlpha();		// only needed in DX11 premultiplied mode
		}

		callBack(rdPtr->pMemSf);

		return;
		});
}
