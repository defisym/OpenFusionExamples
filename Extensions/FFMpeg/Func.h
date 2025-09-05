// ReSharper disable CppInconsistentNaming
// ReSharper disable CppTooWideScope
#pragma once

#include <functional>

#include "FFMpegAdapter.h"
#include "CopyAdapter.h"

// -----------------------------
// Forward declaration
// -----------------------------

inline void CleanCache(LPRDATA rdPtr, bool forceClean = false);

// -----------------------------
// Display
// -----------------------------

inline void UpdateScale(LPRDATA rdPtr, int width, int height) {
	if (rdPtr->bStretch) {
		rdPtr->rc.rcScaleX = static_cast<float>(static_cast<double>(rdPtr->swidth) / width);
		rdPtr->rc.rcScaleY = static_cast<float>(static_cast<double>(rdPtr->sheight) / height);
	}
	else {
		rdPtr->rc.rcScaleX = 1.0;
		rdPtr->rc.rcScaleY = 1.0;
	}
}

inline void ReDisplay(LPRDATA rdPtr) {
	if (rdPtr->pDisplaySf != nullptr && rdPtr->pDisplaySf->IsValid()) {
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

inline void CopyData(LPRDATA rdPtr, LPSURFACE pDst,
    // passed in callback
    const unsigned char* pData, const int width, const int height) {
    if (pData == nullptr || pDst == nullptr) { return; }

    // pDst must match bCopyToTexture, see `InitSurface`
    rdPtr->pCopyAdapter->CopyTexture(pDst, pData, width, height);
}

inline void BlitVideoFrame(LPRDATA rdPtr, size_t ms, const LPSURFACE& pSf) {
	if (rdPtr->pFFMpeg == nullptr) {
		return;
	}

	rdPtr->pFFMpeg->get_videoFrame(ms, rdPtr->bAccurateSeek, [&](const unsigned char* pData, const int stride, const int height) {
        if (rdPtr->pFFMpeg->get_copyToTextureState()) { rdPtr->pFFMpeg->gpu_wait(); }

        CopyData(rdPtr, pSf, pData, stride, height);
		ReDisplay(rdPtr);
		});
}

inline void NextVideoFrame(LPRDATA rdPtr) {
	if (rdPtr->pFFMpeg == nullptr) {
		return;
	}

	rdPtr->pFFMpeg->get_nextFrame([&](const unsigned char* pData, const int stride, const int height) {
        CopyData(rdPtr, rdPtr->pDisplaySf, pData, stride, height);
		ReDisplay(rdPtr);
		});
}

// convert pSrc to pDst if needed
inline long ReturnVideoFrame(LPRDATA rdPtr, bool bWantHWA, const LPSURFACE& pSrc, LPSURFACE& pDst) {
    // want bitmap
	if (!bWantHWA) {
        // source is bitmap
        if(!IsHWA(pSrc)){ return ConvertToLong(pSrc); }
        // source is HWA
        else {
            delete pDst;
            pDst = nullptr;
            pDst = ConvertBitmap(rdPtr, pSrc);

            return ConvertToLong(pSrc);
        }
	}
    // want HWA
	else {
        // source is HWA
        if (IsHWA(pSrc)) { return ConvertToLong(pSrc); }
        // source is bitmap
        else {
            delete pDst;
            pDst = nullptr;
            pDst = ConvertHWATexture(rdPtr, pSrc);

            return ConvertToLong(pDst);
        }
	}
}

// -----------------------------
// Video control
// -----------------------------

inline bool VideoSingleFrame(LPRDATA rdPtr) {
    return rdPtr->pFFMpeg->get_videoFrameCount() == 1;
}

constexpr auto SeekFlag_NoGoto = 0x0001 << 16;	// do not decode to next valid frame
constexpr auto SeekFlag_NoRevert = 0x0002 << 16;	// do not revert if timestamp is 0

// seek video to given position, return false on decode error
// by default, it will decode to first valid frame (accurate seek enabled & not `AVSEEK_FLAG_BYTE`)
// and revert back if target timestamp is 0
// this behaviour can be controlled by flags
inline bool SetPositionGeneral(LPRDATA rdPtr, int ms, int flags = SeekFlags) {
    if (!rdPtr->bOpen) { return true; }

    // add protection for minus position
    ms = (std::max)(ms, 0);

    // in case of only one frame, aka static picture, only use goto
    // as seek for picture won't reset the context and cannot decode next time
    const auto bSingleFrame = VideoSingleFrame(rdPtr);

    int response = 0;

    //auto pos = rdPtr->pFFMpeg->get_videoPosition();
    if (!bSingleFrame) { response = rdPtr->pFFMpeg->set_videoPosition(ms, flags); }
    if (response < 0) { return false; }

    const bool bGoto = rdPtr->bAccurateSeek && (flags & AVSEEK_FLAG_BYTE) != AVSEEK_FLAG_BYTE;
    if (!(bGoto || bSingleFrame) || flags & SeekFlag_NoGoto) { return true; }
    response = rdPtr->pFFMpeg->goto_videoPosition(ms, [&] (const unsigned char* pData, const int stride, const int height) {
        // to avoid green screen at start
        if (rdPtr->pFFMpeg->get_copyToTextureState()) { rdPtr->pFFMpeg->gpu_wait(); }

        CopyData(rdPtr, rdPtr->pDisplaySf, pData, stride, height);
        ReDisplay(rdPtr);
        });
    if (response < 0) { return false; }

    // revert
    if (ms != 0 || flags & SeekFlag_NoRevert) { return true; }
    if (!bSingleFrame) { response = rdPtr->pFFMpeg->set_videoPosition(ms, flags); }
    if (response < 0) { return false; }

    return true;
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

inline Encryption* LoadMemVideo(LPRDATA rdPtr, const std::wstring& filePath, const std::wstring& key) {
	const auto pMemVideoLib = rdPtr->pData->pMemVideoLib;

	if (rdPtr->bCache) {
		auto it = pMemVideoLib->GetItem(filePath);

		if (pMemVideoLib->ItemExist(it)) {
			return it->second;
		}
	}

	const auto pEncrypt = new Encryption;
	pEncrypt->GenerateKey(key.c_str());

	try {		
		pEncrypt->DecryptFileDirectly(filePath.c_str());
	}
	catch ([[maybe_unused]] std::bad_alloc& e) {
		if (rdPtr->bCache) {
			CleanCache(rdPtr, true);
			pEncrypt->DecryptFileDirectly(filePath.c_str());			
		}
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

inline bool CopyToTextureValid(const AVHWDeviceType type) {
    return FFMpegAdapterSupport(type) && CopyAdapterSupport(type);
}

inline FFMpegOptions GetOptions(LPRDATA rdPtr) {
	FFMpegOptions opt;

    opt.flag = rdPtr->hwDeviceType
        | (rdPtr->bForceNoAudio ? FFMpegFlag_ForceNoAudio : FFMpegFlag_Disabled)
        | (rdPtr->bCopyToTexture ? FFMpegFlag_CopyToTexture : FFMpegFlag_Disabled)
        | (rdPtr->bSharedHardWareDevice ? FFMpegFlag_SharedHardWareDevice : FFMpegFlag_Disabled);

	opt.videoCodecName = *rdPtr->pVideoOverrideCodecName;
	opt.audioCodecName = *rdPtr->pAudioOverrideCodecName;
    
    opt.SetThreadCount(rdPtr->threadCount);

	return opt;
}

inline void OpenGeneral(LPRDATA rdPtr, std::wstring& filePath, std::wstring& key,
	const FFMpegOptions& opt = FFMpegOptions(), const size_t ms = 0) {
	CloseGeneral(rdPtr);

	try {
		do {
			// URL:
			//	http://...	https://...
			//	ftp://...	file://...
			const auto bUrl = [&] {
				const auto idx = filePath.find_first_of(L':');
				if (idx == std::wstring::npos) { return false; }
				if (filePath[idx + 1] != L'/' || filePath[idx + 2] != L'/') { return false; }

				const auto prefix = filePath.substr(0, idx);
				return StrIEqu(prefix.c_str(), L"http")
					|| StrIEqu(prefix.c_str(), L"https")
					|| StrIEqu(prefix.c_str(), L"ftp")
					|| StrIEqu(prefix.c_str(), L"file");
				}();

			if (bUrl) {
				rdPtr->pFFMpeg = new FFMpeg(filePath, opt);
				break;
			}

			// Pointer address
			//	validation of address is not granteed
			const auto pData = reinterpret_cast<uint8_t*>(ston<size_t>(filePath.c_str()));  // NOLINT(performance-no-int-to-ptr)
			const auto sz = ston<size_t>(key.c_str());
			const auto bFromMem = pData != nullptr && sz != 0;

			if(bFromMem) {
				rdPtr->pFFMpeg = new FFMpeg(pData, sz, opt);
				break;
			}

			// Unencrpyted file
			if (StrEmpty(key.c_str())) {
				rdPtr->pFFMpeg = new FFMpeg(filePath, opt);
				break;
			}

			// Encrypted file
			rdPtr->pEncrypt = LoadMemVideo(rdPtr, filePath, key);
			rdPtr->pFFMpeg = new FFMpeg(rdPtr->pEncrypt->GetOutputData(), rdPtr->pEncrypt->GetOutputDataLength(), opt);
		} while (false);

		// update state
		rdPtr->bOpen = true;
		*rdPtr->pFilePath = filePath;
		rdPtr->bPlay = rdPtr->bPlayAfterLoad;
		rdPtr->bPlayStateUpdated = true;

        // update copy adapter
        UpdateCopyAdapter(rdPtr->pCopyAdapter, rdPtr, rdPtr->pFFMpeg->get_hwDeviceType());

		// update display
        UpdateScale(rdPtr, rdPtr->pFFMpeg->get_width(), rdPtr->pFFMpeg->get_height());
        rdPtr->pCopyAdapter->InitTexture(rdPtr->pDisplaySf,
            rdPtr->pFFMpeg->get_width(), rdPtr->pFFMpeg->get_height());
		
        // display first valid frame
        if (!SetPositionGeneral(rdPtr, static_cast<int>(ms))) {
            // failed to display first frame, which indicates error
            throw FFMpegException(FFMpegException_DecodeFailed);
        }

		// update FFMpeg
		// audio pause is updated in handle routine, in case event cost a long time
		// rdPtr->pFFMpeg->set_pause(!rdPtr->bPlay, false);

		rdPtr->pFFMpeg->set_loop(rdPtr->bLoop);
		rdPtr->pFFMpeg->set_volume(rdPtr->volume);

		rdPtr->pFFMpeg->set_audioTempo(rdPtr->atempo);
		rdPtr->atempo = rdPtr->pFFMpeg->get_audioTempo();

		ReDisplay(rdPtr);
	}
	catch ([[maybe_unused]] FFMpegException& e) {
		CloseGeneral(rdPtr);

        // fallback to software decode
        if (rdPtr->hwDeviceType != AV_HWDEVICE_TYPE_NONE || rdPtr->bCopyToTexture) {
            rdPtr->hwDeviceType = AV_HWDEVICE_TYPE_NONE;
            rdPtr->bCopyToTexture = false;

            OpenGeneral(rdPtr, filePath, key, GetOptions(rdPtr), ms);
        }
        // fallback to default decoder
        else if (!opt.NoOverride()) {
            FFMpegOptions newOpt = opt;
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

// -----------------------------
// Cache control
// -----------------------------

inline auto GetRefList(LPRDATA rdPtr) {
	std::vector<const uint8_t*> pBufs;

	if(rdPtr->pData->pMemVideoLib->data.empty()) {
		return pBufs;
	}

	pBufs.reserve(rdPtr->pData->pMemVideoLib->data.size());

	for (const auto& ppFFMpeg : rdPtr->pData->ppFFMpegs_record) {
		const auto pFFMpeg = *ppFFMpeg;

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

	if (rdPtr->pData->pMemVideoLib->data.empty()) {
		return toRemove;
	}

	toRemove.reserve((std::max)(0u, rdPtr->pData->pMemVideoLib->data.size() - pBufs.size()));

	for (auto& [name, pEncrypt] : rdPtr->pData->pMemVideoLib->data) {
		auto it = std::ranges::find(pBufs, pEncrypt->GetOutputData());

		// not referred
		if (it == pBufs.end()) {
			toRemove.emplace_back(&name);
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

	if (rdPtr->pData->pMemVideoLib->data.empty()) {
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
