// ReSharper disable CppInconsistentNaming
// ReSharper disable CppClangTidyClangDiagnosticReservedIdentifier
// ReSharper disable CppClangTidyBugproneReservedIdentifier
#pragma once

#include <cmath>
#include <format>

#include "GeneralDefinition.h"

enum class StrType {
	NotNum,
	IsNum,
	IsFloat,
};

//check if a string is number
constexpr inline StrType StrIsNumCore(const wchar_t* p) {
	auto type = StrType::IsNum;

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
		type = StrType::IsFloat;

		++p;

		while (*p >= L'0' && *p <= L'9') {
			++p;
		}

	}

	if (*p != L'\0') {
		type = StrType::NotNum;
	}

	return type;
}

constexpr inline StrType StrIsNumCore(const std::wstring& p) {
	return StrIsNumCore(p.c_str());
}

//check if a string is number
constexpr inline bool StrIsNum(const wchar_t* p) {
	return StrIsNumCore(p) != StrType::NotNum;
}

constexpr inline bool StrIsNum(const std::wstring& p) {
	return StrIsNum(p.c_str());
}

//check if a string is float
constexpr inline bool StrIsFloat(const wchar_t* p) {
	return StrIsNumCore(p) == StrType::IsFloat;
}

constexpr inline bool StrIsFloat(const std::wstring& p) {
	return StrIsFloat(p.c_str());
}

inline void _copy_str(const std::wstring& src, wchar_t** ppDst) {
	const auto size = src.length();
	const auto bufsz = size + 1;

	*ppDst = new wchar_t[bufsz];
	memset(*ppDst, 0, bufsz);

	memcpy(*ppDst, src.data(), size);
}

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<typename Arithmetic>
//convert double to string, 2X faster than to_string
inline std::wstring ntos(Arithmetic Val) {
	return std::format(L"{}", Val);
}

inline std::wstring _itos(int Val) {
	return ntos(Val);
}

inline std::wstring _itos(size_t Val) {
	return ntos(Val);
}

inline std::wstring _dtos(double Val) {
	return ntos(Val);
}

inline std::wstring _ftos(float Val) {
	return ntos(Val);
}

inline void _dtos(double Val, std::wstring& Str) {
	Str = ntos(Val);
}

inline void _ftos(float Val, std::wstring& Str) {
	Str = ntos(Val);
}


template<typename Arithmetic>
//Same, but with sign
inline std::wstring ntos_signed(Arithmetic Val) {
	return std::format(L"{:+}", Val);
}
inline std::wstring _dtos_signed(double Val) {
	return ntos_signed(Val);
}

inline std::wstring _ftos_signed(float Val) {
	return ntos_signed(Val);
}

inline void _dtos_signed(double Val, std::wstring& Str) {
	Str = ntos_signed(Val);
}

inline void _dtos_signed(double Val, wchar_t** Str) {
	_copy_str(_dtos_signed(Val), Str);
}

inline void _ftos_signed(float Val, std::wstring& Str) {
	_dtos_signed((double)Val, Str);
}

inline void _ftos_signed(float Val, wchar_t** Str) {
	_dtos_signed((double)Val, Str);
}

inline std::wstring _dtos_signed_s(double Val, size_t SpaceNum = 1) {
	const auto positive = Val >= 0;
	auto sign = positive ? L'+' : L'-';

	auto ret = _dtos(abs(Val));
	auto totalLength = SpaceNum + ret.length();

	const auto fmt = std::format(L"{}{{:>{}}}", sign, totalLength);

	return std::vformat(std::wstring_view(fmt), std::make_wformat_args(ret));
}

inline std::wstring _ftos_signed_s(float Val, size_t SpaceNum = 1) {
	return _dtos_signed_s((double)Val, SpaceNum);
}

inline void _dtos_signed_s(double Val, wchar_t** Str, size_t SpaceNum = 1) {
	_copy_str(_dtos_signed_s(Val, SpaceNum), Str);
}

inline void _ftos_signed_s(float Val, wchar_t** Str, size_t SpaceNum = 1) {
	_dtos_signed_s((double)Val, Str, SpaceNum);
}

