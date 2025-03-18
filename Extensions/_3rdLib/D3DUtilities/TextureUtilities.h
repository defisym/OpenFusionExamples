#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

// fusion surface info use pointer to pointer to hold texture & view
template<typename T>
inline T* CastPointer(void** ppTexture) {
    if (ppTexture == nullptr) { return nullptr; }
    return *reinterpret_cast<T**>(ppTexture);
}

template<typename T>
inline T* CastPointer(void* pTexture) {
    return reinterpret_cast<T*>(pTexture);
}

inline ID3D11Texture2D* CastTexturePointer(void** pTexture) {
    return CastPointer<ID3D11Texture2D>(pTexture);
}

inline ID3D11Texture2D* CastTexturePointer(void* pTexture) {
    return CastPointer<ID3D11Texture2D>(pTexture);
}

inline ID3D11RenderTargetView* CastRenderTargetViewPointer(void** pTexture) {
    return CastPointer<ID3D11RenderTargetView>(pTexture);
}

inline ID3D11RenderTargetView* CastRenderTargetViewPointer(void* pTexture) {
    return CastPointer<ID3D11RenderTargetView>(pTexture);
}

inline D3D11_TEXTURE2D_DESC GetTextureDesc(ID3D11Texture2D* pD3DTexture) {
    if (pD3DTexture == nullptr) { return {}; }
    
    D3D11_TEXTURE2D_DESC desc = {};
    pD3DTexture->GetDesc(&desc);

    return desc;
}

// use void* for texture to hide conversion routine
inline D3D11_TEXTURE2D_DESC GetTextureDesc(void* pTexture) {
    return GetTextureDesc(CastTexturePointer(pTexture));
}