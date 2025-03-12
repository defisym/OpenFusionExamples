#pragma once

#include <string>
#include <format>
#include <exception>

struct D3DException final :std::runtime_error {
    D3DException(const char* pMessage) :std::runtime_error(pMessage) {}
    template <class... Types>
    D3DException(const char* pFmt, Types&&... args)
        :std::runtime_error(std::vformat(pFmt, std::make_format_args(args...))) {}
};