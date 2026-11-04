#pragma once

#include "GeneralMacros.h"

#ifndef _WIN32
#include <filesystem>
#include <string>
#endif

#ifdef _WIN32
#define CUR_FOLDER CONNECT_STR(__FILE__, "\\..\\")
#else
namespace DeLibDetail {
    class PathString {
        std::string value;
    public:
        explicit PathString(const std::filesystem::path& path) : value(path.string()) {}

        // Pointer access requires an owner whose lifetime the caller can retain.
        operator char*() & noexcept { return value.data(); }
        operator const char*() const & noexcept { return value.c_str(); }
        operator char*() && = delete;
        operator const char*() const && = delete;
        size_t size() const noexcept { return value.size(); }
    };
}
#define CUR_FOLDER DeLibDetail::PathString(std::filesystem::path(__FILE__).parent_path() / "")
#endif

// Note: if you are using files inside a shard network folder
// RELATIVE_PATH macro will return a path like \\Mac\Home\Dev
// which is not supported by the linker.
// in this case, please use symbolic link (mklink /D) to create a local folder
// which points to the network folder, and use RELATIVE_PATH with that local folder
// for example, mklink /D C:\Dev Z:\Mac\Home\Dev
// depending on your network drive settings.
// direct link to path like mklink /D C:\Dev \\Mac\Home\Dev is not supported by Windows
// and path like that cannot open in Explorer either.
#ifdef _WIN32
#define RELATIVE_PATH(path) CONNECT_STR(CUR_FOLDER, path)
#else
#define RELATIVE_PATH(relative) DeLibDetail::PathString( \
    (std::filesystem::path(__FILE__).parent_path() / (relative)).lexically_normal())
#endif
