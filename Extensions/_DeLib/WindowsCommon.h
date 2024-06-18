#pragma once

// STL
#include <string>
#include <vector>
#include <thread>

////Boost
//#define BOOST
//#include "RegexHelper.h"

// Windows
#include <windows.h>

// Thread
#include <tlhelp32.h>
#pragma comment(lib,"version.lib")
#pragma comment(lib,"Kernel32.lib")

// FileList
#include	<shlwapi.h>
#pragma	comment(lib,"shlwapi.lib")

// Memory
#include <psapi.h>

// General
#include "GeneralDefinition.h"

// Process
inline DWORD GetProcessIDByName(LPCTSTR ApplicationName) {
	//返回参数
	DWORD	ProcessID = 0;

	//获取快照
	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	//循环遍历
	PROCESSENTRY32* info = new PROCESSENTRY32;
	info->dwSize = sizeof(PROCESSENTRY32);

	Process32First(snapshot, info);
	while (Process32Next(snapshot, info) != FALSE) {
		//进程名一致则结束进程		
		if (wcscmp(ApplicationName, info->szExeFile) == 0) {
			ProcessID = info->th32ProcessID;
			break;
		}
	}

	delete info;
	CloseHandle(snapshot);

	return ProcessID;
}

// FileVersion
struct TRANSLATION {
	WORD wLanguage;		// language ID
	WORD wCodePage;		// character set (code page)
};

inline bool operator ==(const TRANSLATION& A, const TRANSLATION& B) {
	return (A.wLanguage == B.wLanguage) && (A.wCodePage == B.wCodePage);
}

inline bool operator !=(const TRANSLATION& A, const TRANSLATION& B) {
	return !(A == B);
}

static LPCWSTR DefaultBlock[] = { L"Comments",	L"InternalName",	L"ProductName",
						L"CompanyName",	L"LegalCopyright",	L"ProductVersion",
						L"FileDescription",	L"LegalTrademarks",	L"PrivateBuild",
						L"FileVersion",	L"OriginalFilename",	L"SpecialBuild" };

inline BYTE* GetFileVersion_GetFileVersionInfo(LPCWSTR FileName) {
	// Get FileVersionInfo
	const auto size = GetFileVersionInfoSize(FileName, nullptr);

	if (size == 0) {
		return nullptr;
	}

	BYTE* pData = new BYTE[size * sizeof(BYTE)];

	GetFileVersionInfo(FileName, NULL, size, pData);

	return pData;
}

inline TRANSLATION* GetFileVersion_GetTranslation(BYTE* pData) {
	// Query
	void* Buffer = nullptr;
	UINT Len = 0;

	// Get Translation
	TRANSLATION* pTranslation = nullptr;

	if (VerQueryValue(pData, L"\\VarFileInfo\\Translation", &Buffer, &Len)) {
		pTranslation = new TRANSLATION;
		memcpy(pTranslation, Buffer, sizeof(TRANSLATION));
	}

	return pTranslation;
}

inline LPWSTR GetFileVersion_GetSubBlock(BYTE* pData, TRANSLATION* pTranslation, LPCWSTR SubBlock) {
	// Query
	void* Buffer = nullptr;
	UINT Len = 0;

	// Get SubBlock based on translation
	const auto Format = L"\\StringFileInfo\\%04x%04x\\%s";
	const auto sz = swprintf(nullptr, 0, Format, pTranslation->wLanguage, pTranslation->wCodePage, SubBlock);
	const auto bufsz = sz + 1;

	const auto block = new wchar_t[bufsz];
	const auto byteWrite = swprintf(block, bufsz, Format, pTranslation->wLanguage, pTranslation->wCodePage, SubBlock);

	// Get the real STR
	LPWSTR info = nullptr;

	auto newStr = [] (LPWSTR* des, LPCWSTR src)->void {
		const auto isz = swprintf(nullptr, 0, L"%s", src);
		const auto ibufsz = isz + 1;

		*des = new wchar_t[ibufsz];
		const auto byteWrite = swprintf(*des, ibufsz, L"%s", src);
	};

	if (VerQueryValue(pData, block, &Buffer, &Len)) {
		newStr(&info, (wchar_t*)Buffer);
	}
	else {
		newStr(&info, L"NULL");
	}

	delete[] block;

	return info;
}

