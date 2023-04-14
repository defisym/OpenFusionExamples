#pragma once

#include "JsonInterface.h"
#include "Encryption.h"

using JsonData = decltype(JsonInterface().Get());
constexpr auto Animation_JsonParseError = -1;

struct AnimationInfo {
    std::wstring name;
    bool reset = false;
    int speed = 50;
    bool loop = true;
    int repeat = 1;
    int backTo = 1;

    explicit AnimationInfo(JsonData& data) {
        if(!data.is_object()) {
            throw Animation_JsonParseError;
        }

        // general
        JsonInterface::GetData(data, "reset", reset);
        JsonInterface::GetData(data, "speed", speed);
        JsonInterface::GetData(data, "loop", loop);
        JsonInterface::GetData(data, "repeat", repeat);
        JsonInterface::GetData(data, "backTo", backTo);

        // required
        if (!JsonInterface::GetData(data, "name", name)) {
            throw Animation_JsonParseError;
        }
    }
};

struct FrameData {
    std::wstring file;

    struct Interpolation {
        std::wstring type = L"liner";
        int interval = 1;

        Interpolation() = default;

        explicit Interpolation(JsonData& data) {
            if (!data.is_object()) {
                throw Animation_JsonParseError;
            }

            JsonInterface::GetData(data, "type", type);
            JsonInterface::GetData(data, "interval", interval);
        }
    };

    Interpolation* pInterpolation = nullptr;

    int angle = 0;
    int alpha = 0;

    struct RGBCoef {
        int r = 255;
        int g = 255;
        int b = 255;

        RGBCoef() = default;

        explicit RGBCoef(JsonData& data) {
            if (!data.is_object()) {
                throw Animation_JsonParseError;
            }

            JsonInterface::GetData(data, "r", r);
            JsonInterface::GetData(data, "g", g);
            JsonInterface::GetData(data, "b", b);
        }
    };

    RGBCoef* pRGBCoef=nullptr;

    struct Scale {
        double x = 0;
        double y = 0;

        Scale() = default;

        explicit Scale(JsonData& data) {
            if (!data.is_object()) {
                throw Animation_JsonParseError;
            }

            JsonInterface::GetData(data, "x", x);
            JsonInterface::GetData(data, "y", y);
        }
    };

    Scale* pScale = nullptr;

    struct HotSpot {
        int x = 0;
        int y = 0;
        std::wstring type;

        HotSpot() = default;

        explicit HotSpot(JsonData& data) {
            if (!data.is_object()) {
                throw Animation_JsonParseError;
            }

            JsonInterface::GetData(data, "x", x);
            JsonInterface::GetData(data, "y", y);
            JsonInterface::GetData(data, "type", type);
        }
    };

    HotSpot* pHotSpot = nullptr;

    explicit FrameData(JsonData& data) {
        if (!data.is_object()) {
            throw Animation_JsonParseError;
        }

        JsonInterface::GetData(data, "interpolation", pInterpolation);
        JsonInterface::GetData(data, "rgbCoef", pRGBCoef);
        JsonInterface::GetData(data, "scale", pScale);
        JsonInterface::GetData(data, "hotSpot", pHotSpot);

        JsonInterface::GetData(data, "angle", angle);
        JsonInterface::GetData(data, "alpha", alpha);

        // required
        if (!JsonInterface::GetData(data, "file", file)) {
            throw Animation_JsonParseError;
        }       
    }
    ~FrameData() {
        delete pInterpolation;
        delete pRGBCoef;
        delete pScale;
        delete pHotSpot;
    }
};

class Animation {
private:
    AnimationInfo* pAnimationInfo = nullptr;
    std::vector<FrameData*> pFrameDatas;

public:
    explicit Animation(JsonData& data) {
        JsonInterface::GetData(data, "info", pAnimationInfo);

        if(!JsonInterface::Contains(data,"frames")) {
            throw Animation_JsonParseError;
        }

        for(auto& it: JsonInterface::GetData(data, "frames")) {
            FrameData* pFrameData = nullptr;
            pFrameData = new FrameData(it);

            pFrameDatas.emplace_back(pFrameData);
        }
    }
    ~Animation() {
        delete pAnimationInfo;

        for(const auto& it : pFrameDatas) {
            delete it;
        }
    }

};
