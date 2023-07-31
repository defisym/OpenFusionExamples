#pragma once

#include "EOSUtilities.h"

class EOSAchievement :private EOSBase {
	EOSUtilities* pEU;

	EOSAchievement() {
		pEU->bAchievementQuery;
	}
};
