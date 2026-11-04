#include "UnicodeConversion.h"

#include <iostream>

static int checks = 0;

static void check(bool result) {
    ++checks;
    if (!result) throw std::runtime_error("Unicode conversion regression");
}

template<class Function>
static void invalid(Function function) {
    try { function(); }
    catch (const std::invalid_argument&) { ++checks; return; }
    throw std::runtime_error("Expected invalid Unicode input");
}

int main() {
    const std::string utf8 = "A\xe4\xb8\xad\xf0\x9f\x98\x80";
    const std::wstring wide = L"A\u4e2d\U0001f600";
    const std::u16string units = u"A\u4e2d\U0001f600";
    check(to_utf16_string(std::string_view(utf8)) == units);
    check(to_utf16_string(std::wstring_view(wide)) == units);
    check(to_utf16_bytes(wide) == std::vector<std::uint8_t>({0x41, 0, 0x2d, 0x4e, 0x3d, 0xd8, 0, 0xde}));
    check(to_utf16_string(std::string_view()).empty());
    check(to_utf16_string(std::wstring_view()).empty());
    check(to_utf16_bytes(L"").empty());
    check(to_utf16_string(std::string_view("A\0B", 3)) == std::u16string(u"A\0B", 3));
    check(to_utf16_string(std::wstring_view(L"A\0B", 3)) == std::u16string(u"A\0B", 3));
    check(to_utf16_bytes(std::wstring_view(L"A\0B", 3)) == std::vector<std::uint8_t>({0x41, 0, 0, 0, 0x42, 0}));
    check(to_utf16_string(std::string_view("\xf4\x8f\xbf\xbf")) == u"\U0010ffff");

    // Wide input historically preserves raw UTF-16 units, including isolated surrogates.
    const std::wstring surrogate(1, static_cast<wchar_t>(0xd800));
    check(to_utf16_string(std::wstring_view(surrogate)) == std::u16string(1, 0xd800));
    check(to_utf16_bytes(surrogate) == std::vector<std::uint8_t>({0, 0xd8}));

    for (const auto* text : {"\x80", "\xc0\x80", "\xe4\xb8", "\xe4x\xad",
            "\xed\xa0\x80", "\xf0\x9f", "\xf4\x90\x80\x80", "\xf5\x80\x80\x80"}) {
        invalid([&] { to_utf16_string(std::string_view(text)); });
    }
    if (sizeof(wchar_t) > 2) {
        const std::wstring outside(1, static_cast<wchar_t>(0x110000));
        invalid([&] { to_utf16_string(std::wstring_view(outside)); });
        invalid([&] { to_utf16_bytes(outside); });
    }
    std::cout << "PASS: " << checks << " standalone C++17 Unicode checks\n";
}
