#pragma once
#ifndef _WIN32
#include "CopyAdapter.h"
struct CopyAdapterAndroid final : CopyAdapter {
    AVHWDeviceType type;
    explicit CopyAdapterAndroid(AVHWDeviceType value) : type(value) {}
    bool CopyTexture(const AndroidCopyTarget& target, const unsigned char* data) override;
};
#endif
