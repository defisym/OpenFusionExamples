#pragma once

#include "AudioInterface.h"

constexpr auto SDL_EXCEPTION_INIT = 0;
constexpr auto SDL_EXCEPTION_AUDIO = 1;

class SI :public AudioInterface {
private:
	using AudioCallback = void(*)(void* userdata, Uint8* stream, int len);

	FFMpeg** ppFFMpeg = nullptr;
	std::vector<FFMpeg**> ppFFMpegs;

public:
	SI() {
		//initialize the video audio & timer subsystem 
		//if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
			auto error = SDL_GetError();

			throw SDL_EXCEPTION_INIT;
		}

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
			const auto setter = SDL_memset;
			const auto mixer = [] (void* dst, const void* src, size_t len, int volume) {
				SDL_MixAudio((Uint8*)dst, (const Uint8*)src, len, volume);
			};

			const auto pSI = (SI*)userdata;
			const auto ppFFMpeg = pSI->ppFFMpeg;

			if (ppFFMpeg == nullptr) {
				setter(stream, 0, len);

				return;
			}

			const auto pFFMpeg = *ppFFMpeg;

			if (pFFMpeg == nullptr) {
				setter(stream, 0, len);

				return;
			}

			pFFMpeg->audio_fillData(stream, len, setter, mixer);
		};

		if (SDL_OpenAudio(&wanted_spec, nullptr) < 0) {
			auto error = SDL_GetError();

			throw SDL_EXCEPTION_AUDIO;
		}
	}
	~SI() override {
		SDL_CloseAudio();
		SDL_Quit();
	}

	inline void AddInstance(FFMpeg** ppFFMpeg, void* pData) override {
		const bool bForceNoAudio = *static_cast<bool*>(pData);

		if (!bForceNoAudio) {
			this->ppFFMpeg = ppFFMpeg;
			this->ppFFMpegs.emplace_back(ppFFMpeg);
		}
	}
	inline void RemoveInstance(FFMpeg** ppFFMpeg, void* pData) override {
		const auto it = std::ranges::find(ppFFMpegs, ppFFMpeg);
		if (it != ppFFMpegs.end()) {
			ppFFMpegs.erase(it);
		}

		if (!ppFFMpegs.empty()) {
			this->ppFFMpeg = ppFFMpegs.back();
		}
		else {
			this->ppFFMpeg = nullptr;
		}
	}
};
