include_guard(GLOBAL)

# JNI-local build configuration; the headers themselves stay in the shared _DeLib tree.
set(FTW_CORE_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/../../_DeLib" CACHE PATH "Shared FindTheWay headers")
add_library(delib_helpers INTERFACE)
add_library(DeLib::Helpers ALIAS delib_helpers)
target_compile_features(delib_helpers INTERFACE cxx_std_20)
target_include_directories(delib_helpers INTERFACE
    "${FTW_CORE_INCLUDE}"
    "${FTW_CORE_INCLUDE}/../_3rdLib")

if(NOT WIN32)
    # Linux normally supplies iconv in libc; Apple/libiconv may require a library.
    # Android has no public NDK iconv API: supply a cross-built iconv to FindIconv.
    set(FTW_ICONV_ROOT "" CACHE PATH "iconv prefixes containing one directory per Android ABI")
    if(ANDROID AND FTW_ICONV_ROOT)
        if(NOT EXISTS "${FTW_ICONV_ROOT}/${ANDROID_ABI}/include/iconv.h"
                OR NOT EXISTS "${FTW_ICONV_ROOT}/${ANDROID_ABI}/lib/libiconv.a")
            message(FATAL_ERROR "Missing target iconv in ${FTW_ICONV_ROOT}/${ANDROID_ABI}")
        endif()
        set(Iconv_INCLUDE_DIR "${FTW_ICONV_ROOT}/${ANDROID_ABI}/include" CACHE PATH "Target iconv headers" FORCE)
        set(Iconv_LIBRARY "${FTW_ICONV_ROOT}/${ANDROID_ABI}/lib/libiconv.a" CACHE FILEPATH "Target iconv library" FORCE)
    endif()
    find_package(Iconv REQUIRED)
    target_link_libraries(delib_helpers INTERFACE Iconv::Iconv)
endif()

set(DELIB_ACP_ENCODING "UTF-8" CACHE STRING "Non-Windows CP_ACP compatibility encoding")
if(NOT WIN32)
    target_compile_definitions(delib_helpers INTERFACE "DELIB_ACP_ENCODING=\"${DELIB_ACP_ENCODING}\"")
endif()
