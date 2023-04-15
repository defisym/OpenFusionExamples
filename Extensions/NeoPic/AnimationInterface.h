#pragma once

#include "Animation.h"
#include "JsonInterface.h"

struct AnimationInterface {
	LPRDATA rdPtr = nullptr;
	LPRDATA pLib = nullptr;

	Animation* pA = nullptr;
	std::wstring basePath;
	std::wstring key;

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

		if(ret) {
			StopAnimation();

			pA = new Animation(JI.Get());
		}

		return ret;
	}

	inline void StopAnimation() {
		delete pA;
		pA = nullptr;
	}

	inline void UpdateAnimation() const {
		if (pA == nullptr) {
			return;
		}

		pA->UpdateFrame();
		const auto pCurFrame = pA->GetCurrentFrame();

		const auto curFile = basePath + pCurFrame->file;
		if (*rdPtr->FilePath != curFile) {
			LoadFromLib(rdPtr, reinterpret_cast<LPRO>(pLib), curFile.c_str(), key.c_str());
		}

		Zoom(rdPtr, static_cast<float>(pCurFrame->pScale->x), static_cast<float>(pCurFrame->pScale->y));
		Rotate(rdPtr, pCurFrame->angle);
	}
};
