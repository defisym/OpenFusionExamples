// ReSharper disable CppInconsistentNaming
// ReSharper disable CppClangTidyClangDiagnosticReservedIdentifier
// ReSharper disable CppClangTidyBugproneReservedIdentifier
#pragma once

#include <cmath>
#include <format>

#include "StringTraits.h"
#include "ArithmeticTraits.h"

#include "GeneralDefinition.h"

enum class StrType :uint8_t {
    NotNum,
    IsNum,
    IsFloat,
};

template<StringConcept StringType = std::wstring>
struct MathSign {};

template<>
struct MathSign<std::string> {
    constexpr static auto END = '\0';

    constexpr static auto POS = '+';
    constexpr static auto NEG = '-';
    constexpr static auto POINT = '.';

    constexpr static auto SHARP = '#';
    constexpr static auto x = 'x';
    constexpr static auto X = 'X';
    constexpr static auto a = 'a';
    constexpr static auto A = 'A';
    constexpr static auto b = 'b';
    constexpr static auto B = 'B';
    constexpr static auto c = 'c';
    constexpr static auto C = 'C';
    constexpr static auto d = 'd';
    constexpr static auto D = 'D';
    constexpr static auto e = 'e';
    constexpr static auto E = 'E';
    constexpr static auto f = 'f';
    constexpr static auto F = 'F';

    constexpr static auto ZERO = '0';
    constexpr static auto ONE = '1';
    constexpr static auto TWO = '2';
    constexpr static auto THREE = '3';
    constexpr static auto FOUR = '4';
    constexpr static auto FIVE = '5';
    constexpr static auto SIX = '6';
    constexpr static auto SEVEN = '7';
    constexpr static auto EIGHT = '8';
    constexpr static auto NINE = '9';
};

template<>
struct MathSign<std::wstring> {
    constexpr static auto END = L'\0';

    constexpr static auto POS = L'+';
    constexpr static auto NEG = L'-';
    constexpr static auto POINT = L'.';

    constexpr static auto SHARP = L'#';
    constexpr static auto x = L'x';
    constexpr static auto X = L'X';
    constexpr static auto a = L'a';
    constexpr static auto A = L'A';
    constexpr static auto b = L'b';
    constexpr static auto B = L'B';
    constexpr static auto c = L'c';
    constexpr static auto C = L'C';
    constexpr static auto d = L'd';
    constexpr static auto D = L'D';
    constexpr static auto e = L'e';
    constexpr static auto E = L'E';
    constexpr static auto f = L'f';
    constexpr static auto F = L'F';

    constexpr static auto ZERO = L'0';
    constexpr static auto ONE = L'1';
    constexpr static auto TWO = L'2';
    constexpr static auto THREE = L'3';
    constexpr static auto FOUR = L'4';
    constexpr static auto FIVE = L'5';
    constexpr static auto SIX = L'6';
    constexpr static auto SEVEN = L'7';
    constexpr static auto EIGHT = L'8';
    constexpr static auto NINE = L'9';
};

//check if a string is number
template<StringConcept StringType = std::wstring>
constexpr StrType StrIsNumCore(const typename CharType<StringType>::Type* p) {
	auto type = StrType::IsNum;

	if (*p == MathSign<StringType>::NEG) {
		++p;
	}
	if (*p == MathSign<StringType>::POS) {
		++p;
	}

	while (*p >= MathSign<StringType>::ZERO && *p <= MathSign<StringType>::NINE) {
		++p;
	}

	if (*p == MathSign<StringType>::POINT) {
		type = StrType::IsFloat;

		++p;

		while (*p >= MathSign<StringType>::ZERO && *p <= MathSign<StringType>::NINE) {
			++p;
		}

	}

	if (*p != MathSign<StringType>::END) {
		type = StrType::NotNum;
	}

	return type;
}

template<StringConcept StringType = std::wstring>
constexpr StrType StrIsNumCore(const StringType& p) {
	return StrIsNumCore<StringType>(p.c_str());
}

