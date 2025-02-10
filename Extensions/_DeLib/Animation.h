#pragma once

#include <functional>

#include "EasingFunctions.h"
#include "JsonInterface.h"

constexpr auto Animation_JsonParseError = "Json Parse Error";
constexpr auto MakeAnimationJsonParseErrorString(const char* pStr) {
    return MakeJsonParseErrorString(Animation_JsonParseError, pStr);
}

constexpr auto Animation_MinSpeed = 0;
constexpr auto Animation_MaxSpeed = 100;

constexpr auto Animation_MinAngle = 0;
constexpr auto Animation_MaxAngle = 360;

constexpr auto Animation_MinAlpha = 0;
constexpr auto Animation_MaxAlpha = 255;

constexpr auto Animation_MinColor = 0;
constexpr auto Animation_MaxColor = 255;
constexpr auto Animation_DefaultCoef = 16777215;

struct AnimationInfo: public JsonObject {
    std::wstring name;
    bool restore = true;
    bool updateCur = false;
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
            throw std::exception(MakeAnimationJsonParseErrorString("Invalid Animation Info").c_str());
        }

        // general
        JsonInterface::GetData(data, "restore", restore);
        JsonInterface::GetData(data, "updateCur", updateCur);
        JsonInterface::GetData(data, "reset", reset);
        JsonInterface::GetData(data, "speed", speed);
        JsonInterface::GetData(data, "loop", loop);
        JsonInterface::GetData(data, "repeat", repeat);
        JsonInterface::GetData(data, "backTo", backTo);

        // required
        if (!JsonInterface::GetData(data, "name", name)) {
            throw std::exception(MakeAnimationJsonParseErrorString("Invalid Animation Name").c_str());
        }
    }
};

struct AnimationFrameData : public JsonObject {
    int frameID = 0;

    std::wstring file;

    float angle = 0;
    int alpha = 0;

    struct Interpolation : public JsonObject {
        int interval = 1;

    	std::wstring type = L"easeIn";
    	std::wstring func_a = L"liner";
    	std::wstring func_b = L"liner";

        int type_ID = 0;
        int func_a_ID = 0;
        int func_b_ID =0;

    	double overshoot = 1.5;
    	double amplitude = 1.0;
    	double period = 0.4;

        Interpolation() = default;
        explicit Interpolation(const JsonData& data) {
	        Interpolation::Update(data);
        }

        inline void Update(const JsonData& data) override {
            if (!data.is_object()) {
                throw std::exception(MakeAnimationJsonParseErrorString("Invalid Interpolation").c_str());
            }

            JsonInterface::GetData(data, "interval", interval);

        	JsonInterface::GetData(data, "type", type);
        	JsonInterface::GetData(data, "func_a", func_a);
        	JsonInterface::GetData(data, "func_b", func_b);

        	JsonInterface::GetData(data, "overshoot", overshoot);
        	JsonInterface::GetData(data, "amplitude", amplitude);
        	JsonInterface::GetData(data, "period", period);

            type_ID = Easing::StrToEasingMode(type.c_str());
            func_a_ID = Easing::StrToFunctionID(func_a.c_str());
            func_b_ID = Easing::StrToFunctionID(func_b.c_str());
        }
    };

    Interpolation* pInterpolation = nullptr;

    struct RGBCoef : public JsonObject {
        int r = 255;
        int g = 255;
        int b = 255;

        DWORD rgbCoef = Animation_DefaultCoef;

        bool operator==(const RGBCoef& data) const {
            return this->rgbCoef == data.rgbCoef
                && this->r == data.r
                && this->g == data.g
                && this->b == data.b;
        }

        RGBCoef() = default;

        explicit RGBCoef(const JsonData& data) {
	        RGBCoef::Update(data);
        }

        explicit RGBCoef(DWORD rgbCoef) {
            const auto& [r, g, b] = EffectUtilities::GetRGB(rgbCoef);

            this->rgbCoef = rgbCoef;
            this->r = r;
            this->g = g;
            this->b = b;
        }

        explicit RGBCoef(UCHAR r, UCHAR g, UCHAR b) {
            this->rgbCoef = EffectUtilities::GetRGBCoef(r, g, b);
            this->r = r;
            this->g = g;
            this->b = b;
        }

