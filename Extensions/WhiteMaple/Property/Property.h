#pragma once

#include <map>
#include <string>
#include <format>

using PropCoefSheet = std::map<std::wstring, double>;

inline static const PropCoefSheet charCoefSheet{
	{L"WF", -0.5},
	{L"CW", -0.5},
	{L"FE", -0.5},
	{L"EC", -0.5},
	{L"FW", 1},
	{L"WC", 1},
	{L"EF", 1},
	{L"CE", 1}
};
inline static const PropCoefSheet weatherCoefSheet{
	{L"WF", -0.5},
	{L"CW", -0.5},
	{L"FE", -0.5},
	{L"EC", -0.5},
	{L"FF", 1},
	{L"WW", 1},
	{L"CC", 1},
	{L"EE", 1}
};

struct Property {
	wchar_t internalProp;

	inline void SetProp(wchar_t prop) {
		internalProp = prop;
	}

	inline double GetPropCoef(wchar_t prop, const PropCoefSheet& sheet) {
		std::wstring key = std::format(L"{}{}", internalProp, prop);

		const auto it = sheet.find(key);

		if (it == sheet.end()) {
			return 1;
		}

		return it->second + 1;
	}

	inline double GetCharPropCoef(wchar_t prop) {
		return GetPropCoef(prop, charCoefSheet);
	}

	inline double GetWeatherPropCoef(wchar_t prop) {
		return GetPropCoef(prop, weatherCoefSheet);
	}
};