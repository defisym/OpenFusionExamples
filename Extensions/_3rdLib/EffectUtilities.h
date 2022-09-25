#pragma once

#include "EffectEx.h"

inline void GetEffect(LPRDATA rdPtr) {
	auto pE = (CEffectEx*)rdPtr->rs.rsEffect;
	auto pEP = (EFFECTPARAM*)rdPtr->rs.rsEffectParam;

	auto hoOi = rdPtr->rHo.hoOi;
	auto rhApp = rdPtr->rHo.hoAdRunHeader->rhApp;
	CEffectEx* pEB = (CEffectEx*)rhApp->m_pEffects;

	do {
		pEB = pEB +1;
	} while (pEB != nullptr);

	auto pOI = rhApp->m_ois[hoOi];
	auto pCurEffect = rhApp->m_ois[hoOi]->oiExtEffectEx;

	return;
}