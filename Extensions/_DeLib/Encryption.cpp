#include "Encryption.h"

Encryption::Encryption() {

}

Encryption::~Encryption() {
    Release(this->InputData);
    Release(this->OutputData);
    Release(this->Key);
    Release(this->IV);
    Release(this->InputStr);
    Release(this->OutputStr);

    Release(this->HashStr);
}

inline void Encryption::Release(void* Pointer){    
    delete[] Pointer;
    Pointer = nullptr;

    return;
}

void Encryption::OpenFile(const wchar_t* FileName) {    
    Release(this->InputData);
    
    FILE* fp = nullptr;
    
    _wfopen_s(&fp, FileName, L"rb");
    if (fp == nullptr) {
        return;
    }

    fseek(fp, 0, SEEK_END);
    this->InputLength = ftell(fp);
    rewind(fp);
    
    this->InputData = new BYTE[this->InputLength];
    fread(this->InputData, this->InputLength, 1, fp);
    fclose(fp);

    return;
}

void Encryption::SaveFile(const wchar_t* FileName, bool SaveSrc) {
    PBYTE Output = SaveSrc ? this->InputData : this->OutputData;
    DWORD Length = SaveSrc ? this->InputLength : this->OutputLength;

    if (Output == nullptr) {
        return;
    }

    FILE* fp = nullptr;

    _wfopen_s(&fp, FileName, L"wb");
    if (fp == nullptr) {
        return;
    }

    fwrite(Output, Length, 1, fp);
    fclose(fp);

    return;
}

void Encryption::SetEncryptStr(std::string& Str) {
    SetEncryptStr(Str.c_str(), Str.length());
}

void Encryption::SetEncryptStr(const char* Str, DWORD StrLength) {
    Release(this->InputData);

    this->InputLength = StrLength;
    this->InputData = new BYTE[this->InputLength];
    memcpy(this->InputData, Str, this->InputLength);

    return;
}

void Encryption::SetEncryptStr(std::wstring& Str) {
    SetEncryptStr(Str.c_str(), Str.length());
}

void Encryption::SetEncryptStr(const wchar_t* Str, DWORD StrLength) {
    Release(this->InputData);

    this->InputLength = StrLength * sizeof(wchar_t);
    this->InputData = new BYTE[this->InputLength];
    memcpy(this->InputData, Str, this->InputLength);

    return;
}

char* Encryption::GetInputStr() {
    if (this->InputData == nullptr) {
        return nullptr;
    }

    this->ReleaseInputStr();

    //allocate and ensure NULL terminated
    this->InputStr = new char[this->InputLength + 1];
    this->InputStr[this->InputLength] = 0;
    memcpy(this->InputStr, this->InputData, this->InputLength);

    return this->InputStr;
}

DWORD Encryption::GetInputStrLength() {
    return this->InputLength + 1;
}

void Encryption::ReleaseInputStr() {
    if (this->InputStr != nullptr) {
        delete[] this->InputStr;
    }
    this->InputStr = nullptr;
}

char* Encryption::GetOutputStr() {
    if (this->OutputData == nullptr) {
        return nullptr;
    }

    this->ReleaseOutputStr();

    //allocate and ensure NULL terminated
    this->OutputStr = new char[this->OutputLength + 1];
    this->OutputStr[this->OutputLength] = 0;
    memcpy(this->OutputStr, this->OutputData, this->OutputLength);

    return this->OutputStr;
}

DWORD Encryption::GetOutputStrLength() {
    return this->OutputLength + 1;
}

void Encryption::ReleaseOutputStr() {
    if (this->OutputStr != nullptr) {
        delete[] this->OutputStr;
    }
    this->OutputStr = nullptr;
}

