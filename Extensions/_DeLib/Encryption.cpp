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

bool Encryption::SaveFile(const wchar_t* pFileName, PBYTE pData, DWORD sz) {
	if (pData == nullptr) { return false; }

	FILE* fp = nullptr;

	const auto err = _wfopen_s(&fp, pFileName, L"wb");
	if (err != 0|| fp == nullptr) { return false; }

	const auto elementCount = fwrite(pData, sz, 1, fp);
	const auto ret = fclose(fp);

	return ret == 0;
}

bool Encryption::SaveFile(const wchar_t* FileName, bool SaveSrc) const {
    PBYTE Output = SaveSrc ? this->InputData : this->OutputData;
    DWORD Length = SaveSrc ? this->InputLength : this->OutputLength;

	return SaveFile(FileName, Output, Length);
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

	this->Key = new BYTE[KEY_LENGTH];
	this->IV = new BYTE[KEY_LENGTH];

	// empty, use default string
	if (KeyStr == nullptr || wcslen(KeyStr) == 0) {
		memcpy(this->Key, DefaultKey, KEY_LENGTH);
		memcpy(this->IV, DefaultIV, KEY_LENGTH);

		return;
	}

	const auto keyStrLength = wcslen(KeyStr);

	const DWORD strLength = sizeof(wchar_t) * static_cast<DWORD>(keyStrLength);
	const DWORD wantedLength = this->KeyLength + this->IVLength;

	// length enough
	if (strLength == wantedLength) {
		memcpy(this->Key, KeyStr, KEY_LENGTH);
		memcpy(this->IV, KeyStr + STR_LENGTH, KEY_LENGTH);

		return;
	}

	// too long, do hash move
	if (strLength > wantedLength) {
		size_t loop = keyStrLength - KEY_LENGTH;
		auto pChar = KeyStr + KEY_LENGTH;

		memcpy(this->Key, KeyStr, KEY_LENGTH);
		memcpy(this->IV, KeyStr + STR_LENGTH, KEY_LENGTH);

		for (; loop != 0; loop--) {
			const auto curChar = pChar[0];
			pChar++;

			for (size_t pos = 0; pos < STR_LENGTH; pos++) {
				(reinterpret_cast<wchar_t*>(this->Key) + pos)[0] ^= curChar;
				(reinterpret_cast<wchar_t*>(this->IV) + pos)[0] ^= curChar;
			}
		}

		return;
	}

	// too short, loop to fill
	size_t seed = keyStrLength;

	for (size_t loop = keyStrLength; loop != 0; loop--) {
		seed ^= KeyStr[loop - 1] + HASHER_MOVE(seed);
	}

	const auto generatedKeyStr = new wchar_t[KEY_LENGTH];
	memset(generatedKeyStr, 0, KEY_LENGTH * sizeof(wchar_t));
	memcpy(generatedKeyStr, KeyStr, keyStrLength * sizeof(wchar_t));

	for (size_t loop = keyStrLength; loop < KEY_LENGTH; loop++) {
		const auto pChar = generatedKeyStr + loop;
		const wchar_t high = HIBYTE(seed);
		const wchar_t low = LOBYTE(seed);
		pChar[0] = high ^ low;

		seed ^= pChar[0] + HASHER_MOVE(seed);
	}

	memcpy(this->Key, generatedKeyStr, KEY_LENGTH);
	memcpy(this->IV, generatedKeyStr + STR_LENGTH, KEY_LENGTH);

	delete[] generatedKeyStr;
}