#pragma once

// if given file in relative path exists, then return false
// aka you need to disable given platform features
static inline bool EnablePlatform(const wchar_t* pDisableFileName) {
	const auto appPath = GetModuleFileNameStr();
	auto [disk, dir, file, ext] = SplitPath(appPath.c_str());

	const auto fullPath = disk + dir + pDisableFileName;

	FILE* pFile = nullptr;
	const auto openErr = _wfopen_s(&pFile, fullPath.c_str(), L"r");
	if (pFile) { auto closeErr = fclose(pFile); }

	return openErr != 0;
}