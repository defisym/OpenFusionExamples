#pragma once

#include <exception>

// ------------
// Char Defaults
// ------------
constexpr auto CHAR_SPACE = L' ';
constexpr auto CHAR_EMSPACE = L'¡¡';
constexpr auto CHAR_TAB = L'\t';

constexpr auto DEFAULT_EBORDER_OFFSET = 20;
constexpr auto DEFAULT_FORMAT_RESERVE = 10;

constexpr auto DEFAULT_CHARACTER = CHAR_EMSPACE;
constexpr auto EMPTY_CHAR = CHAR_SPACE;

constexpr auto DEFAULT_PARAM = L" ";

// ------------
// Effect Defaults
// ------------
constexpr auto SHAKE_RANDOM_RANGE = 1000;

// ------------
// Limitation
// ------------
// D3D limitation
// https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-limits
constexpr long D3D11_TEXTURE_SIZE = 16384;

// ------------
// Format flag
// ------------
constexpr auto FORMAT_IGNORE_UNKNOWN = 0b00000001;
constexpr auto FORMAT_IGNORE_INCOMPLETE = 0b00000010;
// text is still valid if only empty char left (aka iCon placeholder only)
constexpr auto FORMAT_IGNORE_AllowEmptyCharStringAfterFormatParsing = 0b00000100;

//constexpr auto FORMAT_IGNORE_DEFAULTFLAG = FORMAT_IGNORE_INCOMPLETE;
constexpr auto FORMAT_IGNORE_DEFAULTFLAG = FORMAT_IGNORE_UNKNOWN | FORMAT_IGNORE_INCOMPLETE;

// ------------
// Format operation
// ------------
// will stop parsing and use exception to return
constexpr auto FORMAT_OPERATION_GetRawStringByFilteredStringLength = 0b00000001;

constexpr auto FORMAT_INVALID_ICON = -1;

struct NeoStrFormatException final :std::exception {
private:
    int flag = 0;

public:
    NeoStrFormatException(char const* const pMsg, int flag = 0) noexcept
        :std::exception(pMsg) {
        this->flag = flag;
    }

    inline int GetFlag() const noexcept { return this->flag; }
};
