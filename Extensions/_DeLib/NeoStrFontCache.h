#pragma once

#include <memory>

#include "NeoStrBackend.h"
#include "NeoStrLayout.h"
#include "NeoStrWordBreakHandler.h"

struct NeoStrFontCache {
    using CharSizeCache = std::map<wchar_t, CharSize>;
    WordBreakHandler* pWordBreakCache = nullptr;

    virtual bool CacheValid() const;
    virtual void Alloc();
    virtual void Release();
};

std::unique_ptr<NeoStrFontCache> NeoStrFontCacheFactory(const NeoStrBackendType type);