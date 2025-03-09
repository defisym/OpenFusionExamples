#pragma once

#include <string>
#include <string_view>

#include <type_traits>

template<typename T>
concept StringConcept = std::is_same_v<std::remove_cvref_t<T>, std::string>
|| std::is_same_v<std::remove_cvref_t<T>, std::wstring>;

template<StringConcept StringType = std::wstring>
struct CharType {
    using Type = std::remove_cvref_t<decltype(StringType{}[0]) > ;

    static size_t Length(const Type* p) {
        if constexpr (std::is_same_v<std::remove_cvref_t<StringType>, std::wstring>) {
            return wcslen(p);
        }
        else {
            return strlen(p);
        }
    }
};

template<StringConcept StringType = std::wstring>
struct ViewType {};

template<>
struct ViewType<std::string> {
    using Type = std::string_view;
};

template<>
struct ViewType<std::wstring> {
    using Type = std::wstring_view;
};

template <typename T>
concept CStyleWideStrConcept = std::is_same_v<std::remove_cvref_t<T>, wchar_t*>
|| std::is_same_v<std::remove_cvref_t<T>, const wchar_t*>;

template <typename T>
concept CStyleCharStrConcept = std::is_same_v<std::remove_cvref_t<T>, char*>
|| std::is_same_v<std::remove_cvref_t<T>, const char*>;

template <typename T>
concept CStyleStrConcept = CStyleWideStrConcept<T> || CStyleCharStrConcept<T>;

template<typename T>
concept CharConcept = std::is_same_v<std::remove_cvref_t<T>, char>
|| std::is_same_v<std::remove_cvref_t<T>, wchar_t>;