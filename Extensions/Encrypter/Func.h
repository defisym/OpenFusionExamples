#pragma once

#define USE_PROCESS_DIRECTLY

inline bool EncryptCore(LPCTSTR pSrcFilePath, LPCTSTR pDesFilePath, LPCTSTR pKey, encryption::Mode mode) {
    Encryption Encrypt;

    Encrypt.GenerateKey(pKey);

#ifdef USE_PROCESS_DIRECTLY
    auto ret = mode == encryption::Mode::encrypt
        ? Encrypt.EncryptFileDirectly(pSrcFilePath)
        : Encrypt.DecryptFileDirectly(pSrcFilePath);
#else
    auto ret = mode == encryption::Mode::encrypt
        ? Encrypt.EncryptFile(pSrcFilePath)
        : Encrypt.DecryptFile(pSrcFilePath);
#endif

    if (!ret) {
        return false;
    }

    Encrypt.SaveFile(pDesFilePath);

    return true;
}