void Encryption::GenerateKey(const wchar_t* KeyStr) {
    //release old
    Release(this->Key);
    Release(this->IV);
    
    //invalid keystr
    if ((sizeof(wchar_t) * wcslen(KeyStr)) < (this->KeyLength + this->IVLength)) {
        this->Key = new BYTE[16];        
        this->IV = new BYTE[16];

        memcpy(this->Key, this->DefaultKey, 16);
        memcpy(this->IV, this->DefaultIV, 16);

        return;
    }

    //Generate
    this->Key = new BYTE[16];
    this->IV = new BYTE[16];

    memcpy(this->Key, KeyStr, 8 * sizeof(wchar_t));
    memcpy(this->IV, KeyStr + 8, 8 * sizeof(wchar_t));

    return;
}

bool Encryption::Encrypt_Core(bool Encrypt, LPCWSTR Algorithm) {
    //do nothing if input is invalid
    if (this->InputData == nullptr) {
        return false;
    }

    //If no key generated, generate default key
    if ((this->IV == nullptr) || (this->Key == nullptr)) {
        this->GenerateKey(L"");
    }

    //Define Vars
    BCRYPT_ALG_HANDLE       hAesAlg = NULL;
    BCRYPT_KEY_HANDLE       hKey = NULL;
    NTSTATUS                status = STATUS_UNSUCCESSFUL;
    DWORD                   
        //cbOuputText = 0,
        //InputText = 0,
        cbData = 0,
        cbKeyObject = 0,
        cbBlockLen = 0,
        cbBlob = 0;
    PBYTE
        //pbOuputText = NULL,
        //pbInputText = NULL,
        pbKeyObject = NULL,
        pbIV = NULL,
        pbBlob = NULL;

    bool result = true;

    //General routine of En/Decrypt
    // Open an algorithm handle.
    if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
        &hAesAlg,
        Algorithm,
        NULL,
        0)))
    {
        //wprintf(L"**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
        result = false;
        goto Cleanup;
    }

    // Calculate the size of the buffer to hold the KeyObject.
    if (!NT_SUCCESS(status = BCryptGetProperty(
        hAesAlg,
        BCRYPT_OBJECT_LENGTH,
        (PBYTE)&cbKeyObject,
        sizeof(DWORD),
        &cbData,
        0)))
    {
        //wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
        result = false;
        goto Cleanup;
    }

    // Allocate the key object on the heap.
    pbKeyObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbKeyObject);
    if (NULL == pbKeyObject)
    {
        //wprintf(L"**** memory allocation failed\n");
        result = false;
        goto Cleanup;
    }

    // Calculate the block length for the IV.
    if (!NT_SUCCESS(status = BCryptGetProperty(
        hAesAlg,
        BCRYPT_BLOCK_LENGTH,
        (PBYTE)&cbBlockLen,
        sizeof(DWORD),
        &cbData,
        0)))
    {
        //wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
        result = false;
        goto Cleanup;
    }

    // Determine whether the cbBlockLen is not longer than the IV length.
    if (cbBlockLen > this->IVLength)
    {
        //wprintf(L"**** block length is longer than the provided IV length\n");
        result = false;
        goto Cleanup;
    }

    // Allocate a buffer for the IV. The buffer is consumed during the 
    // encrypt/decrypt process.
    pbIV = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbBlockLen);
    if (NULL == pbIV)
    {
        //wprintf(L"**** memory allocation failed\n");
        result = false;
        goto Cleanup;
    }

    memcpy(pbIV, this->IV, cbBlockLen);

    if (!NT_SUCCESS(status = BCryptSetProperty(
        hAesAlg,
        BCRYPT_CHAINING_MODE,
        (PBYTE)BCRYPT_CHAIN_MODE_CBC,
        sizeof(BCRYPT_CHAIN_MODE_CBC),
        0)))
    {
        //wprintf(L"**** Error 0x%x returned by BCryptSetProperty\n", status);
        result = false;
        goto Cleanup;
    }

    // Generate the key from supplied input key bytes.
    if (!NT_SUCCESS(status = BCryptGenerateSymmetricKey(
        hAesAlg,
        &hKey,
        pbKeyObject,
        cbKeyObject,
        this->Key,
        this->KeyLength,
        0)))
    {
        //wprintf(L"**** Error 0x%x returned by BCryptGenerateSymmetricKey\n", status);
        result = false;
        goto Cleanup;
    }

    ////Copy Text
    //cbInputText = this->InputLength;
    //pbInputText = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbInputText);
    //if (NULL == pbInputText)
    //{
    //    //wprintf(L"**** memory allocation failed\n");
    //    result = false;
    //    goto Cleanup;
    //}

    //memcpy(pbInputText, this->InputData, this->InputLength);
    
    // Get the output buffer size.
    if (!NT_SUCCESS(status = BCryptEncrypt(
        hKey,
        this->InputData,
        this->InputLength,
        NULL,
        pbIV,
        cbBlockLen,
        NULL,
        0,
        &(this->OutputLength),
        BCRYPT_BLOCK_PADDING)))
    {
        //wprintf(L"**** Error 0x%x returned by BCryptEncrypt\n", status);
        result = false;
        goto Cleanup;
    }

    //pbOuputText = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbOuputText);
    //if (NULL == pbOuputText)
    //{
    //    //wprintf(L"**** memory allocation failed\n");
    //    result = false;
    //    goto Cleanup;
    //}
    Release(this->OutputData);
    this->OutputData = new BYTE[this->OutputLength];
    
    if (NULL == this->OutputData)
    {
        // wprintf(L"**** memory allocation failed\n");
        result = false;
        goto Cleanup;
    }

    if (Encrypt) {
        // Use the key to encrypt the plaintext buffer.
        // For block sized messages, block padding will add an extra block.
        if (!NT_SUCCESS(status = BCryptEncrypt(
            hKey,
            this->InputData,
            this->InputLength,
            NULL,
            pbIV,
            cbBlockLen,
            this->OutputData,
            this->OutputLength,
            &cbData,
            BCRYPT_BLOCK_PADDING)))
        {
            //wprintf(L"**** Error 0x%x returned by BCryptEncrypt\n", status);
            result = false;
            goto Cleanup;
        }
    }
    else {
        if (!NT_SUCCESS(status = BCryptDecrypt(
            hKey,
            this->InputData,
            this->InputLength,
            NULL,
            pbIV,
            cbBlockLen,
            this->OutputData,
            this->OutputLength,
            &(this->OutputLength),
            BCRYPT_BLOCK_PADDING)))
        {
            //wprintf(L"**** Error 0x%x returned by BCryptDecrypt\n", status);
            result = false;
            goto Cleanup;
        }
    }

    ////Output
    ////Release old
    //Release(this->OutputData);
    ////Update output length
    //this->OutputLength = cbOuputText;
    ////Copy
    //this->OutputData = new BYTE[this->OutputLength];
    //memcpy(this->OutputData, pbOuputText, this->OutputLength);

