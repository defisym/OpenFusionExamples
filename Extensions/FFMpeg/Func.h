#pragma once

#include <functional>

inline void UpdateScale(LPRDATA rdPtr, int width, int height) {
	if (rdPtr->bStretch) {
		rdPtr->rc.rcScaleX = ((float)rdPtr->swidth) / width;
		rdPtr->rc.rcScaleY = ((float)rdPtr->sheight) / height;
	}
	else {
		rdPtr->rc.rcScaleX = 1.0;
		rdPtr->rc.rcScaleY = 1.0;
	}
}

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

inline void InitSurface(LPSURFACE& pSf, const int width, const int height) {
	if (pSf == nullptr || pSf->GetWidth() != width || pSf->GetHeight() != height) {

#ifdef _VIDEO_ALPHA
		pSf = CreateSurface(32, width, height);
#else
		pSf = CreateSurface(24, width, height);
#endif

		auto alphaSz = width * height;

		auto pAlpha = new BYTE[alphaSz];
		memset(pAlpha, 255, alphaSz);

		pSf->SetAlpha(pAlpha, width);

		delete[] pAlpha;
		pAlpha = nullptr;
	}
}

inline void CopyData(const unsigned char* pData, int srcLineSz, LPSURFACE pMemSf, bool bPm) {
	auto sfCoef = GetSfCoef(pMemSf);

	auto lineSz = sfCoef.pitch;	
	auto alphaSz = sfCoef.sz / sfCoef.byte;

	auto width = pMemSf->GetWidth();
	auto height = pMemSf->GetHeight();

	for (int y = 0; y < height; y++) {
		auto pMemData = sfCoef.pData + y * lineSz;
		auto pVideo = pData + (height - 1 - y) * srcLineSz;

		memcpy(pMemData, pVideo, lineSz);

#ifdef _VIDEO_ALPHA
		// 32 bit: 4 bytes per pixel: blue, green, red, unused (0)
		for (int x = 0; x < width; x++) {
			auto pAlphaData = sfCoef.pAlphaData + (height - 1 - y) * sfCoef.alphaPitch + x * sfCoef.alphaByte;
			auto curAlpha = pVideo + (PIXEL_BYTE - 1) + x * PIXEL_BYTE;
			pAlphaData[0] = *curAlpha;
		}
#endif
	}

//#ifdef _DEBUG
//	_SavetoClipBoard(pMemSf, false);
//#endif // _DEBUG

	ReleaseSfCoef(pMemSf, sfCoef);

	if (bPm) {
		pMemSf->PremultiplyAlpha();		// only needed in DX11 premultiplied mode
	}
}

inline void BlitVideoFrame(LPRDATA rdPtr, size_t ms, LPSURFACE& pSf) {
	if (rdPtr->pFFMpeg == nullptr) {
		return;
	}

	rdPtr->pFFMpeg->get_videoFrame(ms, [&] (const unsigned char* pData, const int stride, const int height) {
		CopyData(pData, stride, pSf, rdPtr->bPm);
		ReDisplay(rdPtr);
		});
}

inline void NextVideoFrame(LPRDATA rdPtr) {
	if (rdPtr->pFFMpeg == nullptr) {
		return;
	}

	rdPtr->pFFMpeg->get_nextFrame([&](const unsigned char* pData, const int stride, const int height) {
		CopyData(pData, stride, rdPtr->pMemSf, rdPtr->bPm);
		ReDisplay(rdPtr);
		});
}

// return pMemSf, if bHwa == true, cast it to HWA and save to pHwaSf;
inline long ReturnVideoFrame(LPRDATA rdPtr, bool bHwa, LPSURFACE& pMemSf, LPSURFACE& pHwaSf) {
	if (!bHwa) {
		return ConvertToLong(pMemSf);
	}
	else {
		delete pHwaSf;
		pHwaSf = nullptr;

		pHwaSf = ConvertHWATexture(rdPtr, pMemSf);

		return ConvertToLong(pHwaSf);
	}
}

inline void CloseGeneral(LPRDATA rdPtr) {
	delete rdPtr->pEncrytpt;
	rdPtr->pEncrytpt = nullptr;

	delete rdPtr->pFFMpeg;
	rdPtr->pFFMpeg = nullptr;

	rdPtr->bOpen = false;
	rdPtr->bPlay = false;
}

inline void SetPositionGeneral(LPRDATA rdPtr, int msRaw, int flags = seekFlags) {
	if (!rdPtr->bOpen) {
		return;
	}

	// add protection for minus position
	msRaw = msRaw < 0 ? 0 : msRaw;

	size_t ms = (size_t)msRaw;

	if ((flags & AVSEEK_FLAG_BYTE) != AVSEEK_FLAG_BYTE) {
		rdPtr->bJumped = true;
		rdPtr->jumpedPts = int64_t(ms / 1000.0);
	}

	rdPtr->pFFMpeg->set_videoPosition(ms, flags);

	if (!rdPtr->bPlay) {
		BlitVideoFrame(rdPtr, ms, rdPtr->pMemSf);
	}

	ReDisplay(rdPtr);
}