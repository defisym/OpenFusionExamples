#pragma once

//#define FMOD_AUDIO

#ifdef FMOD_AUDIO

#include "fmod.hpp"
#include "WindowsCommon.h"

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
    struct SoundMapItem {
        FMOD::Sound* pSound;
        FMOD::Channel* channel = 0;
    };

    std::map<std::wstring, SoundMapItem> soundMap;

    FMOD::System* system = nullptr;
    FMOD_RESULT result = FMOD_OK;
    // https://qa.fmod.com/t/pcmreadcallback-not-being-called/13862
    // When creating the sound, using FMOD_CREATESTREAM will stream the data in chunks as it plays which will need to continuously read the data to play the sand, also using far less memory.
    // Without that flag, a sound will open as a static sound that is decompressed fully into memory after one read callback.
    FMOD_MODE mode = FMOD_OPENUSER | FMOD_LOOP_NORMAL | FMOD_CREATESTREAM;
    FMOD_CREATESOUNDEXINFO exinfo = { 0 };
    void* extradriverdata = 0;

    using SoundMapIt = decltype(soundMap.begin());

    inline void FMI_LoadSound(std::wstring&& soundName
        , const char* name_or_data
        , FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* exinfo = 0, int loopcount = -1) {
        FMOD::Sound* sound;

        result = system->createSound(name_or_data, mode, exinfo, &sound);
        sound->setLoopCount(loopcount);

        this->soundMap.emplace(soundName, SoundMapItem{ sound });

        FMI_SetSound(std::forward<std::wstring>(soundName), [&](SoundMapIt it)->void {
            result = system->playSound(it->second.pSound, 0, false, &it->second.channel);
            });
    }

    inline void FMI_SetSound(std::wstring&& soundName,std::function<void(SoundMapIt)> updater) {
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
        FMI_SetSound(std::forward<std::wstring>(soundName), [&](SoundMapIt it)->void {
            result = system->playSound(it->second.pSound, 0, bPaused, &it->second.channel);
            });
    }

    inline void FMI_StopSound(std::wstring&& soundName) {
        FMI_SetSound(std::forward<std::wstring>(soundName), [&](SoundMapIt it)->void {
            result = it->second.pSound->release();
            this->soundMap.erase(it);
            });
    }

    inline void FMI_SetPos(std::wstring&& soundName, size_t pos = 0) {
        FMI_SetSound(std::forward<std::wstring>(soundName), [&](SoundMapIt it)->void {
#ifdef  _DEBUG
            size_t position = 0;
            result = it->second.channel->getPosition(&position, FMOD_TIMEUNIT_MS);
#endif //  _DEBUG            
            result = it->second.channel->setPosition(pos, FMOD_TIMEUNIT_MS);
            });
    }

    inline void FMI_ReplaySound(std::wstring&& soundName) {
        FMI_SetSound(std::forward<std::wstring>(soundName), [&](SoundMapIt it)->void {
            result = it->second.channel->stop();
            result = system->playSound(it->second.pSound, 0, false, &it->second.channel);

            return;
            });
    }

    inline void FMI_SetVolume(std::wstring&& soundName, float volume) {
        FMI_SetSound(std::forward<std::wstring>(soundName), [&](SoundMapIt it)->void {
            result = it->second.channel->setVolume(volume);
            });
    }
        
    inline void FMI_PlaySoundFile(std::wstring&& soundName, std::wstring&& fileName, int loopcount = -1) {
        FMI_LoadSound(std::forward<std::wstring>(soundName)
            ,ConvertWStrToStr(fileName, CP_UTF8).c_str()
            , FMOD_DEFAULT | FMOD_CREATESTREAM, 0, loopcount);
    }

    inline void FMI_PlaySoundMemFile(std::wstring&& soundName, const char* pData, size_t sz, int loopcount = -1) {
        memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
        exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);       /* Required. */

        exinfo.length = sz;

        FMI_LoadSound(std::forward<std::wstring>(soundName)
            , pData
            , FMOD_DEFAULT | FMOD_CREATESAMPLE | FMOD_OPENMEMORY, &exinfo, loopcount);
    }

    inline void FMI_Update() {
        result = system->update();
    }
};

#endif