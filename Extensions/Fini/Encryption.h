#pragma once

#include <windows.h>
#include <stdio.h>
#include <bcrypt.h>
#include <string>

#pragma comment(lib, "bcrypt.lib")

constexpr auto ENCRY = true;
constexpr auto DECRY = false;

#define NT_SUCCESS(Status)          (((NTSTATUS)(Status)) >= 0)
#define STATUS_UNSUCCESSFUL         ((NTSTATUS)0xC0000001L)

class Encryption
{
private:
	PBYTE InputText = nullptr;
	PBYTE OutputText = nullptr;

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

	char* DecryptPutStr = nullptr;

	void Release(PBYTE Pointer);
	void Encrypt_Core(bool Encrypt);
	
public:
	Encryption();
	~Encryption();

	void OpenFile(const wchar_t* FileName);
	void SaveFile(const wchar_t* FileName);

	void GetEncryptStr(std::string& Str);
	void GetEncryptStr(const char* Str, DWORD StrLength);

	char* GetDecryptStr();
	void ReleaseDecryptStr();
	DWORD GetDecryptStrLength();

	void GenerateKey(const wchar_t* KeyStr);

	inline void Encrypt() {
		Encrypt_Core(ENCRY);
	}
	inline void Decrypt() {
		Encrypt_Core(DECRY);
	}
};

