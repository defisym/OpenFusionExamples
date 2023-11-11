#pragma once

#include <string>

struct IniInterface {
	virtual inline double GetValue(const std::wstring& section, const std::wstring& item);
	virtual inline std::wstring GetString(const std::wstring& section, const std::wstring& item);
};