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
		
		const auto pCurFrame = pA->GetCurrentFrame();

		if (!pAI->updateCur) {
			const auto curFile = basePath + pCurFrame->file;
			if (*rdPtr->FilePath != curFile) {
				LoadFromLib(rdPtr, reinterpret_cast<LPRO>(pLib), curFile.c_str(), key.c_str());
			}
		}

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
