#pragma once

#include "Animation.h"
#include "JsonInterface.h"

struct AnimationInterface {
	LPRDATA rdPtr = nullptr;
	LPRDATA pLib = nullptr;

	Animation* pA = nullptr;
	std::wstring basePath;
	std::wstring key;

	bool bPaused = false;
	int speed = 100;

	double speedAccumulate = 0.0;

	AnimationInterface(const LPRDATA rdPtr) {
		this->rdPtr = rdPtr;
	}
	~AnimationInterface() {
		StopAnimation();
	}

	inline void SetAnimationSource(const LPRDATA pLib, const std::wstring& basePath, const std::wstring& key) {
		this->pLib = pLib;
		this->basePath = basePath;
		this->key = key;

		if (this->basePath.back() != '\\') {
			this->basePath.push_back('\\');
		}
	}

	inline bool LoadAnimation(const LPCWSTR FileName, const LPCTSTR Key = L"") {
		JsonInterface JI;
		JI.SetComment(true);

		auto ret = true;		

		if (StrEmpty(Key)) {
			ret = JI.Load(FileName);
		}
		else {
			Encryption E;
			E.GenerateKey(Key);
			E.DecryptFile(FileName);

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
		} catch (...) {
			ret = false;

#if !defined(RUN_ONLY)
			MSGBOX(L"Json file parse error");
#endif
		}

		return ret;
	}

	inline void StopAnimation() {
		speedAccumulate = 0;

		delete pA;
		pA = nullptr;
	}

	inline void UpdateAnimation() {
		if (pA == nullptr) {
			return;
		}

		if(bPaused) {
			return;
		}

		const auto correctedSpeed = (this->speed * pA->GetAnimationInfo()->speed) / (100.0 * 100.0);
		speedAccumulate += correctedSpeed;

		if(speedAccumulate<1.0) {
			return;
		}

		speedAccumulate -= 1.0;

		pA->UpdateFrame();
		const auto pCurFrame = pA->GetCurrentFrame();

		const auto curFile = basePath + pCurFrame->file;
		if (*rdPtr->FilePath != curFile) {
			LoadFromLib(rdPtr, reinterpret_cast<LPRO>(pLib), curFile.c_str(), key.c_str());
		}

		//TODO hotspot, RGB
		Rotate(rdPtr, pCurFrame->angle);
		EffectUtilities::SetAlpha(reinterpret_cast<LPRO>(rdPtr), static_cast<UCHAR>(pCurFrame->alpha));

		//EffectUtilities::SetRGBCoef(reinterpret_cast<LPRO>(rdPtr), pCurFrame->pRGBCoef->rgbCoef);
		Zoom(rdPtr, static_cast<float>(pCurFrame->pScale->x), static_cast<float>(pCurFrame->pScale->y));
		//TODO type doesn't work
		//UpdateHotSpot(rdPtr, pCurFrame->pHotSpot->typeID, pCurFrame->pHotSpot->x, pCurFrame->pHotSpot->y);
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
		this->speed = speed;
	}
};
