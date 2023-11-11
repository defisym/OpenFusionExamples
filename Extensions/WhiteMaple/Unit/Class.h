#pragma once

#include "InfoBase.h"

struct Class :InfoBase {
	size_t moveRange;

	Class() = default;
	Class(IniInterface* pIni) :InfoBase(pIni) {

	}
};