inline bool GetFileVersion_CMPDefault(BYTE* pDataA, BYTE* pDataB, int CMP) {
	bool ret = true;

	if (CMP == 0b000000000000) {
		return ret;
	}

	TRANSLATION* pTranslationA = GetFileVersion_GetTranslation(pDataA);
	TRANSLATION* pTranslationB = GetFileVersion_GetTranslation(pDataB);

	if (*pTranslationA != *pTranslationB) {
		ret = false;
		goto clear;
	}

	for (int i = 0b000000000001, count = 0; i != 0b100000000000; i = i << 1, count++) {
		if (i & CMP) {
			const LPCWSTR infoA = GetFileVersion_GetSubBlock(pDataA, pTranslationA, DefaultBlock[count]);
			const LPCWSTR infoB = GetFileVersion_GetSubBlock(pDataB, pTranslationB, DefaultBlock[count]);

			const bool unequal = (wcscmp(infoA, infoB) != 0);

			delete[] infoA;
			delete[] infoB;

			if (unequal) {
				ret = false;
				break;
			}
		}
	}

clear:
	delete pTranslationA;
	delete pTranslationB;

	return ret;
}

inline bool GetFileVersion_CMPDefaultFile(LPCWSTR FileNameA, LPCWSTR FileNameB, int CMP) {
	BYTE* pDataA = GetFileVersion_GetFileVersionInfo(FileNameA);
	BYTE* pDataB = GetFileVersion_GetFileVersionInfo(FileNameB);

	const bool ret = GetFileVersion_CMPDefault(pDataA, pDataB, CMP);

	delete[] pDataA;
	delete[] pDataB;

	return ret;
}

inline LPWSTR GetFileVersion(LPCWSTR FileName, LPCWSTR SubBlock) {
	// Get FileVersionInfo
	const auto size = GetFileVersionInfoSize(FileName, nullptr);
	const auto pData = new BYTE[size * sizeof(BYTE)];
	//unique_ptr<BYTE> pData(new BYTE[size * sizeof(BYTE)]);

	GetFileVersionInfo(FileName, NULL, size, pData);

	// Query
	void* Buffer = nullptr;
	UINT Len = 0;

	// Get FixedFileInfo
	VerQueryValue(pData, L"\\", &Buffer, &Len);

	const auto pFixedFileInfo = new VS_FIXEDFILEINFO;
	memcpy(pFixedFileInfo, Buffer, sizeof(VS_FIXEDFILEINFO));

	// Get Translation
	VerQueryValue(pData, L"\\VarFileInfo\\Translation", &Buffer, &Len);

	const auto pTranslation = new TRANSLATION;
	memcpy(pTranslation, Buffer, sizeof(TRANSLATION));

	// Get SubBlock based on translation
	const auto Format = L"\\StringFileInfo\\%04x%04x\\%s";
	const auto sz = swprintf(nullptr, 0, Format, pTranslation->wLanguage, pTranslation->wCodePage, SubBlock);
	const auto bufsz = sz + 1;

	const auto block = new wchar_t[bufsz];
	const auto byteWrite = swprintf(block, bufsz, Format, pTranslation->wLanguage, pTranslation->wCodePage, SubBlock);

	// Get the real STR
	LPWSTR info = nullptr;

	auto newStr = [] (LPWSTR* des, LPCWSTR src)->void {
		const auto isz = swprintf(nullptr, 0, L"%s", src);
		const auto ibufsz = isz + 1;

		*des = new wchar_t[ibufsz];
		swprintf(*des, ibufsz, L"%s", src);
	};

	if (VerQueryValue(pData, block, &Buffer, &Len)) {
		newStr(&info, (wchar_t*)Buffer);
	}
	else {
		newStr(&info, L"NULL");
	}

	// Clear
	delete[] pData;
	delete pFixedFileInfo;
	delete pTranslation;

	delete[] block;

	return info;
}

struct ProcessHandle {
	HANDLE hProcess = nullptr;

	ProcessHandle(DWORD processID = _getpid()) {
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
										PROCESS_VM_READ,
										FALSE, processID);
	}
	~ProcessHandle() {
		CloseHandle(hProcess);
	}
};

// Process mem usage
enum class MemoryUsageType {
	PeakWorkingSetSize,
	WorkingSetSize,
	QuotaPeakPagedPoolUsage,
	QuotaPagedPoolUsage,
	QuotaPeakNonPagedPoolUsage,
	QuotaNonPagedPoolUsage,
	PagefileUsage,
	PeakPagefileUsage,
};

