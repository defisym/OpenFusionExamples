// ReSharper disable CppInconsistentNaming
// ReSharper disable CppTooWideScope
#pragma once

#include <functional>

#include "FFMpegAdapterD3D11.h"

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

inline void InitSurface(LPSURFACE& pSf, 
    const int width, const int height, 
    bool bHWA = false) {
    if (pSf == nullptr
        || pSf->GetWidth() != width || pSf->GetHeight() != height
        || IsHWA(pSf) != bHWA) {
        if (!bHWA) {
#ifdef VIDEO_ALPHA
            pSf = CreateSurface(32, width, height);
#else
            pSf = CreateSurface(24, width, height);
#endif
        }
        else {
            pSf = CreateHWASurface(32, width, height, ST_HWA_RTTEXTURE);
        }

        // alpha channel is ignored if surface type is ST_HWA_RTTEXTURE
        _AddAlpha(pSf);
	}
}

inline void CopyBitmap(const unsigned char* pData, int srcLineSz,
    LPSURFACE pMemSf, bool bPm) {
	// pMemSf must have alpha channel, see `InitSurface`
	auto sfCoef = GetSfCoef(pMemSf);
	if (sfCoef.pData == nullptr || sfCoef.pAlphaData == nullptr) {
		return;
	}

	auto lineSz = sfCoef.pitch;	
	auto alphaSz = sfCoef.alphaSz / sfCoef.alphaByte;

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
			const auto line = (height - 1 - y);

			auto pMemData = sfCoef.pData + y * lineSz;
			auto pVideo = pData + line * srcLineSz;

#ifndef _MANUAL_PM
			memcpy(pMemData, pVideo, lineSz);
#ifdef VIDEO_ALPHA
			// 32 bit: 4 bytes per pixel: blue, green, red, unused (0)
#ifdef _OPENMP
//#pragma omp for
#endif
			const auto pSfAlphaOffset = sfCoef.pAlphaData + line * sfCoef.alphaPitch;
			const auto pBitmapAlphaOffset = pVideo + (PIXEL_BYTE - 1);

			for (int x = 0; x < width; x++) {
				auto pAlphaData = pSfAlphaOffset + x * sfCoef.alphaByte;
				const auto pCurAlpha = pBitmapAlphaOffset + x * PIXEL_BYTE;
				pAlphaData[0] = pCurAlpha[0];
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
				auto pAlphaData = pSfAlphaOffset + x * sfCoef.alphaByte;
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

struct CopyTextureContext {
    D3DSharedHandler* pD3DSharedHandler = nullptr;
    D3DLocalHandler* pD3DLocalHandler = nullptr;
    const CopyToTextureContext* pFFMpegCtx = nullptr;
};

inline void CopyTexture(const unsigned char* pData, const int width, const int height,
     LPSURFACE pRTTSf, bool bPm) {
    // 1. basic info
    const auto pCtx = (const CopyTextureContext*)pData;
    auto pD3DSharedHandler = pCtx->pD3DSharedHandler;
    auto pD3DLocalHandler = pCtx->pD3DLocalHandler;
    
    HRESULT hr = S_OK;

    // Format:      DXGI_FORMAT_NV12 (YUV 4:2:0)
    //              DXGI_FORMAT_P010 (YUV 4:2:2)
    //              DXGI_FORMAT_P010 (YUV 4:2:2)
    // Usage:       D3D11_USAGE_DEFAULT (GPU write & read)
    // Bind flags:  512 (D3D11_BIND_DECODER)
    //                  Set this flag to indicate that a 2D texture is used to 
    //                  receive output from the decoder API. The common way to 
    //                  create resources for a decoder output is by calling the 
    //                  ID3D11Device::CreateTexture2D method to create an array 
    //                  of 2D textures. However, you cannot use texture arrays 
    //                  that are created with this flag in calls to 
    //                  ID3D11Device::CreateShaderResourceView.
    auto sharedHandle = pCtx->pFFMpegCtx->sharedHandle;
    auto textureFormat = pCtx->pFFMpegCtx->textureFormat;

    // Fusion Context
    auto renderHelper = RenderHelper{ pRTTSf };     // ST_HWA_RTTEXTURE
    auto RTTInfo = GetSurfaceInfo(pRTTSf);

    auto pFusionDevice = (ID3D11Device*)RTTInfo.m_pD3D11Device;
    auto pFusionDeviceCtx = (ID3D11DeviceContext*)RTTInfo.m_pD3D11Context;

    // Format:      DXGI_FORMAT_B8G8R8A8_UNORM
    // Usage:       D3D11_USAGE_DEFAULT (GPU write & read)
    // Bind flags:  40 (D3D11_BIND_SHADER_RESOURCE & D3D11_BIND_RENDER_TARGET)
    //              D3D11_BIND_SHADER_RESOURCE
    //                  Bind a buffer or texture to a shader stage; this flag cannot 
    //                  be used with the D3D11_MAP_WRITE_NO_OVERWRITE flag.
    //              D3D11_BIND_RENDER_TARGET
    //                  Bind a texture as a render target for the output-merger stage.
    auto pRTTexture = *(ID3D11Texture2D**)(RTTInfo.m_ppD3D11RenderTargetTexture);    
    auto pRTTextureView = *(ID3D11RenderTargetView**)(RTTInfo.m_ppD3D11RenderTargetView);

    // 2. vertex shader
    pD3DSharedHandler->vertexHelper.UpdateContext(pFusionDeviceCtx);

    // 3. pixel shader
    pD3DSharedHandler->pixelHelper.UpdateContext(pFusionDeviceCtx);

    // 4. resolution & shared texture
    pD3DLocalHandler->UpdateResolution(pFusionDeviceCtx, width, height);
    if (FAILED(pD3DLocalHandler->hr)) { return; }
    pD3DLocalHandler->UpdateSharedTexture(pFusionDeviceCtx, sharedHandle, textureFormat);
    if (FAILED(pD3DLocalHandler->hr)) { return; }

    pD3DLocalHandler->UpdateContext(pFusionDeviceCtx);

    // 5. render
    ID3D11RenderTargetView* rtvs[] = { pRTTextureView };
    pFusionDeviceCtx->OMSetRenderTargets(1, rtvs, nullptr);

    pFusionDeviceCtx->DrawIndexed(pD3DSharedHandler->vertexHelper.indicesSize, 0, 0);
}

inline void CopyData(LPRDATA rdPtr, LPSURFACE pDst,
    // passed in callback
    const unsigned char* pData, const int width, const int height) {
    if (pData == nullptr || pDst == nullptr) { return; }

    // pDst must match bCopyToTexture, see `InitSurface`
    if (!rdPtr->pFFMpeg->get_copyToTextureState()) {
        CopyBitmap(pData, width, pDst, rdPtr->bPm);
    }
    else {
        auto ctx = CopyTextureContext{
        .pD3DSharedHandler = rdPtr->pData->pD3DSharedHandler,
        .pD3DLocalHandler = rdPtr->pD3DLocalHandler,
        .pFFMpegCtx = (const CopyToTextureContext*)pData
        };

        CopyTexture((const unsigned char*)(&ctx), width, height, pDst, rdPtr->bPm);
    }
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

		// update display
        UpdateScale(rdPtr, rdPtr->pFFMpeg->get_width(), rdPtr->pFFMpeg->get_height());
        InitSurface(rdPtr->pDisplaySf,
            rdPtr->pFFMpeg->get_width(), rdPtr->pFFMpeg->get_height(),
            rdPtr->pFFMpeg->get_copyToTextureState());
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