        inline void Update(const JsonData& data) override {
            if (!data.is_object()) {
                throw std::exception(MakeAnimationJsonParseErrorString("Invalid RGB Coef").c_str());
            }

            JsonInterface::GetData(data, "r", r);
            JsonInterface::GetData(data, "g", g);
            JsonInterface::GetData(data, "b", b);

            r = Range(r, Animation_MinColor, Animation_MaxColor);
            g = Range(g, Animation_MinColor, Animation_MaxColor);
            b = Range(b, Animation_MinColor, Animation_MaxColor);

            rgbCoef = EffectUtilities::GetRGBCoef(static_cast<UCHAR>(r),
                static_cast<UCHAR>(g),
                static_cast<UCHAR>(b));
        }

        inline void Interpolation(double step, const RGBCoef* pPrevious, const RGBCoef* pNext) {
            r = static_cast<int>(static_cast<double>(pPrevious->r) + step * (pNext->r - pPrevious->r));
            g = static_cast<int>(static_cast<double>(pPrevious->g) + step * (pNext->g - pPrevious->g));
            b = static_cast<int>(static_cast<double>(pPrevious->b) + step * (pNext->b - pPrevious->b));

            rgbCoef = EffectUtilities::GetRGBCoef(static_cast<UCHAR>(r),
               static_cast<UCHAR>(g),
               static_cast<UCHAR>(b));
        }

        inline DWORD MulRGBCoef(DWORD dwRGB) const {
            const auto&[R,G,B] = EffectUtilities::GetRGB(dwRGB);

            auto GetNew = [] (UCHAR o, UCHAR n) {
                return static_cast<UCHAR>(o * (n / static_cast<double>(Animation_MaxColor)));
            };

            return EffectUtilities::GetRGBCoef(GetNew(static_cast<UCHAR>(r), R),
                GetNew(static_cast<UCHAR>(g), G),
                GetNew(static_cast<UCHAR>(b), B));
        }
    };

    RGBCoef* pRGBCoef = nullptr;

    struct Scale : public JsonObject {
        double x = 1.0;
        double y = 1.0;

        bool operator==(const Scale& data) const {
            constexpr double eps = 1e-6;

            return fabs(this->x - data.x) < eps
                && fabs(this->y - data.y) < eps;            
        }

        Scale() = default;

        Scale(double x, double y) {
            this->x = x;
            this->y = y;
        }

        explicit Scale(const JsonData& data) {
	        Scale::Update(data);
        }

        inline void Update(const JsonData& data) override {
            if (!data.is_object()) {
                throw std::exception(MakeAnimationJsonParseErrorString("Invalid Scale").c_str());
            }

            JsonInterface::GetData(data, "x", x);
            JsonInterface::GetData(data, "y", y);
        }

    	inline void Interpolation(double step, const Scale* pPrevious, const Scale* pNext) {
        	x = static_cast<double>(static_cast<double>(pPrevious->x) + step * (pNext->x- pPrevious->x));
            y = static_cast<double>(static_cast<double>(pPrevious->y) + step * (pNext->y - pPrevious->y));
        }
    };

    Scale* pScale = nullptr;

    struct HotSpot : public JsonObject {
        double x = 0;
        double y = 0;
        std::wstring type = L"MM";
        HotSpotPos typeID = HotSpotPos::MM;

        HotSpot() = default;

        HotSpot(double x, double y, HotSpotPos typeID) {
            this->x = x;
            this->y = y;
            this->typeID = typeID;
        }

        explicit HotSpot(const JsonData& data) {
	        HotSpot::Update(data);
        }

        inline void Update(const JsonData& data) override {
            if (!data.is_object()) {
                throw std::exception(MakeAnimationJsonParseErrorString("Invalid Hotspot").c_str());
            }

            JsonInterface::GetData(data, "x", x);
            JsonInterface::GetData(data, "y", y);
            JsonInterface::GetData(data, "type", type);

            typeID = StrToHotSpotPos(type.c_str());
        }

