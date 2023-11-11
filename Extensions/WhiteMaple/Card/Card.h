#pragma once

#include "InfoBase.h"

struct Card :InfoBase {
	Card() = default;
	Card(IniInterface* pIni) :InfoBase(pIni) {

	}
};