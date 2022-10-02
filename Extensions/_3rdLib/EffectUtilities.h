#pragma once

#include <string>
#include <vector>

#include "EffectEx.h"
#include "WindowsCommon.h"

inline DWORD GetEffectID(LPRDATA rdPtr) {
	return rdPtr->rs.rsEffect;
}

inline bool EffectShader(LPRDATA rdPtr) {
	auto effectID = GetEffectID(rdPtr);

	// if so, EffectParam is now the pointer to CEffectEx
	return (effectID & EFFECT_MASK) == BOP_EFFECTEX;
}

inline CEffectEx* GetEffect(LPRDATA rdPtr) {
	if (!EffectShader(rdPtr)) {
		return nullptr;
	}

	auto pEP = (CEffectEx*)rdPtr->rs.rsEffectParam;

	return pEP;
}

struct EffectData {
	LPCSTR pName = nullptr;
	LPBYTE pData = 0;
};

inline void IterateEffect(LPRDATA rdPtr) {
	auto pData = rdPtr->rHo.hoAdRunHeader->rhApp->m_pEffects;

	if (pData == nullptr) {
		return;
	}

	auto pEffect = rdPtr->rHo.hoAdRunHeader->rhApp->m_pEffects;

	auto nEffects = *(int*)pEffect;
	pEffect += sizeof(int);

	auto effectsOffset = new int[nEffects];

	for (auto i = 0; i < nEffects; i++) {
		effectsOffset[i] = *(int*)pEffect;
		pEffect += sizeof(int);
	}

	for (auto j = 0; j < nEffects; j++) {
		auto pEH = (EffectHdr*)(pData + effectsOffset[j]);

		auto pName = (LPCSTR)((LPBYTE)pEH + pEH->dwEffectNameOffset);
		auto pData = ((LPBYTE)pEH + pEH->dwEffectDataOffset);

		if (pEH->dwEffectParamsOffset == 0) {
			continue;
		}

		auto pParam = (EffectParamsHdr*)((LPBYTE)pEH + pEH->dwEffectParamsOffset);

		auto pParamType = (LPBYTE)pParam + pParam->dwParamTypesOffset;
		auto pParamName = (LPBYTE)pParam + pParam->dwParamNamesOffset;

		for (auto k = 0; k < (int)pParam->dwNParams; k++) {
			auto paramType = *pParamType;
			pParamType++;

			auto paramName = (LPCSTR)pParamName;
			pParamName += strlen(paramName) + 1;
		}
	}

	delete[] effectsOffset;
}
