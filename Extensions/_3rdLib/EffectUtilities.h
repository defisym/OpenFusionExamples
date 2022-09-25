#pragma once

#include <string>

#include "EffectEx.h"
#include "WindowsCommon.h"

inline CEffectEx* GetEffect(LPRDATA rdPtr) {
	auto pEP = (CEffectEx*)rdPtr->rs.rsEffectParam;

	return pEP;
}
