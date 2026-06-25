#pragma once

#include <memory>

#include "NeoStrContext.h"
#include "NeoStrFontCache.h"

enum class NeoStrBackendType {
    GDIPLUS,
};

bool NeoStrBackendSupport(const NeoStrBackendType type);

std::unique_ptr<NeoStrContext> NeoStrContextFactory(const NeoStrBackendType type);
std::unique_ptr<NeoStrFontCache> NeoStrFontCacheFactory(const NeoStrBackendType type);