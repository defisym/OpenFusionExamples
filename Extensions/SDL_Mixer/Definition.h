#pragma once

//immediate conditon ID
//constexpr auto ON_FINISH = 4;

// SDL mixer can work with SDL Audio together
#include <SDL.h>
#include <SDL_mixer.h>

#include <map>
#include <vector>
#include <string>
#include <functional>

enum class AudioType {
	BGM,
	BGS,
	SE,
	DUB,
};

constexpr auto AudioDataException_DecryptFailed = -1;
constexpr auto AudioDataException_CreateRWFailed = -2;
constexpr auto AudioDataException_CreateMusicFailed = -3;

struct AudioData {
	Encryption* pDecrypt = nullptr;

	SDL_RWops* pRW = nullptr;
	Mix_Music* pMusic = nullptr;

	std::wstring fileName;

	AudioData(const wchar_t* pFileName) {
		fileName = pFileName;

		pRW = SDL_RWFromFile(ConvertWStrToStr(pFileName, CP_UTF8).c_str(), "rb");

		if (!pRW) {
			throw AudioDataException_CreateRWFailed;
		}

		if (!CreateMusic()) {
			throw AudioDataException_CreateMusicFailed;
		}
	}
	AudioData(const wchar_t* pFileName, const wchar_t* pKey) {
		fileName = pFileName;

		pDecrypt = new Encryption;

		pDecrypt->GenerateKey(pKey);
		auto bRet = pDecrypt->DecryptFileDirectly(pFileName);

		if(!bRet) {
			throw AudioDataException_DecryptFailed;
		}

		pRW = SDL_RWFromConstMem(pDecrypt->GetOutputData(), pDecrypt->GetOutputDataLength());

		if (!pRW) {
			throw AudioDataException_CreateRWFailed;
		}

		if (!CreateMusic()) {
			throw AudioDataException_CreateMusicFailed;
		}
	}
	
	~AudioData() {
		Mix_FreeMusic(pMusic);
		SDL_RWclose(pRW);

		delete pDecrypt;
	}

	inline bool	CreateMusic() {
		pMusic = Mix_LoadMUS_RW(pRW, 0);

		return pMusic != nullptr;
	}
};

constexpr auto GlobalDataException_SDLInitFailed = -1;
constexpr auto GlobalDataException_MixOpenAudioFailed = -2;

struct GlobalData {
	std::vector<AudioData*> pAudioDatas;

	using HookCallback = std::function<void(Mix_Music*, void*)>;

	GlobalData() {
		if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
			auto error = SDL_GetError();

			throw GlobalDataException_SDLInitFailed;
		}

		if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1) {
			auto error = SDL_GetError();

			throw GlobalDataException_MixOpenAudioFailed;
		}
	}
	~GlobalData() {
		ReleaseAudioData();

		Mix_CloseAudio();
		SDL_Quit();
	}

	inline auto GetAudioData(const wchar_t* pFileName) {
		return std::ranges::find_if(pAudioDatas,
			[pFileName] (const AudioData* pAudioData) {
				return StrIEqu(pFileName, pAudioData->fileName.c_str());
		});
	}

	inline void GetAudioData(const wchar_t* pFileName,
		std::function<void(Mix_Music*)> processor) {
		auto it = GetAudioData(pFileName);

		if (it != pAudioDatas.end()) {
			auto pMusic = (*it)->pMusic;

			processor(pMusic);
		}
	}

	inline bool PlayAudio(const wchar_t* pFileName,
		int loops = 0, int fadeMs = -1, 
		const wchar_t* pCrossFade = nullptr) {
		auto it = GetAudioData(pFileName);

		if (it == pAudioDatas.end()) {
			return false;
		}

		auto pMusic = (*it)->pMusic;

	//	Mix_HookMusicStreamFinished(pMusic,
	//		[] (Mix_Music* pMusic, void* pUserData) {
	//			auto pGlobalData = (GlobalData*)pUserData;
	//			pGlobalData->ReleaseAudioData(pMusic);
	//	},
	//this);

		if (fadeMs == -1) {
			Mix_PlayMusicStream(pMusic, loops);
		}
		else {
			do {
				if (pCrossFade == nullptr) {
					break;
				}
				auto crossFadeIt = GetAudioData(pCrossFade);

				if (crossFadeIt == pAudioDatas.end()) {
					break;
				}

				auto pOldMusic = (*crossFadeIt)->pMusic;

				Mix_CrossFadeMusicStream(pOldMusic, pMusic,
					loops, fadeMs, true);

				return true;
			} while (0);

			Mix_FadeInMusicStream(pMusic, loops, fadeMs);
		}

		return true;
	}

	inline bool CreateAudio(const wchar_t* pFileName) {
		try {
			pAudioDatas.emplace_back(new AudioData(pFileName));			

			return true;
		}catch(...) {
			return false;
		}
	}

	inline bool CreateAudio(const wchar_t* pFileName, const wchar_t* pKey) {
		try {
			pAudioDatas.emplace_back(new AudioData(pFileName, pKey));

			return true;
		} catch (...) {
			return false;
		}
	}

	// in seconds
	inline void SetAudioPosition(const wchar_t* pFileName, double position) {
		GetAudioData(pFileName, [position] (Mix_Music* pMusic) {
			Mix_SetMusicPositionStream(pMusic, position);
		});
	}

	// in seconds
	inline double GetAudioPosition(const wchar_t* pFileName) {
		double pos = -1;

		GetAudioData(pFileName, [&] (Mix_Music* pMusic) {
			pos = Mix_GetMusicPosition(pMusic);
		});

		return pos;
	}

	// 0 ~ 100 -> 0 ~ 128
	inline int VolumeConverter(int volume) {
		return (int)((volume / 100.0) * 128.0);
	}

	inline int VolumeReverseConverter(int volume) {
		return (int)((volume / 128.0) * 100.0);
	}

	// 0 ~ 100
	inline void SetAudioVolume(const wchar_t* pFileName, int volume) {
		GetAudioData(pFileName, [&] (Mix_Music* pMusic) {
			Mix_VolumeMusicStream(pMusic, VolumeConverter(volume));
		});
	}

	// 0 ~ 100
	inline int GetAudioVolume(const wchar_t* pFileName) {
		int volume = -1;

		GetAudioData(pFileName, [&] (Mix_Music* pMusic) {
			volume = Mix_GetVolumeMusicStream(pMusic);
		});

		return VolumeReverseConverter(volume);
	}

	inline void ReleaseAudioData() {
		for(auto& pData:pAudioDatas) {
			delete pData;
		}

		pAudioDatas.clear();
	}

	inline void ReleaseAudioData(Mix_Music* pMusic) {
		auto it = std::ranges::find_if(pAudioDatas, [pMusic] (AudioData* pAudioData) {
			return pAudioData->pMusic == pMusic;
		});

		if (it != pAudioDatas.end()) {
			pAudioDatas.erase(it);
		}
	}

	inline void ReleaseAudioData(const wchar_t* pFileName) {
		auto it = GetAudioData(pFileName);

		if(it!= pAudioDatas.end()) {
			pAudioDatas.erase(it);
		}
	}
};