#pragma once

#include <SDL_atomic.h>
#pragma comment(lib, "SDL2.lib")

struct LockHelper {
	private:
		SDL_SpinLock* _plock = nullptr;
	public:
		explicit LockHelper(SDL_SpinLock* plock) :_plock(plock) {
			SDL_AtomicLock(plock);
		}
		~LockHelper() {
			SDL_AtomicUnlock(_plock);
		}
	};