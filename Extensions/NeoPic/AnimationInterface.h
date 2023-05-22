#pragma once

#include "Animation.h"
#include "JsonInterface.h"

struct AnimationInterface {
	LPRDATA rdPtr = nullptr;

	LPRDATA pLib = nullptr;
	std::wstring basePath;
	std::wstring key;

	Animation* pA = nullptr;
	std::wstring fileName;

	bool bPaused = false;
	int speed = 100;

	double speedAccumulate = 0.0;

	AnimationInterface(const LPRDATA rdPtr) {
		this->rdPtr = rdPtr;
	}
	~AnimationInterface() {
		StopAnimation();
	}

	inline bool AnimationValid() const {
		return pA != nullptr;
	}

	inline void SetAnimationSource(const LPRDATA pLib, const std::wstring& basePath, const std::wstring& key) {
		this->pLib = pLib;
		this->basePath = basePath;
		this->key = key;

		if (this->basePath.back() != '\\') {
			this->basePath.push_back('\\');
		}
	}

	inline bool LoadAnimation(const LPCWSTR pFileName, const LPCTSTR pKey = L"") {
		JsonInterface JI;
		JI.SetComment(true);

		auto ret = true;		

		if (StrEmpty(pKey)) {
			ret = JI.Load(pFileName);
		}
		else {
			Encryption E;
			E.GenerateKey(pKey);
			E.DecryptFile(pFileName);

			ret = JI.Load(E.GetOutputStr());
		}

		if (!ret) {
#if !defined(RUN_ONLY)
			MSGBOX(L"Json file load error");
#endif

			return ret;
		}
		
		StopAnimation();

		try {
			pA = new Animation(JI.Get());
			fileName = pFileName;
		} catch (...) {
			ret = false;

#if !defined(RUN_ONLY)
			MSGBOX(L"Json file parse error");
#endif
		}

		return ret;
	}

	inline void StopAnimation() {
		fileName.clear();
		speedAccumulate = 0;

		delete pA;
		pA = nullptr;
	}

private:
	inline double GetCorrectedSpeed() const {
		const auto correctedSpeed = (this->speed * pA->GetAnimationInfo()->speed)
			/ static_cast<double>(Animation_MaxSpeed * Animation_MaxSpeed);

		return correctedSpeed;
	}

public:

#define UPDATECOEF

#ifdef UPDATECOEF
	struct ObjectCoef {
		int alpha = 0;
		FrameData::RGBCoef* pRGBCoef = nullptr;
		FrameData::Scale* pScale = nullptr;

		ObjectCoef() {
			pRGBCoef = new FrameData::RGBCoef();
			pScale = new FrameData::Scale();
		}
		~ObjectCoef() {
			delete pRGBCoef;
			delete pScale;
		}

		enum class CoefType {
			Alpha = 0,
			RGBCoef,
			Scale,
		};

		inline auto GetObjectCoef(LPRDATA rdPtr) const {
			const auto objectAlpha = EffectUtilities::GetAlpha(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam);
			const auto objectRGBCoef = EffectUtilities::GetRGBCoef(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam);
			const auto objectScale = FrameData::Scale((double)rdPtr->zoomScale.XScale, (double)rdPtr->zoomScale.YScale);

			return std::make_tuple(objectAlpha, objectRGBCoef, objectScale);
		}

		inline void Init(LPRDATA rdPtr) {			
			const auto& [objectAlpha, objectRGBCoef, objectScale] = GetObjectCoef(rdPtr);

			alpha = objectAlpha;
			pRGBCoef->rgbCoef = objectRGBCoef;
			(*pScale) = objectScale;
		}

		// update params that set by fusion built-in actions
		inline bool Update(LPRDATA rdPtr, const FrameData* pPrevFrame) {
			const auto& [objectAlpha, objectRGBCoef, objectScale] = GetObjectCoef(rdPtr);

			const bool bChanged = !(pPrevFrame->alpha == objectAlpha
				&& pPrevFrame->pRGBCoef->rgbCoef == objectRGBCoef);

			if (bChanged) {
				alpha = objectAlpha;
				pRGBCoef->rgbCoef = objectRGBCoef;
			}

			return bChanged;
		}

		// zoom is updated by object action so update it by calling this.
		inline void UpdateScale(const ZoomScale& zoomScale) const {
			*pScale = FrameData::Scale((double)zoomScale.XScale, (double)zoomScale.YScale);
		}
	};

	ObjectCoef objectCoef;
#endif