//check if a string is number
constexpr bool StrIsNum(const wchar_t* p) {
	return StrIsNumCore<std::wstring>(p) != StrType::NotNum;
}

constexpr bool StrIsNum(const char* p) {
    return StrIsNumCore<std::string>(p) != StrType::NotNum;
}

template<StringConcept StringType = std::wstring>
constexpr bool StrIsNum(const StringType& p) {
	return StrIsNum(p.c_str());
}

//check if a string is float number
constexpr bool StrIsFloat(const wchar_t* p) {
	return StrIsNumCore<std::wstring>(p) == StrType::IsFloat;
}

constexpr bool StrIsFloat(const char* p) {
    return StrIsNumCore<std::string>(p) == StrType::IsFloat;
}

template<StringConcept StringType = std::wstring>
constexpr bool StrIsFloat(const StringType& p) {
	return StrIsFloat(p.c_str());
}

template<StringConcept StringType = std::wstring>
void _copy_str(const StringType& src,
               typename CharType<StringType>::Type** ppDst) {
    using CT = typename CharType<StringType>::Type;

	const auto size = src.length();
	const auto bufsz = size + 1;

    delete *ppDst;
	*ppDst = new CT[bufsz];
	memset(*ppDst, 0, sizeof(CT) * bufsz);

	memcpy(*ppDst, src.data(), sizeof(CT) * size);
}

//convert double to string, 2X faster than to_string
template<ArithmeticConcept Arithmetic>
void ntos(std::wstring& str, const Arithmetic Val) {
    str = std::format(L"{}", Val);
}

template<ArithmeticConcept Arithmetic>
void ntos(std::string& str, const Arithmetic Val) {
    str = std::format("{}", Val);
}

template<ArithmeticConcept Arithmetic, StringConcept StringType = std::wstring>
StringType ntos(const Arithmetic Val) {
    StringType str;
    ntos<Arithmetic>(str, Val);
    return str;
}

template<StringConcept StringType = std::wstring>
StringType _itos(const int Val) {
    return ntos<decltype(Val), StringType>(Val);
}

template<StringConcept StringType = std::wstring>
StringType _itos(const size_t Val) {
	return ntos<decltype(Val), StringType>(Val);
}

template<StringConcept StringType = std::wstring>
StringType _dtos(const double Val) {
	return ntos<decltype(Val), StringType>(Val);
}

template<StringConcept StringType = std::wstring>
StringType _ftos(const float Val) {
	return ntos<decltype(Val), StringType>(Val);
}

template<StringConcept StringType = std::wstring>
void _dtos(const double Val, StringType& Str) {
	Str = ntos<decltype(Val), StringType>(Val);
}

template<StringConcept StringType = std::wstring>
void _ftos(const float Val, StringType& Str) {
	Str = ntos<decltype(Val), StringType>(Val);
}

//Same as ntos, but with sign
template<ArithmeticConcept Arithmetic>
void ntos_signed(std::wstring& Str, const Arithmetic Val) {
    Str = std::format(L"{:+}", Val);
}

template<ArithmeticConcept Arithmetic>
void ntos_signed(std::string& Str, const Arithmetic Val) {
    Str = std::format("{:+}", Val);
}

template<ArithmeticConcept Arithmetic, StringConcept StringType = std::wstring>
StringType ntos_signed(const Arithmetic Val) {
    StringType str;
    ntos_signed<Arithmetic>(str, Val);
    return str;
}

template<StringConcept StringType = std::wstring>
StringType _dtos_signed(const double Val) {
	return ntos_signed<decltype(Val), StringType>(Val);
}

template<StringConcept StringType = std::wstring>
StringType _ftos_signed(const float Val) {
	return ntos_signed<decltype(Val), StringType>(Val);
}

template<StringConcept StringType = std::wstring>
void _dtos_signed(const double Val, StringType& Str) {
	Str = ntos_signed<decltype(Val), StringType>(Val);
}

inline void _dtos_signed(const double Val, wchar_t** Str) {
	_copy_str<std::wstring>(_dtos_signed<std::wstring>(Val), Str);
}

