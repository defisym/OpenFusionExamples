#pragma once

enum class NeoStrBackendType {
    GDIPLUS,   
};

bool NeoStrBackendSupport(const NeoStrBackendType type);