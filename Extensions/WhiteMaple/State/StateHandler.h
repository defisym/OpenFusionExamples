#pragma once

#include <vector>

#include "State.h"

struct StateHandler {
	State replaceable;

	std::vector<State> states;

	inline bool HasState(){
		return false;
	}

	inline bool AttachState(){
		return false;
	}

	inline bool RemoveState(){
		return false;
	}

	inline void UpdateState(){
	}
};