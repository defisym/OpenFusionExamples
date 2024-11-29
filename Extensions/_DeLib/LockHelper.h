#pragma once

#include <SDL_mutex.h>
#include <SDL_atomic.h>
#pragma comment(lib, "SDL2.lib")

#include <type_traits>

template<typename T>
concept LockConcept = std::is_same_v<std::remove_cvref_t<T>, SDL_SpinLock>
|| std::is_same_v<std::remove_cvref_t<T>, SDL_mutex>;

template<LockConcept Lock>
struct LockHelper;

template<>
struct LockHelper<SDL_SpinLock> {
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

using SpinLockHelper = LockHelper<SDL_SpinLock>;

template<>
struct LockHelper<SDL_mutex> {
private:
    SDL_mutex* _plock = nullptr;
public:
    explicit LockHelper(SDL_mutex* plock) :_plock(plock) {
        SDL_LockMutex(plock);
    }
    ~LockHelper() {
        SDL_UnlockMutex(_plock);
    }
};

using MutexLockHelper = LockHelper<SDL_mutex>;
