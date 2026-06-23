#pragma once

struct NeoStrContext {
    bool bInitialized = false;

    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
};