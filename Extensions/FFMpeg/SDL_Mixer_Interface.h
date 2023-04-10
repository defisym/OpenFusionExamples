#pragma once

#include "AudioInterface.h"

// Documents for Mixer X
// https://github.com/WohlSoft/SDL-Mixer-X/tree/master/docs
#include <SDL.h>
#include <SDL_mixer.h>

#include <vector>
#include <functional>

constexpr auto SMIException_SDLInitFailed = -1;
constexpr auto SMIException_MixOpenAudioFailed = -2;

constexpr auto AudioDataException_CreateRWFailed = -1;
constexpr auto AudioDataException_CreateMusicFailed = -2;

class SMI:public AudioInterface {
private:
    struct AudioData {
        BYTE* pData = nullptr;
        int len = 8192;

        SDL_RWops* pRW = nullptr;
        Mix_Music* pMusic = nullptr;
        FFMpeg** ppFFMpeg = nullptr;

        AudioData(FFMpeg** ppFFMpeg) {
            pData = new BYTE[len];
            memset(pData, 0, len);

            pRW = SDL_RWFromConstMem(pData, len);

            if (!pRW) {
                throw AudioDataException_CreateRWFailed;
            }

            pMusic = Mix_LoadMUS_RW(pRW, 0);

            if (!pMusic) {
                throw AudioDataException_CreateMusicFailed;
            }

            this->ppFFMpeg = ppFFMpeg;
        }
        ~AudioData() {
            Mix_FreeMusic(pMusic);
            SDL_RWclose(pRW);

            delete[] pData;
        }
    };

    std::vector<AudioData*> pAudioDatas;

    inline void ReleaseAudioData() {
	    for(auto& pAudioData: pAudioDatas) {
            delete pAudioData;
	    }

        pAudioDatas.clear();
    }

public:
    SMI() {
        if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
            auto error = SDL_GetError();

            throw SMIException_SDLInitFailed;
        }

        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1) {
            auto error = SDL_GetError();

            throw SMIException_MixOpenAudioFailed;
        }
    }
    ~SMI() {
        ReleaseAudioData();

        Mix_CloseAudio();
        SDL_Quit();
    }

    inline void AddInstance(FFMpeg** ppFFMpeg, void* pData) {
        auto pAudioData = new AudioData(ppFFMpeg);
        auto pMusic = pAudioData->pMusic;

        Mix_HookMusicStreamFinished(pMusic,
    [] (Mix_Music* pMusic, void* pUserData) {
        const auto pAudioData = (AudioData*)pUserData;
        const auto pFFMpeg = *pAudioData->ppFFMpeg;
        const auto pData = pAudioData->pData;
        const auto len = pAudioData->len;

        if (pFFMpeg == nullptr) {
            memset(pData, 0, len);

            return;
        }

        pFFMpeg->audio_fillData(pData, len,
            SDL_memset,
            [] (void* dst, const void* src, size_t len, int volume) {
                SDL_MixAudio((Uint8*)dst, (const Uint8*)src, len, volume);
        });

        Mix_RewindMusicStream(pMusic);
    },
    pAudioData);

        pAudioDatas.emplace_back(pAudioData);
    }
    inline void RemoveInstance(FFMpeg** ppFFMpeg, void* pData) {
        auto removeIt = std::ranges::remove_if(pAudioDatas, [&](const AudioData* pAudioData) {
            return pAudioData->ppFFMpeg == ppFFMpeg;
        });

        for(const auto& it: removeIt) {
            delete it;
        }

        pAudioDatas.erase(removeIt.begin(), removeIt.end());
    }
};