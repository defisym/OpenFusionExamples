#pragma once

// ------------------------------------------------
// Include
// ------------------------------------------------

#include <SDL_mutex.h>
#include <SDL_atomic.h>
#pragma comment(lib, "SDL2.lib")

#include <type_traits>

// ------------------------------------------------
// Concept
// ------------------------------------------------

template<typename T>
concept LockConcept = std::is_same_v<std::remove_cvref_t<T>, SDL_SpinLock>
|| std::is_same_v<std::remove_cvref_t<T>, SDL_mutex>;

// ------------------------------------------------
// Lock
// ------------------------------------------------

template<LockConcept LockType>
struct Lock;

template<>
struct Lock<SDL_SpinLock> {
    SDL_SpinLock lock = 0;

    SDL_SpinLock* GetLock() { return &lock; }
};

template<>
struct Lock<SDL_mutex> {
    SDL_mutex* plock = nullptr;

    Lock() { plock = SDL_CreateMutex(); }
    ~Lock() { SDL_DestroyMutex(plock); }

    SDL_mutex* GetLock() { return plock; }
};

// ------------------------------------------------
// LockHelper
// ------------------------------------------------

template<LockConcept LockType>
struct LockHelper;

template<>
struct LockHelper<SDL_SpinLock> {
private:
    SDL_SpinLock* _plock = nullptr;
public:
    explicit LockHelper(SDL_SpinLock* plock) :_plock(plock) {
        SDL_AtomicLock(plock);
    }
    explicit LockHelper(Lock<SDL_SpinLock>& lock)
        :LockHelper(lock.GetLock()) { }
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
    explicit LockHelper(Lock<SDL_mutex>& lock)
        :LockHelper(lock.GetLock()) {
    }
    ~LockHelper() {
        SDL_UnlockMutex(_plock);
    }
};

using MutexLockHelper = LockHelper<SDL_mutex>;

// ------------------------------------------------
// Get LockHelper
// ------------------------------------------------

template<LockConcept LockType>
struct GetLockHelper;

template<>
struct GetLockHelper<SDL_SpinLock> {
    using Type = SpinLockHelper;
};

template<>
struct GetLockHelper<SDL_mutex> {
    using Type = MutexLockHelper;
};