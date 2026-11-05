cmake_minimum_required(VERSION 3.22)
project(findtheway LANGUAGES CXX)
add_library(findtheway SHARED FindTheWayJNI.cpp)
target_compile_features(findtheway PRIVATE cxx_std_20)
target_compile_options(findtheway PRIVATE -Wall -Wextra -Wpedantic)
# All clients share the helper interface and the same _DeLib/_3rdLib search paths.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/shared/_DeLib/FindTheWay.h")
    set(FTW_DEFAULT_CORE_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/shared/_DeLib")
else()
    set(FTW_DEFAULT_CORE_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../_DeLib")
endif()
set(FTW_CORE_INCLUDE "${FTW_DEFAULT_CORE_INCLUDE}" CACHE PATH "Shared FindTheWay headers")
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

if(ANDROID AND OFE_SDL_MIXER_SOURCES)
    set(OFE_SDL_SOURCE "${OFE_SDL_MIXER_SOURCES}/SDL2-5d249570393f7a37e037abf22cd6012a4cc56a71")
    set(OFE_MIXERX_SOURCE "${OFE_SDL_MIXER_SOURCES}/MixerX-72139aa7332062de541de50708742fe4ee2ae05c")
    set(OFE_SOUNDTOUCH_SOURCE "${OFE_SDL_MIXER_SOURCES}/SoundTouch-e83424d5928ab8513d2d082779c275765dee31b9")
    set(OFE_AUDIOCODECS_SOURCE "${OFE_SDL_MIXER_SOURCES}/AudioCodecs-bb11ca3cc1d7597b23a530d519b3e636dceb7b23")
    get_filename_component(ofe_audio_cache "${OFE_SDL_MIXER_SOURCES}/../build" ABSOLUTE)
    set(OFE_SDL_MIXER_BUILD_ROOT "${ofe_audio_cache}" CACHE PATH "Prepared shared audio builds")
    set(OFE_AUDIOCODECS_ROOT "${OFE_SDL_MIXER_BUILD_ROOT}/${ANDROID_ABI}/codecs-sdk")
    if(NOT EXISTS "${OFE_AUDIOCODECS_ROOT}/lib/libopusfile.a")
        message(FATAL_ERROR "Prepare the audio SDK using SDL_Mixer/Android/build_android.py and set OFE_SDL_MIXER_BUILD_ROOT to its build root")
    endif()
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../../../../SDL_Mixer/Android/cpp"
                     "${CMAKE_CURRENT_BINARY_DIR}/sdl-mixer")
endif()

# Optional development build. Export packages carry prebuilt FFmpeg JNI libraries.
if(ANDROID AND OFE_FFMPEG_ROOT)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../../../../FFMpeg/Android/cpp"
                     "${CMAKE_CURRENT_BINARY_DIR}/ffmpeg")
endif()
