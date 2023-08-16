#pragma once

#include "Animation.h"

#include "JsonInterface.h"
#include "JsonUtilities.h"

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

	AnimationInterface(const LPRDATA rdPtr) :objectCoef(rdPtr) {
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

		if (!LoadJsonFile(JI, pFileName, pKey)) {
#if !defined(RUN_ONLY)
			MSGBOX(L"Json file load error");
#endif

			return false;
		}
		
		StopAnimation();

		try {
			fileName = pFileName;
			pA = new Animation(JI.Get());
			UpdateHotSpot(pA->GetAnimationInfo(), pA->GetCurrentFrame());
		} catch (std::exception& e) {
			const auto pErr = e.what();

#if !defined(RUN_ONLY)
			MSGBOX(ConvertStrToWStr(pErr));
#endif

			return false;
		}

		return true;
	}

	inline void StopAnimation() {
		// restore
		if (pA != nullptr && pA->GetAnimationInfo()->restore) {
			objectCoef.Restore();
		}

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
		// Run header
		LPRDATA rdPtr = nullptr;

		// External: params updated by fusion built-in actions
		int alpha = 0;
		AnimationFrameData::RGBCoef* pRGBCoef = nullptr;

		UCHAR newAlpha = 0;
		DWORD newRGBCoef = Animation_DefaultCoef;

		// Internal: params updated by object actions
		float angle = 0;
		AnimationFrameData::Scale* pScale = nullptr;
		AnimationFrameData::HotSpot* pHotSpot = nullptr;		

		ObjectCoef(LPRDATA rdPtr) {
			this->rdPtr = rdPtr;
			this->pRGBCoef = new AnimationFrameData::RGBCoef();
			this->pScale = new AnimationFrameData::Scale();
			this->pHotSpot = new AnimationFrameData::HotSpot();
		}
		~ObjectCoef() {
			delete pRGBCoef;
			delete pScale;
			delete pHotSpot;
		}

		enum class CoefType {
			Alpha = 0,
			RGBCoef,
			Scale,
		};

		inline auto GetObjectCoef() const {
			const auto objectAlpha = EffectUtilities::GetAlpha(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam);
			const auto objectRGBCoef = EffectUtilities::GetRGBCoef(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam);
			//const auto objectScale = AnimationFrameData::Scale((double)rdPtr->zoomScale.XScale, (double)rdPtr->zoomScale.YScale);

			return std::make_tuple(objectAlpha, objectRGBCoef);
		}

		inline bool UpdateExternal() {
			const auto& [objectAlpha, objectRGBCoef] = GetObjectCoef();

			const bool bChanged = !(newAlpha == objectAlpha
				&& newRGBCoef == objectRGBCoef);

			if (bChanged) {
				alpha = objectAlpha;
				pRGBCoef->rgbCoef = objectRGBCoef;
			}

			return bChanged;
		}

		inline void UpdateInternal(const float angle) {
			this->angle = angle;
		}

		inline void UpdateInternal(const ZoomScale& zoomScale) const {
			*pScale = AnimationFrameData::Scale((double)zoomScale.XScale, (double)zoomScale.YScale);
		}

		inline void UpdateInternal(const int x, const int y, const HotSpotPos& hotSpotPos) const {
			*pHotSpot = AnimationFrameData::HotSpot(x, y, hotSpotPos);
		}

		inline void Restore() {
			// External
			UpdateExternal();

			EffectUtilities::SetAlpha(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam, static_cast<UCHAR>(alpha));
			EffectUtilities::SetRGBCoef(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam, pRGBCoef->rgbCoef);
			Zoom(rdPtr,
				static_cast<float>(pScale->x),
				static_cast<float>(pScale->y));

			// Internal
			Rotate(rdPtr, angle);
			::UpdateHotSpot(rdPtr,
				pHotSpot->typeID,
				static_cast<int>(pHotSpot->x), static_cast<int>(pHotSpot->y));
		}
	};

	ObjectCoef objectCoef;
#endif

private:
	inline void UpdateHotSpot(const AnimationInfo* pAnimationInfo, const AnimationFrameData* pFrameData) const {
		auto updateBySurface = [&] (const LPSURFACE pSf) {
			if (pSf == nullptr) {
				return;
			}
#define HOTSPOT_DELTA

#ifndef HOTSPOT_DELTA
			::UpdateHotSpot(pFrameData->pHotSpot->typeID,
				pSf->GetWidth(), pSf->GetHeight(),
				pFrameData->pHotSpot->x, pFrameData->pHotSpot->y);
#else
			int hX = 0, hY = 0;

			::UpdateHotSpot(pFrameData->pHotSpot->typeID,
				pSf->GetWidth(), pSf->GetHeight(),
				hX, hY);

			if (pFrameData->pHotSpot->typeID != HotSpotPos::CUSTOM) {
				//pFrameData->pHotSpot->x += hX;
				//pFrameData->pHotSpot->y += hY;

				auto updateDelta = [&] (const int size, const int hotSpot, double& delta) {
					if (abs(delta) < 1.0) {
						delta += size * delta;
					}

					delta += hotSpot;
				};

				updateDelta(pSf->GetWidth(), hX, pFrameData->pHotSpot->x);
				updateDelta(pSf->GetHeight(), hY, pFrameData->pHotSpot->y);
			}
#endif

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
	}

public:
	inline void UpdateAnimation() {
		if (pA == nullptr || pLib == nullptr) {
			return;
		}

		const auto pAI = pA->GetAnimationInfo();
		
		// update current hotspot
		UpdateHotSpot(pAI, pA->GetPreviousFrame());
		UpdateHotSpot(pAI, pA->GetNextFrame());

		// update display but won't update interpolation
		do {
			if (bPaused) { break; }

			speedAccumulate += GetCorrectedSpeed();

			while (speedAccumulate >= 1.0) {
				speedAccumulate -= 1.0;

				pA->UpdateFrame(
				[&] () {
						// if read to next frame, update next hotspot
						// cur is updated previously
						UpdateHotSpot(pAI, pA->GetNextFrame());
				},
				[&] () {
						CallEvent(ONANIMATIONFINISHED)
				});
			}
		} while (false);

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

		objectCoef.UpdateExternal();

		// object * frame
		objectCoef.newAlpha = static_cast<UCHAR>(objectCoef.alpha * (255 - pCurFrame->alpha) / 255.0);
		objectCoef.newRGBCoef = AnimationFrameData::RGBCoef(objectCoef.pRGBCoef->rgbCoef).MulRGBCoef(pCurFrame->pRGBCoef->rgbCoef);

		EffectUtilities::SetAlpha(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam, objectCoef.newAlpha);
		EffectUtilities::SetRGBCoef(rdPtr->rs.rsEffect, rdPtr->rs.rsEffectParam, objectCoef.newRGBCoef);
		Zoom(rdPtr,
			static_cast<float>(pCurFrame->pScale->x * objectCoef.pScale->x),
			static_cast<float>(pCurFrame->pScale->y * objectCoef.pScale->y));

		// ------------
		// Overwrite
		// ------------

		Rotate(rdPtr, pCurFrame->angle);
		// Update x & y by pos before calling this
		::UpdateHotSpot(rdPtr, 
			HotSpotPos::CUSTOM,
			static_cast<int>(pCurFrame->pHotSpot->x), static_cast<int>(pCurFrame->pHotSpot->y));

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
		//this->speed = Range(speed, Animation_MinSpeed, Animation_MaxSpeed);
		this->speed = (std::max)(speed, Animation_MinSpeed);
	}
};