Cleanup:

    if (hAesAlg)
    {
        BCryptCloseAlgorithmProvider(hAesAlg, 0);
    }

    if (hKey)
    {
        BCryptDestroyKey(hKey);
    }

    //if (pbOuputText)
    //{
    //    HeapFree(GetProcessHeap(), 0, pbOuputText);
    //}

    //if (pbInputText)
    //{
    //    HeapFree(GetProcessHeap(), 0, pbInputText);
    //}

    if (pbKeyObject)
    {
        HeapFree(GetProcessHeap(), 0, pbKeyObject);
    }

    if (pbIV)
    {
        HeapFree(GetProcessHeap(), 0, pbIV);
    }

    return result;
}

LPCWSTR Encryption::GetHash(LPCWSTR Algorithm) {
    //do nothing if input is invalid
    if (this->InputData == nullptr) {
        return nullptr;
    }    

    //Define Vars
    BCRYPT_ALG_HANDLE       hAlg = NULL;
    BCRYPT_HASH_HANDLE      hHash = NULL;
    NTSTATUS                status = STATUS_UNSUCCESSFUL;
    DWORD                   
        cbData = 0,
        //cbHash = 0,
        cbHashObject = 0;
    PBYTE
        //pbHash = NULL,
        pbHashObject = NULL;

    bool result = true;

    //open an algorithm handle
    if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
        &hAlg,
        //BCRYPT_SHA256_ALGORITHM,
        Algorithm,
        NULL,
        0)))
    {
        // wprintf(L"**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
        result = false;
        goto Cleanup;
    }

    //calculate the size of the buffer to hold the hash object
    if (!NT_SUCCESS(status = BCryptGetProperty(
        hAlg,
        BCRYPT_OBJECT_LENGTH,
        (PBYTE)&cbHashObject,
        sizeof(DWORD),
        &cbData,
        0)))
    {
        // wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
        result = false;
        goto Cleanup;
    }

    //allocate the hash object on the heap
    pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
    if (NULL == pbHashObject)
    {
        // wprintf(L"**** memory allocation failed\n");
        result = false;
        goto Cleanup;
    }

    //calculate the length of the hash
    //if (!NT_SUCCESS(status = BCryptGetProperty(
    //    hAlg,
    //    BCRYPT_HASH_LENGTH,
    //    (PBYTE)&cbHash,
    //    sizeof(DWORD),
    //    &cbData,
    //    0)))
    if (!NT_SUCCESS(status = BCryptGetProperty(
        hAlg,
        BCRYPT_HASH_LENGTH,
        (PBYTE)&(this->OutputLength),
        sizeof(DWORD),
        &cbData,
        0)))
    {
        // wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
        result = false;
        goto Cleanup;
    }

    //allocate the hash buffer on the heap
    //pbHash = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHash);
    //if (NULL == pbHash)
    //{
    //    // wprintf(L"**** memory allocation failed\n");
    //    result = false;
    //    goto Cleanup;
    //}

    Release(this->OutputData);
    this->OutputData = new BYTE[this->OutputLength];

    if (NULL == this->OutputData)
    {
        // wprintf(L"**** memory allocation failed\n");
        result = false;
        goto Cleanup;
    }

    //create a hash
    if (!NT_SUCCESS(status = BCryptCreateHash(
        hAlg,
        &hHash,
        pbHashObject,
        cbHashObject,
        NULL,
        0,
        0)))
    {
        // wprintf(L"**** Error 0x%x returned by BCryptCreateHash\n", status);
        result = false;
        goto Cleanup;
    }

    //hash some data
    //if (!NT_SUCCESS(status = BCryptHashData(
    //    hHash,
    //    (PBYTE)this->InputData,
    //    sizeof(this->InputData),
    //    0)))
    if (!NT_SUCCESS(status = BCryptHashData(
        hHash,
        this->InputData,
        this->InputLength,
        0)))
    {
        // wprintf(L"**** Error 0x%x returned by BCryptHashData\n", status);
        result = false;
        goto Cleanup;
    }

    //close the hash
    if (!NT_SUCCESS(status = BCryptFinishHash(
        hHash,
        this->OutputData,
        this->OutputLength,
        0)))
    {
        // wprintf(L"**** Error 0x%x returned by BCryptFinishHash\n", status);
        result = false;
        goto Cleanup;
    }
    
    //Output
    //Release old
    Release(this->HashStr);
    //Update output length
    //CryptBinaryToString(pbHash, cbHash, CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF, NULL, &this->HashLength);
    CryptBinaryToString(this->OutputData, this->OutputLength, CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF, NULL, &this->HashLength);
    //Copy
    this->HashStr = new WCHAR[this->HashLength];
    //CryptBinaryToString(pbHash, cbHash, CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF, this->HashStr, &this->HashLength);
    CryptBinaryToString(this->OutputData, this->OutputLength, CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF, this->HashStr, &this->HashLength);

Cleanup:

    if (hAlg)
    {
        BCryptCloseAlgorithmProvider(hAlg, 0);
    }

    if (hHash)
    {
        BCryptDestroyHash(hHash);
    }

    if (pbHashObject)
    {
        HeapFree(GetProcessHeap(), 0, pbHashObject);
    }

    //if (pbHash)
    //{
    //    HeapFree(GetProcessHeap(), 0, pbHash);
    //}

    return result ? this->HashStr : nullptr;
}
