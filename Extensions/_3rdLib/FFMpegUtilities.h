#pragma once

#include <algorithm>
#include <cmath>
#include <codecvt>
#include <cstdint>
#include <cstring>
#include <cwctype>
#include <limits>
#include <locale>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// Narrow dependencies of the shared decoder; no Fusion or Windows SDK types.
namespace FFMpegDetails {
template<class T> bool NearlyEqual(T a, T b) {
    if (a == b) return true;
    const auto norm = (std::min)(std::abs(a) + std::abs(b), (std::numeric_limits<T>::max)());
    return std::abs(a - b) < (std::max)((std::numeric_limits<T>::min)(),
        128 * std::numeric_limits<T>::epsilon() * norm);
}
inline bool EqualInsensitive(const wchar_t* a, const wchar_t* b) {
    while (*a && *b && std::towlower(*a) == std::towlower(*b)) { ++a; ++b; }
    return std::towlower(*a) == std::towlower(*b);
}
using WideCodec = std::conditional_t<sizeof(wchar_t) == 2,
    std::codecvt_utf8_utf16<wchar_t>, std::codecvt_utf8<wchar_t>>;
inline std::string UTF8(const std::wstring& value) {
    return std::wstring_convert<WideCodec>().to_bytes(value);
}
inline std::wstring Wide(const std::string& value) {
    return std::wstring_convert<WideCodec>().from_bytes(value);
}
}
