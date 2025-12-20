#pragma once

#include <SDL.h>
#include <SDL_mixer.h>

constexpr auto SDLGeneral_BufferSize = 4096;

constexpr auto SDLGeneral_OK = 0;
constexpr auto SDLGeneralException_SDLInitFailed = -1;
constexpr auto SDLGeneralException_MixOpenAudioFailed = -2;

namespace InitDetails {
    inline static size_t InitCount = 0u;
    inline static bool bInitSDL = false;
    inline static bool bInitMix = false;
};


// Init SDL with check, to make extensions compatible
// SDL mixer will close previous channels if already initialized, which may crash
// All extensions shares the same audio params
inline int SDL_GeneralInit() {
    if (!SDL_WasInit(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
            auto error = SDL_GetError();

            return SDLGeneralException_SDLInitFailed;
        }

        InitDetails::bInitSDL = true;
    }

    if (!Mix_AudioOpened()) {
		// If your app needs precisely what is requested, specify zero for `allowed_changes`
        if (Mix_OpenAudioDevice(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, SDLGeneral_BufferSize,
            nullptr, 0) == -1) {
            auto error = SDL_GetError();

            return SDLGeneralException_MixOpenAudioFailed;
        }

        InitDetails::bInitMix = true;
    }

    ++InitDetails::InitCount;

    return SDLGeneral_OK;
}

inline void SDL_GeneralQuit() {
    if (InitDetails::InitCount == 0u) { return; }

    --InitDetails::InitCount;
    if (InitDetails::InitCount == 0u) {
        if (InitDetails::bInitMix) {
            InitDetails::bInitMix = false;
            Mix_CloseAudio();
        }

        if (InitDetails::bInitSDL) {
            InitDetails::bInitSDL = false;
            SDL_Quit();
        }
    }
}