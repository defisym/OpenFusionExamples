#pragma once
#include <cstdint>

// Stable property values, also used by the runtime factory and diagnostics.
enum class CaptureBackend : std::uint32_t {
    Dc = 0,
    Secondary = 1,
    WindowsGraphicsCapture = 2,
};

constexpr CaptureBackend NormalizeCaptureBackend(std::uint32_t value) {
    return value <= static_cast<std::uint32_t>(CaptureBackend::WindowsGraphicsCapture)
        ? static_cast<CaptureBackend>(value) : CaptureBackend::Dc;
}
