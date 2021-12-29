#pragma once

#include <windows.h>
#include <stdio.h>
#include <bcrypt.h>
#include <wincrypt.h>
#include <string>

//#include <ntstatus.h>

#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "Crypt32.lib")

constexpr auto ENCRY = true;
constexpr auto DECRY = false;

#define NT_SUCCESS(Status)          (((NTSTATUS)(Status)) >= 0)
#define STATUS_UNSUCCESSFUL         ((NTSTATUS)0xC0000001L)

class Encryption
{
private:
	PBYTE InputData = nullptr;
	PBYTE OutputData = nullptr;

	DWORD InputLength = 0;
	DWORD OutputLength = 0;

	BYTE DefaultKey[16] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
	};

	BYTE DefaultIV[16] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
	};

	PBYTE Key = nullptr;
	PBYTE IV = nullptr;

	DWORD KeyLength = 16;
	DWORD IVLength = 16;

	char* InputStr = nullptr;
	char* OutputStr = nullptr;

	DWORD HashLength = 0;
	LPWSTR HashStr = nullptr;

	void Release(void* Pointer);
	bool Encrypt_Core(bool Encrypt, LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM);

protected:
	char* GetInputData() {
		return (char*)this->InputData;
	}
	char* GetOutputData() {
		return (char*)this->OutputData;
	}

public:
	Encryption();
	~Encryption();

	void OpenFile(const wchar_t* FileName);
	void SaveFile(const wchar_t* FileName, bool SaveSrc = false);

	void SetEncryptStr(std::string& Str);
	void SetEncryptStr(const char* Str, DWORD StrLength);

	void SetEncryptStr(std::wstring& Str);
	void SetEncryptStr(const wchar_t* Str, DWORD StrLength);

	char* GetInputStr();
	void ReleaseInputStr();

	DWORD GetInputStrLength();

	char* GetOutputStr();
	void ReleaseOutputStr();

	DWORD GetOutputStrLength();

	void GenerateKey(const wchar_t* KeyStr);

	inline bool Encrypt(LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM) {
		return Encrypt_Core(ENCRY, Algorithm);
	}
	inline bool Decrypt(LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM) {
		return Encrypt_Core(DECRY, Algorithm);
	}

	LPCWSTR GetHash(LPCWSTR Algorithm = BCRYPT_MD5_ALGORITHM);
};

