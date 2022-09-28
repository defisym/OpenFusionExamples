#pragma once

#include <string>

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
