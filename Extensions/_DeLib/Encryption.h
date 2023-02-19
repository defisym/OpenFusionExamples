#pragma once

#include <windows.h>
#include <stdio.h>
#include <bcrypt.h>
#include <wincrypt.h>
#include <string>

#include <functional>

//#include <ntstatus.h>

#define _USER_MODE

#ifdef _USER_MODE
#pragma comment(lib, "bcrypt.lib")
#else
// requires cng.sys
#pragma comment(lib, "Cng.lib")
#endif

// for hash
#pragma comment(lib, "Crypt32.lib")

constexpr auto ENCRY = true;
constexpr auto DECRY = false;

constexpr static BYTE DefaultKey[16] = {
0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

constexpr static BYTE DefaultIV[16] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

#define STATUS_SUCCESS				((NTSTATUS)0x00000000L)

#define NT_SUCCESS(Status)          (((NTSTATUS)(Status)) >= 0)
#define STATUS_UNSUCCESSFUL         ((NTSTATUS)0xC0000001L)

class Encryption {
private:
	PBYTE InputData = nullptr;
	PBYTE OutputData = nullptr;

	DWORD InputLength = 0;
	DWORD OutputLength = 0;

	PBYTE Key = nullptr;
	PBYTE IV = nullptr;

	DWORD KeyLength = 16;
	DWORD IVLength = 16;

	char* InputStr = nullptr;
	char* OutputStr = nullptr;

	DWORD HashLength = 0;
	LPWSTR HashStr = nullptr;

	template<typename T>
	inline T* GetStr(T*& pOutput, PBYTE pSrc, size_t sz) {
		if (pSrc == nullptr) {
			return nullptr;
		}

		//allocate and ensure NULL terminated
		Alloc(pOutput, sz + 1);
		memcpy(pOutput, pSrc, sz);
		pOutput[sz] = 0;

		return pOutput;
	}

	template<typename T>
	inline void Release(T*& Pointer) {
		delete[] Pointer;
		Pointer = nullptr;

		return;
	}

	template<typename T>
	inline bool Alloc(T*& Pointer, size_t sz) {
		Release(Pointer);
		Pointer = new T[sz];
		memset(Pointer, 0, sizeof(T) * sz);

		return NULL != Pointer;
	}

	inline void GetReadableHash() {
		//Release
		Release(this->HashStr);

		//Update output length		
		CryptBinaryToString(this->OutputData, this->OutputLength
			, CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF
			, NULL, &this->HashLength);

		//Output		
		this->HashStr = new WCHAR[this->HashLength];
		CryptBinaryToString(this->OutputData, this->OutputLength
			, CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF
			, this->HashStr, &this->HashLength);
	}

	using HandlerType = decltype(BCryptEncrypt);

	inline bool Encrypt_Core(HandlerType handler
		, LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM
		, std::function<NTSTATUS(HandlerType handler
			, BCRYPT_KEY_HANDLE hKey
			, PBYTE pbIV, DWORD cbBlockLen)> externalEcrypter = nullptr) {
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
			cbData = 0,
			cbKeyObject = 0,
			cbBlockLen = 0;
		PBYTE
			pbKeyObject = NULL,
			pbIV = NULL;

		bool result = true;

		auto AllocOutputBuffer = [&] (HandlerType pHandler) {
			if (!NT_SUCCESS(status = pHandler(
				hKey,
				this->InputData, this->InputLength,
				NULL,
				pbIV, cbBlockLen,
				NULL, 0,
				&(this->OutputLength),
				BCRYPT_BLOCK_PADDING))) {
				//wprintf(L"**** Error 0x%x returned by BCryptEncrypt\n", status);
				return false;
			}

			return Alloc(this->OutputData, this->OutputLength);
		};

		// General routine of En/Decrypt
		// Open an algorithm handle.
		if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
			&hAesAlg,
			Algorithm,
			NULL,
			0))) {
			//wprintf(L"**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
			result = false;
			goto Cleanup;
		}

		// Calculate the size of the buffer to hold the KeyObject.
		if (!NT_SUCCESS(status = BCryptGetProperty(
			hAesAlg,
			BCRYPT_OBJECT_LENGTH,
			(PBYTE)&cbKeyObject, sizeof(DWORD),
			&cbData, 0))) {
			//wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
			result = false;
			goto Cleanup;
		}

		// Allocate the key object on the heap.
		pbKeyObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbKeyObject);
		if (NULL == pbKeyObject) {
			//wprintf(L"**** memory allocation failed\n");
			result = false;
			goto Cleanup;
		}

		// Calculate the block length for the IV.
		if (!NT_SUCCESS(status = BCryptGetProperty(
			hAesAlg,
			BCRYPT_BLOCK_LENGTH,
			(PBYTE)&cbBlockLen, sizeof(DWORD),
			&cbData, 0))) {
			//wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
			result = false;
			goto Cleanup;
		}

		// Determine whether the cbBlockLen is not longer than the IV length.
		if (cbBlockLen > this->IVLength) {
			//wprintf(L"**** block length is longer than the provided IV length\n");
			result = false;
			goto Cleanup;
		}

		// Allocate a buffer for the IV. The buffer is consumed during the 
		// encrypt/decrypt process.
		pbIV = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbBlockLen);
		if (NULL == pbIV) {
			//wprintf(L"**** memory allocation failed\n");
			result = false;
			goto Cleanup;
		}

		memcpy(pbIV, this->IV, cbBlockLen);

		if (!NT_SUCCESS(status = BCryptSetProperty(
			hAesAlg,
			BCRYPT_CHAINING_MODE,
			(PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC),
			0))) {
			//wprintf(L"**** Error 0x%x returned by BCryptSetProperty\n", status);
			result = false;
			goto Cleanup;
		}

		// Generate the key from supplied input key bytes.
		if (!NT_SUCCESS(status = BCryptGenerateSymmetricKey(
			hAesAlg,
			&hKey,
			pbKeyObject, cbKeyObject,
			this->Key, this->KeyLength,
			0))) {
			//wprintf(L"**** Error 0x%x returned by BCryptGenerateSymmetricKey\n", status);
			result = false;
			goto Cleanup;
		}

		// alloc output buffer.
		if (!AllocOutputBuffer(handler)) {
			// wprintf(L"**** memory allocation failed\n");
			result = false;
			goto Cleanup;
		}

		// Use the key to encrypt the plaintext buffer.
		// For block sized messages, block padding will add an extra block.
		status = externalEcrypter!=nullptr
			? externalEcrypter(handler
				, hKey
				, pbIV, cbBlockLen)
			: handler(
				hKey,
				this->InputData, this->InputLength,
				NULL,
				pbIV, cbBlockLen,
				this->OutputData, this->OutputLength,
				&(this->OutputLength),
				BCRYPT_BLOCK_PADDING);

		if (!NT_SUCCESS(status)) {
			//wprintf(L"**** Error 0x%x returned by BCryptEncrypt\n", status);
			result = false;
			goto Cleanup;
		}

	Cleanup:

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

		return result;
	}

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

