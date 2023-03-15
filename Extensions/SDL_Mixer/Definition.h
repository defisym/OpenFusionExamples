#pragma once

//immediate conditon ID
//constexpr auto ON_FINISH = 4;

// SDL mixer can work with SDL Audio together
// AKA compatible with FFMpeg
#include <SDL.h>
#include <SDL_mixer.h>

#include <SoundTouchDLL.h>

#include <map>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <functional>

#include "MusicScore.h"

using namespace std::literals::chrono_literals;

struct AudioEffect;
struct AudioData;
struct GlobalData;

using EffectBuffer = unsigned char;

enum class AudioType {
	BGM,
	BGS,
	SE,
	DUB,
};

constexpr auto GlobalEffectBufferSz = 65536;
inline EffectBuffer pGlobalEffectBuffer[GlobalEffectBufferSz] = { 0 };

struct AudioEffect {
	GlobalData* pGlobalData = nullptr;

	HANDLE hSoundTouch = nullptr;

	EffectBuffer* pBuf = pGlobalEffectBuffer;
	int bufSz = GlobalEffectBufferSz;

	using AudioProcessor = std::function<void(HANDLE, float)>;
	struct AudioProcessorPair {
		AudioProcessor processor = nullptr;
		float param = 0.0;
	};

	std::vector<AudioProcessorPair> effects;

	AudioEffect(GlobalData* pGlobalData, HANDLE hSoundTouch,
		const std::vector<AudioProcessorPair>& effects) {
		this->pGlobalData = pGlobalData;

		this->hSoundTouch = hSoundTouch;
		
		this->effects = effects;
	}

	inline bool ProcessAudio(void* stream, const int len) {
		// not rigorous but a big enough global buffer for default spec is ok
		// len = chunk size * (16bit / 8bit) * channel
		memset(pBuf, 0, GlobalEffectBufferSz);

		unsigned int dataWrite = 0;

		constexpr auto sountTouchSize = sizeof(const short);

		const auto protectedBufSz = min(bufSz, len);
		const auto shortSize = protectedBufSz / sountTouchSize;

		memcpy(pBuf, stream, protectedBufSz);

		for (auto& effectPair : effects) {
			soundtouch_clear(hSoundTouch);
			effectPair.processor(hSoundTouch, effectPair.param);


			soundtouch_putSamples_i16(hSoundTouch, (short*)pBuf, shortSize);
			dataWrite = soundtouch_receiveSamples_i16(hSoundTouch, (short*)pBuf, shortSize);

			// sound touch uses a float convert[8192] as interbuffer
			// and will not output until data is enough (usually more than 8192 bytes, 4096 shorts)
			// so the buffer size should be okay here, we don't need a loop to flush
			if (dataWrite == 0) {
				return false;
			}
		}

		//memset(stream, 0, len);
		memcpy(stream, pBuf, protectedBufSz);
		//memcpy(stream, pBuf, dataWrite* sountTouchSize);

		return true;
	}
};

constexpr auto AudioDataException_DecryptFailed = -1;
constexpr auto AudioDataException_CreateRWFailed = -2;
constexpr auto AudioDataException_CreateMusicFailed = -3;

struct AudioData {
	std::wstring fileName;

	Encryption* pDecrypt = nullptr;

	bool bHook = false;

	SDL_RWops* pRW = nullptr;
	Mix_Music* pMusic = nullptr;	
	AudioEffect* pEffect = nullptr;

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
		const auto bRet = pDecrypt->DecryptFileDirectly(pFileName);

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
		delete pEffect;

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

	SDL_SpinLock dataLock = 0;
	std::vector<Mix_Music*> toRelease;

	HANDLE hSoundTouch = nullptr;

	GlobalData() {
		if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
			auto error = SDL_GetError();

			throw GlobalDataException_SDLInitFailed;
		}

