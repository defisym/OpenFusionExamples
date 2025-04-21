#pragma once

#include <functional>

// pData, stride, height
using FrameDataCallBack = std::function<void(const unsigned char*, const int, const int)>;
