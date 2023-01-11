#pragma once

//Condition_OnVideoFinish
constexpr auto ON_FINISH = 4;
constexpr auto ON_OPENFAILED = 5;

using namespace std::literals;
using Timer = std::chrono::time_point<std::chrono::steady_clock>;

struct MemVideoLib {
	std::map<std::wstring, Encryption*> data;

	using It = decltype(data.find(std::wstring()));

	MemVideoLib() {

	}

	~MemVideoLib() {
		EraseAll();
	}

	inline void PutItem(std::wstring& key, Encryption* pData) {
		data[key] = pData;
	}

	inline void EraseItem(std::wstring& key) {
		auto it = GetItem(key);

		if (it != data.end()) {
			delete it->second;
			data.erase(it);
		}
	}

	inline void EraseAll() {
		for (auto& it : data) {
			delete it.second;
		}

		data.clear();
	}

	inline It GetItem(std::wstring& key) {
		return data.find(key);
	}

	inline bool ItemExist(std::wstring& key) {
		return GetItem(key) != data.end();
	}

	inline bool ItemExist(It& it) {
		return it != data.end();
	}
};

struct GlobalData {
	bool bSDLInit = false;
	FFMpeg** ppFFMpeg = nullptr;
	std::vector<FFMpeg**> ppFFMpegs;

	MemVideoLib* pMemVideoLib = nullptr;
#ifdef FMOD_AUDIO
	FModInterface cFMI;
#endif
	//using AudioCallback = void(*)(void* userdata, Uint8* stream, int len);
	
	static void AudioCallback(void* userdata, Uint8* stream, int len) {
		// No mutex needed here as audio is paused when deleting pFFMpeg
		auto CallBackCore = [&](FFMpeg** ppFFMpeg, Setter setter, Mixer mixer) {
			if (ppFFMpeg == nullptr) {
				setter(stream, 0, len);

				return;
			}

			auto pFFMpeg = *ppFFMpeg;

			if (pFFMpeg == nullptr) {
				setter(stream, 0, len);

				return;
			}

			pFFMpeg->audio_fillData(stream, len, setter, mixer);

			return;
		};

#ifdef FMOD_AUDIO
		auto ppFFMpeg = (FFMpeg**)userdata;
		CallBackCore(ppFFMpeg, memset, [](void* dst, const void* src, size_t len, int volume) {
			memcpy(dst, src, len);
			});
#else
		GlobalData* pData = (GlobalData*)userdata;
		CallBackCore(pData->ppFFMpeg, SDL_memset, [](void* dst, const void* src, size_t len, int volume) {
			SDL_MixAudio((Uint8*)dst, (const Uint8*)src, len, volume);
			});
#endif
	};

	GlobalData() {
		pMemVideoLib = new MemVideoLib;
				
#ifdef FMOD_AUDIO		
#else
		//initialize the video audio & timer subsystem 
		//if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {	
			auto error = SDL_GetError();

			throw SDL_EXCEPTION_INIT;
		}
#endif	

#ifdef _EXTERNAL_AUDIO_INIT
#ifdef FMOD_AUDIO
#else
		SDL_AudioSpec wanted_spec = { 0 };

		//DSP frequency -- samples per second
		wanted_spec.freq = TARGET_SAMPLE_RATE;
		wanted_spec.format = AUDIO_S16SYS;
		wanted_spec.channels = 2;
		//sclient if no output
		wanted_spec.silence = 0;
		//specifies a unit of audio data refers to the size of the audio buffer in sample frames
		//recommand: 512~8192, ffplay: 1024
		wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;

		wanted_spec.userdata = (void*)this;
		wanted_spec.callback = AudioCallback;

		if (SDL_OpenAudio(&wanted_spec, nullptr) < 0) {
			auto error = SDL_GetError();

			throw SDL_EXCEPTION_AUDIO;
		}
#endif

#endif // _EXTERNAL_AUDIO_INIT	

