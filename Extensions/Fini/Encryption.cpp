#include "Encryption.h"

Encryption::Encryption() {

}

Encryption::~Encryption() {
    Release(this->InputText);
    Release(this->OutputText);
    Release(this->Key);
    Release(this->IV);
}

inline void Encryption::Release(PBYTE Pointer) {
    if (Pointer != nullptr) {
        delete[] Pointer;
    }
    Pointer = nullptr;

    return;
}

void Encryption::OpenFile(const wchar_t* FileName) {
    Release(this->InputText);

    FILE* fp = nullptr;

    _wfopen_s(&fp, FileName, L"rb");
    if (fp == nullptr) {
        return;
    }

    fseek(fp, 0, SEEK_END);
    this->InputLength = ftell(fp);
    rewind(fp);

    this->InputText = new BYTE[this->InputLength];
    fread(this->InputText, this->InputLength, 1, fp);
    fclose(fp);

    return;
}

void Encryption::SaveFile(const wchar_t* FileName) {
    if (this->OutputText == nullptr) {
        return;
    }

    FILE* fp = nullptr;

    _wfopen_s(&fp, FileName, L"wb");
    if (fp == nullptr) {
        return;
    }

    fwrite(this->OutputText, this->OutputLength, 1, fp);
    fclose(fp);

    return;
}

void Encryption::SetEncryptStr(std::string& Str) {
    SetEncryptStr(Str.c_str(), Str.length());
}

void Encryption::SetEncryptStr(const char* Str, DWORD StrLength) {
    Release(this->InputText);

    this->InputLength = StrLength;
    this->InputText = new BYTE[this->InputLength];
    memcpy(this->InputText, Str, this->InputLength);

    return;
}

void Encryption::SetEncryptStr(std::wstring& Str) {
    SetEncryptStr(Str.c_str(), Str.length());
}

void Encryption::SetEncryptStr(const wchar_t* Str, DWORD StrLength) {
    Release(this->InputText);

    this->InputLength = StrLength * sizeof(wchar_t);
    this->InputText = new BYTE[this->InputLength];
    memcpy(this->InputText, Str, this->InputLength);

    return;
}

char* Encryption::GetInputStr() {
    if (this->InputText == nullptr) {
        return nullptr;
    }

    this->ReleaseInputStr();

    //allocate and ensure NULL terminated
    this->InputStr = new char[this->InputLength + 1];
    this->InputStr[this->InputLength] = 0;
    memcpy(this->InputStr, this->InputText, this->InputLength);

    return this->InputStr;
}

void Encryption::ReleaseInputStr() {
    if (this->InputStr != nullptr) {
        delete[] this->InputStr;
    }
    this->InputStr = nullptr;
}

DWORD Encryption::GetInputStrLength() {
    return this->InputLength + 1;
}

char* Encryption::GetOutputStr() {
    if (this->OutputText == nullptr) {
        return nullptr;
    }

    this->ReleaseOutputStr();

    //allocate and ensure NULL terminated
    this->OutputStr = new char[this->OutputLength + 1];
    this->OutputStr[this->OutputLength] = 0;
    memcpy(this->OutputStr, this->OutputText, this->OutputLength);

    return this->OutputStr;
}

void Encryption::ReleaseOutputStr() {
    if (this->OutputStr != nullptr) {
        delete[] this->OutputStr;
    }
    this->OutputStr = nullptr;
}

DWORD Encryption::GetOutputStrLength() {
    return this->OutputLength + 1;
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

bool Encryption::Encrypt_Core(bool Encrypt) {
    //If no key generated, generate default key
    if ((this->IV == nullptr) || (this->Key == nullptr)) {
        this->GenerateKey(L"");
    }

    //Define Vars
    BCRYPT_ALG_HANDLE       hAesAlg = NULL;
    BCRYPT_KEY_HANDLE       hKey = NULL;
    NTSTATUS                status = STATUS_UNSUCCESSFUL;
    DWORD
        cbOuputText = 0,
        cbInputText = 0,
        cbData = 0,
        cbKeyObject = 0,
        cbBlockLen = 0,
        cbBlob = 0;
    PBYTE
        pbOuputText = NULL,
        pbInputText = NULL,
        pbKeyObject = NULL,
        pbIV = NULL,
        pbBlob = NULL;

    bool result = true;

    //General routine of En/Decrypt
    // Open an algorithm handle.
    if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
        &hAesAlg,
        BCRYPT_AES_ALGORITHM,
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

    //Copy Text
    cbInputText = this->InputLength;
    pbInputText = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbInputText);
    if (NULL == pbInputText)
    {
        //wprintf(L"**** memory allocation failed\n");
        result = false;
        goto Cleanup;
    }

    memcpy(pbInputText, this->InputText, this->InputLength);

    // Get the output buffer size.
    if (!NT_SUCCESS(status = BCryptEncrypt(
        hKey,
        pbInputText,
        cbInputText,
        NULL,
        pbIV,
        cbBlockLen,
        NULL,
        0,
        &cbOuputText,
        BCRYPT_BLOCK_PADDING)))
    {
        //wprintf(L"**** Error 0x%x returned by BCryptEncrypt\n", status);
        result = false;
        goto Cleanup;
    }

    pbOuputText = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbOuputText);
    if (NULL == pbOuputText)
    {
        //wprintf(L"**** memory allocation failed\n");
        result = false;
        goto Cleanup;
    }

    if (Encrypt) {
        // Use the key to encrypt the plaintext buffer.
        // For block sized messages, block padding will add an extra block.
        if (!NT_SUCCESS(status = BCryptEncrypt(
            hKey,
            pbInputText,
            cbInputText,
            NULL,
            pbIV,
            cbBlockLen,
            pbOuputText,
            cbOuputText,
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
            pbInputText,
            cbInputText,
            NULL,
            pbIV,
            cbBlockLen,
            pbOuputText,
            cbOuputText,
            &cbOuputText,
            BCRYPT_BLOCK_PADDING)))
        {
            //wprintf(L"**** Error 0x%x returned by BCryptDecrypt\n", status);
            result = false;
            goto Cleanup;
        }
    }

    //Output
    //Release old
    Release(this->OutputText);
    //Update output length
    this->OutputLength = cbOuputText;
    //Copy
    this->OutputText = new BYTE[this->OutputLength];
    memcpy(this->OutputText, pbOuputText, this->OutputLength);

Cleanup:

    if (hAesAlg)
    {
        BCryptCloseAlgorithmProvider(hAesAlg, 0);
    }

    if (hKey)
    {
        BCryptDestroyKey(hKey);
    }

    if (pbOuputText)
    {
        HeapFree(GetProcessHeap(), 0, pbOuputText);
    }

    if (pbInputText)
    {
        HeapFree(GetProcessHeap(), 0, pbInputText);
    }

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
