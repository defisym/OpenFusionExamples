#include "FFMpegAdapterD3D11.h"

AVD3D11VADeviceContext* FFMpegAdapterD3D11::GetHWCtx() {
    return (AVD3D11VADeviceContext*)pHWDCtx->hwctx;
}

FFMpegAdapterD3D11::FFMpegAdapterD3D11(AVHWDeviceContext* pHWDCtx) : FFMpegAdapter(pHWDCtx) {
    D3D11_QUERY_DESC queryDesc = { D3D11_QUERY_EVENT, 0 };
    GetHWCtx()->device->CreateQuery(&queryDesc, &pEvent);
}

void FFMpegAdapterD3D11::gpu_flush() {
    auto pHWCtx = GetHWCtx();
    if (pHWCtx == nullptr) { return; }

    pHWCtx->device_context->Flush();
}

void FFMpegAdapterD3D11::gpu_wait() {
    auto pHWCtx = GetHWCtx();
    if (pHWCtx == nullptr) { return; }

    pHWCtx->device_context->End(pEvent.Get());
    while (pHWCtx->device_context->GetData(pEvent.Get(), nullptr, 0, 0) == S_FALSE) {}
}

TextureContextHandle FFMpegAdapterD3D11::AllocContext() {
    return new CopyToTextureContext{};
}

void FFMpegAdapterD3D11::FreeContext(TextureContextHandle& h) {
    delete (CopyToTextureContext*)h;
    h = nullptr;
}

void FFMpegAdapterD3D11::convert_textureFrame(AVCodecContext* pCodecContext,
    AVFrame* pFrame, const FrameDataCallBack& callBack) {
    auto pHWCtx = GetHWCtx();
    if (pHWCtx == nullptr) { return; };

    auto pTexture = (ID3D11Texture2D*)pFrame->data[0];
    auto pIndex = (intptr_t)pFrame->data[1];

    auto pOpaque = (AVCodecCtxOpaque*)pCodecContext->opaque;
    auto pCTTCtx = (CopyToTextureContext*)pOpaque->hTextureCtx;

    // init
    if (pCTTCtx->pTexture == nullptr) {
        HRESULT hr = S_OK;

        // 1. create shared texture
        D3D11_TEXTURE2D_DESC sharedDesc = {};
        pTexture->GetDesc(&sharedDesc);

        sharedDesc.ArraySize = 1;
        sharedDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        sharedDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
        hr = pHWCtx->device->CreateTexture2D(&sharedDesc, nullptr, &pCTTCtx->pTexture);
        if (FAILED(hr)) { return; }

        // 2. Create shared handle    
        ComPtr<IDXGIResource> dxgiShareTexture;
        hr = pCTTCtx->pTexture->QueryInterface(IID_PPV_ARGS(&dxgiShareTexture));
        if (FAILED(hr)) { return; }

        pCTTCtx->sharedHandle = nullptr;
        hr = dxgiShareTexture->GetSharedHandle(&pCTTCtx->sharedHandle);
        if (FAILED(hr)) { return; }

        // 3. update device & texture format
        pCTTCtx->pD3D11VADeciveCtx = pHWCtx;
        pCTTCtx->textureFormat = sharedDesc.Format;
    }

    // failed to create
    if (pCTTCtx->sharedHandle == nullptr) { return; }

    // the result maybe an array texture
    // one solution is use CopySubresourceRegion to copy it to a new texture
    // and set SrcSubresource to pFrameIndex
    pHWCtx->device_context->CopySubresourceRegion(pCTTCtx->pTexture.Get(), 0, 0, 0, 0, pTexture, pIndex, 0);
    pHWCtx->device_context->Flush();
    //gpu_wait();

    //// another solution create a srv and use it as shader resource
    //D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    //srvDesc.Format = FrameDesc.Format;
    //srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    //
    //// decoded video texture only has one mip level
    //// MostDetailedMip: which one start to use, the max reslution one is 0
    //// MipLevels: how many we can use, only one is 1
    //
    //// ID3D11Texture2D is texture
    //if (pFrameIndex == 0) {
    //    srvDesc.Texture2D.MostDetailedMip = 0;
    //    srvDesc.Texture2D.MipLevels = 1;
    //}
    //// ID3D11Texture2D is an array texture
    //else {
    //    srvDesc.Texture2DArray.MostDetailedMip = 0;
    //    srvDesc.Texture2DArray.MipLevels = 1;
    //    srvDesc.Texture2DArray.FirstArraySlice = pFrameIndex;
    //    srvDesc.Texture2DArray.ArraySize = 1;
    //}

    callBack((const unsigned char*)(pCTTCtx), pFrame->width, pFrame->height);
}

// CopyToTextureContext only copy pD3D11VADeciveCtx (shared)
// other members shouldn't be copied!
void FFMpegAdapterD3D11::CopyContext(TextureContextHandle src, TextureContextHandle dst) {
    ((CopyToTextureContext*)dst)->pD3D11VADeciveCtx
        = ((CopyToTextureContext*)src)->pD3D11VADeciveCtx;
}
