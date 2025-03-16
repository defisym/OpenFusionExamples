// ReSharper disable CppInconsistentNaming
// ReSharper disable CppTooWideScope
#pragma once

#include <functional>
#include <directxmath.h>

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

inline void InitSurface(LPSURFACE& pSf, 
    const int width, const int height, 
    bool bHWA = false) {
	if (pSf == nullptr || pSf->GetWidth() != width || pSf->GetHeight() != height) {
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

inline void CopyTexture(const unsigned char* pData,
     LPSURFACE pRTTSf, bool bPm) {
    // FFMpeg Context
    const auto pCtx = (const FFMpeg::CopyToTextureContext*)pData;

    auto pFFMpegDevice = pCtx->pD3D11VADeciveCtx->device;
    auto pFFMpegDeviceCtx = pCtx->pD3D11VADeciveCtx->device_context;

    auto pFrameTexture = pCtx->pTexture;
    auto sharedHandle = pCtx->sharedHandle;

    // Format:      DXGI_FORMAT_NV12 (YUV 4:2:0)
    // Usage:       D3D11_USAGE_DEFAULT (GPU write & read)
    // Bind flags:  512 (D3D11_BIND_DECODER)
    //                  Set this flag to indicate that a 2D texture is used to 
    //                  receive output from the decoder API. The common way to 
    //                  create resources for a decoder output is by calling the 
    //                  ID3D11Device::CreateTexture2D method to create an array 
    //                  of 2D textures. However, you cannot use texture arrays 
    //                  that are created with this flag in calls to 
    //                  ID3D11Device::CreateShaderResourceView.
    auto FrameDesc = GetTextureDesc(pFrameTexture);

    // Fusion Context
    auto renderHelper = RenderHelper{ pRTTSf };     // ST_HWA_RTTEXTURE
    auto RTTInfo = GetSurfaceInfo(pRTTSf);

    auto pFusionDevice = CastPointer<ID3D11Device>((void*)RTTInfo.m_pD3D11Device);
    auto pFusionDeviceCtx = CastPointer<ID3D11DeviceContext>((void*)RTTInfo.m_pD3D11Context);

    auto pRTTTexture = CastTexturePointer((void**)RTTInfo.m_ppD3D11RenderTargetTexture);    
    auto pRTTTextureView = CastRenderTargetViewPointer((void**)RTTInfo.m_ppD3D11RenderTargetView);

    // Format:      DXGI_FORMAT_B8G8R8A8_UNORM
    // Usage:       D3D11_USAGE_DEFAULT (GPU write & read)
    // Bind flags:  40 (D3D11_BIND_SHADER_RESOURCE & D3D11_BIND_RENDER_TARGET)
    //              D3D11_BIND_SHADER_RESOURCE
    //                  Bind a buffer or texture to a shader stage; this flag cannot 
    //                  be used with the D3D11_MAP_WRITE_NO_OVERWRITE flag.
    //              D3D11_BIND_RENDER_TARGET
    //                  Bind a texture as a render target for the output-merger stage.
    auto RTTDesc = GetTextureDesc(pRTTTexture);

    // Compiler
    auto compiler = ShaderCompiler{ pFusionDevice };

    // Start Process

    // 1. Create a shared texture
    HRESULT hr = S_OK;
 
    // TODO 
    // 2 & 3 can be done in FFMpeg

    // 4. vertex shader    
    // do nothing this stage
    
    // TODO 
    // reuse shader
    auto [vsBlob, vs] = compiler.CreateVertexShader(L"D:\\Dev\\OpenFusionExamples\\Extensions\\FFMpeg\\Shader\\vs.hlsl", "Main");

    // input layouts

    // What we expect in shader
    // struct VSInput {
    //     float3 position : POSITION;
    //     float3 color : COLOR0;
    // };

    // We should then create an input layout exactly with this format
    using Position = DirectX::XMFLOAT3;
    using Color = DirectX::XMFLOAT3;

    struct VertexShaderInput {
        Position position = {};
        Color color = {};
    };

    D3D11_INPUT_ELEMENT_DESC vertexInputLayoutInfo[] = {
        {
            "POSITION",
            0,
            DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            offsetof(VertexShaderInput, position),
            D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "COLOR",
            0,
            DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            offsetof(VertexShaderInput, color),
            D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    ComPtr<ID3D11InputLayout> pInputLayout;
    hr = pFusionDevice->CreateInputLayout(vertexInputLayoutInfo, std::size(vertexInputLayoutInfo),
        vsBlob.Get()->GetBufferPointer(), vsBlob.Get()->GetBufferSize(), &pInputLayout);
    if (FAILED(hr)) { return; }

    pFusionDeviceCtx->IASetInputLayout(pInputLayout.Get());

    // vertex buffer

    // Center: (0, 0)
    // 
    //  (-1, 1)©°©¤©¤©Ð©¤©¤©´(1, 1)
    //         ©À©¤©¤©à©¤©¤©È
    //  (-1,-1)©¸©¤©¤©Ø©¤©¤©¼(1,-1)
    //

    constexpr Position POSITION_LT = { -1, 1, 0 };  // 0
    constexpr Position POSITION_RT = {  1, 1, 0 };  // 1
    constexpr Position POSITION_RB = {  1,-1, 0 };  // 2
    constexpr Position POSITION_LB = { -1,-1, 0 };  // 3

    constexpr Color COLOR_WHITE = { 1.0f, 1.0f, 1.0f };

    constexpr VertexShaderInput vertices[] = {
        { POSITION_LT, COLOR_WHITE },
        { POSITION_RT, COLOR_WHITE },
        { POSITION_RB, COLOR_WHITE },
        { POSITION_LB, COLOR_WHITE },
    };

    D3D11_BUFFER_DESC vsibd = {};
    vsibd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vsibd.ByteWidth = sizeof(vertices);
    vsibd.StructureByteStride = sizeof(VertexShaderInput);
    
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;

    ComPtr<ID3D11Buffer> pVertexBuffer;
    hr = pFusionDevice->CreateBuffer(&vsibd, &sd, &pVertexBuffer);
    if (FAILED(hr)) { return; }

    UINT stride = sizeof(VertexShaderInput);
    UINT offset = 0u;
    ID3D11Buffer* vertexBuffers[] = { pVertexBuffer.Get() };
    pFusionDeviceCtx->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

    // indice buffer

    // D3D render triangle in clockwise order

    //
    //  (0)©°©¤©¤©Ð©¤©¤©´(1)
    //     ©À©¤©¤©à©¤©¤©È
    //  (3)©¸©¤©¤©Ø©¤©¤©¼(2)
    //

    const UINT16 indices[] = {
        0,1,2,      // first
        0,2,3,      // second
    };

    auto indicesSize = std::size(indices);
    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.ByteWidth = sizeof(indices);
    ibd.StructureByteStride = sizeof(UINT16);

    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;

    ComPtr<ID3D11Buffer> pIndexBuffer;
    hr = pFusionDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);
    if (FAILED(hr)) { return; }

    pFusionDeviceCtx->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    pFusionDeviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pFusionDeviceCtx->VSSetShader(vs.Get(), 0, 0);

    // 5. rasterizer
    D3D11_VIEWPORT viewPort = {};
    viewPort.TopLeftX = 0;
    viewPort.TopLeftY = 0;
    viewPort.Width = (FLOAT)RTTDesc.Width;
    viewPort.Height = (FLOAT)RTTDesc.Height;
    viewPort.MaxDepth = 1;
    viewPort.MinDepth = 0;
    pFusionDeviceCtx->RSSetViewports(1, &viewPort);

    // 6. pixel shader
    // we do actual conversion here

    auto [psBlob, ps] = compiler.CreatePixelShader(L"D:\\Dev\\OpenFusionExamples\\Extensions\\FFMpeg\\Shader\\NV12ToBGRA.hlsl", "Main");

    // open shared resource
    ComPtr<ID3D11Texture2D> pSharedFrameTexture;
    hr = pFusionDevice->OpenSharedResource(sharedHandle, IID_PPV_ARGS(&pSharedFrameTexture));
    if (FAILED(hr)) { return; }

    // create srv
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    // create new shader resource view
    // NV12 cannot create srv directly, split to two planes
    ComPtr<ID3D11ShaderResourceView> pSrvY = nullptr;
    srvDesc.Format = DXGI_FORMAT_R8_UNORM;
    hr = pFusionDevice->CreateShaderResourceView(pSharedFrameTexture.Get(), &srvDesc, &pSrvY);
    if (FAILED(hr)) { return; }

    ComPtr<ID3D11ShaderResourceView> pSrvUV = nullptr;
    srvDesc.Format = DXGI_FORMAT_R8G8_UNORM;
    hr = pFusionDevice->CreateShaderResourceView(pSharedFrameTexture.Get(), &srvDesc, &pSrvUV);
    if (FAILED(hr)) { return; }

    ID3D11ShaderResourceView* srvs[] = { pSrvY.Get(), pSrvUV.Get() };
    pFusionDeviceCtx->PSSetShaderResources(0, std::size(srvs), srvs);

    // create sampler
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

    ComPtr<ID3D11SamplerState> pSamplerStateY = nullptr;
    hr = pFusionDevice->CreateSamplerState(&sampDesc, &pSamplerStateY);
    if (FAILED(hr)) { return; }

    ComPtr<ID3D11SamplerState> pSamplerStateUV = nullptr;
    hr = pFusionDevice->CreateSamplerState(&sampDesc, &pSamplerStateUV);
    if (FAILED(hr)) { return; }

    ID3D11SamplerState* sss[] = {
        pSamplerStateY.Get(),pSamplerStateUV.Get()
    };
    pFusionDeviceCtx->PSSetSamplers(0, std::size(sss), sss);

    // create constant buffer

    // size of buffer must be multiple of 16
    struct alignas(16) PixelSizeBuffer {
        float fPixelWidth;
        float fPixelHeight;
    };

    PixelSizeBuffer psb = { 1.0f / RTTDesc.Width, 1.0f / RTTDesc.Height };

    D3D11_BUFFER_DESC psbDesc = {};
    psbDesc.Usage = D3D11_USAGE_DEFAULT;
    psbDesc.ByteWidth = sizeof(PixelSizeBuffer);
    psbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    psbDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &psb;

    ComPtr<ID3D11Buffer> pConstantBuffer = nullptr;
    hr = pFusionDevice->CreateBuffer(&psbDesc, &initData, &pConstantBuffer);
    if (FAILED(hr)) { return; }

    ID3D11Buffer* cb[] = { pConstantBuffer.Get()};
    pFusionDeviceCtx->PSSetConstantBuffers(0, std::size(cb), cb);

    pFusionDeviceCtx->PSSetShader(ps.Get(), 0, 0);

    // 7. render
    ID3D11RenderTargetView* rtvs[] = { pRTTTextureView };
    pFusionDeviceCtx->OMSetRenderTargets(1, rtvs, nullptr);

    pFusionDeviceCtx->DrawIndexed(indicesSize, 0, 0);

    return;
}

inline void CopyData(LPRDATA rdPtr, LPSURFACE pDst,
    // passed in callback
    const unsigned char* pData, const int stride, const int height) {
    if (pData == nullptr || pDst == nullptr) { return; }

    // pDst must match bCopyToTexture, see `InitSurface`
    if (!rdPtr->bCopyToTexture) {
        CopyBitmap(pData, stride, pDst, rdPtr->bPm);
    }
    else {
        CopyTexture(pData, pDst, rdPtr->bPm);
    }
}

inline void BlitVideoFrame(LPRDATA rdPtr, size_t ms, const LPSURFACE& pSf) {
	if (rdPtr->pFFMpeg == nullptr) {
		return;
	}

	rdPtr->pFFMpeg->get_videoFrame(ms, rdPtr->bAccurateSeek, [&](const unsigned char* pData, const int stride, const int height) {
        CopyData(rdPtr, pSf, pData, stride, height);
		ReDisplay(rdPtr);
		});
}

inline void NextVideoFrame(LPRDATA rdPtr) {
	if (rdPtr->pFFMpeg == nullptr) {
		return;
	}

	rdPtr->pFFMpeg->get_nextFrame([&](const unsigned char* pData, const int stride, const int height) {
        CopyData(rdPtr, rdPtr->pMemSf, pData, stride, height);
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

// seek video to given position
// by default, it will decode to first valid frame (accurate seek enabled & not `AVSEEK_FLAG_BYTE`)
// and revert back if target timestamp is 0
// this behaviour can be controlled by flags
inline void SetPositionGeneral(LPRDATA rdPtr, int ms, int flags = SeekFlags) {
    if (!rdPtr->bOpen) { return; }

    // add protection for minus position
    ms = (std::max)(ms, 0);

    // in case of only one frame, aka static picture, only use goto
    // as seek for picture won't reset the context and cannot decode next time
    const auto bSingleFrame = VideoSingleFrame(rdPtr);

    //auto pos = rdPtr->pFFMpeg->get_videoPosition();
    if (!bSingleFrame) { rdPtr->pFFMpeg->set_videoPosition(ms, flags); }

    const bool bGoto = rdPtr->bAccurateSeek && (flags & AVSEEK_FLAG_BYTE) != AVSEEK_FLAG_BYTE;
    if (!(bGoto || bSingleFrame) || flags & SeekFlag_NoGoto) { return; }
    rdPtr->pFFMpeg->goto_videoPosition(ms, [&] (const unsigned char* pData, const int stride, const int height) {
        CopyData(rdPtr, rdPtr->pMemSf, pData, stride, height);     
        ReDisplay(rdPtr);
        });

    // revert
    if (ms != 0 || flags & SeekFlag_NoRevert) { return; }
    if (!bSingleFrame) { rdPtr->pFFMpeg->set_videoPosition(ms, flags); }
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
        | (rdPtr->bForceNoAudio ? FFMpegFlag_ForceNoAudio : 0)
        | (rdPtr->bCopyToTexture ? FFMpegFlag_CopyToTexture : 0);
	opt.videoCodecName = *rdPtr->pVideoOverrideCodecName;
	opt.audioCodecName = *rdPtr->pAudioOverrideCodecName;

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
        InitSurface(rdPtr->pMemSf,
            rdPtr->pFFMpeg->get_width(), rdPtr->pFFMpeg->get_height(),
            rdPtr->bCopyToTexture);
		// display first valid frame
		SetPositionGeneral(rdPtr, static_cast<int>(ms));

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