inline void _dtos_signed(const double Val, char** Str) {
    _copy_str<std::string>(_dtos_signed<std::string>(Val), Str);
}

template<StringConcept StringType = std::wstring>
void _ftos_signed(const float Val, StringType& Str) {
	_dtos_signed(static_cast<double>(Val), Str);
}

inline void _ftos_signed(const float Val, wchar_t** Str) {
	_dtos_signed(Val, Str);
}

inline void _ftos_signed(const float Val, char** Str) {
    _dtos_signed(Val, Str);
}

inline void _dtos_signed_s(std::wstring& Str, const double Val, const size_t SpaceNum = 1) {
    const auto positive = Val >= 0;
    auto sign = positive ? MathSign<std::wstring>::POS : MathSign<std::wstring>::NEG;

    auto ret = _dtos<std::wstring>(abs(Val));
    auto totalLength = SpaceNum + ret.length();

    const auto fmt = std::format(L"{}{{:>{}}}", sign, totalLength);
    Str = std::vformat(std::wstring_view(fmt), std::make_wformat_args(ret));
}

inline void _dtos_signed_s(std::string& Str, const double Val, const size_t SpaceNum = 1) {
    const auto positive = Val >= 0;
    auto sign = positive ? MathSign<std::string>::POS : MathSign<std::string>::NEG;

    auto ret = _dtos<std::string>(abs(Val));
    auto totalLength = SpaceNum + ret.length();

    const auto fmt = std::format("{}{{:>{}}}", sign, totalLength);
    Str = std::vformat(std::string_view(fmt), std::make_format_args(ret));
}

template<StringConcept StringType = std::wstring>
StringType _dtos_signed_s(const double Val, const size_t SpaceNum = 1) {
    StringType str;
    _dtos_signed_s(str, Val, SpaceNum);
    return str;
}
template<StringConcept StringType = std::wstring>
StringType _ftos_signed_s(const float Val, const size_t SpaceNum = 1) {
	return _dtos_signed_s<StringType>(static_cast<double>(Val), SpaceNum);
}

inline void _dtos_signed_s(const double Val, wchar_t** Str, const size_t SpaceNum = 1) {
	_copy_str(_dtos_signed_s<std::wstring>(Val, SpaceNum), Str);
}

inline void _dtos_signed_s(const double Val, char** Str, const size_t SpaceNum = 1) {
    _copy_str(_dtos_signed_s<std::string>(Val, SpaceNum), Str);
}

inline void _ftos_signed_s(const float Val, wchar_t** Str, const size_t SpaceNum = 1) {
	_dtos_signed_s(Val, Str, SpaceNum);
}

inline void _ftos_signed_s(const float Val, char** Str, const size_t SpaceNum = 1) {
    _dtos_signed_s(Val, Str, SpaceNum);
}

