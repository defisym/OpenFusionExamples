#pragma once

#include <functional>

inline void CleanCache(LPRDATA rdPtr, bool forceClean = false);

//-----------------------------

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
		rdPtr->bChanged = true;
		rdPtr->rc.rcChanged = true;

		// use video resolution to fix collision, as scale works differently here
		UpdateHoImgInfo(rdPtr
			//, rdPtr->swidth, rdPtr->sheight
			, rdPtr->pFFMpeg->get_width(), rdPtr->pFFMpeg->get_height()
			, (float)rdPtr->rc.rcScaleX, (float)rdPtr->rc.rcScaleY
			, HotSpotPos::LT, 0, 0
			, 0);
	}
}

inline void InitSurface(LPSURFACE& pSf, const int width, const int height) {
	if (pSf == nullptr || pSf->GetWidth() != width || pSf->GetHeight() != height) {

#ifdef VIDEO_ALPHA
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
	if (pData == nullptr || pMemSf == nullptr) {
		return;
	}

	auto sfCoef = GetSfCoef(pMemSf);

	auto lineSz = sfCoef.pitch;	
	auto alphaSz = sfCoef.sz / sfCoef.byte;

	auto width = pMemSf->GetWidth();
	auto height = pMemSf->GetHeight();

//#define _MANUAL_PM

#ifdef _OPENMP
	omp_set_num_threads(std::thread::hardware_concurrency());
#pragma omp parallel
#endif
	{
#ifdef _OPENMP
#pragma omp for
#endif
		for (int y = 0; y < height; y++) {
			auto pMemData = sfCoef.pData + y * lineSz;
			auto pVideo = pData + (height - 1 - y) * srcLineSz;

#ifndef _MANUAL_PM
			memcpy(pMemData, pVideo, lineSz);
#ifdef VIDEO_ALPHA
			// 32 bit: 4 bytes per pixel: blue, green, red, unused (0)
#ifdef _OPENMP
//#pragma omp for
#endif
			for (int x = 0; x < width; x++) {
				auto pAlphaData = sfCoef.pAlphaData + (height - 1 - y) * sfCoef.alphaPitch + x * sfCoef.alphaByte;
				auto curAlpha = pVideo + (PIXEL_BYTE - 1) + x * PIXEL_BYTE;
				pAlphaData[0] = *curAlpha;
			}
#endif
#else
			for (int x = 0; x < width; x++) {
				auto pVideoData = pVideo + x * PIXEL_BYTE;

				auto pRGBData = pMemData + x * sfCoef.byte;
				if (!bPm) {
					pRGBData[0] = pVideoData[0];
					pRGBData[1] = pVideoData[1];
					pRGBData[2] = pVideoData[2];
				}
				else {
					auto alphaCoef = pVideoData[3] / 255.0;

					pRGBData[0] = (BYTE)(pVideoData[0] * alphaCoef);
					pRGBData[1] = (BYTE)(pVideoData[1] * alphaCoef);
					pRGBData[2] = (BYTE)(pVideoData[2] * alphaCoef);
				}

#ifdef VIDEO_ALPHA
				auto pAlphaData = sfCoef.pAlphaData + (height - 1 - y) * sfCoef.alphaPitch + x * sfCoef.alphaByte;
				pAlphaData[0] = pVideoData[3];
#endif
			}

#endif
		}
	}
#ifdef _DEBUG
	//_SavetoClipBoard(pMemSf, false);
#endif // _DEBUG

	ReleaseSfCoef(pMemSf, sfCoef);

#ifndef _MANUAL_PM
	if (bPm) {
		pMemSf->PremultiplyAlpha();		// only needed in DX11 premultiplied mode
	}
#endif

	return;
}

inline void BlitVideoFrame(LPRDATA rdPtr, size_t ms, LPSURFACE& pSf) {
	if (rdPtr->pFFMpeg == nullptr) {
		return;
	}

	rdPtr->pFFMpeg->get_videoFrame(ms, rdPtr->bAccurateSeek, [&](const unsigned char* pData, const int stride, const int height) {
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

inline void SetPositionGeneral(LPRDATA rdPtr, int msRaw, int flags = seekFlags) {
	if (!rdPtr->bOpen) {
		return;
	}

	// add protection for minus position
	msRaw = msRaw < 0 ? 0 : msRaw;

	const auto ms = static_cast<size_t>(msRaw);

	//auto pos = rdPtr->pFFMpeg->get_videoPosition();

	rdPtr->pFFMpeg->set_videoPosition(ms, flags);

	if (rdPtr->bAccurateSeek && (flags & AVSEEK_FLAG_BYTE) != AVSEEK_FLAG_BYTE) {
		rdPtr->pFFMpeg->goto_videoPosition(ms, [&](const unsigned char* pData, const int stride, const int height) {
			CopyData(pData, stride, rdPtr->pMemSf, rdPtr->bPm);
			ReDisplay(rdPtr);
			});
	}

	ReDisplay(rdPtr);
}

inline bool GetVideoPlayState(LPRDATA rdPtr) {
	if (rdPtr->pFFMpeg == nullptr) {
		return false;
	}

	return rdPtr->bPlay
		? rdPtr->bLoop
			? true
			: !rdPtr->pFFMpeg->get_finishState()
		: false;
}

inline bool GetVideoFinishState(LPRDATA rdPtr) {
	if (rdPtr->pFFMpeg == nullptr) {
		return false;
	}

	return rdPtr->pFFMpeg != nullptr && rdPtr->pFFMpeg->get_finishState();
}

inline Encryption* LoadMemVideo(LPRDATA rdPtr, std::wstring& filePath, std::wstring& key) {
	auto pMemVideoLib = rdPtr->pData->pMemVideoLib;

	if (rdPtr->bCache) {
		auto it = pMemVideoLib->GetItem(filePath);

		if (pMemVideoLib->ItemExist(it)) {
			return it->second;
		}
	}

	bool bRetry = false;

	auto pEncrypt = new Encryption;
	pEncrypt->GenerateKey(key.c_str());

retry:
	try {
		//pEncrypt->DecryptFile(filePath.c_str());
		pEncrypt->DecryptFileDirectly(filePath.c_str());
	}
	catch (std::bad_alloc& e) {
		if (rdPtr->bCache && !bRetry) {
			bRetry = true;
			CleanCache(rdPtr, true);

			goto retry;
		}

		throw e;
	}
	
	if (rdPtr->bCache) {
		pMemVideoLib->PutItem(filePath, pEncrypt);
	}

	return pEncrypt;
}

inline void CloseGeneral(LPRDATA rdPtr) {
	delete rdPtr->pFFMpeg;
	rdPtr->pFFMpeg = nullptr;

	if (!rdPtr->bCache) {
		delete rdPtr->pEncrypt;
	}

	rdPtr->pEncrypt = nullptr;

	rdPtr->bOpen = false;
	rdPtr->bPlay = false;

	*rdPtr->pFilePath = L"";
}

inline FFMpegOptions GetOptions(LPRDATA rdPtr) {
	FFMpegOptions opt;

	opt.flag = rdPtr->hwDeviceType | (rdPtr->bForceNoAudio ? FFMpegFlag_ForceNoAudio : 0);
	opt.videoCodecName = *rdPtr->pVideoOverrideCodecName;
	opt.audioCodecName = *rdPtr->pAudioOverrideCodecName;

	return opt;
}

inline void OpenGeneral(LPRDATA rdPtr, std::wstring& filePath, std::wstring& key,
	const FFMpegOptions& opt = FFMpegOptions(), size_t ms = 0) {
	CloseGeneral(rdPtr);

	try {
		if (StrEmpty(key.c_str())) {
			rdPtr->pFFMpeg = new FFMpeg(filePath, opt);
		}
		else {
			rdPtr->pEncrypt = LoadMemVideo(rdPtr, filePath, key);
			rdPtr->pFFMpeg = new FFMpeg(rdPtr->pEncrypt->GetOutputData(), rdPtr->pEncrypt->GetOutputDataLength(), opt);
		}

		rdPtr->pFFMpeg->set_queueSize(rdPtr->audioQSize, rdPtr->videoQSize);

		rdPtr->bOpen = true;
		rdPtr->bPlay = rdPtr->bPlayAfterLoad;
		*rdPtr->pFilePath = filePath;

		rdPtr->pFFMpeg->set_volume(rdPtr->volume);
		rdPtr->pFFMpeg->set_loop(rdPtr->bLoop);

		rdPtr->pFFMpeg->set_audioTempo(rdPtr->atempo);

		UpdateScale(rdPtr, rdPtr->pFFMpeg->get_width(), rdPtr->pFFMpeg->get_height());

		InitSurface(rdPtr->pMemSf, rdPtr->pFFMpeg->get_width(), rdPtr->pFFMpeg->get_height());

		SetPositionGeneral(rdPtr, ms);

		//BlitVideoFrame(rdPtr, ms, rdPtr->pMemSf);

		ReDisplay(rdPtr);
	}
	catch (...) {
		CloseGeneral(rdPtr);

		if (!opt.NoOverride()) {
			auto newOpt = opt;
			newOpt.ResetOverride();

			OpenGeneral(rdPtr, filePath, key, newOpt, ms);
		}
		else {
			// update path for condition to check
			*rdPtr->pFilePath = filePath;

			CallEvent(ON_OPENFAILED);

			*rdPtr->pFilePath = L"";
		}
	}
}

inline auto GetRefList(LPRDATA rdPtr) {
	std::vector<const uint8_t*> pBufs;
	pBufs.reserve(rdPtr->pData->pMemVideoLib->data.size());

	for (auto& ppFFMpeg : rdPtr->pData->ppFFMpegs_record) {
		auto pFFMpeg = *ppFFMpeg;

		if (!pFFMpeg) {
			continue;
		}

		auto pBufSrc = pFFMpeg->get_memBufSrc();

		pBufs.emplace_back(pBufSrc);
	}

	return pBufs;
}

inline auto GetToRemove(LPRDATA rdPtr, const std::vector<const uint8_t*>& pBufs) {
	std::vector<const std::wstring*> toRemove;
	toRemove.reserve(rdPtr->pData->pMemVideoLib->data.size() - pBufs.size());

	for (auto& pair : rdPtr->pData->pMemVideoLib->data) {
		auto it = std::find(pBufs.begin(), pBufs.end(), pair.second->GetOutputData());

		// not referred
		if (it == pBufs.end()) {
			toRemove.emplace_back(&pair.first);
		}
	}

	return toRemove;
}

inline void CleanCache(LPRDATA rdPtr, bool forceClean) {
	auto ExceedMemLimit = [] (size_t memLimit = DEFAULT_MEMORYLIMIT) {
		return memLimit <= GetProcessMemoryUsageMB();
	};

	if (!rdPtr->bCache) {
		return;
	}

	forceClean |= SystemMemoryNotEnough();

	if (!forceClean && !ExceedMemLimit()) {
		return;
	}

	const auto pBufs = GetRefList(rdPtr);
	const auto toRemove = GetToRemove(rdPtr, pBufs);

	const auto curLimit = GetProcessMemoryUsageMB();
	const auto limit = min(curLimit, DEFAULT_MEMORYLIMIT) / 4;

	for (auto& pStr : toRemove) {
		if (!ExceedMemLimit(limit)) {
			break;
		}

		rdPtr->pData->pMemVideoLib->EraseItem(*pStr);
	}

	return;
}