#undef EncryptFile
#undef DecryptFile

	// load to memory, faster
	inline bool EncryptFile(const wchar_t* pFileName, LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM) {
		this->OpenFile(pFileName);
		auto ret = this->Encrypt();
		this->ReleaseInputData();

		return ret;
	}
	inline bool DecryptFile(const wchar_t* pFileName, LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM) {
		this->OpenFile(pFileName);
		auto ret = this->Decrypt();
		this->ReleaseInputData();

		return ret;
	}

	inline bool Encrypt(LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM) {
		return Encrypt_Core(BCryptEncrypt, Algorithm);
	}
	inline bool Decrypt(LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM) {
		return Encrypt_Core(BCryptDecrypt, Algorithm);
	}

#define DecryDirectly

#ifdef DecryDirectly
	private:
		inline bool ProcessFileDirectly(const wchar_t* FileName, HandlerType handler, LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM) {
			bool bRet = false;

			OpenFileGeneral(FileName, [&](FILE* fp, long fileSz) {
				this->InputData = (PBYTE)fp;

			bRet = Encrypt_Core(handler, Algorithm, [&](HandlerType handler
				, BCRYPT_KEY_HANDLE hKey
				, PBYTE pbIV, DWORD cbBlockLen) {
					auto bufSz = cbBlockLen;
			auto pBuf = new BYTE[bufSz];

			bool bFinish = false;

			DWORD byteWrite = 0;
			DWORD totalByteWrite = 0;

			auto GetInputLeft = [&]() {
				auto fPos = ftell(fp);

				return this->InputLength - ftell(fp);
			};

			do {
				auto sz = (std::min)(bufSz, GetInputLeft());
				// Encrypt with padding, so the last block must be padded
				auto bPadding = (GetInputLeft() <= cbBlockLen) /*|| (sz < bufSz)*/;

				memset(pBuf, 0, bufSz);
				fread(pBuf, sz, 1, fp);

				auto ret = handler(
					hKey,
					pBuf, sz,
					NULL,
					pbIV, cbBlockLen,
					this->OutputData + totalByteWrite, this->OutputLength - totalByteWrite,
					&byteWrite,
					bPadding ? BCRYPT_BLOCK_PADDING : 0);

				if (!NT_SUCCESS(ret)) {
					break;
				}

				totalByteWrite += byteWrite;
				bFinish = (GetInputLeft() == 0);

				if (bFinish) {
					this->OutputLength = totalByteWrite;

					break;
				}
			} while (true);

			delete[] pBuf;

			return bFinish ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
				});

			this->InputData = nullptr;
			this->InputLength = 0;
				});

			return bRet;
		}
	public:
		// read from disk, slower
		inline bool EncryptFileDirectly(const wchar_t* FileName, LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM) {
			return ProcessFileDirectly(FileName, BCryptEncrypt, Algorithm);
		}
		inline bool DecryptFileDirectly(const wchar_t* FileName, LPCWSTR Algorithm = BCRYPT_AES_ALGORITHM) {
			return ProcessFileDirectly(FileName, BCryptDecrypt, Algorithm);
		}
