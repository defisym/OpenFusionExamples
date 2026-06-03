#include "NeoStrBackend.h"

bool NeoStrBackendSupport(const NeoStrBackendType type) {
    switch (type) {
    case NeoStrBackendType::GDIPLUS: { return true; }
    default: return false;
    }

    return false;
}