inline SIZE_T GetMemoryUsage(const PROCESS_MEMORY_COUNTERS& pmc, MemoryUsageType type) {
	switch (type) {
	case MemoryUsageType::PeakWorkingSetSize:
		return pmc.PeakWorkingSetSize;
	case MemoryUsageType::WorkingSetSize:
		return pmc.WorkingSetSize;
	case MemoryUsageType::QuotaPeakPagedPoolUsage:
		return pmc.QuotaPeakPagedPoolUsage;
	case MemoryUsageType::QuotaPagedPoolUsage:
		return pmc.QuotaPagedPoolUsage;
	case MemoryUsageType::QuotaPeakNonPagedPoolUsage:
		return pmc.QuotaPeakNonPagedPoolUsage;
	case MemoryUsageType::QuotaNonPagedPoolUsage:
		return pmc.QuotaNonPagedPoolUsage;
	case MemoryUsageType::PagefileUsage:
		return pmc.PagefileUsage;
	case MemoryUsageType::PeakPagefileUsage:
		return pmc.PeakPagefileUsage;
	}

	return 0;
}

inline SIZE_T GetProcessMemoryUsage(HANDLE hProcess, MemoryUsageType type = MemoryUsageType::WorkingSetSize) {
	PROCESS_MEMORY_COUNTERS pmc;
	SIZE_T ret = 0;

	if (hProcess == nullptr) {
		return ret;
	}

	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
		ret = GetMemoryUsage(pmc, type);
	}

	return ret;
}

inline SIZE_T GetProcessMemoryUsage(DWORD processID = _getpid(), MemoryUsageType type = MemoryUsageType::WorkingSetSize) {
	const ProcessHandle processHandle(processID);
	const SIZE_T ret = GetProcessMemoryUsage(processHandle.hProcess);

	return ret;
}

inline SIZE_T GetProcessMemoryUsageMB(HANDLE hProcess, MemoryUsageType type = MemoryUsageType::WorkingSetSize) {
	return (GetProcessMemoryUsage(hProcess, type) >> 20);
}

inline SIZE_T GetProcessMemoryUsageMB(DWORD processID = _getpid(), MemoryUsageType type = MemoryUsageType::WorkingSetSize) {
	return (GetProcessMemoryUsage(processID, type) >> 20);
}

inline SIZE_T GetProcessMemoryLimitMB() {
	// 1 MB
	const auto sz = 1024 * 1024 * 1;
	std::vector<char* > bufs;

	do {
		try {
			auto ptr = new char[sz];
			memset(ptr, 0, sz);
			bufs.emplace_back(ptr);
		}
		catch (std::bad_alloc& e) {
			auto info = e.what();
			const auto max = GetProcessMemoryUsageMB();
			//MSGBOX(_itos(max));			

			for (const auto& it : bufs) {
				delete[] it;
			}

			return max;
		}
	} while (true);
}

// System mem usage
enum class MemoryInfoType {
	MemoryLoad,
	TotalPhysicalMemory,
	FreePhysicalMemory,
	TotalPagingFile,
	FreePagingFile,
	TotalVirtualMemory,
	FreeVirtualMemory,
};

inline DWORDLONG GetMemoryInfo(const MEMORYSTATUSEX& statex, MemoryInfoType type = MemoryInfoType::FreePhysicalMemory) {
	switch (type) {
	case MemoryInfoType::MemoryLoad:
		return statex.dwMemoryLoad;
	case MemoryInfoType::TotalPhysicalMemory:
		return statex.ullTotalPhys;
	case MemoryInfoType::FreePhysicalMemory:
		return statex.ullAvailPhys;
	case MemoryInfoType::TotalPagingFile:
		return statex.ullTotalPageFile;
	case MemoryInfoType::FreePagingFile:
		return statex.ullAvailPageFile;
	case MemoryInfoType::TotalVirtualMemory:
		return statex.ullTotalVirtual;
	case MemoryInfoType::FreeVirtualMemory:
		return statex.ullAvailVirtual;		
	}

	return 0;
}

inline DWORDLONG GetSystemMemoryInfo(MemoryInfoType type = MemoryInfoType::FreePhysicalMemory) {
	MEMORYSTATUSEX statex = { };
	statex.dwLength = sizeof(statex);

	GlobalMemoryStatusEx(&statex);

	return GetMemoryInfo(statex, type);
}

inline SIZE_T GetSystemMemoryInfoMB(MemoryInfoType type = MemoryInfoType::FreePhysicalMemory) {
	return SIZE_T(GetSystemMemoryInfo(type) >> 20);
}

