#pragma once

//immediate conditon ID
//constexpr auto ON_FINISH = 4;

// SDL mixer can work with SDL Audio together
// AKA compatible with FFMpeg
#include <SDL.h>
#include <SDL_mixer.h>

#include <map>
#include <vector>
#include <string>
#include <algorithm>
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
	bool bHook = false;

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

		if (!bRet) {
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

constexpr auto Mix_MinVolume = 0;
constexpr auto Mix_MaxVolume = 128;

constexpr auto Fusion_MinVolume = 0;
constexpr auto Fusion_MaxVolume = 100;

struct GlobalData {
	using AudioDataVec = std::vector<AudioData*>;
	AudioDataVec pAudioDatas;

	using AudioDataIt = decltype(pAudioDatas.begin());
	using HookCallback = std::function<void(Mix_Music*, void*)>;

	SDL_SpinLock gDataLock = 0;
	std::vector<Mix_Music*> toRelease;

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

	// ------------
	// lib -> refresh
	// ------------

	// should be called periodically
	// e.g., call it each frame in HandleRunObject
	inline void AudioUpdate() {
		ReleaseStoppedAudioData();
		UpdateExclusiveABLoop();
	}

	// ------------
	// lib -> get
	// ------------

	inline Mix_Music* GetAudioPointer(const wchar_t* pFileName) {
		if (pFileName == nullptr) {
			return nullptr;
		}

		auto it = GetAudioData(pFileName);

		return it != pAudioDatas.end()
			? (*it)->pMusic
			: nullptr;
	}

	inline AudioDataIt GetAudioData(const wchar_t* pFileName) {
		return std::ranges::find_if(pAudioDatas,
			[pFileName] (const AudioData* pAudioData) {
				return pAudioData != nullptr
					? StrIEqu(pFileName, pAudioData->fileName.c_str())
					: false;
		});
	}

	inline void GetAudioData(const wchar_t* pFileName,
		std::function<void(Mix_Music*)> processor) {
		auto pMusic = GetAudioPointer(pFileName);

		if (pMusic != nullptr) {
			processor(pMusic);
		}
	}

	inline AudioDataIt GetAudioData(const Mix_Music* pMusic) {
		return std::ranges::find_if(pAudioDatas,
			[pMusic] (const AudioData* pAudioData) {
				return pAudioData != nullptr
					? pAudioData->pMusic == pMusic
					: false;
		});
	}

	inline const std::wstring* const GetAudioName(const Mix_Music* pMusic) {
		auto ppAudioData = GetAudioData(pMusic);

		if (ppAudioData == pAudioDatas.end()) {
			return nullptr;
		}

		auto fileName = &((*ppAudioData)->fileName);

		return fileName;
	}

	// ------------
	// lib -> release
	// ------------

	inline void ReleaseAudioData() {
		for (auto& pAudioData : pAudioDatas) {
			// Mix_CloseAudio will call close hook
			// the data is released in hook, for mixing & exclusive channels
			// so don't handle it here
			if (!pAudioData->bHook) {
				delete pAudioData;

				pAudioData = nullptr;
			}
		}

		auto removeRet = std::ranges::remove(pAudioDatas, nullptr);
	}

	inline void ReleaseAudioData(Mix_Music* pMusic) {
		if (pMusic == nullptr) {
			return;
		}

		EraseExclusive(pMusic);
		EraseMixing(pMusic);

		const auto it = GetAudioData(pMusic);

		if (it != pAudioDatas.end()) {
			const auto pAudioData = (*it);
			delete pAudioData;

			(*it) = nullptr;

			pAudioDatas.erase(it);
		}
	}

	inline void ReleaseAudioData(const wchar_t* pFileName) {
		ReleaseAudioData(GetAudioPointer(pFileName));
	}

	// release stopped audio data.
	// should be called periodically
	inline void ReleaseStoppedAudioData() {
		SDL_AtomicLock(&gDataLock);
		std::vector<Mix_Music*> local = toRelease;

		toRelease.clear();
		SDL_AtomicUnlock(&gDataLock);

		std::ranges::sort(local);
		local.erase(std::ranges::unique(local).begin(), local.end());

		for (auto& pMusic : local) {
#ifdef _DEBUG
			auto fileName = GetAudioName(pMusic);
#endif

			if (!Mix_PlayingMusicStream(pMusic)) {
				ReleaseAudioData(pMusic);
			}
		}
	}

	// ------------
	// lib -> create
	// ------------

	inline AudioData* CreateAudio(const wchar_t* pFileName) {
		try {
			auto pAudioData = new AudioData(pFileName);
			pAudioDatas.emplace_back(pAudioData);

			return pAudioData;
		} catch (...) {
			return nullptr;
		}
	}

	inline AudioData* CreateAudio(const wchar_t* pFileName, const wchar_t* pKey) {
		try {
			auto pAudioData = new AudioData(pFileName, pKey);
			pAudioDatas.emplace_back(pAudioData);

			return pAudioData;
		} catch (...) {
			return nullptr;
		}
	}

	inline AudioData* CreateAudioAuto(const wchar_t* pFileName, const wchar_t* pKey) {
		auto bDecrypt = !(pKey == nullptr || StrEmpty(pKey));

		auto pAudioData = bDecrypt
			? CreateAudio(pFileName, pKey)
			: CreateAudio(pFileName);

		return pAudioData;
	}

	// ------------
	// play
	// ------------

	inline bool PlayAudio(Mix_Music* pMusic,
		int loops = 0, int fadeMs = -1,
		Mix_Music* pOldMusic = nullptr) {
		if (pMusic == nullptr) {
			return false;
		}

		//Mix_SetFreeOnStop(pMusic, true);

		if (fadeMs == -1) {
			Mix_PlayMusicStream(pMusic, loops);
		}
		else {
			if (pOldMusic != nullptr) {
				Mix_CrossFadeMusicStream(pOldMusic, pMusic,
					loops, fadeMs, false);
			}
			else {
				Mix_FadeInMusicStream(pMusic, loops, fadeMs);
			}
		}

		return true;
	}
	inline bool PlayAudio(const wchar_t* pFileName,
		int loops = 0, int fadeMs = -1,
		const wchar_t* pCrossFade = nullptr) {
		auto pMusic = GetAudioPointer(pFileName);
		auto pOldMusic = GetAudioPointer(pCrossFade);

		return PlayAudio(pMusic, loops, fadeMs, pOldMusic);
	}

	inline void StopAudio(Mix_Music* pMusic, int fadeMs = -1) {
		if (fadeMs == -1) {
			Mix_HaltMusicStream(pMusic);
		}
		else {
			Mix_FadeOutMusicStream(pMusic, fadeMs);
		}
	}

	inline void PauseAudio(Mix_Music* pMusic) {
		Mix_PauseMusicStream(pMusic);
	}

	inline void ResumeAudio(Mix_Music* pMusic) {
		Mix_ResumeMusicStream(pMusic);
	}

	// ------------
	// auto close (hook)
	// ------------

	inline void SetAutoClose(Mix_Music* pMusic) {
		const auto pAudioData = (*GetAudioData(pMusic));
		pAudioData->bHook = true;

		Mix_HookMusicStreamFinished(pMusic,
			[] (Mix_Music* pMusic, void* pUserData) {
				const auto pGlobalData = (GlobalData*)pUserData;

				SDL_AtomicLock(&(pGlobalData->gDataLock));
				pGlobalData->toRelease.emplace_back(pMusic);
				SDL_AtomicUnlock(&(pGlobalData->gDataLock));
			},
			this);
	}
	inline void SetAutoClose(const wchar_t* pFileName) {
		GetAudioData(pFileName, [&] (Mix_Music* pMusic) {
			SetAutoClose(pMusic);
		});
	}

	// ------------
	// States
	// ------------

	inline bool AudioPlaying(Mix_Music* pMusic) {
		return Mix_PlayingMusicStream(pMusic);
	}
	inline bool AudioPlaying(AudioData* pAudioData) {
		return pAudioData != nullptr
			? AudioPlaying(pAudioData->pMusic)
			: false;
	}

	inline Mix_Fading AudioFadeState(Mix_Music* pMusic) {
		return Mix_FadingMusicStream(pMusic);		
	}

	// ------------
	// Prop
	// ------------

	// in seconds
	inline void SetAudioPosition(Mix_Music* pMusic, double position) {
		Mix_SetMusicPositionStream(pMusic, position);
	}
	// in seconds
	inline void SetAudioPosition(const wchar_t* pFileName, double position) {
		GetAudioData(pFileName, [&] (Mix_Music* pMusic) {
			SetAudioPosition(pMusic, position);
		});
	}

	// in seconds
	inline double GetAudioPosition(Mix_Music* pMusic) {
		return Mix_GetMusicPosition(pMusic);
	}
	// in seconds
	inline double GetAudioPosition(const wchar_t* pFileName) {
		double pos = -1;

		GetAudioData(pFileName, [&] (Mix_Music* pMusic) {
			pos = GetAudioPosition(pMusic);
		});

		return pos;
	}

	// in seconds
	inline double GetAudioDuration(Mix_Music* pMusic) {
		return Mix_MusicDuration(pMusic);
	}

	// 0 ~ 100 -> 0 ~ 128
	inline int Range(int v, int l, int h) {
		return max(min(v, h), l);
	}

	inline int VolumeConverter(int volume) {
		return (int)((Range(volume, Fusion_MinVolume, Fusion_MaxVolume) /
			static_cast<double>(Fusion_MaxVolume)) * static_cast<double>(Mix_MaxVolume));
	}
	inline int VolumeReverseConverter(int volume) {
		return (int)((Range(volume, Mix_MinVolume, Mix_MaxVolume) /
			static_cast<double>(Mix_MaxVolume)) * static_cast<double>(Fusion_MaxVolume));
	}

	// 0 ~ 100
	inline void SetAudioVolume(Mix_Music* pMusic, int volume) {
		Mix_VolumeMusicStream(pMusic, VolumeConverter(volume));
	}
	// 0 ~ 100
	inline void SetAudioVolume(const wchar_t* pFileName, int volume) {
		GetAudioData(pFileName, [&] (Mix_Music* pMusic) {
			SetAudioVolume(pMusic, volume);
		});
	}

	// 0 ~ 100
	inline int GetAudioVolume(Mix_Music* pMusic) {
		return VolumeReverseConverter(Mix_GetVolumeMusicStream(pMusic));
	}
	// 0 ~ 100
	inline int GetAudioVolume(const wchar_t* pFileName) {
		int volume = -1;

		GetAudioData(pFileName, [&] (Mix_Music* pMusic) {
			volume = GetAudioVolume(pMusic);
		});

		return volume;
	}

	// ------------
	// virtual channel
	//
	// audio in those channels will be auto removed after playing
	// ------------

	using ChannelVolume = std::vector<int>;

	struct AudioSettings {
		// ------------
		// general
		// ------------
		bool bLoop = false;

		// ------------
		// exclusive only
		// ------------

		bool excl_bABLoop = false;
		double excl_start = 0;
		double excl_end = 0;

		// ------------
		// mixing only
		// ------------

		//bool mix_bAttenuation = false;
	};

	using ChannelSettings = std::vector<AudioSettings>;

	template<typename T>
	inline void ExtendVec(std::vector<T>& vec, size_t newSz, T val) {
		if (vec.size() <= newSz) {
			vec.resize(newSz + 1, val);
		}
	}

	inline void EraseVec(AudioDataVec& vec, Mix_Music* pMusic) {
		for (size_t i = 0; i < vec.size(); i++) {
			if (vec[i] != nullptr && vec[i]->pMusic == pMusic) {
				vec[i] = nullptr;
			}
		}
	}

	// 0 ~ 100
	inline int GetChannelVolume(ChannelVolume& vec, int channel) {
		ExtendVec(vec, channel, Fusion_MaxVolume);
		return Range(vec[channel], Fusion_MinVolume, Fusion_MaxVolume);
	}

	// 0 ~ 100
	inline void SetChannelVolume(ChannelVolume& vec, int channel, int volume) {
		ExtendVec(vec, channel, Fusion_MaxVolume);
		vec[channel] = Range(volume, Fusion_MinVolume, Fusion_MaxVolume);
	}

	// ------------
	// exclusive
	//
	// only one audio can play in the same channel, switch will cross fade previous one
	// ------------

	AudioDataVec exclusiveChannel;
	ChannelVolume exclusiveChannelVolume;
	ChannelSettings exclusiveChannelSettings;

	inline void EraseExclusive(Mix_Music* pMusic) {
		EraseVec(exclusiveChannel, pMusic);
	}

	inline bool PlayExclusive(const wchar_t* pFileName, const wchar_t* pKey,
		int channel, int loops = 0, int fadeInMs = -1) {
		auto pAudioData = CreateAudioAuto(pFileName, pKey);

		if (pAudioData == nullptr) {
			return false;
		}

		SetAutoClose(pAudioData->pMusic);
		SetAudioVolume(pAudioData->pMusic, GetExclusiveVolume(channel));

		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);
		ExtendVec(exclusiveChannelSettings, channel, AudioSettings());

		exclusiveChannelSettings[channel].bLoop = loops != 0;
				
		auto pOldData = exclusiveChannel[channel];
		auto pOldMusic = pOldData != nullptr ? pOldData->pMusic : nullptr;

		PlayAudio(pAudioData->pMusic, loops, fadeInMs, pOldMusic);

		//ReleaseAudioData(pOldMusic);

		exclusiveChannel[channel] = pAudioData;

		return true;
	}

	inline void StopAllExclusive(int fadeInMs = -1) {
		for (size_t channel = 0; channel < exclusiveChannel.size(); channel++) {
			StopExclusive(channel, fadeInMs);
		}
	}

	inline void StopExclusive(int channel, int fadeInMs = -1) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			StopAudio(pAudioData->pMusic, fadeInMs);
		}
	}

	inline void PauseExclusive(int channel) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			PauseAudio(pAudioData->pMusic);
		}
	}

	inline void ResumeExclusive(int channel) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			ResumeAudio(pAudioData->pMusic);
		}
	}

	inline bool ExclusiveChannelFadingState(int channel) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			return AudioFadeState(pAudioData->pMusic);
		}
		
		return MIX_NO_FADING;
	}

	inline bool ExclusiveChannelPlaying() {
		auto bRet = false;

		for (size_t channel = 0; channel < exclusiveChannel.size(); channel++) {
			bRet |= ExclusiveChannelPlaying(channel);
		}

		return bRet;
	}

	inline bool ExclusiveChannelPlaying(int channel) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			return AudioPlaying(pAudioData);
		}

		return false;
	}

	inline void SetExclusivePosition(int channel, double position) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			SetAudioPosition(pAudioData->pMusic, position);
		}
	}

	inline double GetExclusivePosition(int channel) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			return GetAudioPosition(pAudioData->pMusic);
		}

		return -1;
	}

	inline double GetExclusiveDuration(int channel) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			return GetAudioDuration(pAudioData->pMusic);
		}

		return -1;
	}

	// 0 ~ 100
	inline int GetExclusiveVolume(int channel) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);
		return GetChannelVolume(exclusiveChannelVolume, channel);
	}

	// 0 ~ 100
	inline void SetExclusiveVolume(int channel, int volume) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);
		SetChannelVolume(exclusiveChannelVolume, channel, volume);

		auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			SetAudioVolume(pAudioData->pMusic, volume);
		}
	}

	// ------------
	// AudioSettings
	// ------------

	inline void SetExclusiveABLoop(int channel, double start = 0, double end = 0) {
		ExtendVec(exclusiveChannelSettings, channel, AudioSettings());

		auto pSettings = &exclusiveChannelSettings[channel];

		pSettings->excl_bABLoop = true;
		pSettings->excl_start = start;
		pSettings->excl_end = end;
	}

	// should be called periodically
	inline void UpdateExclusiveABLoop() {
		ExtendVec(exclusiveChannelSettings, exclusiveChannel.size(), AudioSettings());

		for (size_t i = 0; i < exclusiveChannel.size(); i++) {
			const auto pAudioData = exclusiveChannel[i];

			if (pAudioData == nullptr) {
				continue;
			}

			const auto& channelSettings = exclusiveChannelSettings[i];

			if (channelSettings.excl_bABLoop) {
				const auto currentPosition = GetAudioPosition(pAudioData->pMusic);
				const auto endPos = channelSettings.excl_end == 0
					? GetAudioDuration(pAudioData->pMusic)
					: channelSettings.excl_end;

				// TODO will this affect the loop times?
				if (currentPosition >= endPos) {
					SetAudioPosition(pAudioData->pMusic, channelSettings.excl_start);
				}
			}
		}
	}

	// ------------
	// mixing
	//
	// not managed, one channel can play multiple musics
	// ------------

	std::vector<AudioDataVec> mixingChannel;
	ChannelVolume mixingChannelVolume;

	inline void EraseMixing(Mix_Music* pMusic) {
		for (auto& it : mixingChannel) {
			EraseVec(it, pMusic);

			// don't keep enpty pointers
			auto resultIt = std::remove(it.begin(), it.end(), nullptr);
		}
	}

	inline bool PlayMixing(const wchar_t* pFileName, const wchar_t* pKey,
		int channel, int loops = 0, int fadeInMs = -1) {
		auto pAudioData = CreateAudioAuto(pFileName, pKey);

		if (pAudioData == nullptr) {
			return false;
		}

		SetAutoClose(pAudioData->pMusic);
		SetAudioVolume(pAudioData->pMusic, GetMixingVolume(channel));

		ExtendVec(mixingChannel, channel, AudioDataVec());

		PlayAudio(pAudioData->pMusic, loops, fadeInMs);

		//MixingChannelAttenuation(channel);

		mixingChannel[channel].emplace_back(pAudioData);

		return true;
	}

	inline void StopAllMixing(int fadeInMs = -1) {
		for (size_t channel = 0; channel < mixingChannel.size(); channel++) {
			StopMixing(channel, fadeInMs);
		}
	}

	inline void StopMixing(int channel, int fadeInMs = -1) {
		ExtendVec(mixingChannel, channel, AudioDataVec());

		auto vec = mixingChannel[channel];

		for (auto& pAudioData : vec) {
			if (pAudioData != nullptr) {
				StopAudio(pAudioData->pMusic, fadeInMs);
			}
		}
	}

	inline void PauseMixing(int channel) {
		ExtendVec(mixingChannel, channel, AudioDataVec());

		auto vec = mixingChannel[channel];

		for (auto& pAudioData : vec) {
			if (pAudioData != nullptr) {
				PauseAudio(pAudioData->pMusic);
			}
		}
	}

	inline void ResumeMixing(int channel) {
		ExtendVec(mixingChannel, channel, AudioDataVec());

		auto vec = mixingChannel[channel];

		for (auto& pAudioData : vec) {
			if (pAudioData != nullptr) {
				ResumeAudio(pAudioData->pMusic);
			}
		}
	}

	inline bool MixingChannelPlaying() {
		auto bRet = false;

		for (size_t channel = 0; channel < mixingChannel.size(); channel++) {
			bRet |= MixingChannelPlaying(channel);
		}

		return bRet;
	}

	inline bool MixingChannelPlaying(int channel) {
		ExtendVec(mixingChannel, channel, AudioDataVec());

		auto vec = mixingChannel[channel];

		auto bRet = false;

		for (auto& pAudioData : vec) {
			if (pAudioData != nullptr) {
				bRet |= AudioPlaying(pAudioData);
			}
		}

		return bRet;
	}

	// 0 ~ 100
	inline int GetMixingVolume(int channel) {
		ExtendVec(mixingChannel, channel, AudioDataVec());
		return GetChannelVolume(mixingChannelVolume, channel);
	}

	// 0 ~ 100
	inline void SetMixingVolume(int channel, int volume) {
		ExtendVec(mixingChannel, channel, AudioDataVec());
		SetChannelVolume(mixingChannelVolume, channel, volume);

		auto& vec = mixingChannel[channel];

		for (auto& pAudioData : vec) {
			if (pAudioData != nullptr) {
				SetAudioVolume(pAudioData->pMusic, volume);
			}
		}
	}

	// ------------
	// AudioSettings
	// ------------

	//// reduce current channel item's volume
	//inline void MixingChannelAttenuation(int channel) {
	//	ExtendVec(mixingChannel, channel, AudioDataVec());

	//	for(auto& pAudioData: mixingChannel[channel]) {
	//		if (pAudioData != nullptr) {
	//			SetAudioVolume(pAudioData->pMusic, GetAudioVolume(pAudioData->pMusic) / 2);
	//		}
	//	}
	//}
};