#endif 

#define ENCRYPTER_USE_CONVENIENCE_HASH

#ifndef ENCRYPTER_USE_CONVENIENCE_HASH
	// BCRYPT_SHA256_ALGORITHM,
	inline LPCWSTR GetHash(LPCWSTR Algorithm = BCRYPT_MD5_ALGORITHM) {
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
			cbHashObject = 0;
		PBYTE
			pbHashObject = NULL;

		bool result = true;

		//open an algorithm handle
		if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
			&hAlg,
			Algorithm,
			NULL,
			0))) {
			// wprintf(L"**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
			result = false;
			goto Cleanup;
		}

		//calculate the size of the buffer to hold the hash object
		if (!NT_SUCCESS(status = BCryptGetProperty(
			hAlg,
			BCRYPT_OBJECT_LENGTH,
			(PBYTE)&cbHashObject, sizeof(DWORD),
			&cbData, 0))) {
			// wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
			result = false;
			goto Cleanup;
		}

		//allocate the hash object on the heap
		pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
		if (NULL == pbHashObject) {
			// wprintf(L"**** memory allocation failed\n");
			result = false;
			goto Cleanup;
		}

		//calculate the length of the hash
		if (!NT_SUCCESS(status = BCryptGetProperty(
			hAlg,
			BCRYPT_HASH_LENGTH,
			(PBYTE) & (this->OutputLength), sizeof(DWORD),
			&cbData, 0))) {
			// wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
			result = false;
			goto Cleanup;
		}

		if (!Alloc(this->OutputData, this->OutputLength)) {
			// wprintf(L"**** memory allocation failed\n");
			result = false;
			goto Cleanup;
		}

		//create a hash
		if (!NT_SUCCESS(status = BCryptCreateHash(
			hAlg,
			&hHash,
			pbHashObject, cbHashObject,
			NULL, 0,
			0))) {
			// wprintf(L"**** Error 0x%x returned by BCryptCreateHash\n", status);
			result = false;
			goto Cleanup;
		}

		//hash some data
		if (!NT_SUCCESS(status = BCryptHashData(
			hHash,
			this->InputData, this->InputLength,
			0))) {
			// wprintf(L"**** Error 0x%x returned by BCryptHashData\n", status);
			result = false;
			goto Cleanup;
		}

		//close the hash
		if (!NT_SUCCESS(status = BCryptFinishHash(
			hHash,
			this->OutputData, this->OutputLength,
			0))) {
			// wprintf(L"**** Error 0x%x returned by BCryptFinishHash\n", status);
			result = false;
			goto Cleanup;
		}

		GetReadableHash();

	Cleanup:

		if (hAlg) {
			BCryptCloseAlgorithmProvider(hAlg, 0);
		}

		if (hHash) {
			BCryptDestroyHash(hHash);
		}

		if (pbHashObject) {
			HeapFree(GetProcessHeap(), 0, pbHashObject);
		}

		return result ? this->HashStr : nullptr;
	}
#else
	inline LPCWSTR GetHash(LPCWSTR Algorithm = BCRYPT_MD5_ALGORITHM) {
		//do nothing if input is invalid
		if (this->InputData == nullptr) {
			return nullptr;
		}

		//Define Vars
		BCRYPT_ALG_HANDLE       hAlg = NULL;
		BCRYPT_HASH_HANDLE      hHash = NULL;
		NTSTATUS                status = STATUS_UNSUCCESSFUL;

		DWORD
			cbData = 0;

		bool result = true;

		//open an algorithm handle
		if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
			&hAlg,
			Algorithm,
			NULL,
			0))) {
			// wprintf(L"**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
			result = false;
			goto Cleanup;
		}

		if (!NT_SUCCESS(status = BCryptGetProperty(
			hAlg,
			BCRYPT_HASH_LENGTH,
			(PBYTE) & (this->OutputLength), sizeof(DWORD),
			&cbData, 0))) {
			// wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
			result = false;
			goto Cleanup;
		}

		if (!Alloc(this->OutputData, this->OutputLength)) {
			// wprintf(L"**** memory allocation failed\n");
			result = false;
			goto Cleanup;
		}

		//Output
		if (!NT_SUCCESS(status = BCryptHash(
			hAlg,
			NULL, 0,
			this->InputData, this->InputLength,
			this->OutputData, this->OutputLength
		))) {
			// wprintf(L"**** Error 0x%x returned by BCryptFinishHash\n", status);
			result = false;
			goto Cleanup;
		};

		GetReadableHash();

	Cleanup:

		if (hAlg) {
			BCryptCloseAlgorithmProvider(hAlg, 0);
		}

		if (hHash) {
			BCryptDestroyHash(hHash);
		}

		return result ? this->HashStr : nullptr;
	}
#endif ENCRYPTER_USE_CONVENIENCE_HASH
};

