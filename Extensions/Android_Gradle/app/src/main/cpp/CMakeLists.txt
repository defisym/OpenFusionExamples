cmake_minimum_required(VERSION 3.22)
project(findtheway LANGUAGES CXX)
add_library(findtheway SHARED FindTheWayJNI.cpp)
target_compile_features(findtheway PRIVATE cxx_std_20)
target_compile_options(findtheway PRIVATE -Wall -Wextra -Wpedantic)
# All clients share the helper interface and the same _DeLib/_3rdLib search paths.
set(FTW_CORE_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../_DeLib" CACHE PATH "Shared FindTheWay headers")
include("${CMAKE_CURRENT_LIST_DIR}/../../../../cmake/DeLibHelpers.cmake")
target_link_libraries(findtheway PRIVATE DeLib::Helpers)
if(ANDROID)
    target_link_options(findtheway PRIVATE "-Wl,-z,max-page-size=16384")
endif()
if(NOT ANDROID)
    # Headers only: no dependency on AWT or libjvm (also works with headless JDKs).
    find_path(FTW_JNI_INCLUDE jni.h HINTS "$ENV{JAVA_HOME}/include" REQUIRED)
    find_path(FTW_JNI_PLATFORM_INCLUDE jni_md.h
        HINTS "$ENV{JAVA_HOME}/include/linux" "$ENV{JAVA_HOME}/include/darwin"
              "$ENV{JAVA_HOME}/include/win32" REQUIRED)
    target_include_directories(findtheway PRIVATE "${FTW_JNI_INCLUDE}" "${FTW_JNI_PLATFORM_INCLUDE}")
endif()
