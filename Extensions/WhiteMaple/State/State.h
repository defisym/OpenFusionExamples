#pragma once

#include <string>

#include "RandGenerator.h"

#include "InfoBase.h"
#include "Correction.h"

struct State :InfoBase {
	// basic
	bool bNoAnime = false;
	size_t animeID;

	bool bNegative = false;
	bool bUnitAbility = false;
	bool bNoEffect = false;
	bool bNoMessage = false;
	std::wstring eigenStr;

	// continue & remove
	size_t continueRound;
	size_t randomRound;

	inline size_t GetRound() {
		RandGenerator<size_t> randGenerator(0, randomRound);

		return continueRound + randGenerator.GenerateRandNumber();
	}

	bool bPermanent = false;
	bool bRemoveByTurnStart = false;
	bool bRemoveByDamage = false;
	bool bRemoveByItem = true;

	size_t possibility;
	size_t priority;

	// accumulate
	bool bAccumulate = false;
	bool bAlterable = false;
	size_t stateLayerLimit;

	// hint
	std::wstring attachInfo;
	std::wstring addInfo;
	std::wstring continueInfo;
	std::wstring removeInfo;

	// restrict
	bool bCanNotAct = false;
	bool CanNotMove = false;
	bool CanNotAttack = false;
	bool CanNotUseItem = false;
	bool CanNotUseSpell = false;
	bool CanNotUseSkill = false;

	// correction
	Correction correction;

	State() = default;
	State(IniInterface* pIni) :InfoBase(pIni) {

	}
};