	inline void UpdateAnimation() {
		if (pA == nullptr) {
			return;
		}

		const auto pAI = pA->GetAnimationInfo();

		auto updateHotSpot = [&] (const AnimationInfo* pAnimationInfo, const FrameData* pFrameData) {
			auto updateBySurface = [&] (const LPSURFACE pSf) {
				if (pSf == nullptr) {
					return;
				}

				::UpdateHotSpot(pFrameData->pHotSpot->typeID,
					pSf->GetWidth(), pSf->GetHeight(),
					pFrameData->pHotSpot->x, pFrameData->pHotSpot->y);

				pFrameData->pHotSpot->typeID = HotSpotPos::CUSTOM;
			};

			if (pFrameData == nullptr) {
				return;
			}

			if (pFrameData->pHotSpot->typeID == HotSpotPos::CUSTOM) {
				return;
			}

			auto pSf = rdPtr->src;

			if (!pAnimationInfo->updateCur) {
				const auto it = LoadLib(rdPtr, pLib,
					(basePath + pFrameData->file).c_str(), key.c_str());

				if (it == pLib->pLib->end()) {
					return;
				}

				pSf = it->second.pSf;
			}

			updateBySurface(pSf);
		};

		// update current hotspot
		updateHotSpot(pAI, pA->GetPreviousFrame());
		updateHotSpot(pAI, pA->GetNextFrame());

		// update display but won't update interpolation
		do {
			if (bPaused) { break; }

			speedAccumulate += GetCorrectedSpeed();
			if (speedAccumulate < 1.0) { break; }
			speedAccumulate -= 1.0;

			pA->UpdateFrame(
			[&] () {
				// if read to next frame, update next hotspot
				// cur is updated previously
				updateHotSpot(pAI, pA->GetNextFrame());
			},
			[&] () {
				CallEvent(ONANIMATIONFINISHED)
			});
		} while (false);

		const auto pPrevFrame = pA->GetPreviousFrame();
		const auto pCurFrame = pA->GetCurrentFrame();

		if (!pAI->updateCur) {
			const auto curFile = basePath + pCurFrame->file;
			if (*rdPtr->FilePath != curFile) {
				LoadFromLib(rdPtr, reinterpret_cast<LPRO>(pLib), curFile.c_str(), key.c_str());
			}
		}

#ifdef UPDATECOEF

		// ------------
		// Coef
		// ------------

		if (pPrevFrame == pCurFrame) {
			objectCoef.Init(rdPtr);
		}

		objectCoef.Update(rdPtr, pPrevFrame);

		// object * frame
		const auto newAlpha = static_cast<UCHAR>(objectCoef.alpha * (255 - pCurFrame->alpha) / 255.0);
		const auto newRGBCoef = FrameData::RGBCoef(objectCoef.pRGBCoef->rgbCoef).MulRGBCoef(pCurFrame->pRGBCoef->rgbCoef);

		EffectUtilities::SetAlpha(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam, newAlpha);
		EffectUtilities::SetRGBCoef(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam, newRGBCoef);
		Zoom(rdPtr,
			static_cast<float>(pCurFrame->pScale->x * objectCoef.pScale->x),
			static_cast<float>(pCurFrame->pScale->y * objectCoef.pScale->y));

		// ------------
		// Overwrite
		// ------------

		Rotate(rdPtr, pCurFrame->angle);
		UpdateHotSpot(rdPtr, 
			// Update x & y by pos before calling this
			HotSpotPos::CUSTOM,
			pCurFrame->pHotSpot->x, pCurFrame->pHotSpot->y);

#else
		Rotate(rdPtr, pCurFrame->angle);
		EffectUtilities::SetAlpha(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam,
			static_cast<UCHAR>(pCurFrame->alpha));

		EffectUtilities::SetRGBCoef(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam,
			pCurFrame->pRGBCoef->rgbCoef);
		Zoom(rdPtr, static_cast<float>(pCurFrame->pScale->x), static_cast<float>(pCurFrame->pScale->y));
		UpdateHotSpot(rdPtr,
			// Update x & y by pos before calling this
		HotSpotPos::CUSTOM,
			pCurFrame->pHotSpot->x, pCurFrame->pHotSpot->y);
#endif
	}

	inline bool GetPauseState() const {
		return bPaused;
	}

	inline void PauseAnimation() {
		bPaused = true;
	}

	inline void ResumeAnimation() {
		bPaused = false;
	}

	inline int GetSpeed() const {
		return speed;
	}

	inline void SetSpeed(const int speed) {
		this->speed = Range(speed, Animation_MinSpeed, Animation_MaxSpeed);
	}
};
