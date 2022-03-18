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
inline std::wstring _itos(int Val) {
	std::wstring ret(DoubleStrSize, '\0');
	swprintf(&ret[0], DoubleStrSize, L"%d", Val);

	return ret;
}

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

inline void _dtos_signed_s(double Val, wchar_t** Str, size_t SpaceNum = 1) {
	auto positive = Val >= 0;
	Val = abs(Val);

	auto size = swprintf(nullptr, 0, L"%g", Val);
	auto bufsz = size + 1;
	auto signsz = 1 + SpaceNum;

	*Str = new wchar_t[signsz + bufsz];

	**Str = positive ? L'+' : L'-';

	for (size_t i = 1; i <= SpaceNum; i++) {
		*(*Str + i) = L' ';
	}

	swprintf(*Str + signsz, bufsz, L"%g", Val);
}

inline void _ftos_signed_s(double Val, wchar_t** Str, size_t SpaceNum = 1) {
	_dtos_signed_s(Val, Str, SpaceNum);
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

//convert Hex to Dex
inline int _hexSheet(const wchar_t p) {
	switch (p) {
	//numbers
	case L'0': {
		return 0;
	}
	case L'1': {
		return 1;
	}
	case L'2': {
		return 2;
	}
	case L'3': {
		return 3;
	}
	case L'4': {
		return 4;
	}
	case L'5': {
		return 5;
	}
	case L'6': {
		return 6;
	}
	case L'7': {
		return 7;
	}
	case L'8': {
		return 8;
	}
	case L'9': {
		return 9;
	}
	//char
	case L'A': {
		return 10;
	}
	case L'a': {
		return 10;
	}
	case L'B': {
		return 11;
	}
	case L'b': {
		return 11;
	}
	case L'C': {
		return 12;
	}
	case L'c': {
		return 12;
	}
	case L'D': {
		return 13;
	}
	case L'd': {
		return 13;
	}
	case L'E': {
		return 14;
	}
	case L'e': {
		return 14;
	}
	case L'F': {
		return 15;
	}
	case L'f': {
		return 15;
	}

	 //Error
	default: {
		return -1;
	}
	}
}

inline int _h2d(const wchar_t* p) {
	if (*p == L'#') {
		p += 1;
	}
	if ((*p == L'0') && (*(p + 1) == L'x' || *(p + 1) == L'X')) {
		p += 2;
	}

	auto len = wcslen(p);
	int ret = 0;

	for (int pow = 1; len != 0; pow = pow << 4, len--) {
		auto conv = _hexSheet(*(p + len - 1));

		if (conv == -1) {
			break;
		}

		ret += conv * pow;
	}

	return ret;
}