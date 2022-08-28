#pragma once

inline void BlitVideoFrame(LPRDATA rdPtr, size_t ms = 0) {
	if (rdPtr->pFFMpeg == nullptr) {
		return;
	}

	rdPtr->pFFMpeg->get_videoFrame(ms, [rdPtr](const unsigned char* pData, const int width, const int height) {
		auto pMemSf = CreateSurface(24, width, height);

		auto sfCoef = GetSfCoef(pMemSf);

		auto lineSz = sfCoef.pitch;
		auto alphaSz = sfCoef.sz / sfCoef.byte;

		for (int y = 0; y < height; y++) {
			auto pMemData = sfCoef.pData + y * sfCoef.pitch;
			auto pVideo = pData + (height - 1 - y) * sfCoef.pitch;

			memcpy(pMemData, pVideo, lineSz);
		}

		ReleaseSfCoef(pMemSf, sfCoef);

		auto pAlpha = new BYTE[alphaSz];
		memset(pAlpha, 255, alphaSz);

		pMemSf->SetAlpha(pAlpha, pMemSf->GetWidth());

		delete[] pAlpha;
		pAlpha = nullptr;

#ifdef _DEBUG
		_SavetoClipBoard(pMemSf, false);
#endif // _DEBUG

		delete rdPtr->pFrame;
		rdPtr->pFrame = nullptr;

		rdPtr->pFrame = CreateHWASurface(rdPtr, 32, width, height, ST_HWA_ROMTEXTURE);

		//pMemSf->DemultiplyAlpha();
		if (PreMulAlpha(rdPtr)) {
			pMemSf->PremultiplyAlpha();		// only needed in DX11 premultiplied mode
		}

		pMemSf->Blit(*rdPtr->pFrame);

		delete pMemSf;

		return;
		});
}