#pragma once

struct NeoStrContext {
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
};