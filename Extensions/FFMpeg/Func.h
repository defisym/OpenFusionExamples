#pragma once

#include <functional>

inline void ReDisplay(LPRDATA rdPtr) {
	if (rdPtr->pMemSf != nullptr && rdPtr->pMemSf->IsValid()) {
		//callRunTimeFunction(rdPtr, RFUNCTION_REDRAW, 0, 0);
		rdPtr->rc.rcChanged = true;

		//rdPtr->rHo.hoImgXSpot = rdPtr->hotSpot.x;
		//rdPtr->rHo.hoImgYSpot = rdPtr->hotSpot.y;

		rdPtr->rHo.hoImgXSpot = 0;
		rdPtr->rHo.hoImgYSpot = 0;

		rdPtr->rHo.hoImgWidth = rdPtr->pMemSf->GetWidth();
		rdPtr->rHo.hoImgHeight = rdPtr->pMemSf->GetHeight();

		rdPtr->bChanged = true;
	}
}

inline void CopyData(const unsigned char* pData, LPSURFACE pMemSf, bool bPm) {
	auto sfCoef = GetSfCoef(pMemSf);
	
	auto lineSz = sfCoef.pitch;
	auto alphaSz = sfCoef.sz / sfCoef.byte;

	for (int y = 0; y < pMemSf->GetHeight(); y++) {
		auto pMemData = sfCoef.pData + y * sfCoef.pitch;
		auto pVideo = pData + (pMemSf->GetHeight() - 1 - y) * sfCoef.pitch;

		memcpy(pMemData, pVideo, lineSz);
	}

//#ifdef _DEBUG
//		_SavetoClipBoard(rdPtr->pMemSf, false);
//#endif // _DEBUG

	ReleaseSfCoef(pMemSf, sfCoef);

	if (bPm) {
		pMemSf->PremultiplyAlpha();		// only needed in DX11 premultiplied mode
	}
}

// pMemSf
using blitCallBack = std::function<void(LPSURFACE&)>;

// allocate pMemSf
inline void BlitVideoFrame(LPRDATA rdPtr, size_t ms, blitCallBack callBack) {
	if (rdPtr->pFFMpeg == nullptr) {
		return;
	}

	rdPtr->pFFMpeg->get_videoFrame(ms, [&](const unsigned char* pData, const int width, const int height) {
		if (rdPtr->pMemSf == nullptr) {
			rdPtr->pMemSf = CreateSurface(24, width, height);

			auto alphaSz = width * height;

			auto pAlpha = new BYTE[alphaSz];
			memset(pAlpha, 255, alphaSz);

			rdPtr->pMemSf->SetAlpha(pAlpha, rdPtr->pMemSf->GetWidth());

			delete[] pAlpha;
			pAlpha = nullptr;
		}

		CopyData(pData, rdPtr->pMemSf, rdPtr->bPm);

		callBack(rdPtr->pMemSf);
		});
}
