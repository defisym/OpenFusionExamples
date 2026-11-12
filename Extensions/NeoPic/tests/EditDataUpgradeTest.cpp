// The public CI does not contain Fusion's proprietary headers. Use only a mock
// SDK header; the versioned payload and upgrade function below are production code.
#include <cstdint>
#include <cstddef>
struct extHeader {
    std::uint32_t extSize, extMaxSize, extVersion;
    void* extID;
    void* extPrivateData;
};
#include "../EditData.h"
#include <cassert>
#include <iostream>
#include <limits>
#include <type_traits>

static_assert(std::is_standard_layout_v<EDITDATA_V1> && std::is_standard_layout_v<EDITDATA>);
static_assert(sizeof(CaptureBackend) == 4 && sizeof(EDITDATA::reserved) == 64);
static_assert(offsetof(EDITDATA_V1, swidth) == offsetof(EDITDATA, swidth));
static_assert(offsetof(EDITDATA_V1, memoryLimit) == offsetof(EDITDATA, memoryLimit));
static_assert(offsetof(EDITDATA_V1, hotSpotComboID) == offsetof(EDITDATA, hotSpotComboID));
static_assert(offsetof(EDITDATA, captureBackend) == sizeof(EDITDATA_V1));
static_assert(offsetof(EDITDATA, reserved) == sizeof(EDITDATA_V1) + sizeof(CaptureBackend));
#if INTPTR_MAX == INT32_MAX
static_assert(sizeof(EDITDATA_V1) == 48 && sizeof(EDITDATA) == 116);
#endif

int main() {
    EDITDATA_V1 old{};
    old.swidth = 123; old.sheight = 456;
    old.isLib = true; old.bLoadCallback = true; old.bLoadKeepAngle = true;
    old.HWA = true; old.stretchQuality = false;
    old.memoryLimit = 1024; old.sizeLimit = 321; old.autoClean = true;
    old.hotSpotComboID = 8;
    EDITDATA current{};
    current.captureBackend = CaptureBackend::WindowsGraphicsCapture;
    std::fill_n(current.reserved, sizeof(current.reserved), std::uint8_t{0xA5});
    UpgradeNeoPicEditData(&old, &current);
    assert(current.swidth == 123 && current.sheight == 456 && current.isLib);
    assert(current.bLoadCallback && current.bLoadKeepAngle && current.HWA && !current.stretchQuality);
    assert(current.memoryLimit == 1024 && current.sizeLimit == 321 && current.autoClean && current.hotSpotComboID == 8);
    assert(current.captureBackend == CaptureBackend::Dc && current.eHeader.extMaxSize == sizeof(EDITDATA));
    for (auto byte : current.reserved) assert(byte == 0);
    for (std::uint32_t value = 0; value < 3; ++value)
        assert(static_cast<std::uint32_t>(NormalizeCaptureBackend(value)) == value);
    assert(NormalizeCaptureBackend(3) == CaptureBackend::Dc);
    assert(NormalizeCaptureBackend((std::numeric_limits<std::uint32_t>::max)()) == CaptureBackend::Dc);
    std::cout << "PASS: V1 payload preservation, DC upgrade default, 64-byte reserve and backend validation\n";
}
