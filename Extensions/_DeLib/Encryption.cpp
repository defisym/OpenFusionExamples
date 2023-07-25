#include "Encryption.h"

Encryption::~Encryption() {
    Release(this->InputData);
    Release(this->OutputData);
    Release(this->Key);
    Release(this->IV);
    Release(this->InputStr);
    Release(this->OutputStr);

    Release(this->HashStr);
}

bool Encryption::OpenFile(const wchar_t* FileName) {
    return OpenFileGeneral(FileName, [this] (FILE* fp, long sz) {
        this->InputData = new BYTE[this->InputLength];
        fread(this->InputData, this->InputLength, 1, fp);
        });
}

bool Encryption::SaveFile(const wchar_t* FileName, bool SaveSrc) const {
    PBYTE Output = SaveSrc ? this->InputData : this->OutputData;
    DWORD Length = SaveSrc ? this->InputLength : this->OutputLength;

    if (Output == nullptr) {
        return false;
    }

    FILE* fp = nullptr;

    _wfopen_s(&fp, FileName, L"wb");
    if (fp == nullptr) {
        return false;
    }

    fwrite(Output, Length, 1, fp);
    fclose(fp);

    return true;
}

template<typename T>
void Encryption::SetEncryptData(const T* pBuf, DWORD sz) {
    Release(this->InputData);

    this->InputLength = sz * sizeof(T);
    this->InputData = new BYTE[this->InputLength];
    memcpy(this->InputData, pBuf, this->InputLength);

    return;
}

void Encryption::SetEncryptStr(const std::string& Str) {
    SetEncryptData(Str.c_str(), (DWORD)Str.length());
}

void Encryption::SetEncryptStr(const char* Str, DWORD StrLength) {
    SetEncryptData(Str, StrLength);
}

void Encryption::SetEncryptStr(const std::wstring& Str) {
    SetEncryptData(Str.c_str(), (DWORD)Str.length());
}

void Encryption::SetEncryptStr(const wchar_t* Str, DWORD StrLength) {
    SetEncryptData(Str, StrLength);
}

char* Encryption::GetInputStr() {
    return GetStr(this->InputStr, this->InputData, this->InputLength);
}

DWORD Encryption::GetInputStrLength() const {
    return this->InputLength + 1;
}

void Encryption::ReleaseInputStr() {
    Release(this->InputStr);
    this->InputLength = 0;
}

void Encryption::ReleaseInputData() {
    Release(this->InputData);
    this->InputLength = 0;
}

char* Encryption::GetOutputStr() {
    return GetStr(this->OutputStr, this->OutputData, this->OutputLength);
}

DWORD Encryption::GetOutputStrLength() const {
    return this->OutputLength + 1;
}

void Encryption::ReleaseOutputStr() {
    Release(this->OutputStr);
    this->OutputLength = 0;
}

void Encryption::GenerateKey(const wchar_t* KeyStr) {
    //release old
    Release(this->Key);
    Release(this->IV);

    this->Key = new BYTE[16];
    this->IV = new BYTE[16];

    // invalid keystr
    if ((sizeof(wchar_t) * wcslen(KeyStr)) < (this->KeyLength + this->IVLength)) {
        memcpy(this->Key, DefaultKey, 16);
        memcpy(this->IV, DefaultIV, 16);
    } else {
        // Generate
        memcpy(this->Key, KeyStr, 8 * sizeof(wchar_t));
        memcpy(this->IV, KeyStr + 8, 8 * sizeof(wchar_t));
    }

    return;
}