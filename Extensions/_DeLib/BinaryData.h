// Used to manage memory data, let extensions read from it
#pragma once

#include <map>
#include <vector>
#include <ranges>
#include <format>
#include <functional>

#include "Base64.h"
#include "Encryption.h"

using BinaryBuffer = const unsigned char;

template<typename DataType>
struct BinaryData {
	using DataRef = std::vector<DataType*>;

	struct Data {
		Encryption* pDecrypt = nullptr;
		std::wstring hash;
		DataRef dataRef;
	};

	std::map<std::wstring, Data> pDatas;

	BinaryData() = default;
	~BinaryData() {
		for (const auto& pData : pDatas | std::views::values) {
			delete pData.pDecrypt;
		}
	}

	inline const BinaryBuffer* GetAddress(const std::wstring& fileName) const {
		const auto it = pDatas.find(fileName);

		return it != pDatas.end()
			? it->second.pDecrypt->GetData()
			: nullptr;
	}

	inline bool DataNotReferenced(const std::wstring& fileName) {
		const auto it = pDatas.find(fileName);

		if (it == pDatas.end()) { return true; }

		return it->second.dataRef.empty();
	}

	inline bool UpdateData(const std::wstring& fileName, const std::wstring& key) {
		const auto it = pDatas.find(fileName);

		// normal load
		if (it == pDatas.end()) {
			return AddData(fileName, key);
		}

		// don't need to update
		if(it->second.hash == GetFileHash(fileName)) {
			return false;
		}

		// cannot release old
		if(!ReleaseData(fileName)) {
			return false;
		}

		return AddData(fileName, key);
	}

	inline bool AddData(const std::wstring& fileName, const std::wstring& key) {
		if(pDatas.contains(fileName)) {
			return true;
		}

		const auto bDecrypt = !(StrEmpty(key.c_str()));
		const auto pDecrypt = new Encryption;

		pDecrypt->GenerateKey(key.c_str());

		bool bRet = true;

		if(!bDecrypt) {
			bRet = pDecrypt->OpenFile(fileName.c_str());
		}else {
			bRet = pDecrypt->DecryptFileDirectly(fileName.c_str());
		}

		if (!bRet) { return false; }

		pDatas[fileName] = { pDecrypt,  GetFileHash(fileName), DataRef() };

		return true;
	}

	inline bool ReleaseData(const std::wstring& fileName) {
		const auto it = pDatas.find(fileName);

		if (it != pDatas.end() && it->second.dataRef.empty()) {
			delete it->second.pDecrypt;
			pDatas.erase(it);

			return true;
		}

		return false;
	}

	inline void ReleaseData() {
		for (auto it = pDatas.begin(); it != pDatas.end();) {
			if(!it->second.dataRef.empty()) {
				++it;

				continue;
			}

			delete it->second.pDecrypt;
			it = pDatas.erase(it);
		}
	}

	inline bool AddRef(const std::wstring& fileName, DataType* pAudioData) {
		const auto it = pDatas.find(fileName);

		if (it == pDatas.end()) { return false; }

		const auto pRefVec = &it->second.dataRef;
		if (std::ranges::find(*pRefVec, pAudioData) != pRefVec->end()) {
			return false;
		}

		pRefVec->emplace_back(pAudioData);

		return true;
	}

	inline bool RemoveRef(const std::wstring& fileName, DataType* pAudioData) {
		const auto it = pDatas.find(fileName);

		if (it == pDatas.end()) { return false; }

		const auto pRefVec = &it->second.dataRef;
		if (std::ranges::find(*pRefVec, pAudioData) == pRefVec->end()) {
			return false;
		}

		const auto toRemove = std::ranges::remove(*pRefVec, pAudioData);
		pRefVec->erase(toRemove.begin(), toRemove.end());

		return true;
	}
};

template<typename DataType>
struct BinaryDataInfo {
	BinaryData<DataType>* pBinaryData = nullptr;
	char* pData = nullptr;
	size_t sz = 0;

	std::wstring dataFileName;
	std::wstring accessFileName;

	// FromMem_Serialization
	constexpr static const wchar_t* pFromMemPrefix = L"FromMem_";

	BinaryDataInfo() = default;
	BinaryDataInfo(const wchar_t* pFileName) {
		// FromMem_AccessFileName_Address_Size
		constexpr auto delimiter = L'_';

		size_t first = std::wstring::npos;
		size_t last = std::wstring::npos;

		// Remove Prefix
		std::wstring rawText = pFileName;
		first = rawText.find_first_of(delimiter);
		rawText = rawText.substr(first + 1);

		// Get Size
		last = rawText.find_last_of(delimiter);
		const std::wstring size = rawText.substr(last + 1);
		rawText = rawText.substr(0, last);

		// Get Address
		last = rawText.find_last_of(delimiter);
		const std::wstring address = rawText.substr(last + 1);
		rawText = rawText.substr(0, last);

		// Update info
		accessFileName = rawText;

		pData = reinterpret_cast<char*>(_stoi(address));
		sz = _stoi(size);
	}

