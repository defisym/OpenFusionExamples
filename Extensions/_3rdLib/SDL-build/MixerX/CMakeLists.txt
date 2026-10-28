cmake_minimum_required(VERSION 3.21)
project(OFEMixerX LANGUAGES C CXX)

add_subdirectory("${OFE_MIXER_SOURCE}" mixerx)
# Git for Windows may check out AudioCodecs' header symlink as path text.
# Use the real installed header explicitly: MSVC can search the source include
# directory before the installed SDK's external/system include directory.
if(TARGET SDL2_mixer_ext)
    target_compile_definitions(SDL2_mixer_ext PRIVATE
        "MODPLUG_HEADER=\"${AUDIO_CODECS_INSTALL_PATH}/include/libmodplug/modplug.h\"")
endif()
