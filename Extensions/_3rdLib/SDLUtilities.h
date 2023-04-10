#pragma once

#include <SDL.h>
#include <SDL_mixer.h>

constexpr auto SDLGeneralException_SDLInitFailed = -1;
constexpr auto SDLGeneralException_MixOpenAudioFailed = -2;

// Init SDL with check, to make extensions compatible
// SDL mixer will close previous channels if already initialized, which may crash
// All extensions shares the same audio params
inline void SDL_GeneralInit() {
    if (!SDL_WasInit(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
            auto error = SDL_GetError();

            throw SDLGeneralException_SDLInitFailed;
        }
    }

    if (!Mix_AudioOpened()) {
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1) {
            auto error = SDL_GetError();

            throw SDLGeneralException_MixOpenAudioFailed;
        }
    }
}