	static constexpr inline size_t GetPrefixLength() {
		return std::char_traits<wchar_t>::length(pFromMemPrefix);
	}

	static inline bool FromMemory(const wchar_t* pFileName) {
		const size_t pFromMemPrefixLength = GetPrefixLength();

		bool bFromMem = true;

		for (size_t i = 0; i < pFromMemPrefixLength; i++) {
			if (pFileName[i] != pFromMemPrefix[i]) {
				bFromMem = false;

				break;
			}
		}

		return bFromMem;
	}

	inline std::wstring Serialization() const {
		const size_t bufSz = sizeof(BinaryData*)
			+ sizeof(char*)
			+ sizeof(size_t)
			+ sizeof(wchar_t) * dataFileName.length()
			+ sizeof(size_t) * accessFileName.length();
		const auto pBuf = new unsigned char[bufSz];

		auto pBuffer = pBuf;
		memcpy(pBuffer, &pBinaryData, sizeof(BinaryData*));

		pBuffer += sizeof(BinaryData*);
		memcpy(pBuffer, &pData, sizeof(char*));

		pBuffer += sizeof(char*);
		memcpy(pBuffer, &sz, sizeof(size_t));

		pBuffer += sizeof(size_t);
		memcpy(pBuffer, accessFileName.data(), sizeof(size_t) * accessFileName.length());

		pBuffer += sizeof(wchar_t) * accessFileName.length();
		((LPWSTR)pBuffer)[0] = L'\0';
		pBuffer += sizeof(wchar_t);

		memcpy(pBuffer, dataFileName.data(), sizeof(wchar_t) * dataFileName.length());
		pBuffer += sizeof(wchar_t) * dataFileName.length();
		((LPWSTR)pBuffer)[0] = L'\0';
		pBuffer += sizeof(wchar_t);

		Base64<std::wstring> base64;
		auto ret = base64.base64_encode(pBuf, bufSz);

		delete[] pBuf;

		return ret;
	}

	static inline BinaryDataInfo DeSerialization(const std::wstring& data) {
		BinaryDataInfo info;

		Base64<std::wstring> base64;
		base64.base64_decode_callback(data, [&] (const BYTE* buf, const size_t sz) {
			auto pBuffer = buf;

			info.pBinaryData = *(BinaryData**)pBuffer;
			pBuffer += sizeof(BinaryData*);

			info.pData = *(char**)pBuffer;
			pBuffer += sizeof(char*);

			info.sz = *(size_t*)pBuffer;
			pBuffer += sizeof(size_t);

			info.accessFileName = (LPCWSTR)pBuffer;
			pBuffer += sizeof(wchar_t) * (info.accessFileName.length() + 1);

			info.dataFileName = (LPCWSTR)pBuffer;
			pBuffer += sizeof(wchar_t) * (info.dataFileName.length() + 1);
		});

		return info;
	}
};

template<typename DataType>
inline std::wstring GetFromMemoryName(LPCWSTR pAccessFilePath, void* address, size_t size) {
	BinaryDataInfo<DataType> info;
	info.accessFileName = pAccessFilePath;
	info.pData = static_cast<char*>(address);
	info.sz = size;

	return std::format(L"FromMem_{}", info.Serialization());
}

template<typename DataType>
inline std::wstring GetFromHandledMemoryName(BinaryData<DataType>* pBinaryData,
	LPCWSTR pAccessFilePath,
	LPCWSTR pDataFilePath, size_t offset,size_t size) {
	BinaryDataInfo<DataType> info;
	info.dataFileName = pDataFilePath;
	info.accessFileName = pAccessFilePath;

	info.pBinaryData = pBinaryData;
	const auto pManagedBinaryData = info.pBinaryData->GetAddress(info.dataFileName);
	info.pData = pManagedBinaryData != nullptr
		? static_cast<char*>(pManagedBinaryData + offset)
		: nullptr;
	info.sz = size;

	return std::format(L"FromMem_{}", info.Serialization());
}

template<typename DataType, typename ReturnType>
inline ReturnType LoadFromMemoryWrapper(const wchar_t* pFileName,
	const std::function<ReturnType()>& cb,
	const std::function<ReturnType(BinaryDataInfo<DataType>)>& memoryCb) {
	if (!BinaryDataInfo<DataType>::FromMemory(pFileName)) { return cb(); }

	auto pBinaryDataInfo = BinaryDataInfo<DataType>::DeSerialization(pFileName + BinaryDataInfo<DataType>::GetPrefixLength());

	return memoryCb(pBinaryDataInfo);
}