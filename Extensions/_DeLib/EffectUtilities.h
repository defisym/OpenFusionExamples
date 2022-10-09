#pragma once

#include <string>
#include <vector>
#include <functional>

#include "EffectEx.h"

#include "Fusion.h"
#include "WindowsCommon.h"

#ifdef HWABETA
constexpr auto additionalLayerSize = 8;
#endif
#ifndef HWABETA
constexpr auto additionalLayerSize = 0;
#endif

//#define _ENABLE_TEST_FEATURE

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

	LPRDATA rdPtr = nullptr;
	LPRH rhPtr = nullptr;

	std::vector<EffectData> effectData;
	
	std::vector<LPSURFACE> effectImageParamData;

	using EffectIt = decltype(effectData.begin());

	inline EffectIt GetEffectData(const std::wstring& effectName) {
		return std::find_if(effectData.begin(), effectData.end()
			, [effectName](EffectUtilities::EffectData& item) -> bool {
				return item.effectName == effectName;
			});
	}

public:
	EffectUtilities(LPRDATA rdPtr) {
		this->rdPtr = rdPtr;
		this->rhPtr = rdPtr->rHo.hoAdRunHeader;

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

	~EffectUtilities() {
		for (auto& it : effectImageParamData) {
			delete it;
		}
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

#ifdef _ENABLE_TEST_FEATURE
	inline bool SetEffect(const std::wstring& backDropName, const std::wstring& effectName) {
		auto framePtr = rhPtr->rhFrame;

		struct BKD {
			LPLO pLO = nullptr;
			LPBKD2 pBKD = nullptr;
		};

		using BackdropVec = std::vector<BKD>;

		auto GetBackdropLO = [=](const std::wstring& backDropName)->BackdropVec {
			BackdropVec pBackdropVec;

			auto layerPtr = framePtr->m_pLayers;

			for (int i = 0; i < framePtr->m_nLayers; ++i) {
				LPLO backdropPtr = (LPLO)(framePtr->m_los + layerPtr->nFirstLOIndex);

				for (int j = 0; j < (int)layerPtr->nBkdLOs; ++j) {
					LPOI objOI = rhPtr->rhApp->m_ois[rhPtr->rhApp->m_oi_handle_to_index[backdropPtr->loOiHandle]];

					if (wcscmp(objOI->oiName, backDropName.c_str()) == 0) {
						pBackdropVec.emplace_back(BKD{ backdropPtr,nullptr });
					}

					backdropPtr++;
				}

				//LPBKD2 backdrop2Ptr = layerPtr->m_pBkd2;

				//for (int j = 0; j < (int)layerPtr->m_nBkd2Count; ++j) {
				//	pBackdropVec.emplace_back(BKD{ nullptr,backdrop2Ptr });

				//	backdrop2Ptr++;
				//}

				layerPtr = (RunFrameLayer*)(((char*)layerPtr++) + additionalLayerSize);
			}

			return pBackdropVec;
		};

		auto pLO = GetBackdropLO(backDropName);

		if (pLO.empty()) {
			return false;
		}

		for (auto& it : pLO) {
			if (it.pLO != nullptr) {
				auto pSpr = it.pLO->loSpr[0];

				if (pSpr == nullptr) {
					continue;
				}

				SetEffect(pSpr->sprEffect, pSpr->sprEffectParam, effectName);
				pSpr->sprFlags |= SF_REAF;

				RECT rc = { pSpr->sprX1 , pSpr->sprY1, pSpr->sprX2 , pSpr->sprY2 };
				WinAddZone(rhPtr->rhIdEditWin, &rc);

				//DWORD effect = pSpr->sprEffect;
				//LPARAM effectParam = pSpr->sprEffectParam;

				//SetEffect(effect, effectParam, effectName);

				//auto win = rhPtr->mvIdEditWin;
				//ModifSpriteEffect(win, pSpr, effect, effectParam);
				//pSpr->sprFlags |= SF_REAF;

				//SpriteDraw(win);
			}

			if (it.pBKD != nullptr) {
				auto pBKD = it.pBKD;
				auto pSpr = pBKD->pSpr[0];

				SetEffect(pBKD->inkEffect, (LPARAM&)pBKD->inkEffectParam, effectName);
				SetEffect(pSpr->sprEffect, pSpr->sprEffectParam, effectName);
				pSpr->sprFlags |= SF_REAF;
			}
		}

		return true;
	}
#endif // _ENABLE_TEST_FEATURE
	
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

	inline static int GetParamType(CEffectEx* pEffect, const std::wstring& paramName) {
		if (pEffect == nullptr) {
			return -1;
		}

		auto paramIndex = pEffect->GetParamIndex(ConvertWStrToStr(paramName).c_str());

		if (paramIndex == -1) {
			return -1;
		}

		return pEffect->GetParamType(paramIndex);
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

	// handle surface by this object, or you need to manage it by yourself
	inline bool SetParamEx(CEffectEx* pEffect, const std::wstring& paramName, int paramType, void* pParam) {
		if (paramType == EFFECTPARAM_SURFACE) {
			auto pInputSf = (LPSURFACE)pParam;
			LPSURFACE pLocalSf = nullptr;

			if (IsHWA(pInputSf)) {
				pLocalSf = CreateCloneSurface(pInputSf);
			}
			else {
				pLocalSf = ConvertHWATexture(rdPtr, pInputSf);
			}

			effectImageParamData.emplace_back(pLocalSf);

			return SetParam(pEffect, paramName, paramType, pLocalSf);
		}
		else {
			return SetParam(pEffect, paramName, paramType, pParam);
		}
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
