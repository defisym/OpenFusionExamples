cmake_minimum_required(VERSION 3.21)
project(OFESDLSDK LANGUAGES C CXX)

if(NOT WIN32)
    message(FATAL_ERROR "This SDK is for Windows. Use Visual Studio or a Windows cross toolchain.")
endif()
include(ExternalProject)

get_filename_component(dependencies "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(sdl_source "${dependencies}/SDL2")
set(mixer_source "${dependencies}/SDL-Mixer-X")
foreach(source IN ITEMS "${sdl_source}" "${mixer_source}")
    if(NOT EXISTS "${source}/CMakeLists.txt")
        message(FATAL_ERROR "Initialize the SDL2 and SDL-Mixer-X submodules before configuring this SDK.")
    endif()
endforeach()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(sdk_arch x64)
else()
    set(sdk_arch x86)
endif()
set(OFE_SDL_SDK_ROOT "${dependencies}/SDL-sdk" CACHE PATH "Combined SDL2/MixerX SDK output")
set(OFE_BUILD_JOBS 4 CACHE STRING "Parallel jobs for dependency builds")
set(OFE_AUDIOCODECS_SOURCE_DIR "" CACHE PATH "Optional existing checkout of the pinned AudioCodecs revision")
mark_as_advanced(OFE_AUDIOCODECS_SOURCE_DIR)
# Separate intermediate installs by architecture. Only final SDK headers/libs are shared.
set(stage "${CMAKE_BINARY_DIR}/stage")
set(common_args
    "-DCMAKE_INSTALL_PREFIX:PATH=${stage}"
    "-DCMAKE_INSTALL_LIBDIR:STRING=lib"
    "-DCMAKE_BUILD_TYPE:STRING=Release")
if(CMAKE_TOOLCHAIN_FILE)
    list(APPEND common_args "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE}")
endif()
foreach(flag IN ITEMS CMAKE_C_FLAGS CMAKE_CXX_FLAGS CMAKE_EXE_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS)
    if(DEFINED ${flag})
        list(APPEND common_args "-D${flag}:STRING=${${flag}}")
    endif()
endforeach()
# All child builds use Release, including when the consuming plugin is Debug.
set(build_command "${CMAKE_COMMAND}" --build <BINARY_DIR> --config Release --parallel "${OFE_BUILD_JOBS}")
set(install_command "${CMAKE_COMMAND}" --install <BINARY_DIR> --config Release)
ExternalProject_Add(ofe_sdl2
    SOURCE_DIR "${sdl_source}" DOWNLOAD_COMMAND "" UPDATE_COMMAND ""
    CMAKE_ARGS ${common_args}
        -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TEST=OFF -DSDL_TESTS=OFF
        -DSDL2_DISABLE_INSTALL=OFF -DSDL2_DISABLE_UNINSTALL=ON
        -DSDL_INSTALL_CMAKEDIR=lib/cmake/SDL2
    BUILD_COMMAND ${build_command} INSTALL_COMMAND ${install_command}
    BUILD_ALWAYS TRUE)

if(OFE_AUDIOCODECS_SOURCE_DIR)
    set(codec_download SOURCE_DIR "${OFE_AUDIOCODECS_SOURCE_DIR}" DOWNLOAD_COMMAND "" UPDATE_COMMAND "")
else()
    set(codec_download
        GIT_REPOSITORY https://github.com/WohlSoft/AudioCodecs.git
        GIT_TAG bb11ca3cc1d7597b23a530d519b3e636dceb7b23
        UPDATE_DISCONNECTED TRUE)
endif()
ExternalProject_Add(ofe_audio_codecs
    ${codec_download} DEPENDS ofe_sdl2
    CMAKE_ARGS ${common_args}
        "-DSDL2_REPO_PATH:PATH=${stage}" "-DSDL2_INSTALL_DIR:PATH=${stage}"
        "-DCMAKE_PREFIX_PATH:PATH=${stage}"
        "-DSDL2_DIR:PATH=${stage}/lib/cmake/SDL2"
        -DDOWNLOAD_SDL2_DEPENDENCY=OFF -DUSE_LOCAL_SDL2=OFF
        -DCODECS_BUILD_SHARED=OFF -DCODECS_BUILD_STATIC=ON
        -DBUILD_ADLMIDI=OFF -DBUILD_OPNMIDI=OFF -DBUILD_MPG123=OFF -DBUILD_FLAC=OFF
        -DGME_YM2612_EMU=GENS
    BUILD_COMMAND ${build_command} INSTALL_COMMAND ${install_command}
    BUILD_ALWAYS TRUE)
ExternalProject_Get_Property(ofe_audio_codecs SOURCE_DIR)
set(mixer_args ${common_args})
if(MINGW)
    list(APPEND mixer_args "-DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS} -Dalloca=__builtin_alloca")
endif()
ExternalProject_Add(ofe_mixerx
    SOURCE_DIR "${mixer_source}" DOWNLOAD_COMMAND "" UPDATE_COMMAND ""
    DEPENDS ofe_audio_codecs
    CMAKE_ARGS ${mixer_args}
        "-DCMAKE_PREFIX_PATH:PATH=${stage}"
        "-DSDL2_DIR:PATH=${stage}/lib/cmake/SDL2"
        "-DAUDIO_CODECS_REPO_PATH:PATH=${SOURCE_DIR}"
        "-DAUDIO_CODECS_INSTALL_PATH:PATH=${stage}"
        -DUSE_SYSTEM_SDL2=ON -DDOWNLOAD_AUDIO_CODECS_DEPENDENCY=OFF
        -DSDL_MIXER_X_SHARED=ON -DSDL_MIXER_X_STATIC=OFF
        -DMIXERX_ENABLE_LGPL=ON -DMIXERX_ENABLE_GPL=OFF -DBUILD_EXAMPLES=OFF
    BUILD_COMMAND ${build_command} INSTALL_COMMAND ${install_command}
    BUILD_ALWAYS TRUE)
add_custom_target(sdl_sdk ALL
    COMMAND "${CMAKE_COMMAND}"
        "-DSTAGE:PATH=${stage}" "-DSDK:PATH=${OFE_SDL_SDK_ROOT}"
        "-DARCH:STRING=${sdk_arch}" "-DMIXER_SOURCE:PATH=${mixer_source}"
        -P "${CMAKE_CURRENT_LIST_DIR}/StageSDK.cmake"
    DEPENDS ofe_mixerx VERBATIM)
message(STATUS "Combined ${sdk_arch} SDK: ${OFE_SDL_SDK_ROOT}")
