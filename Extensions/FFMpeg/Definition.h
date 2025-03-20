#pragma once

#include <ranges>

#include "AudioInterface.h"

// Condition_OnVideoFinish
constexpr auto ON_FINISH = 4;
// Condition_OnVideoOpenFailed
constexpr auto ON_OPENFAILED = 5;

using Timer = std::chrono::time_point<std::chrono::steady_clock>;

struct MemVideoLib {
	std::map<std::wstring, Encryption*> data;

	using It = decltype(data.find(std::wstring()));

	MemVideoLib() = default;
	~MemVideoLib() {
		EraseAll();
	}

	inline void PutItem(const std::wstring& key, Encryption* pData) {
		data[key] = pData;
	}

	inline void EraseItem(const std::wstring& key) {
		const auto it = GetItem(key);

		if (it != data.end()) {
			delete it->second;
			data.erase(it);
		}
	}

	inline void EraseAll() {
		for (const auto& pEncryption : data | std::views::values) {
			delete pEncryption;
		}

		data.clear();
	}

	inline It GetItem(const std::wstring& key) {
		return data.find(key);
	}

	inline bool ItemExist(const std::wstring& key) {
		return GetItem(key) != data.end();
	}

	inline bool ItemExist(const It& it) {
		return it != data.end();
	}
};

struct D3DSharedHandler;
struct D3DLocalHandler;

struct GlobalData {
	std::vector<FFMpeg**> ppFFMpegs_record;
	MemVideoLib* pMemVideoLib = nullptr;
	AudioInterface* pAI = nullptr;

    D3DSharedHandler* pD3DSharedHandler = nullptr;

	GlobalData() {
		pMemVideoLib = new MemVideoLib();
		pAI = new SMI();
	}

	~GlobalData() {
		delete pMemVideoLib;
		delete pAI;
	}

	inline void Create(FFMpeg** ppFFMpeg, void* pUserData) {
		// Update global data
		ppFFMpegs_record.emplace_back(ppFFMpeg);

		pAI->AddInstance(ppFFMpeg, pUserData);
	}

	inline void Destroy(FFMpeg** ppFFMpeg) {
		const auto recordIt = std::ranges::find(ppFFMpegs_record, ppFFMpeg);
		if (recordIt != ppFFMpegs_record.end()) {
			ppFFMpegs_record.erase(recordIt);
		}

		pAI->RemoveInstance(ppFFMpeg, nullptr);	
	}
};
