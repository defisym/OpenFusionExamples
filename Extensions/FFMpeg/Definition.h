#pragma once

//Condition_OnVideoFinish
constexpr auto ON_FINISH = 4;

using namespace std::literals;
using Timer = std::chrono::time_point<std::chrono::steady_clock>;

struct MemVideoLib {
	std::map<std::wstring, Encryption*> data;

	using It = decltype(data.find(std::wstring()));

	MemVideoLib() {

	}

	~MemVideoLib() {
		EraseAll();
	}

	inline void PutItem(std::wstring& key, Encryption* pData) {
		data[key] = pData;
	}

	inline void EraseItem(std::wstring& key) {
		auto it = GetItem(key);

		if (it != data.end()) {
			delete it->second;
			data.erase(it);
		}
	}

	inline void EraseAll() {
		for (auto& it : data) {
			delete it.second;
		}
	}

	inline It GetItem(std::wstring& key) {
		return data.find(key);
	}

	inline bool ItemExist(It& it) {
		return it != data.end();
	}
};

struct GlobalData {
	bool bSDLInit = false;

	MemVideoLib* pMemVideoLib = nullptr;

	GlobalData() {
		pMemVideoLib = new MemVideoLib;
	}

	~GlobalData() {
		delete pMemVideoLib;
	}
};
