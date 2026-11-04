cmake_minimum_required(VERSION 3.16)
project(Encryption LANGUAGES C CXX)
find_package(OpenSSL 1.1.1 REQUIRED COMPONENTS Crypto)
add_library(encryption CAPI.cpp ../Encryption.cpp)
target_include_directories(encryption PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:include/encryption>)
target_compile_features(encryption PUBLIC cxx_std_17)
target_link_libraries(encryption PUBLIC OpenSSL::Crypto)
target_compile_definitions(encryption PRIVATE ENCRYPTION_BUILD)
if(BUILD_SHARED_LIBS)
    target_compile_definitions(encryption PUBLIC ENCRYPTION_SHARED)
endif()
if(NOT WIN32)
    target_compile_definitions(encryption PUBLIC _FILE_OFFSET_BITS=64)
endif()
if(MSVC)
    target_compile_options(encryption PRIVATE /W4)
else()
    target_compile_options(encryption PRIVATE -Wall -Wextra -Wpedantic)
endif()
option(ENCRYPTION_BUILD_TESTS "Build native regression tests (disable when cross-compiling)" OFF)
if(ENCRYPTION_BUILD_TESTS)
    enable_testing()
    add_executable(encryption_tests tests/core.cpp tests/c_api.c tests/windows_headers.cpp tests/asset.cpp)
    target_link_libraries(encryption_tests PRIVATE encryption)
    add_test(NAME encryption_tests COMMAND encryption_tests
        "${CMAKE_CURRENT_SOURCE_DIR}/../../NeoPic/ToInstall/Files/Examples/NeoPic/Test_Encrypt.dat"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../NeoPic/ToInstall/Files/Examples/NeoPic/Test_Origin.jpg")

endif()

include(GNUInstallDirs)
install(TARGETS encryption
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ../Encryption.h ../EncryptionStream.h ../UnicodeConversion.h
    DESTINATION include/encryption)
install(FILES Core.h KeyGen.h CAPI.h
    DESTINATION include/encryption/Encryption)