constexpr double _get_fracture(int n) {
	switch (n) {
	case 1:
		return  1.0 / 10.0;
	case 2:
		return  1.0 / 100.0;
	case 3:
		return  1.0 / 1000.0;
	case 4:
		return  1.0 / 10000.0;
	case 5:
		return  1.0 / 100000.0;
	case 6:
		return  1.0 / 1000000.0;
	case 7:
		return  1.0 / 10000000.0;
	case 8:
		return  1.0 / 100000000.0;
	case 9:
		return  1.0 / 1000000000.0;
	default:
		return 1.0 / std::pow(10.0, n);
	}
}

template<typename Arithmetic>
//convert string to double, 5X faster than std::stod
constexpr inline Arithmetic ston(const wchar_t* p) {
	constexpr bool bUnsigned = std::is_unsigned_v<Arithmetic>;
	constexpr bool bFloat = std::is_floating_point_v<Arithmetic>;

	Arithmetic r = 0;
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

	if constexpr (bFloat) {
		if (*p == L'.') {
			Arithmetic f = 0;
			int n = 0;
			++p;

			while (*p >= L'0' && *p <= L'9') {
				f = (f * 10) + (*p - L'0');
				++p;
				++n;
			}

			r += f * _get_fracture(n);
		}
	}

	if constexpr (!bUnsigned) { if (neg) { r = -r; } }

	return r;
}

constexpr inline double _stod(const wchar_t* p) {
	return ston<double>(p);
}

constexpr inline double _stod(const std::wstring& p) {
	return _stod(p.c_str());
}

constexpr inline double _stod(const std::wstring_view& str) {
	return _stod(GetTrimmedStr(const_cast<wchar_t*>(str.data()), str.size()).data());
}

constexpr inline float _stof(const wchar_t* p) {
	return static_cast<float>(_stod(p));
}

constexpr inline float _stof(const std::wstring& p) {
	return (float)_stod(p);
}

constexpr inline float _stof(const std::wstring_view& str) {
	return static_cast<float>(_stod(str));
}

constexpr inline int _stoi(const wchar_t* p) {
	return ston<int>(p);
}

constexpr inline int _stoi(const std::wstring& p) {
	return _stoi(p.c_str());
}

constexpr inline int _stoi(const std::wstring_view& str) {
	return _stoi(GetTrimmedStr(const_cast<wchar_t*>(str.data()), str.size()).data());
}

//convert Hex to Dex
constexpr inline int _hexSheet(const wchar_t p) {
	switch (p) {
		//numbers
	case L'0':
	{
		return 0;
	}
	case L'1':
	{
		return 1;
	}
	case L'2':
	{
		return 2;
	}
	case L'3':
	{
		return 3;
	}
	case L'4':
	{
		return 4;
	}
	case L'5':
	{
		return 5;
	}
	case L'6':
	{
		return 6;
	}
	case L'7':
	{
		return 7;
	}
	case L'8':
	{
		return 8;
	}
	case L'9':
	{
		return 9;
	}
	//char
	case L'A':
	case L'a':
	{
		return 10;
	}
	case L'B':
	case L'b':
	{
		return 11;
	}
	case L'C':
	case L'c':
	{
		return 12;
	}
	case L'D':
	case L'd':
	{
		return 13;
	}
	case L'E':
	case L'e':
	{
		return 14;
	}
	case L'F':
	case L'f':
	{
		return 15;
	}

	//Error
	default:
	{
		return -1;
	}
	}
}

constexpr inline DWORD _h2d(const wchar_t* p, size_t strLen = -1) {
	auto len = strLen == static_cast<size_t>(-1) ? wcslen(p) : strLen;

	if (*p == L'#') {
		p += 1;
		len -= 1;
	}
	if ((*p == L'0') && (*(p + 1) == L'x' || *(p + 1) == L'X')) {
		p += 2;
		len -= 2;
	}

	DWORD ret = 0;

	for (DWORD pow = 1; len != 0; pow = pow << 4, len--) {
		const auto conv = _hexSheet(*(p + len - 1));

		if (conv == -1) {
			break;
		}

		ret += conv * pow;
	}

	return ret;
}