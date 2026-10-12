cmake_minimum_required(VERSION 3.16)
project(EncryptionPortable LANGUAGES CXX)

find_package(OpenSSL 1.1.1 REQUIRED COMPONENTS Crypto)

add_library(encryption_portable EncryptionPortable.cpp)
target_include_directories(encryption_portable PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(encryption_portable PUBLIC cxx_std_17)
target_link_libraries(encryption_portable PUBLIC OpenSSL::Crypto)

if(MSVC)
  target_compile_options(encryption_portable PRIVATE /W4)
else()
  target_compile_options(encryption_portable PRIVATE -Wall -Wextra -Wpedantic)
endif()
