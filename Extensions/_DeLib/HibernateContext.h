#pragma once

// ------------------------------------------------
// Include
// ------------------------------------------------

#include <SDL_mutex.h>
#pragma comment(lib, "SDL2.lib")

#include <Rule035.h>
#include <LockHelper.h>

struct HibernateContext {
    SDL_mutex* pMutex = nullptr;
    SDL_cond* pCond = nullptr;

    HibernateContext() {
        pMutex = SDL_CreateMutex();
        pCond = SDL_CreateCond();
    }
    ~HibernateContext() {
        SDL_DestroyCond(pCond);
        SDL_DestroyMutex(pMutex);
    }
    RULE_NO_COPY(HibernateContext);

    void Hibernate() const {
        SDL_CondWait(pCond, pMutex);
    }
    void Wake() const {
        auto lockerHelper = MutexLockHelper{ pMutex };
        SDL_CondSignal(pCond);
    }
    void WakeAll() const {
        auto lockerHelper = MutexLockHelper{ pMutex };
        SDL_CondBroadcast(pCond);
    }

    [[nodiscard]] MutexLockHelper GetLockHelper() const {
        return MutexLockHelper{ pMutex };
    }
};