        inline void Interpolation(double step, const HotSpot* pPrevious, const HotSpot* pNext) {
            x = pPrevious->x + step * (pNext->x - pPrevious->x);
            y = pPrevious->y + step * (pNext->y - pPrevious->y);
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

    AnimationFrameData& operator=(const AnimationFrameData& data) {
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

    AnimationFrameData(const AnimationFrameData& data) {
        *this = data;
    }
    explicit AnimationFrameData(const JsonData& data, const AnimationFrameData* pPrevious) {
        auto reset = false;
        JsonInterface::GetData(data, "reset", reset);

        auto resetExceptFile = false;
        JsonInterface::GetData(data, "resetExceptFile", resetExceptFile);

        do {
            if(pPrevious == nullptr) {
	            break;
            }

            if(reset) {
	            break;
            }

            if (resetExceptFile) {
                this->file = pPrevious->file;

                break;
            }

            *this = *pPrevious;
        } while (false);

        AnimationFrameData::Update(data);
    }
    ~AnimationFrameData() override {
        delete pInterpolation;
        delete pRGBCoef;
        delete pScale;
        delete pHotSpot;
    }

    inline bool FrameValid() const {
        return !file.empty();
    }

    inline void Update(const JsonData& data) override {
        if (!data.is_object()) {
            throw std::exception(MakeAnimationJsonParseErrorString("Invalid Frame Data").c_str());
        }

        JsonInterface::GetData(data, "file", file);

        JsonInterface::GetData(data, "angle", angle);
        JsonInterface::GetData(data, "alpha", alpha);

        JsonInterface::GetData(data, "interpolation", pInterpolation);
        JsonInterface::GetData(data, "rgbCoef", pRGBCoef);
        JsonInterface::GetData(data, "scale", pScale);
        JsonInterface::GetData(data, "hotSpot", pHotSpot);

        angle = Range(angle, 1.0f * Animation_MinAngle, 1.0f * Animation_MaxAngle);
        alpha = Range(alpha, Animation_MinAlpha, Animation_MaxAlpha);
    }

    inline void Interpolation(double step, const AnimationFrameData* pPrevious, const AnimationFrameData* pNext) {
        angle = static_cast<float>(static_cast<double>(pPrevious->angle) + step * (pNext->angle - pPrevious->angle));
        alpha = static_cast<int>(static_cast<double>(pPrevious->alpha) + step * (pNext->alpha - pPrevious->alpha));

        pRGBCoef->Interpolation(step, pPrevious->pRGBCoef, pNext->pRGBCoef);
        pScale->Interpolation(step, pPrevious->pScale, pNext->pScale);
        pHotSpot->Interpolation(step, pPrevious->pHotSpot, pNext->pHotSpot);
    }
};

class Animation {
private:
    AnimationInfo* pAnimationInfo = nullptr;

    AnimationFrameData* pCurFrameData = nullptr;
    std::vector<AnimationFrameData*> pFrameDatas;

    int repeatCount = 0;
    int maxFrame = 0;

    int curFrame = 0;
    int curIndex = 0;

    AnimationFrameData* pPrevious = nullptr;
    AnimationFrameData* pNext = nullptr;

    inline AnimationFrameData* GetNext() {
        if (static_cast<size_t>(curIndex + 1) >= pFrameDatas.size()) {
            if (pAnimationInfo->loop || repeatCount > 0) {
                repeatCount = (std::max)(0, repeatCount - 1);

                return pFrameDatas[pAnimationInfo->backTo];
            }
            else {
                return nullptr;
            }
        }
        else {
            return pFrameDatas[curIndex + 1];
        }
    }

    inline void UpdateToCurIndex() {
        pPrevious = pFrameDatas[curIndex];
        pNext = GetNext();

        *pCurFrameData = *pPrevious;
    }

public:
    explicit Animation(const JsonData& data) {
        // load info
        JsonInterface::GetData(data, "info", pAnimationInfo);

        repeatCount = pAnimationInfo->repeat;

    	// load frames
        if(!JsonInterface::Contains(data,"frames")) {
            throw std::exception(MakeAnimationJsonParseErrorString("Animation Doesn't Contain Frame Data").c_str());
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

        	AnimationFrameData* pFrameData = nullptr;
            pFrameData = new AnimationFrameData(it, GetPrevious());

            if (!pAnimationInfo->updateCur && !pFrameData->FrameValid()) {
                throw std::exception(MakeAnimationJsonParseErrorString(std::format("Invalid File at Frame {}", pFrameDatas.size() + 1).c_str()).c_str());
            }

            pFrameData->frameID = frameID;
            frameID += pFrameData->pInterpolation->interval;

            pFrameDatas.emplace_back(pFrameData);
        }

        // validate
        if(pFrameDatas.empty()) {
            throw std::exception(MakeAnimationJsonParseErrorString("Animation Doesn't Contain Frame Data").c_str());
        }

        pCurFrameData = new AnimationFrameData(*pFrameDatas.front());

        pAnimationInfo->speed = (std::max)(pAnimationInfo->speed, Animation_MinSpeed);
        pAnimationInfo->backTo = Range(pAnimationInfo->backTo, 0, static_cast<int>(pFrameDatas.size() - 1));
        pAnimationInfo->repeat = (std::max)(0, pAnimationInfo->repeat);

        // init
        pPrevious = pFrameDatas[0];
        pNext = GetNext();

        const auto pLastFrame = pFrameDatas.back();
        maxFrame = pLastFrame->frameID + pLastFrame->pInterpolation->interval;
    }
    ~Animation() {
        delete pAnimationInfo;
        delete pCurFrameData;

        for(const auto& it : pFrameDatas) {
            delete it;
        }
    }
        
    inline void UpdateFrame(const std::function<void()>& nextCallback = nullptr,
        const std::function<void()>& finishCallback = nullptr) {
        if (curFrame >= pCurFrameData->frameID + pCurFrameData->pInterpolation->interval) {
            if (pNext != nullptr) {
                curIndex = static_cast<int>(static_cast<size_t>(curIndex + 1) % pFrameDatas.size());
                *pCurFrameData = *pNext;
                pPrevious = pNext;
                pNext = GetNext();

                if (nextCallback != nullptr) {
                    nextCallback();
                }
            }
        }

        if (pNext != nullptr) {
            const auto pCurIP = pCurFrameData->pInterpolation;
            const auto step = GetFrameStep();

            const auto easeStep = Easing::calculateEasingValue(pCurIP->type_ID,
                pCurIP->func_a_ID, pCurIP->func_b_ID,
                step,
                Easing::EaseVars{ pCurIP->overshoot,
                    pCurIP->amplitude,
                    pCurIP->period });

            pCurFrameData->Interpolation(easeStep, pPrevious, pNext);

            curFrame = (curFrame + 1) % (maxFrame + 1);
        }else {
            if (finishCallback != nullptr) {
                finishCallback();
            }
        }
    }

    inline bool AnimationFinished() const {
        return pNext == nullptr;
    }

    inline int GetFrameID() const {
        return curFrame;
    }

    inline void SetFrameID(int id) {
        curFrame = Range(id, 0, maxFrame);

        for (size_t index = 0; index < pFrameDatas.size(); index++) {
	        if(pFrameDatas[index]->frameID > curFrame) {
                curIndex = static_cast<int>(Range(index - 1, 0u, pFrameDatas.size() - 1));
                UpdateToCurIndex();
                UpdateFrame();

                break;
	        }
        }
    }

    inline int GetFrameIndex() const {
        return curIndex;
    }

    inline void SetFrameIndex(int index) {
        curIndex = Range(index, 0, static_cast<int>(pFrameDatas.size()) - 1);

        UpdateToCurIndex();
        curFrame = pCurFrameData->frameID;
    }

    inline double GetFrameStep() const {
        // when into next loop, to be update by mod
        const auto progress = curFrame >= maxFrame ? curFrame - maxFrame : curFrame;
        const auto step = static_cast<double>(progress - pCurFrameData->frameID)
            / (pCurFrameData->pInterpolation->interval);

        return step;
    }

    inline void SetFrameStep(double step) {
        step = Range(step, 0.0, 1.0);

    	const auto pCurIP = pCurFrameData->pInterpolation;
        curFrame = static_cast<int>(step * (pCurIP->interval) + pCurFrameData->frameID);         
        curFrame = Range(curFrame, 0, maxFrame);

        UpdateFrame();
    }

	inline const AnimationFrameData* GetCurrentFrame() const {
        return pCurFrameData;
    }

    inline const AnimationFrameData* GetPreviousFrame() const {
        return pPrevious;
    }

    inline const AnimationFrameData* GetNextFrame() const {
        return pNext;
    }

    inline const AnimationInfo* GetAnimationInfo() const {
        return pAnimationInfo;
    }
};
