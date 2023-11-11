#pragma once

#include <string>

#include "IniInterface.h"

using InternalID = long;

struct InfoBase {
	InternalID ID;
	std::wstring name;
	std::wstring explanation;
	std::wstring pictureFileName;

	InfoBase() = default;
	InfoBase(IniInterface* pIni) {

	}
	virtual ~InfoBase() = default;
};
