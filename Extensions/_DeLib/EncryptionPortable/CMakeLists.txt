cmake_minimum_required(VERSION 3.16)
project(EncryptionPortable LANGUAGES C CXX)
find_package(OpenSSL 1.1.1 REQUIRED COMPONENTS Crypto)
add_library(encryption_portable
    EncryptionPortable.cpp EncryptionPortableStream.cpp
    EncryptionPortableBuffer.cpp EncryptionPortableFile.cpp ../Encryption.cpp)
target_include_directories(encryption_portable PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:include/encryption>)
target_compile_features(encryption_portable PUBLIC cxx_std_17)
target_link_libraries(encryption_portable PUBLIC OpenSSL::Crypto)
target_compile_definitions(encryption_portable PRIVATE ENCRYPTION_PORTABLE_BUILD)
if(BUILD_SHARED_LIBS)
    target_compile_definitions(encryption_portable PUBLIC ENCRYPTION_PORTABLE_SHARED)
endif()
if(NOT WIN32)
    target_compile_definitions(encryption_portable PUBLIC _FILE_OFFSET_BITS=64)
endif()
if(MSVC)
    target_compile_options(encryption_portable PRIVATE /W4)
else()
    target_compile_options(encryption_portable PRIVATE -Wall -Wextra -Wpedantic)
endif()
option(ENCRYPTION_BUILD_TESTS "Build native regression tests (disable when cross-compiling)" OFF)
if(ENCRYPTION_BUILD_TESTS)
    enable_testing()
    add_executable(encryption_tests tests/core.cpp tests/c_api.c)
    target_link_libraries(encryption_tests PRIVATE encryption_portable)
    add_test(NAME encryption_tests COMMAND encryption_tests)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(AVFORMAT QUIET IMPORTED_TARGET libavformat libavcodec libavutil)
        if(AVFORMAT_FOUND)
            add_executable(encryption_avio_tests tests/avio.cpp)
            target_link_libraries(encryption_avio_tests PRIVATE encryption_portable PkgConfig::AVFORMAT)
            add_test(NAME encryption_avio_tests COMMAND encryption_avio_tests)
        endif()
    endif()
endif()

include(GNUInstallDirs)
install(TARGETS encryption_portable
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES EncryptionPortable.h EncryptionPortableCallbacks.h EncryptionPortableStream.h
    EncryptionPortableBuffer.h EncryptionPortableFile.h
    ../EncryptionCore.h ../EncryptionLegacy.h ../Encryption.h ../EncryptionAvio.h ../EncryptionStream.h
    DESTINATION include/encryption)
