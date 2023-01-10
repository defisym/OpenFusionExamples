#pragma once

//#define FMOD_AUDIO

#ifdef FMOD_AUDIO

#include "fmod.hpp"

#include <map>
#include <string>
#include <functional>

#ifdef _DEBUG
// L for logging
#pragma comment(lib,"fmodL_vc.lib")
#else
#pragma comment(lib,"fmod_vc.lib")
#endif

class FModInterface {
private:
    struct SoundMapItem    {   
       FMOD::Sound* pSound;
        FMOD::Channel* channel = 0;       
    };    

    std::map<std::wstring, SoundMapItem> soundMap;

    FMOD::System* system = nullptr;
    FMOD_RESULT result = FMOD_OK;
    FMOD_MODE mode = FMOD_OPENUSER | FMOD_LOOP_NORMAL | FMOD_CREATESTREAM;
    FMOD_CREATESOUNDEXINFO exinfo = { 0 };
    void* extradriverdata = 0;

    using IT = decltype(soundMap.begin());

    inline void FMI_SetSound(std::wstring&& soundName,std::function<void(IT)> updater) {
        auto it = std::find_if(this->soundMap.begin(), this->soundMap.end(), [&](auto& pair) {
            return pair.first == soundName;
            });

        if (it == this->soundMap.end()) {
            return;
        }

        updater(it);
    }

public:
    FModInterface() {
        result = FMOD::System_Create(&system);
        if (result != FMOD_OK) {
            return;
        }

        result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
        if (result != FMOD_OK) {
            return;
        }
    }
    ~FModInterface() {
        for (auto& soundItem : soundMap) {
            soundItem.second.pSound->release();
        }

        result = system->close();
        result = system->release();
    }

    //  Usage
    //  FMI_CreateSound(L"name", nullptr, [] (FMOD_CREATESOUNDEXINFO& exinfo) {
    //         exinfo.numchannels = 2;                               /* Number of channels in the sound. */
    //         exinfo.defaultfrequency = 44100;                      /* Default playback rate of sound. */
    //         exinfo.decodebuffersize = 44100;                      /* Chunk size of stream update in samples. This will be the amount of data passed to the user callback. */
    //         exinfo.length =                                       /* Length of PCM data in bytes of whole song (for Sound::getLength) */
    //         exinfo.defaultfrequency * exinfo.numchannels * sizeof(signed short) * 5; 
    //         exinfo.format = FMOD_SOUND_FORMAT_PCM16;              /* Data format of sound. */
    //         exinfo.pcmreadcallback =                              /* User callback for reading. */
    //         [] (FMOD_SOUND* sound, void* data, unsigned int datalen) {
    //             auto pSound=(FMOD::Sound*)sound;
                
    //             void* userdata=nullptr;
    //             pSound->getUserData((void**)&userdata);

    //             return FMOD_OK;
    //         };                
    //         exinfo.pcmsetposcallback = nullptr;                   /* User callback for seeking. */
    //     });

    inline void FMI_CreateSound(std::wstring&& soundName, void *userdata
        , std::function<void(FMOD_CREATESOUNDEXINFO& exinfo)>callBack) {
        memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
        exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);       /* Required. */

        callBack(exinfo);
        exinfo.userdata = userdata;

        FMOD::Sound* sound;

        result = system->createSound(0, mode, &exinfo, &sound);
        if (result == FMOD_OK) {
            this->soundMap.emplace(soundName, SoundMapItem{sound});
        }
    }

    inline void FMI_PlaySound(std::wstring&& soundName, bool bPaused) {
        FMI_SetSound(std::forward<std::wstring>(soundName), [&](IT it)->void {
            result = system->playSound(it->second.pSound, 0, bPaused, &it->second.channel);
            });
    }

    inline void FMI_SetVolume(std::wstring&& soundName, float volume) {
        FMI_SetSound(std::forward<std::wstring>(soundName), [&](IT it)->void {
            result = it->second.channel->setVolume(volume);
            });
    }

    inline void FMI_Update() {
        result = system->update();
    }
};

#endif