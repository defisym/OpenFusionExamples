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

        Mix_Chunk* pChunk = nullptr;
        FFMpeg** ppFFMpeg = nullptr;

        AudioData(FFMpeg** ppFFMpeg) {
            pData = new BYTE[len];
            memset(pData, 0, len);

        	pChunk = Mix_QuickLoad_RAW(pData, len);

            if (!pChunk) {
                throw -1;
            }

            this->ppFFMpeg = ppFFMpeg;
        }
        ~AudioData() {
            Mix_FreeChunk(pChunk);

            delete[] pData;
        }
    };
    struct AudioChannel {
        std::vector<AudioData*> channels;

        AudioChannel() {
            channels.insert(channels.end(), MIX_CHANNELS, nullptr);
        }

        //inline void IterateChannel(const std::function<void(const int index, const AudioData* pAudioData)>& callBack) {
        //    for (size_t i = 0; i < channels.size(); i++) {
        //        callBack(static_cast<int>(i), channels[i]);               
        //    }
        //}

        inline int GetChannelID(const AudioData* pAudioData) const {
            for (size_t i = 0; i < channels.size(); i++) {
	            if (channels[i]== pAudioData) {
                    return static_cast<int>(i);
	            }
            }

            return -1;
        }

        inline void AddInstance(AudioData* pAudioData) {
            const auto it = std::ranges::find(channels, nullptr);

            if(it != channels.end()) {
                *it = pAudioData;
            }else {
                channels.emplace_back(pAudioData);
                Mix_AllocateChannels(static_cast<int>(channels.size()));
            }
        }

        inline void RemoveInstance(AudioData* pAudioData) {
            const auto it = std::ranges::find(channels, pAudioData);

            if (it != channels.end()) {
                *it = nullptr;
            }
        }
    };

    std::vector<AudioData*> pAudioDatas;
    AudioChannel audioChannel;

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

        //const auto dn = SDL_GetNumAudioDrivers();
        //std::vector<const char*> a;

        //for (int i = 0; i < dn; i++) {
        //    a.emplace_back(SDL_GetAudioDriver(i));
        //}

        //SDL_AudioInit("directsound");

        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1) {
            auto error = SDL_GetError();

            throw SMIException_MixOpenAudioFailed;
        }

        Mix_Pause(-1);
    }
    ~SMI() override {
        ReleaseAudioData();

        Mix_CloseAudio();
        SDL_Quit();
    }

    inline void AddInstance(FFMpeg** ppFFMpeg, void* pData) override {
        auto pAudioData = new AudioData(ppFFMpeg);

    	audioChannel.AddInstance(pAudioData);
        const auto channelID = audioChannel.GetChannelID(pAudioData);

        Mix_PlayChannel(channelID, pAudioData->pChunk, -1);
        Mix_RegisterEffect(channelID,
           [] (int chan, void* stream, int len, void* udata) {
               const auto pAudioData = static_cast<AudioData*>(udata);
               const auto pFFMpeg = *pAudioData->ppFFMpeg;

               if (pFFMpeg == nullptr) {
                   memset(stream, 0, len);

                   return;
               }

               pFFMpeg->audio_fillData(static_cast<Uint8*>(stream), len,
                   SDL_memset,
                   [] (void* dst, const void* src, size_t len, int volume) {
                                 SDL_MixAudio(static_cast<Uint8*>(dst), 
                                 static_cast<const Uint8*>(src), 
                                 len, volume);
               });
           },
           [] (int chan, void* udata) {

           },
           pAudioData);

        pAudioDatas.emplace_back(pAudioData);
    }
    inline void RemoveInstance(FFMpeg** ppFFMpeg, void* pData) override {
        auto removeIt = std::ranges::remove_if(pAudioDatas, [&](const AudioData* pAudioData) {
            return pAudioData->ppFFMpeg == ppFFMpeg;
        });

        for(const auto& it: removeIt) {
            audioChannel.RemoveInstance(it);
            delete it;
        }

        pAudioDatas.erase(removeIt.begin(), removeIt.end());
    }
};