		if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1) {
			auto error = SDL_GetError();

			throw GlobalDataException_MixOpenAudioFailed;
		}

		hSoundTouch = soundtouch_createInstance();
		soundtouch_setChannels(hSoundTouch, MIX_DEFAULT_CHANNELS);
		soundtouch_setSampleRate(hSoundTouch, MIX_DEFAULT_FREQUENCY);
	}
	~GlobalData() {
		ReleaseAudioData();

		soundtouch_destroyInstance(hSoundTouch);		

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
		ClearMixingPlayRecord();
	}

	// ------------
	// lib -> get
	// ------------

	inline Mix_Music* GetAudioPointer(const wchar_t* pFileName) {
		if (pFileName == nullptr) {
			return nullptr;
		}

		const auto it = GetAudioData(pFileName);

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
		const std::function<void(Mix_Music*)>& processor) {
		const auto pMusic = GetAudioPointer(pFileName);

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

	inline const std::wstring* GetAudioName(const Mix_Music* pMusic) {
		const auto ppAudioData = GetAudioData(pMusic);

		if (ppAudioData == pAudioDatas.end()) {
			return nullptr;
		}

		const auto fileName = &((*ppAudioData)->fileName);

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

		auto toRemove = std::ranges::remove(pAudioDatas, nullptr);
		pAudioDatas.erase(toRemove.begin(), toRemove.end());
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

	// add to toRelease list
	inline void AddToReleaseList(Mix_Music* pMusic) {
		SDL_AtomicLock(&dataLock);
		toRelease.emplace_back(pMusic);
		SDL_AtomicUnlock(&dataLock);
	}

	// release stopped audio data.
	// should be called periodically
	inline void ReleaseStoppedAudioData() {
		SDL_AtomicLock(&dataLock);
		std::vector<Mix_Music*> local = toRelease;

		toRelease.clear();
		SDL_AtomicUnlock(&dataLock);

		std::ranges::sort(local);
		local.erase(std::ranges::unique(local).begin(), local.end());

		for (const auto& pMusic : local) {
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
		const auto bDecrypt = !(pKey == nullptr || StrEmpty(pKey));

		const auto pAudioData = bDecrypt
			                        ? CreateAudio(pFileName, pKey)
			                        : CreateAudio(pFileName);

		return pAudioData;
	}

	// ------------
	// play
	// ------------

	static inline bool PlayAudio(Mix_Music* pMusic,
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
		const auto pMusic = GetAudioPointer(pFileName);
		const auto pOldMusic = GetAudioPointer(pCrossFade);

		return PlayAudio(pMusic, loops, fadeMs, pOldMusic);
	}

	static inline void StopAudio(Mix_Music* pMusic, int fadeMs = -1) {
		if (fadeMs == -1) {
			Mix_HaltMusicStream(pMusic);
		}
		else {
			Mix_FadeOutMusicStream(pMusic, fadeMs);
		}
	}

	static inline void PauseAudio(Mix_Music* pMusic) {
		Mix_PauseMusicStream(pMusic);
	}

	static inline void ResumeAudio(Mix_Music* pMusic) {
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
				pGlobalData->AddToReleaseList(pMusic);
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

	static inline bool AudioPlaying(Mix_Music* pMusic) {
		return Mix_PlayingMusicStream(pMusic);
	}
	inline bool AudioPlaying(AudioData* pAudioData) {
		return pAudioData != nullptr
			? AudioPlaying(pAudioData->pMusic)
			: false;
	}

	static inline Mix_Fading AudioFadeState(Mix_Music* pMusic) {
		return Mix_FadingMusicStream(pMusic);		
	}

	// ------------
	// Prop
	// ------------

	// in seconds
	static inline void SetAudioPosition(Mix_Music* pMusic, double position) {
		Mix_SetMusicPositionStream(pMusic, position);
	}
	// in seconds
	inline void SetAudioPosition(const wchar_t* pFileName, double position) {
		GetAudioData(pFileName, [&] (Mix_Music* pMusic) {
			SetAudioPosition(pMusic, position);
		});
	}

	// in seconds
	static inline double GetAudioPosition(Mix_Music* pMusic) {
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
	static inline double GetAudioDuration(Mix_Music* pMusic) {
		return Mix_MusicDuration(pMusic);
	}

	// 0 ~ 100 -> 0 ~ 128
	static inline int Range(int v, int l, int h) {
		return max(min(v, h), l);
	}

	static inline int VolumeConverter(int volume) {
		return (int)((Range(volume, Fusion_MinVolume, Fusion_MaxVolume) /
			static_cast<double>(Fusion_MaxVolume)) * static_cast<double>(Mix_MaxVolume));
	}

	static inline int VolumeReverseConverter(int volume) {
		return (int)((Range(volume, Mix_MinVolume, Mix_MaxVolume) /
			static_cast<double>(Mix_MaxVolume)) * static_cast<double>(Fusion_MaxVolume));
	}

	// 0 ~ 100
	static inline void SetAudioVolume(Mix_Music* pMusic, int volume) {
		Mix_VolumeMusicStream(pMusic, VolumeConverter(volume));
	}
	// 0 ~ 100
	inline void SetAudioVolume(const wchar_t* pFileName, int volume) {
		GetAudioData(pFileName, [&] (Mix_Music* pMusic) {
			SetAudioVolume(pMusic, volume);
		});
	}

	// 0 ~ 100
	static inline int GetAudioVolume(Mix_Music* pMusic) {
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

		bool bEffect = false;
		MusicScore::Score score = MusicScore::Score::Loop;
		float base = 0;

		//bool mix_bAttenuation = false;
	};

	using ChannelSettings = std::vector<AudioSettings>;

	template<typename T>
	static inline void ExtendVec(std::vector<T>& vec, size_t newSz, T val) {
		if (vec.size() <= newSz) {
			vec.resize(newSz + 1, val);
		}
	}

	static inline void EraseVec(AudioDataVec& vec, const Mix_Music* pMusic) {
		for (auto& i : vec) {
			if (i != nullptr && i->pMusic == pMusic) {
				i = nullptr;
			}
		}
	}

	// 0 ~ 100
	static inline int GetChannelVolume(ChannelVolume& vec, int channel) {
		ExtendVec(vec, channel, Fusion_MaxVolume);
		return Range(vec[channel], Fusion_MinVolume, Fusion_MaxVolume);
	}

	// 0 ~ 100
	static inline void SetChannelVolume(ChannelVolume& vec, int channel, int volume) {
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
		const auto pAudioData = CreateAudioAuto(pFileName, pKey);

		if (pAudioData == nullptr) {
			StopExclusive(channel, fadeInMs);

			return false;
		}

		SetAutoClose(pAudioData->pMusic);
		SetAudioVolume(pAudioData->pMusic, GetExclusiveVolume(channel));

		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);
		ExtendVec(exclusiveChannelSettings, channel, AudioSettings());

		exclusiveChannelSettings[channel].bLoop = loops != 0;

		const auto pOldData = exclusiveChannel[channel];
		const auto pOldMusic = pOldData != nullptr ? pOldData->pMusic : nullptr;

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

		const auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			StopAudio(pAudioData->pMusic, fadeInMs);
		}
	}

	inline void PauseExclusive(int channel) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		const auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			PauseAudio(pAudioData->pMusic);
		}
	}

	inline void ResumeExclusive(int channel) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		const auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			ResumeAudio(pAudioData->pMusic);
		}
	}

	inline bool ExclusiveChannelFadingState(int channel) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		const auto pAudioData = exclusiveChannel[channel];

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

		const auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			return AudioPlaying(pAudioData);
		}

		return false;
	}

	inline void SetExclusivePosition(int channel, double position) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		const auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			SetAudioPosition(pAudioData->pMusic, position);
		}
	}

	inline double GetExclusivePosition(int channel) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		const auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			return GetAudioPosition(pAudioData->pMusic);
		}

		return -1;
	}

	inline double GetExclusiveDuration(int channel) {
		ExtendVec(exclusiveChannel, channel, (AudioData*)nullptr);

		const auto pAudioData = exclusiveChannel[channel];

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

		const auto pAudioData = exclusiveChannel[channel];

		if (pAudioData != nullptr) {
			SetAudioVolume(pAudioData->pMusic, volume);
		}
	}

	// ------------
	// AudioSettings
	// ------------

	inline void SetExclusiveABLoop(int channel, double start = 0, double end = 0) {
		ExtendVec(exclusiveChannelSettings, channel, AudioSettings());

		const auto pSettings = &exclusiveChannelSettings[channel];

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
	ChannelSettings mixingChannelSettings;

	struct playRecord {
		decltype(std::chrono::steady_clock::now()) updateTime = std::chrono::steady_clock::now();
		int size = 0;
	};

	int delayMs = 1000;
	std::map < std::wstring, playRecord > sameNameSize;

	inline void ClearMixingPlayRecord() {
		const auto now = std::chrono::steady_clock::now();

		for(auto& it: sameNameSize) {
			const auto durMs = (now - it.second.updateTime) / 1ms;
			if(durMs > delayMs) {
				it.second.size = 0;
			}
		}
	}

	inline void EraseMixing(Mix_Music* pMusic) {
		for (auto& it : mixingChannel) {
			EraseVec(it, pMusic);

			// don't keep enpty pointers
			const auto toRemove = std::ranges::remove(it, nullptr);
			it.erase(toRemove.begin(), toRemove.end());
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
		ExtendVec(mixingChannelSettings, channel, AudioSettings());
		
		const auto& channelSettings = mixingChannelSettings[channel];
		do {
			if (!channelSettings.bEffect) {
				break;
			}
			constexpr auto arrDiv = 1;

			const auto audioNum = GetMixingChannelSameAudioNum(channel, pAudioData->pMusic);
			const auto audioPlayed = sameNameSize[std::wstring(pFileName)].size;

			if (audioPlayed == 0) {
				break;
			}

			pAudioData->pEffect = new AudioEffect(this, hSoundTouch,
				{
					{
						soundtouch_setPitchSemiTones,
						//MusicScore::GetNote(arrDiv * audioPlayed)
						//MusicScore::GetNote(arrDiv * audioPlayed, MusicScore::Score::CrystalPrelude,0)
						MusicScore::GetNote(arrDiv * audioPlayed,channelSettings.score,channelSettings.base)
					},
					//{
					//	soundtouch_setRate,
					//	static_cast<float>(1.0 - 0.05 * audioNum)
					//}
				});

#ifdef _DEBUG
			OutputDebugString(_ftos(pAudioData->pEffect->effects[0].param).c_str());
			OutputDebugString(L"\n");
#endif

			Mix_RegisterMusicEffect(pAudioData->pMusic,
				[] (Mix_Music* mus, void* stream, int len, void* udata) {
					const auto pEffect = (AudioEffect*)udata;
					pEffect->ProcessAudio(stream, len);
				},
				nullptr,
					pAudioData->pEffect);
		} while (false);

		PlayAudio(pAudioData->pMusic, loops, fadeInMs);
		sameNameSize[std::wstring(pFileName)].updateTime = std::chrono::steady_clock::now();
		sameNameSize[std::wstring(pFileName)].size++;

		mixingChannel[channel].emplace_back(pAudioData);

		return true;
	}

	inline void StopAllMixing(int fadeInMs = -1) {
		for (size_t channel = 0; channel < mixingChannel.size(); channel++) {
			StopMixing(channel, fadeInMs);
		}
	}

	inline void StopMixing(int channel, int fadeInMs = -1) {
		MixingIterateChannel(channel, [&] (AudioData* pAudioData) {
			StopAudio(pAudioData->pMusic, fadeInMs);
		});
	}

	inline void PauseMixing(int channel) {
		ExtendVec(mixingChannel, channel, AudioDataVec());

		const auto vec = mixingChannel[channel];

		for (const auto& pAudioData : vec) {
			if (pAudioData != nullptr) {
				PauseAudio(pAudioData->pMusic);
			}
		}
	}

	inline void ResumeMixing(int channel) {
		ExtendVec(mixingChannel, channel, AudioDataVec());

		const auto vec = mixingChannel[channel];

		for (const auto& pAudioData : vec) {
			if (pAudioData != nullptr) {
				ResumeAudio(pAudioData->pMusic);
			}
		}
	}

	inline void MixingIterateChannel(int channel,
		const std::function<void(AudioData*)>& processor) {
		ExtendVec(mixingChannel, channel, AudioDataVec());

		const auto& vec = mixingChannel[channel];
		
		for (const auto& pAudioData : vec) {
			if (pAudioData != nullptr) {
				processor(pAudioData);
			}
		}
	}

	inline bool MixingChannelPlaying() {
		auto bRet = false;

		for (size_t channel = 0; channel < mixingChannel.size(); channel++) {
			bRet |= MixingChannelPlaying(static_cast<int>(channel));
		}

		return bRet;
	}

	inline bool MixingChannelPlaying(int channel) {
		auto bRet = false;

		MixingIterateChannel(channel, [&] (AudioData* pAudioData) {
			bRet |= AudioPlaying(pAudioData);
		});

		return bRet;
	}

	inline int GetMixingChannelSize(int channel) {
		int sz = 0;

		MixingIterateChannel(channel, [&] (AudioData* pAudioData) {
			sz++;
		});

		return sz;
	}

	inline int GetMixingChannelSameAudioNum(int channel, const Mix_Music* pMusic) {
		const auto pFileName = GetAudioName(pMusic);

		int num = 0;

		MixingIterateChannel(channel, [&] (const AudioData* pAudioData) {
			if (pAudioData->fileName == *pFileName) {
				num++;
			}
		});

		return num;
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

		const auto& vec = mixingChannel[channel];

		for (const auto& pAudioData : vec) {
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

	inline void SetMixingChannelScore(int channel, bool bEnable,
		MusicScore::Score score = MusicScore::Score::Loop, float base = 0) {
		ExtendVec(mixingChannelSettings, channel, AudioSettings());

		mixingChannelSettings[channel].bEffect = bEnable;

		mixingChannelSettings[channel].score = score;
		mixingChannelSettings[channel].base = base;
	}
};
