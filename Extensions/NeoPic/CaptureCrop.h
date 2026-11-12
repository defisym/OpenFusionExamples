#pragma once
#include <cstdint>

namespace neopic_capture {
struct Crop { std::uint32_t left, top, right, bottom; };

// Map physical client edges into the WGC content size. Keep this independent
// of Win32 so resize/DPI/negative-monitor-coordinate cases can be tested.
inline bool ClientCrop(std::int64_t windowLeft, std::int64_t windowTop,
    std::int64_t windowWidth, std::int64_t windowHeight,
    std::int64_t clientLeft, std::int64_t clientTop,
    std::int64_t clientWidth, std::int64_t clientHeight,
    std::int32_t contentWidth, std::int32_t contentHeight, Crop& result) {
    if (windowWidth <= 0 || windowHeight <= 0 || clientWidth <= 0 || clientHeight <= 0
        || contentWidth <= 0 || contentHeight <= 0) return false;
    const auto x = clientLeft - windowLeft, y = clientTop - windowTop;
    if (x < 0 || y < 0 || x > windowWidth || y > windowHeight
        || clientWidth > windowWidth - x || clientHeight > windowHeight - y) return false;
    auto scale = [](std::int64_t value, std::int64_t size, std::int64_t extent) {
        return static_cast<std::uint32_t>((value * size + extent / 2) / extent);
    };
    const Crop candidate{scale(x, contentWidth, windowWidth), scale(y, contentHeight, windowHeight),
        scale(x + clientWidth, contentWidth, windowWidth), scale(y + clientHeight, contentHeight, windowHeight)};
    if (candidate.right <= candidate.left || candidate.bottom <= candidate.top) return false;
    result = candidate;
    return true;
}
}
