#pragma once

#include <string>

using InternalID = long;

struct InfoBase {
	InternalID ID;
	std::wstring name;
	std::wstring explanation;
	std::wstring pictureFileName;
};
