#pragma once

#include	<cmath>

enum class StrType
{
	NotNum,
	IsNum,
	IsFloat,
};

//check if a string is number
inline StrType StrIsNumCore(const wchar_t* p) {
	StrType Type= StrType::IsNum;

	if (*p == L'-') {
		++p;
	}
	if (*p == L'+') {
		++p;
	}

	while (*p >= L'0' && *p <= L'9') {
		++p;
	}

	if (*p == L'.') {
		Type = StrType::IsFloat;

		++p;

		while (*p >= L'0' && *p <= L'9') {
			++p;
		}

	}

	if (*p != L'\0') {
		Type = StrType::NotNum;
	}

	return Type;
}

inline StrType StrIsNumCore(const std::wstring& p) {
	return StrIsNumCore(p.c_str());
}

//check if a string is number
inline bool StrIsNum(const wchar_t* p) {
	return StrIsNumCore(p) == StrType::NotNum ? false : true;
}

inline bool StrIsNum(const std::wstring& p) {
	return StrIsNum(p.c_str());
}

//check if a string is float
inline bool StrIsFloat(const wchar_t* p) {
	return StrIsNumCore(p) == StrType::IsFloat;
}

inline bool StrIsFloat(const std::wstring& p) {
	return StrIsFloat(p.c_str());
}

constexpr auto DoubleStrSize = 50+1;

//convert double to string, 2X faster than to_string
inline std::wstring _dtos(double Val) {
	//auto size = swprintf(nullptr, 0, L"%f", Val);
	std::wstring ret(DoubleStrSize, '\0');
	swprintf(&ret[0], DoubleStrSize, L"%f", Val);

	return ret;
}

inline std::wstring _ftos(float Val) {
	return _dtos(Val);
}

inline void _dtos(double Val, std::wstring& Str) {
	swprintf(&Str[0], DoubleStrSize, L"%f", Val);
}

inline void _ftos(double Val, std::wstring& Str) {
	_dtos(Val, Str);
}

//Same, but with sign
inline std::wstring _dtos_signed(double Val) {
	std::wstring ret(DoubleStrSize, '\0');
	swprintf(&ret[0], DoubleStrSize, L"%+g", Val);

	return ret;
}

inline std::wstring _ftos_signed(float Val) {
	return _dtos_signed(Val);
}

inline void _dtos_signed(double Val, std::wstring& Str) {
	swprintf(&Str[0], DoubleStrSize, L"%+g", Val);
}

inline void _dtos_signed(double Val, wchar_t** Str) {
	auto size = swprintf(nullptr, 0, L"%+g", Val);
	auto bufsz = size + 1;
	*Str = new wchar_t[bufsz];

	swprintf(*Str, bufsz, L"%+g", Val);
}

inline void _ftos_signed(double Val, std::wstring& Str) {
	_dtos_signed(Val, Str);
}

inline void _ftos_signed(double Val, wchar_t** Str) {
	_dtos_signed(Val, Str);
}

//convert string to double, 5X faster than std::stod
inline double _stod(const wchar_t* p) {
	double r = 0.0;
	bool neg = false;

	if (*p == L'-') {
		neg = true;
		++p;
	}
	if (*p == L'+') {
		++p;
	}

	while (*p >= L'0' && *p <= L'9') {
		r = (r * 10.0) + (*p - L'0');
		++p;
	}

	if (*p == L'.') {
		double f = 0.0;
		int n = 0;
		++p;

		while (*p >= L'0' && *p <= L'9') {
			f = (f * 10.0) + (*p - L'0');
			++p;
			++n;
		}

		r += f / std::pow(10.0, n);
	}

	if (*p != L'\0') {
		return 0.0;
	}

	if (neg) {
		r = -r;
	}

	return r;
}

inline double _stod(const std::wstring& p) {
	return _stod(p.c_str());
}

inline float _stof(const std::wstring& p) {
	return (float)_stod(p.c_str());
}

inline int _stoi(const wchar_t* p) {
	int r = 0;
	bool neg = false;

	if (*p == L'-') {
		neg = true;
		++p;
	}
	if (*p == L'+') {
		++p;
	}

	while (*p >= L'0' && *p <= L'9') {
		r = (r * 10) + (*p - L'0');
		++p;
	}

	if (neg) {
		r = -r;
	}

	return r;
}

inline int _stoi(const std::wstring& p) {
	return _stoi(p.c_str());
}