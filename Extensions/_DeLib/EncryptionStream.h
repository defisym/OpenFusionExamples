#pragma once

#include "Encryption.h"

class EncryptionStreamHelper {
private:
    //Define Vars
    BCRYPT_ALG_HANDLE       hAesAlg = NULL;
    BCRYPT_KEY_HANDLE       hKey = NULL;
    NTSTATUS                status = STATUS_UNSUCCESSFUL;

    DWORD
        cbData = 0,
        cbKeyObject = 0,
        cbBlockLen = 0;
    PBYTE
        pbKeyObject = NULL,
        pbIV = NULL;

public:
    EncryptionStreamHelper(const wchar_t* pKeyStr, LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM) {
        // make key
        Encryption e;
        e.GenerateKey(pKeyStr);

        auto& [pKey, keyLength,
            pIV, ivLength] = e.GetKeyInfo();

        // General routine of En/Decrypt
        // Open an algorithm handle.
        if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
            &hAesAlg,
            Algorithm,
            NULL,
            0))) {
            return;
        }

        // Calculate the size of the buffer to hold the KeyObject.
        if (!NT_SUCCESS(status = BCryptGetProperty(
            hAesAlg,
            BCRYPT_OBJECT_LENGTH,
            (PBYTE)&cbKeyObject, sizeof(DWORD),
            &cbData, 0))) {
            return;
        }

        // Allocate the key object on the heap.
        pbKeyObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbKeyObject);
        if (NULL == pbKeyObject) {
            return;
        }

        // Calculate the block length for the IV.
        if (!NT_SUCCESS(status = BCryptGetProperty(
            hAesAlg,
            BCRYPT_BLOCK_LENGTH,
            (PBYTE)&cbBlockLen, sizeof(DWORD),
            &cbData, 0))) {
            return;
        }

        // Determine whether the cbBlockLen is not longer than the IV length.
        if (cbBlockLen > ivLength) {
            return;
        }

        // Allocate a buffer for the IV. The buffer is consumed during the 
        // encrypt/decrypt process.
        pbIV = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbBlockLen);
        if (NULL == pbIV) {
            return;
        }

        memcpy(pbIV, pIV, cbBlockLen);

        if (!NT_SUCCESS(status = BCryptSetProperty(
            hAesAlg,
            BCRYPT_CHAINING_MODE,
            (PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC),
            0))) {
            return;
        }

        // Generate the key from supplied input key bytes.
        if (!NT_SUCCESS(status = BCryptGenerateSymmetricKey(
            hAesAlg,
            &hKey,
            pbKeyObject, cbKeyObject,
            pKey, keyLength,
            0))) {
            return;
        }

    }
    ~EncryptionStreamHelper() {
        if (hAesAlg) {
            BCryptCloseAlgorithmProvider(hAesAlg, 0);
        }

        if (hKey) {
            BCryptDestroyKey(hKey);
        }

        if (pbKeyObject) {
            HeapFree(GetProcessHeap(), 0, pbKeyObject);
        }

        if (pbIV) {
            HeapFree(GetProcessHeap(), 0, pbIV);
        }
    }

    using HandlerType = decltype(BCryptEncrypt);

    inline size_t Stream(PBYTE pInBuf, size_t inBufSz,
    PBYTE pOutBuf, size_t outBufSz,
    HandlerType handler,
    bool bPadding) {
        DWORD byteWrite = 0;

        handler(
            hKey,
            pInBuf, inBufSz,
            NULL,
            pbIV, cbBlockLen,
            pOutBuf, outBufSz,
            &(byteWrite),
            bPadding ? BCRYPT_BLOCK_PADDING : 0);
    }
};