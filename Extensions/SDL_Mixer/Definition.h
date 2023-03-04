#pragma once

//immediate conditon ID
//constexpr auto ON_FINISH = 4;

#include <SDL.h>
#include <SDL_mixer.h>

//SDL mixer 可以与SDL Audio同时使用

struct GlobalData {
	GlobalData() {
		if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
			auto error = SDL_GetError();

			return;
		}

		if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1) {
			auto error = SDL_GetError();

			return ;
		}

		//auto pMusic = Mix_LoadWAV(ConvertWStrToStr(L"F:\\DEV\\Mobius\\程序\\data\\Audio\\bgm\\kyoumei.ogg").c_str());
		//auto pMusic = Mix_LoadWAV(ConvertWStrToStr(L"F:\\kyoumei.ogg").c_str());
		auto pMusic = Mix_LoadWAV(ConvertWStrToStr(L"F:\\中文.ogg",CP_UTF8).c_str());
		
		if(!pMusic) {
			auto error = SDL_GetError();

			return;
		}

		if (Mix_PlayChannel(-1, pMusic, 0) == -1) {
			auto error = SDL_GetError();

			return;
		}


	}
	~GlobalData() {
	}
};