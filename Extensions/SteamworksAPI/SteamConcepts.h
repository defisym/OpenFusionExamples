#pragma once

template <typename T>
concept WSTR = std::is_same_v<std::remove_cv_t<T>, LPWSTR>
|| std::is_same_v<std::remove_cv_t<T>, LPCWSTR>;

template <typename T>
concept CSTR = std::is_same_v<std::remove_cv_t<T>, LPSTR>
|| std::is_same_v<std::remove_cv_t<T>, LPCSTR>;

template <typename T>
concept STR = WSTR<T> || CSTR<T>;