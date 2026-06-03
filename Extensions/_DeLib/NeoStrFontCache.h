#pragma once

#include <memory>

#include "NeoStrBackend.h"

struct NeoStrFontCache {
};

std::unique_ptr<NeoStrFontCache> NeoStrFontCacheFactory(const NeoStrBackendType type);