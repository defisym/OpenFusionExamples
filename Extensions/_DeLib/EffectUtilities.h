#pragma once

#include <string>
#include <vector>

#include "EffectEx.h"
#include "WindowsCommon.h"

class EffectUtilities {
private:
	struct EffectParamData {
		// point to app data, need not to release
		LPCSTR pName = nullptr;

		// data need to release
		BYTE type = 0;
		std::wstring effectParamName;		
	};

	struct EffectData {
		// point to app data, need not to release
		EffectHdr* pHdr = nullptr;
		LPCSTR pName = nullptr;
		LPBYTE pData = 0;

		LPBYTE pParamType = nullptr;
		LPBYTE pParamName = nullptr;

		// data need to release
		std::wstring effectName;
		std::vector<EffectParamData> effectParam;
	};

	std::vector<EffectData> effectData;

public:
	EffectUtilities(LPRDATA rdPtr) {
		auto pData = rdPtr->rHo.hoAdRunHeader->rhApp->m_pEffects;

		if (pData == nullptr) {
			return;
		}

		auto pEffect = rdPtr->rHo.hoAdRunHeader->rhApp->m_pEffects;

		auto nEffects = *(int*)pEffect;
		pEffect += sizeof(int);

		auto effectsOffset = new int[nEffects];

		this->effectData.reserve(nEffects);

		for (auto i = 0; i < nEffects; i++) {
			effectsOffset[i] = *(int*)pEffect;
			pEffect += sizeof(int);
		}

		for (auto j = 0; j < nEffects; j++) {
			auto pEH = (EffectHdr*)(pData + effectsOffset[j]);

			auto pName = (LPCSTR)((LPBYTE)pEH + pEH->dwEffectNameOffset);
			auto pData = ((LPBYTE)pEH + pEH->dwEffectDataOffset);

			this->effectData.emplace_back(EffectData{ pEH, pName, pData, nullptr, nullptr
				, std::move(ConvertStrToWStr(pName))});

			if (pEH->dwEffectParamsOffset == 0) {
				continue;
			}

			auto pParam = (EffectParamsHdr*)((LPBYTE)pEH + pEH->dwEffectParamsOffset);
			int paramNum = (int)pParam->dwNParams;

			this->effectData.back().effectParam.reserve(paramNum);

			auto pParamType = (LPBYTE)pParam + pParam->dwParamTypesOffset;
			auto pParamName = (LPBYTE)pParam + pParam->dwParamNamesOffset;

			this->effectData.back().pParamType = pParamType;
			this->effectData.back().pParamName = pParamName;

			for (auto k = 0; k < paramNum; k++) {
				auto paramType = *pParamType;
				pParamType++;

				auto paramName = (LPCSTR)pParamName;
				pParamName += strlen(paramName) + 1;

				this->effectData.back().effectParam.emplace_back(EffectParamData{ paramName, paramType
					, std::move(ConvertStrToWStr(paramName)) });
			}
		}

		delete[] effectsOffset;
	}

	inline static bool EffectShader(DWORD effect) {
		// if so, EffectParam is now the pointer to CEffectEx
		return (effect & EFFECT_MASK) == BOP_EFFECTEX;
	}

	inline static CEffectEx* GetEffect(DWORD effect, LPARAM effectParam) {
		if (!EffectShader(effect)) {
			return nullptr;
		}

		auto pEP = (CEffectEx*)effectParam;

		return pEP;
	}

	inline bool SetEffect(LPRO pObject, std::wstring& effectName) {
		auto flags = pObject->roHo.hoOEFlags;

		if (!(flags & OEFLAG_SPRITES)) {
			return false;
		}

		return SetEffect(pObject->ros, effectName);
	}

	inline bool SetEffect(rSpr& ros, std::wstring& effectName) {
		return SetEffect(ros.rsEffect, ros.rsEffectParam, effectName);
	}
	
	inline bool SetEffect(DWORD& effect, LPARAM& effectParam, std::wstring& effectName) {
		if ((effect & EFFECT_MASK) == BOP_EFFECTEX) {
			// reset to BOP_COPY == 0
			effect = ((effect >> 16) << 16);
			DeleteEffect((CEffectEx*)effectParam);
		}
		
		auto it = std::find_if(effectData.begin(), effectData.end()
			, [effectName](EffectUtilities::EffectData& item) -> bool {
				return item.effectName == effectName;
			});

		if (it == effectData.end()) {
			return false;
		}
				
		auto pEffect = NewEffect();
		pEffect->Initialize((LPCSTR)it->pData
			, 0
			, it->pHdr->dwOptions
			, it->effectParam.size()
			, (LPCSTR)it->pParamName
			, it->pParamType);

		effect = effect | BOP_EFFECTEX;
		effectParam = (LPARAM)pEffect;

		return true;
	}

	inline static bool SetParam(CEffectEx* pEffect, std::wstring& paramName, int paramType, void* pParam) {
		if (pEffect == nullptr) {
			return false;
		}

		auto paramIndex = pEffect->GetParamIndex(ConvertWStrToStr(paramName).c_str());

		if (paramIndex == -1) {
			return false;
		}

		if (paramType != pEffect->GetParamType(paramIndex)) {
			return false;
		}

		switch (paramType)
		{
		case EFFECTPARAM_INT: {
			pEffect->SetParamIntValue(paramIndex, *(int*)pParam);

			break;
		}
		case EFFECTPARAM_FLOAT: {
			pEffect->SetParamFloatValue(paramIndex, *(float*)pParam);

			break;
		}
		case EFFECTPARAM_INTFLOAT4: {
			//auto value = *(int*)pParam;

			break;
		}
		case EFFECTPARAM_SURFACE: {
			auto pImpl = GetSurfaceImplementation(*(LPSURFACE)pParam);
			pEffect->SetParamSurfaceValue(paramIndex, pImpl);

			break;
		}
		default: {
			break;
		}
		}

		return true;
	}
};
