#pragma once

#include "AudioInterface.h"

// Documents for Mixer X
// https://github.com/WohlSoft/SDL-Mixer-X/tree/master/docs
#include <SDL.h>
#include <SDL_mixer.h>

#include <vector>
#include <functional>

constexpr auto AudioDataException_CreateChunkFailed = -1;

class SMI:public AudioInterface {
private:
    struct AudioData {
        BYTE* pData = nullptr;
        int len = 4 * SDLGeneral_BufferSize;

        Mix_Chunk* pChunk = nullptr;
        FFMpeg** ppFFMpeg = nullptr;
        int channel = -1;

        bool bExit = false;

        AudioData(FFMpeg** ppFFMpeg) {
            pData = new BYTE[len];
            memset(pData, 0, len);

        	pChunk = Mix_QuickLoad_RAW(pData, len);

            if (!pChunk) {
                auto error = SDL_GetError();

                throw AudioDataException_CreateChunkFailed;
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
            Mix_AllocateChannels(MIX_CHANNELS);
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
        SDL_GeneralInit();
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
        pAudioData->channel = channelID;

        Mix_PlayChannel(channelID, pAudioData->pChunk, -1);
        Mix_RegisterEffect(channelID,
           [] (int chan, void* stream, int len, void* udata) {
               const auto setter = SDL_memset;
               const auto mixer = [] (void* dst, const void* src, size_t len, int volume) {
                   //SDL_MixAudio((Uint8*)dst, (const Uint8*)src, len, volume);
                   SDL_memcpy((Uint8*)dst, (const Uint8*)src, len);
               };

               const auto pAudioData = static_cast<AudioData*>(udata);
               const auto pFFMpeg = *pAudioData->ppFFMpeg;

               if (pFFMpeg == nullptr) {
                   memset(stream, 0, len);

                   return;
               }

               pFFMpeg->audio_fillData(static_cast<Uint8*>(stream), len, setter, mixer);
               
               const auto volume = pFFMpeg->get_sdl_volume();
               Mix_Volume(pAudioData->channel, volume);
           },
           [] (int chan, void* udata) {
               //OutputDebugString(L"Finish");
           },
           pAudioData);


        pAudioDatas.emplace_back(pAudioData);
    }
    inline void RemoveInstance(FFMpeg** ppFFMpeg, void* pData) override {
        auto removeIt = std::ranges::remove_if(pAudioDatas, [&](AudioData* pAudioData) {
            if(pAudioData->ppFFMpeg == ppFFMpeg) {
                audioChannel.RemoveInstance(pAudioData);
                delete pAudioData;

                return true;
            }

            return false;
        });
        
        pAudioDatas.erase(removeIt.begin(), removeIt.end());
    }
};