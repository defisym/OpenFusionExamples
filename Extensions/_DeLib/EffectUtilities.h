#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <functional>

#include "EffectEx.h"

#include "Fusion.h"
#include "FusionUtilities.h"

#include "WindowsCommon.h"

#ifdef HWABETA
constexpr auto additionalLayerSize = 8;
#endif
#ifndef HWABETA
constexpr auto additionalLayerSize = 0;
#endif

#define _ENABLE_TEST_FEATURE

// get Tint
// ((CEffectEx*)effectParam)->GetRGBA();

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

	void* ptrWin = nullptr;
	
	bool bD3D11 = false;

	struct EffectRecord {
		CEffectEx* pEffect = nullptr;
		int effectIndex = -1;
	};

	std::map<LPSURFACE, std::vector<EffectRecord>> effectImageParamRecord;

	using BackdropVec = std::vector<LPLO>;

	BackdropVec GetBackdropLO(const std::wstring& backDropName) {
		BackdropVec pBackdropVec;
		auto framePtr = rhPtr->rhFrame;

		auto layerPtr = framePtr->m_pLayers;

		for (int i = 0; i < framePtr->m_nLayers; ++i) {
			LPLO backdropPtr = (LPLO)(framePtr->m_los + layerPtr->nFirstLOIndex);

			for (int j = 0; j < (int)layerPtr->nBkdLOs; ++j) {
				LPOI objOI = rhPtr->rhApp->m_ois[rhPtr->rhApp->m_oi_handle_to_index[backdropPtr->loOiHandle]];

				if (wcscmp(objOI->oiName, backDropName.c_str()) == 0) {
					pBackdropVec.emplace_back(backdropPtr);
				}

				backdropPtr++;
			}

			layerPtr = (RunFrameLayer*)(((char*)layerPtr++) + additionalLayerSize);
		}

		return pBackdropVec;
	};

	struct BackdropEffect {
		CEffectEx* pEffect = nullptr;
		npSpr pSpr = nullptr;
	};

	using BackdropEffectVec = std::vector<BackdropEffect>;

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
		this->ptrWin = rdPtr->rHo.hoAdRunHeader->rhIdEditWin;

		this->bD3D11 = PreMulAlpha(rdPtr);

		auto pEffectData = rdPtr->rHo.hoAdRunHeader->rhApp->m_pEffects;

		if (pEffectData == nullptr) {
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
			auto pEH = (EffectHdr*)(pEffectData + effectsOffset[j]);

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

	// update by itself
	inline bool ModifSpriteEffect(LPRO pObject) {
		return ModifSpriteEffect(pObject->roc.rcSprite);
	}

	inline bool ModifSpriteEffect(npSpr pSpr) {
		auto ret = ::ModifSpriteEffect(ptrWin, pSpr, pSpr->sprEffect, pSpr->sprEffectParam);

		return true;
	}

	inline static bool ModifSpriteEffect(LPRO pObject
		,std::function<bool(DWORD& effect, LPARAM& effectParam)> modifier) {
		if (!(pObject->roHo.hoOEFlags & OEFLAG_SPRITES)) {
			return false;
		}

		// debug
		auto name = pObject->roHo.hoOiList->oilName;

		pObject->roc.rcChanged = true;
		
		auto effect = pObject->ros.rsEffect;
		auto effectParam = pObject->ros.rsEffectParam;		

		auto bSuccess = modifier(effect, effectParam);

		auto pSpr = pObject->roc.rcSprite;
		auto ptrWin = pObject->roHo.hoAdRunHeader->rhIdEditWin;
		auto bModify = bSuccess && pSpr != nullptr;

		// should be called to update param
		if (bModify) {
			auto ret = ::ModifSpriteEffect(ptrWin, pSpr
				, effect, effectParam);
		}

		return bModify;
	}

	inline void RecordEffectImageParam(LPSURFACE pSf, CEffectEx* pEffect, int index) {
		if (index == -1) {
			return;
		}

		auto it = effectImageParamRecord.find(pSf);

		if (it == effectImageParamRecord.end()) {
			effectImageParamRecord[pSf].emplace_back(EffectRecord{ pEffect ,index });
		}
		else {
			auto& vec = it->second;
			if (std::find_if(vec.begin(), vec.end(), [pEffect](auto& EffectRecord) {
				return EffectRecord.pEffect == pEffect;
				}) != vec.end()) {
				vec.emplace_back(EffectRecord{ pEffect ,index });
			}
		}
	}

	inline bool EffectImageParamReferred(LPSURFACE pSf) {
		auto it = effectImageParamRecord.find(pSf);

		if (it != effectImageParamRecord.end()) {
			auto pImp = GetSurfaceImplementation(*it->first);
			auto& vec = it->second;

			for (auto& effectIt : vec) {
				if (CEffectExValid(effectIt.pEffect)
					&& effectIt.pEffect->GetParamType(effectIt.effectIndex) == EFFECTPARAM_SURFACE
					&& effectIt.pEffect->GetParamSurfaceValue(effectIt.effectIndex) == pImp) {
					return true;
				}
			}
		}

		return false;
	}

	inline void releaseEffectImageParam() {
		bool bClear = false;

		for (auto& it : effectImageParamRecord) {
			if (!EffectImageParamReferred(it.first)) {
				for (auto pSfIt = std::find(effectImageParamData.begin(), effectImageParamData.end(), it.first)
					; pSfIt != effectImageParamData.end()
					; pSfIt = std::find(effectImageParamData.begin(), effectImageParamData.end(), it.first)) {
					effectImageParamData.erase(pSfIt);
				}

				delete it.first;
				it.second.clear();

				bClear = true;
			}
		}

		if (bClear) {
			decltype(effectImageParamRecord) newRecord;

			for (auto& it : effectImageParamRecord) {
				if (!it.second.empty()) {
					newRecord.emplace(it);
				}
			}

			effectImageParamRecord = std::move(newRecord);
		}
	}

	// https://www.cnblogs.com/chechen/p/10259592.html
	inline static bool CEffectExValid(CEffectEx* pEffect) {		
		__try {
			auto flags = pEffect->GetFlags();

			return true;
		}		
		__except (
			GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION
			? EXCEPTION_EXECUTE_HANDLER
			: EXCEPTION_CONTINUE_SEARCH
			) {
			return false;
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

	inline static CEffectEx* GetEffect(rSpr& ros) {
		return GetEffect(ros.rsEffect, ros.rsEffectParam);
	}

	inline static CEffectEx* GetEffect(LPRO pObject) {
		return GetEffect(pObject->ros);
	}

	inline BackdropEffectVec GetEffect(const std::wstring& backDropName) {
		BackdropEffectVec ret;

		auto pLO = GetBackdropLO(backDropName);
		for (auto& it : pLO) {
			if (it != nullptr) {
				auto pSpr = it->loSpr[0];

				if (pSpr == nullptr) {
					continue;
				}

				if (EffectShader(pSpr->sprEffect)) {
					ret.emplace_back(BackdropEffect{ (CEffectEx*)pSpr->sprEffectParam, pSpr });
				}
			}
		}

		return ret;
	}

#ifdef _ENABLE_TEST_FEATURE
	inline bool SetEffect(const std::wstring& backDropName, const std::wstring& effectName) {
		auto pLO = GetBackdropLO(backDropName);
		for (auto& it : pLO) {
			if (it != nullptr) {
				auto pSpr = it->loSpr[0];

				if (pSpr == nullptr) {
					continue;
				}

				SetEffect(pSpr->sprEffect, pSpr->sprEffectParam, effectName);
				::ModifSpriteEffect(ptrWin, pSpr, pSpr->sprEffect, pSpr->sprEffectParam);
			}
		}

		return true;	
	}
#endif // _ENABLE_TEST_FEATURE
	
	// shader with param disappear -> need to initialize param buffer
	// workaround: create an object attached a shader with param to let fusion init it
	inline bool SetEffect(LPRO pObject, const std::wstring& effectName) {
		auto flags = pObject->roHo.hoOEFlags;

		if (!(flags & OEFLAG_SPRITES)) {
			return false;
		}

		pObject->ros.rsFlags |= RSFLAG_CREATEDEFFECT;
		auto ret = SetEffect(pObject->ros, effectName);

		ret &= ModifSpriteEffect(pObject, [&](DWORD& effect, LPARAM& effectParam) {
			effect = pObject->ros.rsEffect;
			effectParam = pObject->ros.rsEffectParam;

			return true;
			});

		return ret;
	}

	inline bool SetEffect(rSpr& ros, const std::wstring& effectName) {
		return SetEffect(ros.rsEffect, ros.rsEffectParam, effectName);
	}
	
	inline bool SetEffect(DWORD& effect, LPARAM& effectParam, const std::wstring& effectName) {
		if (EffectShader(effect)) {
			// reset to BOP_COPY == 0
			effect = ((effect >> 16) << 16);

			auto pOldEffect = ((CEffectEx*)(effectParam));

			DeleteEffect((CEffectEx*)effectParam);
			effectParam = 0;
		}
		
		//auto size = sizeof(EffectParamsHdr);

		auto it = GetEffectData(effectName);

		if (it == effectData.end()) {
			return false;
		}
				
		auto pEffect = NewEffect();
		BOOL ret = TRUE;

		//const auto lUserParam = 0;
		const auto lUserParam = 1;

		if (this->bD3D11) {
			ret = pEffect->ExInitialize((LPBYTE)it->pData
				, it->pHdr->dwEffectDataSize				
				, lUserParam
				, it->pHdr->dwOptions
				, it->effectParam.size()
				, (LPCSTR)it->pParamName
				, it->pParamType);
		}
		else {
			ret = pEffect->Initialize((LPCSTR)it->pData
				, lUserParam
				, it->pHdr->dwOptions
				, it->effectParam.size()
				, (LPCSTR)it->pParamName
				, it->pParamType);
		}

		if (ret == FALSE) {
			return false;
		}

		SetDefaultParam(pEffect, it);
		
		effect = effect | BOP_EFFECTEX;
		effectParam = (LPARAM)pEffect;

		return true;
	}

	inline static int GetParamIndex(CEffectEx* pEffect, const std::wstring& paramName) {
		if (pEffect == nullptr) {
			return -1;
		}

		return pEffect->GetParamIndex(ConvertWStrToStr(paramName).c_str());
	}
	
	inline static int GetParamType(CEffectEx* pEffect, const std::wstring& paramName) {
		if (pEffect == nullptr) {
			return -1;
		}

		auto paramIndex = GetParamIndex(pEffect, paramName);

		if (paramIndex == -1) {
			return -1;
		}

		return pEffect->GetParamType(paramIndex);
	}

	using ParamIndexType = std::tuple<int, int>;

	inline static ParamIndexType GetParamIndexType(CEffectEx* pEffect, const std::wstring& paramName) {
		if (pEffect == nullptr) {
			return std::make_tuple(-1,-1);
		}

		auto paramIndex = pEffect->GetParamIndex(ConvertWStrToStr(paramName).c_str());
		auto paramType = pEffect->GetParamType(paramIndex);

		return std::make_tuple(paramIndex, paramType);
	}

	inline static void SetParamCore(CEffectEx* pEffect, int paramIndex, int paramType, void* pParam) {
		switch (paramType)
		{
		case EFFECTPARAM_INT: {
			pEffect->SetParamIntValue(paramIndex, *(int*)pParam);
			//pEffect->SetFlags(pEffect->GetFlags() | EFFECTFLAG_MODIFIED);

			break;
		}
		case EFFECTPARAM_FLOAT: {
			pEffect->SetParamFloatValue(paramIndex, *(float*)pParam);
			//pEffect->SetFlags(pEffect->GetFlags() | EFFECTFLAG_MODIFIED);

			break;
		}
		case EFFECTPARAM_INTFLOAT4: {
			pEffect->SetParamIntValue(paramIndex, *(int*)pParam);
			//pEffect->SetFlags(pEffect->GetFlags() | EFFECTFLAG_MODIFIED);

			break;
		}
		case EFFECTPARAM_SURFACE: {
			auto pImpl = GetSurfaceImplementation(*(LPSURFACE)pParam);
			pEffect->SetParamSurfaceValue(paramIndex, pImpl);
			//pEffect->SetFlags(pEffect->GetFlags() | EFFECTFLAG_MODIFIED);

			break;
		}
		default: {
			break;
		}
		}
	}
	
	inline static bool SetParam(CEffectEx* pEffect, const std::wstring& paramName, void* pParam) {
		if (pEffect == nullptr) {
			return false;
		}

		auto [effectParamIndex, effectParamType] = GetParamIndexType(pEffect, paramName);

		SetParamCore(pEffect, effectParamIndex, effectParamType, pParam);
		
		return true;
	}
	
	inline static bool SetParam(CEffectEx* pEffect, const std::wstring& paramName, int paramType, void* pParam) {
		if (pEffect == nullptr) {
			return false;
		}

		auto[effectParamIndex, effectParamType] = GetParamIndexType(pEffect, paramName);

		if (effectParamIndex == -1|| effectParamType == -1) {
			return false;
		}

		if (effectParamType != paramType) {
			return false;
		}

		SetParamCore(pEffect, effectParamIndex, effectParamType, pParam);
	
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

			auto [effectParamIndex, effectParamType] = GetParamIndexType(pEffect, paramName);

			RecordEffectImageParam(pLocalSf, pEffect, effectParamIndex);

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

	// https://community.clickteam.com/threads/110022-LPRO-Semi-Transparency-(for-HWA)?p=777502&viewfull=1#post777502
	// BOP_RGBAFILTER -> alpha blending && RGB coef

	inline static UCHAR GetAlpha(LPRO pObject) {
		if (!(pObject->roHo.hoOEFlags & OEFLAG_SPRITES)) {
			return 0;
		}

		return GetAlpha(pObject->ros.rsEffect, pObject->ros.rsEffectParam);
	}

	inline static UCHAR GetAlpha(DWORD effect, LPARAM effectParam) {
		auto effectType = effect & BOP_MASK;

		switch (effectType) {
		case BOP_EFFECTEX: {
			auto pEffect = GetEffect(effect, effectParam);

			if (pEffect != nullptr) {
				return UCHAR(255 - (pEffect->GetRGBA() >> 24));
			}

			break;
		}
		case BOP_BLEND: {
			// old semi-transparency coef
			return UCHAR(255 - SEMITRANSPTOALPHA(effectParam));

			break;
		}
		default: {
			if (effect & BOP_RGBAFILTER) {
				return UCHAR(255 - (((DWORD)effectParam) >> 24));
			}

			break;
		}
		}
	
		return 0;
	}

	inline static bool SetAlpha(LPRO pObject, UCHAR alpha) {
		return ModifSpriteEffect(pObject, [&](DWORD& effect, LPARAM& effectParam) {
			return SetAlpha(effect, effectParam, alpha);;
			});
	}

	inline static bool SetAlpha(DWORD& effect, LPARAM& effectParam, UCHAR alpha) {
		auto effectType = effect & BOP_MASK;

		switch (effectType) {
		case BOP_EFFECTEX: {
			auto pEffect = GetEffect(effect, effectParam);

			if (pEffect != nullptr) {
				auto dwRGBA = pEffect->GetRGBA();
				auto dwRGB = dwRGBA & 0x00FFFFFF;

				auto newDwRGBA = ((255 - alpha) << 24) | dwRGB;

				pEffect->SetRGBA(newDwRGBA);

				return true;
			}

			break;
		}
		case BOP_BLEND: {
			// old semi-transparency coef
			effectParam = ALPHATOSEMITRANSP(255 - alpha);

			return true;

			break;
		}
		default: {
			effect |= BOP_RGBAFILTER;

			if (effect & BOP_RGBAFILTER) {
				// init
				if (effectParam == -1) {
					effectParam = 0xFFFFFFFF;
				}

				auto dwRGB = (DWORD)effectParam & 0x00FFFFFF;

				auto newDwRGBA = ((255 - alpha) << 24) | dwRGB;

				effectParam = newDwRGBA;

				return true;
			}

			break;
		}
		}

		return false;
	}

	inline static DWORD GetRGBCoef(LPRO pObject) {
		if (!(pObject->roHo.hoOEFlags & OEFLAG_SPRITES)) {
			return 0;
		}

		return GetRGBCoef(pObject->ros.rsEffect, pObject->ros.rsEffectParam);
	}

	inline static DWORD GetRGBCoef(DWORD effect, LPARAM effectParam) {
		auto effectType = effect & BOP_MASK;

		switch (effectType) {
		case BOP_EFFECTEX: {
			auto pEffect = GetEffect(effect, effectParam);

			if (pEffect != nullptr) {
				return pEffect->GetRGBA() | 0x00FFFFFF;
			}

			break;
		}
		default: {
			if (effect & BOP_RGBAFILTER) {
				return (DWORD)effectParam | 0x00FFFFFF;
			}

			break;
		}
		}

		return 0;
	}

	inline static bool SetRGBCoef(LPRO pObject, DWORD coef) {
		return ModifSpriteEffect(pObject, [&](DWORD& effect, LPARAM& effectParam) {
			return SetRGBCoef(effect, effectParam, coef);;
			});
	}

	inline static bool SetRGBCoef(DWORD& effect, LPARAM& effectParam, DWORD coef) {
		auto effectType = effect & BOP_MASK;
		
		// normalization
		coef &= 0x00FFFFFF;

		switch (effectType) {
		case BOP_EFFECTEX: {
			auto pEffect = GetEffect(effect, effectParam);

			if (pEffect != nullptr) {
				auto dwRGBA = pEffect->GetRGBA();
				auto dwAlpha = dwRGBA & 0xFF000000;

				auto newDwRGBA = dwAlpha | coef;

				pEffect->SetRGBA(newDwRGBA);

				return true;
			}

			break;
		}
		default: {
			effect |= BOP_RGBAFILTER;

			if (effect & BOP_RGBAFILTER) {
				// init
				if (effectParam == -1) {
					effectParam = 0xFFFFFFFF;
				}

				auto dwAlpha = (DWORD)effectParam & 0xFF000000;

				auto newDwRGBA = dwAlpha | coef;

				effectParam = newDwRGBA;

				return true;

			}

			break;
		}
		}

		return false;
	}
};
