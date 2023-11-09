#pragma once

#include <vector>

#include "State.h"

struct StateHandler {
	State replaceable;

	std::vector<State> states;
};