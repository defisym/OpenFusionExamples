#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include "CaptureBackend.h"

// Frozen V1 layout. The SDK preamble supplies extHeader.
struct tagEDATA_V1 {
    extHeader eHeader;
    short swidth;
    short sheight;
    bool isLib = false;
    bool bLoadCallback = false;
    bool bLoadKeepAngle = false;
    bool HWA = false;
    bool stretchQuality = false;
    std::size_t memoryLimit;
    std::size_t sizeLimit;
    bool autoClean;
    std::size_t hotSpotComboID;
};
using EDITDATA_V1 = tagEDATA_V1;

struct tagEDATA_V2 {
    extHeader eHeader;
    short swidth;
    short sheight;
    bool isLib = false;
    bool bLoadCallback = false;
    bool bLoadKeepAngle = false;
    bool HWA = false;
    bool stretchQuality = false;
    std::size_t memoryLimit;
    std::size_t sizeLimit;
    bool autoClean;
    std::size_t hotSpotComboID;

    CaptureBackend captureBackend = CaptureBackend::Dc;
    // Future fixed-size fields consume this byte budget; reduce it by sizeof(field).
    std::uint8_t reserved[64] = {};
};
using EDITDATA = tagEDATA_V2;
using LPEDATA = EDITDATA*;

static_assert(sizeof(CaptureBackend) == 4);
static_assert(offsetof(EDITDATA, hotSpotComboID) == offsetof(EDITDATA_V1, hotSpotComboID));
static_assert(offsetof(EDITDATA, captureBackend) == sizeof(EDITDATA_V1));
// Keep this Win32 payload budget fixed when future fields consume the reserve.
static_assert(sizeof(void*) != 4 || sizeof(EDITDATA) == sizeof(EDITDATA_V1) + 68);

#define KCX_VERSION_V1 1
#define KCX_VERSION_V2 2
#define KCX_CURRENT_VERSION KCX_VERSION_V2

inline void UpgradeNeoPicEditData(const EDITDATA_V1* old, EDITDATA* current) {
    current->swidth = old->swidth;
    current->sheight = old->sheight;
    current->isLib = old->isLib;
    current->bLoadCallback = old->bLoadCallback;
    current->bLoadKeepAngle = old->bLoadKeepAngle;
    current->HWA = old->HWA;
    current->stretchQuality = old->stretchQuality;
    current->memoryLimit = old->memoryLimit;
    current->sizeLimit = old->sizeLimit;
    current->autoClean = old->autoClean;
    current->hotSpotComboID = old->hotSpotComboID;
    current->captureBackend = CaptureBackend::Dc;
    std::fill_n(current->reserved, sizeof(current->reserved), std::uint8_t{});
    // UpdateEditData copies the other header fields and sets size/version.
    current->eHeader.extMaxSize = sizeof(EDITDATA);
}
