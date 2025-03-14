#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

// fusion surface info use pointer to pointer to hold texture
inline ID3D11Texture2D* CastPointer(void** ppTexture) {
    if (ppTexture == nullptr) { return nullptr; }
    return *reinterpret_cast<ID3D11Texture2D**>(ppTexture);
}

inline ID3D11Texture2D* CastPointer(void* pTexture) {
    return reinterpret_cast<ID3D11Texture2D*>(pTexture);
}

inline D3D11_TEXTURE2D_DESC GetDesc(ID3D11Texture2D* pD3DTexture) {
    if (pD3DTexture == nullptr) { return {}; }
    
    D3D11_TEXTURE2D_DESC desc = {};
    pD3DTexture->GetDesc(&desc);

    return desc;
}

// use void* for texture to hide conversion routine
inline D3D11_TEXTURE2D_DESC GetDesc(void* pTexture) {
    return GetDesc(CastPointer(pTexture));
}