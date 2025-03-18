#pragma once

// #embed macro is not supported by MSVC neither in C/C++
// for now, so the following method is MSVC exlcusive

inline auto GetShaderResource(const HMODULE hModule, const int resourceID) {
    void* pData = nullptr;
    size_t sz = 0;

    do {
        HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(resourceID), RT_RCDATA);
        if (!hResource) { break; }

        HGLOBAL hMemory = LoadResource(hModule, hResource);
        if (!hMemory) { break; }

        pData = LockResource(hMemory);    // no need to unlock
        sz = SizeofResource(hModule, hResource);
    } while (false);

    return std::make_tuple(pData, sz);
}