		bSDLInit = true;
	}

	~GlobalData() {
		delete pMemVideoLib;
				
#ifdef _EXTERNAL_AUDIO_INIT

#ifdef FMOD_AUDIO
#else
		SDL_CloseAudio();
#endif

#endif // _EXTERNAL_AUDIO_INIT	

#ifdef FMOD_AUDIO		
#else
		SDL_Quit();
#endif

		bSDLInit = false;
	}

	inline void Create(bool bForceNoAudio, FFMpeg** ppFFMpeg) {
		// Update global data
		if (!bForceNoAudio) {
			this->ppFFMpeg = ppFFMpeg;
			this->ppFFMpegs.emplace_back(ppFFMpeg);
#ifdef FMOD_AUDIO
			this->cFMI.FMI_CreateSound(std::forward<std::wstring>(_itos((long)ppFFMpeg)), ppFFMpeg, [](FMOD_CREATESOUNDEXINFO& exinfo) {
				/* Number of channels in the sound. */
				exinfo.numchannels = TARGET_CHANNEL_NUMBER;
				/* Default playback rate of sound. */
				exinfo.defaultfrequency = TARGET_SAMPLE_RATE;
				/* Chunk size of stream update in samples. This will be the amount of data passed to the user callback. */
				
				// if size = SDL_AUDIO_BUFFER_SIZE, will micro shutter when seeking
				// if size >> SDL_AUDIO_BUFFER_SIZE will cause lag due if sync with audio timer
#ifdef _EXTERNAL_CLOCK_SYNC
				exinfo.decodebuffersize = 8 * SDL_AUDIO_BUFFER_SIZE;
#else
				exinfo.decodebuffersize = SDL_AUDIO_BUFFER_SIZE;
#endif
		
				/* Length of PCM data in bytes of whole song (for Sound::getLength) */
				exinfo.length = SDL_AUDIO_BUFFER_SIZE;
				//exinfo.length = exinfo.defaultfrequency * exinfo.numchannels * sizeof(signed short) * 5;
				/* Data format of sound. */
				exinfo.format = FMOD_SOUND_FORMAT_PCM16;              
				/* User callback for reading. */
				exinfo.pcmreadcallback =                              
					[](FMOD_SOUND* sound, void* data, unsigned int datalen) {
					auto pSound = (FMOD::Sound*)sound;

					void* lockPtr_1 = nullptr;
					void* lockPtr_2 = nullptr;

					size_t prtLength_1 = 0;
					size_t prtLength_2 = 0;

					pSound->lock(0, datalen
						, &lockPtr_1, &lockPtr_2
						, &prtLength_1, &prtLength_2);

					void* userdata = nullptr;
					pSound->getUserData((void**)&userdata);

					AudioCallback(userdata, (Uint8*)data, datalen);

					pSound->unlock(lockPtr_1, lockPtr_2
						, prtLength_1, prtLength_2);

					return FMOD_OK;
				};
				exinfo.pcmsetposcallback = nullptr;                   /* User callback for seeking. */
				});
			this->cFMI.FMI_PlaySound(std::forward<std::wstring>(_itos((long)ppFFMpeg)), false);
#endif
		}
	}

	inline void Destroy(FFMpeg** ppFFMpeg) {
		auto it = std::find(ppFFMpegs.begin(), ppFFMpegs.end(), ppFFMpeg);
		if (it != ppFFMpegs.end()) {
			ppFFMpegs.erase(it);
		}

		if (!ppFFMpegs.empty()) {
			this->ppFFMpeg = ppFFMpegs.back();
		}
		else {
			this->ppFFMpeg = nullptr;
		}

#ifdef FMOD_AUDIO
		auto name = _itos((long)ppFFMpeg);
		this->cFMI.FMI_StopSound(std::forward<std::wstring>(name));
#endif
	}

#ifdef FMOD_AUDIO
	inline void UpdateVolume(FFMpeg** ppFFMpeg) {
		auto it = std::find(ppFFMpegs.begin(), ppFFMpegs.end(), ppFFMpeg);
		if (it != ppFFMpegs.end() && (*ppFFMpeg) != nullptr) {
			auto name = _itos((long)ppFFMpeg);

			this->cFMI.FMI_SetVolume(std::forward<std::wstring>(name)
				,(float)((*ppFFMpeg)->get_volume() / 128.0));
		}
	}
#endif
};
