#pragma once

#include <string>
#include <vector>
#include <functional>

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
		EFFECTPARAM effectDefaultParam;
	};

	struct EffectData {
		// point to app data, need not to release
		EffectHdr* pHdr = nullptr;
		LPCSTR pName = nullptr;
		LPBYTE pData = 0;

		LPBYTE pParamType = nullptr;
		LPBYTE pParamName = nullptr;

		// data need to release
		bool bDefaultParam = false;

		std::wstring effectName;		
		std::vector<EffectParamData> effectParam;
	};

	std::vector<EffectData> effectData;

	using EffectIt = decltype(effectData.begin());

	inline EffectIt GetEffectData(const std::wstring& effectName) {
		return std::find_if(effectData.begin(), effectData.end()
			, [effectName](EffectUtilities::EffectData& item) -> bool {
				return item.effectName == effectName;
			});
	}

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

			this->effectData.emplace_back(EffectData{ pEH, pName, pData
				, nullptr, nullptr, false
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
					, std::move(ConvertStrToWStr(paramName))});
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

	inline bool SetEffect(LPRO pObject, const std::wstring& effectName) {
		auto flags = pObject->roHo.hoOEFlags;

		if (!(flags & OEFLAG_SPRITES)) {
			return false;
		}

		return SetEffect(pObject->ros, effectName);
	}

	inline bool SetEffect(rSpr& ros, const std::wstring& effectName) {
		return SetEffect(ros.rsEffect, ros.rsEffectParam, effectName);
	}
	
	inline bool SetEffect(DWORD& effect, LPARAM& effectParam, const std::wstring& effectName) {
		if (EffectShader(effect)) {
			// reset to BOP_COPY == 0
			effect = ((effect >> 16) << 16);

			DeleteEffect((CEffectEx*)effectParam);
			effectParam = 0;			
		}
		
		auto it = GetEffectData(effectName);

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

		SetDefaultParam(pEffect, it);
		
		effect = effect | BOP_EFFECTEX;
		effectParam = (LPARAM)pEffect;		

		return true;
	}

	inline static void SetParamCore(CEffectEx* pEffect, int paramIndex, int paramType, void* pParam) {
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
			pEffect->SetParamIntValue(paramIndex, *(int*)pParam);

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
	}

	inline static bool SetParam(CEffectEx* pEffect, const std::wstring& paramName, int paramType, void* pParam) {
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

		SetParamCore(pEffect, paramIndex, paramType, pParam);
	
		return true;
	}

	using ParamCallBack = std::function<void*(const std::wstring&, EFFECTPARAM&)>;	

	// update here
	//void* pParam = nullptr;

	//SetDefaultParam(L"ShaderName", [pParam](const std::wstring& paramName, EFFECTPARAM& param)->void* {
	//	return pParam;	
	//	});

	inline void UpdateDefaultParam(const std::wstring& effectName, ParamCallBack callBack) {
		auto it = GetEffectData(effectName);

		if (it == effectData.end()) {
			return;
		}

		if (it->effectParam.empty()) {
			return;
		}

		it->bDefaultParam = true;

		for (auto& itParam : it->effectParam) {
			itParam.effectDefaultParam.nValueType = (int)itParam.type;
			auto pParam = callBack(itParam.effectParamName, itParam.effectDefaultParam);

			switch (itParam.effectDefaultParam.nValueType)
			{
			case EFFECTPARAM_INT: {
				itParam.effectDefaultParam.nValue = *(int*)pParam;

				break;
			}
			case EFFECTPARAM_FLOAT: {
				itParam.effectDefaultParam.fValue = *(float*)pParam;

				break;
			}
			case EFFECTPARAM_INTFLOAT4: {
				itParam.effectDefaultParam.nValue = *(int*)pParam;

				break;
			}
			case EFFECTPARAM_SURFACE: {
				auto pImpl = GetSurfaceImplementation(*(LPSURFACE)pParam);
				itParam.effectDefaultParam.pTextureSurface = pImpl;

				break;
			}
			default: {
				break;
			}
			}
		}
	}

	inline void SetDefaultParam(CEffectEx* pEffect, EffectIt it) {
		if (it == effectData.end()) {
			return;
		}

		if (!it->bDefaultParam) {
			return;
		}

		for (size_t i = 0; i < it->effectParam.size(); i++) {
			auto& curParam = it->effectParam[i];
			auto pCurParam = &curParam.effectDefaultParam;

			void* pParam = (BYTE*)pCurParam + sizeof(pCurParam->nValueType);

			SetParamCore(pEffect, i, curParam.type, pParam);
		}
	}	
};
