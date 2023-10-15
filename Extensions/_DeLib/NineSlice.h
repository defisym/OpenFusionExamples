#pragma once

#include <functional>

#include "JsonInterface.h"
#include "Encryption.h"

constexpr auto NineSlice_JsonParseError = "Json Parse Error";
constexpr auto MakeNineSliceJsonParseErrorString(const char* pStr) {
    return MakeJsonParseErrorString(NineSlice_JsonParseError, pStr);
}

constexpr auto NineSlice_MixFrame = 9;
constexpr auto NineSlice_MaxFrame = 9;

struct NineSliceSettings : public JsonObject {
    bool stretch = false;

    explicit NineSliceSettings(const JsonData& data) {
        NineSliceSettings::Update(data);
    }

    inline void Update(const JsonData& data) override {
        if (!data.is_object()) {
            throw std::exception(MakeNineSliceJsonParseErrorString("Invalid NineSlice Settings").c_str());
        }

        // general
        JsonInterface::GetData(data, "stretch", stretch);

        // required
    }
};

struct NineSliceFrameData : public JsonObject {
    std::wstring files[9];

    explicit NineSliceFrameData(const JsonData& data) {
        NineSliceFrameData::Update(data);
    }

    inline void Update(const JsonData& data) override {
        if (!data.is_array()) {
            throw std::exception(MakeNineSliceJsonParseErrorString("Invalid NineSlice Frame Data").c_str());
        }

        size_t index = 0;

        for (auto& it : data) {
            if (index > NineSlice_MaxFrame - 1) {
                break;
            }

            // general

            // required
            if (!JsonInterface::GetData(it, "file", files[index])) {
                throw std::exception(MakeNineSliceJsonParseErrorString("Invalid file Name").c_str());
            }

            ++index;
        }

        if (index < NineSlice_MaxFrame - 1) {
            throw std::exception(MakeNineSliceJsonParseErrorString("Frame not enough").c_str());
        }
    }
};

class NineSlice {
private:
    NineSliceSettings* pNineSliceSettings = nullptr;
    NineSliceFrameData* pNineSliceFrameData = nullptr;

public:
    explicit NineSlice(const JsonData& data) {
        // load info
        JsonInterface::GetData(data, "settings", pNineSliceSettings);

        // load frames
        if (!JsonInterface::Contains(data, "frames")) {
            throw std::exception(MakeNineSliceJsonParseErrorString("NineSlice Doesn't Contain Frame Data").c_str());
        }

        JsonInterface::GetData(data, "frames", pNineSliceFrameData);
    }
    ~NineSlice() {
        delete pNineSliceSettings;
        delete pNineSliceFrameData;
    }

    inline const NineSliceSettings* GetNineSliceSettings() const {
        return pNineSliceSettings;
    }
    inline const NineSliceFrameData* GetNineSliceFrameData() const {
        return pNineSliceFrameData;
    }
};