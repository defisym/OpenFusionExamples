#pragma once

#include <windows.h>
#include <stdio.h>
#include <bcrypt.h>
#include <wincrypt.h>
#include <string>

#include <functional>

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

	template<typename T>
	inline void Release(T* &Pointer) {
		delete[] Pointer;
		Pointer = nullptr;

		return;
	}

	//void Release(void* Pointer);
	bool Encrypt_Core(bool Encrypt, LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM);

	inline void OpenFileGeneral(const wchar_t* FileName, std::function<void(FILE*, long)> callBack) {
		Release(this->InputData);

		FILE* fp = nullptr;

		_wfopen_s(&fp, FileName, L"rb");
		if (fp == nullptr) {
			return;
		}

		fseek(fp, 0, SEEK_END);
		this->InputLength = ftell(fp);
		rewind(fp);

		callBack(fp, this->InputLength);
		
		fclose(fp);
	}

protected:

public:
	Encryption();
	~Encryption();

	void OpenFile(const wchar_t* FileName);
	void SaveFile(const wchar_t* FileName, bool SaveSrc = false);

	inline PBYTE GetInputData() {
		return this->InputData;
	}
	inline DWORD GetInputDataLength() {
		return this->InputLength;
	}

	inline PBYTE GetOutputData() {
		return this->OutputData;
	}
	inline DWORD GetOutputDataLength() {
		return this->OutputLength;
	}

	template<typename T>
	void SetEncryptData(const T* pBuf, DWORD sz);

	void SetEncryptStr(std::string& Str);
	void SetEncryptStr(const char* Str, DWORD StrLength);

	void SetEncryptStr(std::wstring& Str);
	void SetEncryptStr(const wchar_t* Str, DWORD StrLength);

	char* GetInputStr();
	void ReleaseInputStr();
	
	DWORD GetInputStrLength();

	void ReleaseInputData();

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
	//inline bool DecryptFileDirectly(const wchar_t* FileName, LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM) {
	//	bool bRet = false;

	//	OpenFileGeneral(FileName, [&](FILE* fp, long sz) {
	//		this->InputData = (PBYTE)fp;
	//		bRet = Encrypt_Core(DECRY, Algorithm);

	//		this->InputData = nullptr;
	//		this->InputLength = 0;
	//		});

	//	return bRet;
	//}

	LPCWSTR GetHash(LPCWSTR Algorithm = BCRYPT_MD5_ALGORITHM);

};

