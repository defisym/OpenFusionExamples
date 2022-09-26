#pragma once

#include <string>

#include "EffectEx.h"
#include "WindowsCommon.h"

inline CEffectEx* GetEffectID(LPRDATA rdPtr) {
	rdPtr->rs.rsEffect;
}

inline CEffectEx* GetEffect(LPRDATA rdPtr) {
	auto pEP = (CEffectEx*)rdPtr->rs.rsEffectParam;

	return pEP;
}