constexpr double _get_fracture(const int n) {
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

template<ArithmeticConcept Arithmetic, StringConcept StringType = std::wstring>
//convert string to double, 5X faster than std::stod
constexpr Arithmetic ston(const typename CharType<StringType>::Type* p) {
	constexpr bool bUnsigned = std::is_unsigned_v<Arithmetic>;
	constexpr bool bFloat = std::is_floating_point_v<Arithmetic>;

	Arithmetic r = 0;
	bool neg = false;

	if (*p == MathSign<StringType>::NEG) {
		neg = true;
		++p;
	}
	if (*p == MathSign<StringType>::POS) {
		++p;
	}

	while (*p >= MathSign<StringType>::ZERO && *p <= MathSign<StringType>::NINE) {
		r = (r * 10) + (*p - MathSign<StringType>::ZERO);
		++p;
	}

	if constexpr (bFloat) {
		if (*p == MathSign<StringType>::POINT) {
			Arithmetic f = 0;
			int n = 0;
			++p;

			while (*p >= MathSign<StringType>::ZERO && *p <= MathSign<StringType>::NINE) {
				f = (f * 10) + (*p - MathSign<StringType>::ZERO);
				++p;
				++n;
			}

			r += f * _get_fracture(n);
		}
	}

	if constexpr (!bUnsigned) { if (neg) { r = -r; } }

	return r;
}

constexpr double _stod(const wchar_t* p) {
    return ston<double, std::wstring>(p);
}

constexpr double _stod(const char* p) {
    return ston<double, std::string>(p);
}

template<StringConcept StringType = std::wstring>
constexpr double _stod(const StringType& p) {
	return _stod(p.c_str());
}

template<StringConcept StringType = std::wstring>
constexpr double _stod(const typename ViewType<StringType>::Type& str) {
    return _stod(GetTrimmedStr(const_cast<typename CharType<StringType>::Type*>(str.data()), str.size()).data());
}

constexpr float _stof(const wchar_t* p) {
	return static_cast<float>(_stod(p));
}

constexpr float _stof(const char* p) {
    return static_cast<float>(_stod(p));
}

template<StringConcept StringType = std::wstring>
constexpr float _stof(const StringType& p) {
	return _stof(p.c_str());
}

template<StringConcept StringType = std::wstring>
constexpr float _stof(const typename ViewType<StringType>::Type& str) {
	return static_cast<float>(_stod(str));
}

constexpr int _stoi(const wchar_t* p) {
	return ston<int, std::wstring>(p);
}

constexpr int _stoi(const char* p) {
    return ston<int, std::string>(p);
}

template<StringConcept StringType = std::wstring>
constexpr int _stoi(const StringType& p) {
	return _stoi(p.c_str());
}

template<StringConcept StringType = std::wstring>
constexpr int _stoi(const typename ViewType<StringType>::Type& str) {
	return _stoi(GetTrimmedStr(const_cast<wchar_t*>(str.data()), str.size()).data());
}

//convert Hex to Dex
template<StringConcept StringType = std::wstring>
constexpr int _hexSheet(const typename CharType<StringType>::Type p) {
	switch (p) {
	//numbers
    case MathSign<StringType>::ZERO:
	{
		return 0;
	}
	case MathSign<StringType>::ONE:
	{
		return 1;
	}
	case MathSign<StringType>::TWO:
	{
		return 2;
	}
	case MathSign<StringType>::THREE:
	{
		return 3;
	}
	case MathSign<StringType>::FOUR:
	{
		return 4;
	}
	case MathSign<StringType>::FIVE:
	{
		return 5;
	}
	case MathSign<StringType>::SIX:
	{
		return 6;
	}
	case MathSign<StringType>::SEVEN:
	{
		return 7;
	}
	case MathSign<StringType>::EIGHT:
	{
		return 8;
	}
	case MathSign<StringType>::NINE:
	{
		return 9;
	}
	//char
	case MathSign<StringType>::A:
	case MathSign<StringType>::a:
	{
		return 10;
	}
	case MathSign<StringType>::B:
	case MathSign<StringType>::b:
	{
		return 11;
	}
	case MathSign<StringType>::C:
	case MathSign<StringType>::c:
	{
		return 12;
	}
	case MathSign<StringType>::D:
	case MathSign<StringType>::d:
	{
		return 13;
	}
	case MathSign<StringType>::E:
	case MathSign<StringType>::e:
	{
		return 14;
	}
	case MathSign<StringType>::F:
	case MathSign<StringType>::f:
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

template<StringConcept StringType = std::wstring>
constexpr DWORD _h2d(const typename CharType<StringType>::Type* p, const size_t strLen = -1) {
    auto len = strLen == static_cast<size_t>(-1)
        ? CharType<StringType>::Length(p)
        : strLen;

	if (*p == MathSign<StringType>::SHARP) {
		p += 1;
		len -= 1;
	}

	if ((*p == MathSign<StringType>::ZERO)
        && (*(p + 1) == MathSign<StringType>::x || *(p + 1) == MathSign<StringType>::X)) {
		p += 2;
		len -= 2;
	}

	DWORD ret = 0;

	for (DWORD pow = 1; len != 0; pow = pow << 4, --len) {
		const auto conv = _hexSheet(*(p + len - 1));

		if (conv == -1) {
			break;
		}

		ret += conv * pow;
	}

	return ret;
}