// memory left
constexpr auto MIN_MEMORYLEFT = 256;

// true if physics & memory both not enough
inline bool SystemMemoryNotEnough() {
	return (GetSystemMemoryInfoMB(MemoryInfoType::FreePhysicalMemory) < MIN_MEMORYLEFT)
		&& (GetSystemMemoryInfoMB(MemoryInfoType::FreeVirtualMemory) < MIN_MEMORYLEFT);
}

// Must end without L'\\'
inline void GetFileList(std::vector<std::wstring>* pList, const std::wstring& folder) {
	HANDLE h = nullptr;
	WIN32_FIND_DATA stFD;

	std::wstring temp = folder + std::wstring(L"\\*");
	h = FindFirstFile(temp.c_str(), &stFD);

	while (FindNextFile(h, &stFD)) {
		temp = folder + L"\\" + stFD.cFileName;
		if (temp == folder + L"\\..") {
			continue;
		}
		else if (PathIsDirectory(temp.c_str())) {
			GetFileList(pList, temp);
		}
		else {
			pList->emplace_back(folder + L"\\" + stFD.cFileName);
		}
	}

	FindClose(h);
}

inline auto SplitPath(const wchar_t* pPath) {
	const auto pDrive = new wchar_t[MAX_PATH];
	memset(pDrive, 0, MAX_PATH);
	const auto pDir = new wchar_t[MAX_PATH];
	memset(pDir, 0, MAX_PATH);
	const auto pFileName = new wchar_t[MAX_PATH];
	memset(pFileName, 0, MAX_PATH);
	const auto pExt = new wchar_t[MAX_PATH];
	memset(pExt, 0, MAX_PATH);

	_wsplitpath_s(pPath,
		pDrive, MAX_PATH,
		pDir, MAX_PATH,
		pFileName, MAX_PATH,
		pExt, MAX_PATH);

	auto ret = std::make_tuple(std::wstring(pDrive),
		std::wstring(pDir),
		std::wstring(pFileName),
		std::wstring(pExt));

	delete[] pDrive;
	delete[] pDir;
	delete[] pFileName;
	delete[] pExt;

	return ret;
}

inline std::wstring GetModuleFileNameStr(const HMODULE hModule = nullptr) {
	const LPWSTR pFullPathName = new wchar_t[MAX_PATH];
	memset(pFullPathName, 0, MAX_PATH);

	auto err = GetModuleFileName(hModule, pFullPathName, MAX_PATH);
	std::wstring ret = pFullPathName;

	delete[] pFullPathName;

	return ret;
}

inline std::wstring GetFullPathNameStr(const std::wstring& fileName,
	const bool bReturnNonAbsolutePathDirectly = true) {
	const auto pFileName = fileName.c_str();

	// to fix work directory issue
	// if input is not a full path (for access filename)
	// then return it directly
	// full path doesn't have work dir issue, as it's just calculate
	if (bReturnNonAbsolutePathDirectly) {
		// regex
		//auto pathRegex = wregex(L"^[a-zA-Z]:\\\\.*$", ECMAScript | optimize);
		//auto bMatch = regex_match(pFileName, pathRegex);

		// raw
		const auto disk = pFileName[0];
		const auto deli = pFileName[1];
		const auto slash = pFileName[2];

		// Windows only supports 26 disks
		const auto bMatch = ((disk >= L'a' && disk <= L'z') 
			|| (disk >= L'A' && disk <= L'Z'))
			&& deli == L':'
			&& slash == L'\\';

		if (!bMatch) {
			return fileName;
		}
	}

	// tailing \0 -> later process issue
	//std::wstring ret(MAX_PATH, 0);
	//auto err = GetFullPathName(pFileName, MAX_PATH, &ret.at(0), nullptr);

	const LPWSTR pFullPathName = new wchar_t [MAX_PATH];
	memset(pFullPathName, 0, MAX_PATH);

	auto err = GetFullPathName(pFileName, MAX_PATH, pFullPathName, nullptr);
	std::wstring ret = pFullPathName;

	delete[] pFullPathName;

	return ret;
}

inline std::string ConvertWStrToStr(const std::wstring& input, const UINT codePage = CP_UTF8) {
	return to_byte_string(input, codePage);
}

inline std::wstring ConvertStrToWStr(const std::string& input, const UINT codePage = CP_UTF8) {
	return to_wide_string(input, codePage);
}
