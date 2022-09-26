#pragma once

//Condition_OnVideoFinish
constexpr auto ON_FINISH = 4;

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

	MemVideoLib* pMemVideoLib = nullptr;

	GlobalData() {
		pMemVideoLib = new MemVideoLib;

		//initialize the video audio & timer subsystem 
		//if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
			auto error = SDL_GetError();

			throw SDL_EXCEPTION_INIT;
		}

#ifdef _EXTERNAL_SDL_AUDIO_INIT
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
		wanted_spec.callback = [](void* userdata, Uint8* stream, int len) {
			// No mutex needed here as audio is paused when deleting pFFMpeg
			GlobalData* pData = (GlobalData*)userdata;

			FFMpeg* pFFMpeg = *pData->ppFFMpeg;

			if (pFFMpeg == nullptr) {
				SDL_memset(stream, 0, len);

				return;
			}

			pFFMpeg->audio_fillData(stream, len);
		};

		if (SDL_OpenAudio(&wanted_spec, nullptr) < 0) {
			auto error = SDL_GetError();

			throw SDL_EXCEPTION_AUDIO;
		}
#endif // _EXTERNAL_SDL_AUDIO_INIT	

		bSDLInit = true;
	}

	~GlobalData() {
		delete pMemVideoLib;
				
#ifdef _EXTERNAL_SDL_AUDIO_INIT
		SDL_CloseAudio();
#endif // _EXTERNAL_SDL_AUDIO_INIT	

		SDL_Quit();

		bSDLInit = false;
	}
};
