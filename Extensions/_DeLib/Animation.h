#pragma once

#include "JsonInterface.h"
#include "Encryption.h"

constexpr auto Animation_JsonParseError = -1;

struct AnimationInfo: public JsonObject {
    std::wstring name;
    bool reset = false;
    int speed = 50;
    bool loop = true;
    int repeat = 0;
    int backTo = 0;

    explicit AnimationInfo(const JsonData& data) {
	    AnimationInfo::Update(data);
    }

    inline void Update(const JsonData& data) override {
        if (!data.is_object()) {
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

struct FrameData : public JsonObject {
    int frameID = 0;

    std::wstring file;

    int angle = 0;
    int alpha = 0;

    struct Interpolation : public JsonObject {
        std::wstring type = L"liner";
        int interval = 1;

        Interpolation() = default;
        explicit Interpolation(const JsonData& data) {
	        Interpolation::Update(data);
        }

        inline void Update(const JsonData& data) override {
            if (!data.is_object()) {
                throw Animation_JsonParseError;
            }

            JsonInterface::GetData(data, "type", type);
            JsonInterface::GetData(data, "interval", interval);
        }
    };

    Interpolation* pInterpolation = nullptr;

    struct RGBCoef : public JsonObject {
        int r = 255;
        int g = 255;
        int b = 255;

        RGBCoef() = default;

        explicit RGBCoef(const JsonData& data) {
	        RGBCoef::Update(data);
        }

        inline void Update(const JsonData& data) override {
            if (!data.is_object()) {
                throw Animation_JsonParseError;
            }

            JsonInterface::GetData(data, "r", r);
            JsonInterface::GetData(data, "g", g);
            JsonInterface::GetData(data, "b", b);
        }

        inline void Interpolation(double step, const RGBCoef* pPrevious, const RGBCoef* pNext) {
            r = static_cast<int>(static_cast<double>(pPrevious->r) + step * (pNext->r - pPrevious->r));
            g = static_cast<int>(static_cast<double>(pPrevious->g) + step * (pNext->g - pPrevious->g));
            b = static_cast<int>(static_cast<double>(pPrevious->b) + step * (pNext->b - pPrevious->b));
        }
    };

    RGBCoef* pRGBCoef=nullptr;

    struct Scale : public JsonObject {
        double x = 1.0;
        double y = 1.0;
        
        Scale() = default;

        explicit Scale(const JsonData& data) {
	        Scale::Update(data);
        }

        inline void Update(const JsonData& data) override {
            if (!data.is_object()) {
                throw Animation_JsonParseError;
            }

            JsonInterface::GetData(data, "x", x);
            JsonInterface::GetData(data, "y", y);
        }

    	inline void Interpolation(double step, const Scale* pPrevious, const Scale* pNext) {
        	x = static_cast<int>(static_cast<double>(pPrevious->x) + step * (pNext->x- pPrevious->x));
            y = static_cast<int>(static_cast<double>(pPrevious->y) + step * (pNext->y - pPrevious->y));            
        }
    };

    Scale* pScale = nullptr;

    struct HotSpot : public JsonObject {
        int x = 0;
        int y = 0;
        std::wstring type;

        HotSpot() = default;

        explicit HotSpot(const JsonData& data) {
	        HotSpot::Update(data);
        }

        inline void Update(const JsonData& data) override {
            if (!data.is_object()) {
                throw Animation_JsonParseError;
            }

            JsonInterface::GetData(data, "x", x);
            JsonInterface::GetData(data, "y", y);
            JsonInterface::GetData(data, "type", type);
        }

        inline void Interpolation(double step, const HotSpot* pPrevious, const HotSpot* pNext) {
            x = static_cast<int>(static_cast<double>(pPrevious->x) + step * (pNext->x - pPrevious->x));
            y = static_cast<int>(static_cast<double>(pPrevious->y) + step * (pNext->y - pPrevious->y));
        }
    };

    HotSpot* pHotSpot = nullptr;

    template<typename T>
    static inline void InitAndCopy(T*& pDst, const T* pSrc) {
        if (pDst == nullptr) {
            pDst = new T;
        }

        if (pSrc != nullptr) {
            *pDst = *pSrc;
        }
    }

    FrameData& operator=(const FrameData& data) {
        if (this != &data) {
            this->frameID = data.frameID;
            this->file = data.file;

            this->angle = data.angle;
            this->alpha = data.alpha;

            InitAndCopy(this->pInterpolation, data.pInterpolation);
            InitAndCopy(this->pRGBCoef, data.pRGBCoef);
            InitAndCopy(this->pScale, data.pScale);
            InitAndCopy(this->pHotSpot, data.pHotSpot);
        }

        return *this;
    }

    FrameData(const FrameData& data) {
        *this = data;
    }
    explicit FrameData(const JsonData& data, const FrameData* pPrevious) {
        if (pPrevious != nullptr) {
            *this = *pPrevious;
        }

        FrameData::Update(data);
    }
    ~FrameData() override {
        delete pInterpolation;
        delete pRGBCoef;
        delete pScale;
        delete pHotSpot;
    }

    inline void Update(const JsonData& data) override {
        if (!data.is_object()) {
            throw Animation_JsonParseError;
        }

        JsonInterface::GetData(data, "file", file);

        JsonInterface::GetData(data, "angle", angle);
        JsonInterface::GetData(data, "alpha", alpha);

        JsonInterface::GetData(data, "interpolation", pInterpolation);
        JsonInterface::GetData(data, "rgbCoef", pRGBCoef);
        JsonInterface::GetData(data, "scale", pScale);
        JsonInterface::GetData(data, "hotSpot", pHotSpot);
    }

    inline void Interpolation(double step, const FrameData* pPrevious, const FrameData* pNext) {
        angle = static_cast<int>(static_cast<double>(pPrevious->angle) + step * (pNext->angle - pPrevious->angle));
        alpha = static_cast<int>(static_cast<double>(pPrevious->alpha) + step * (pNext->alpha - pPrevious->alpha));

        pRGBCoef->Interpolation(step, pPrevious->pRGBCoef, pNext->pRGBCoef);
        pScale->Interpolation(step, pPrevious->pScale, pNext->pScale);
        pHotSpot->Interpolation(step, pPrevious->pHotSpot, pNext->pHotSpot);
    }
};

class Animation {
private:
    AnimationInfo* pAnimationInfo = nullptr;

    FrameData* pCurFrameData = nullptr;
    std::vector<FrameData*> pFrameDatas;

    int curFrame = 0;
    int curIndex = 0;

    FrameData* pPrevious = nullptr;
    FrameData* pNext = nullptr;

    inline FrameData* GetNext() const {
        return static_cast<size_t>(curIndex + 1) >= pFrameDatas.size()
            ? pAnimationInfo->loop
            ? pFrameDatas[0]
            : nullptr
            : pFrameDatas[curIndex + 1];
    }

public:
    explicit Animation(const JsonData& data) {
        JsonInterface::GetData(data, "info", pAnimationInfo);

        if(!JsonInterface::Contains(data,"frames")) {
            throw Animation_JsonParseError;
        }

        int frameID = 0;

        for(auto& it: JsonInterface::GetData(data, "frames")) {
            auto GetPrevious = [&] () {
                const auto previous =  pFrameDatas.empty()
                    ? nullptr
                    : pFrameDatas.back();

                return pAnimationInfo->reset
                    ? nullptr
                    : previous;
            };

        	FrameData* pFrameData = nullptr;
            pFrameData = new FrameData(it, GetPrevious());

            pFrameData->frameID = frameID;
            frameID += pFrameData->pInterpolation->interval;

            pFrameDatas.emplace_back(pFrameData);
        }

        if(pFrameDatas.empty()) {
            throw Animation_JsonParseError;
        }

        pCurFrameData = new FrameData(*pFrameDatas.front());

        if (pCurFrameData->file.empty()) {
            throw Animation_JsonParseError;
        }

        pPrevious = pFrameDatas[0];
        pNext = GetNext();
    }
    ~Animation() {
        delete pAnimationInfo;
        delete pCurFrameData;

        for(const auto& it : pFrameDatas) {
            delete it;
        }
    }

    inline void UpdateFrame() {
        // update
        //TODO curIndex -> mod
        if (curFrame > pCurFrameData->frameID + pCurFrameData->pInterpolation->interval) {



            do {
                if (static_cast<size_t>(curIndex) >= pFrameDatas.size()) {
                    if (pAnimationInfo->loop) {
                        curIndex = 0;
                    }

                	break;
                }

                curIndex++;
            } while (false);

            *pCurFrameData = *pFrameDatas[curIndex];

        	pPrevious = pFrameDatas[curIndex];
            pNext = GetNext();
        }

        if (pNext != nullptr) {
            const auto step = static_cast<double>(curFrame - pCurFrameData->frameID)
                / (pCurFrameData->pInterpolation->interval);

            pCurFrameData->Interpolation(step, pPrevious, pNext);
            curFrame++;
        }
    }
};
