#pragma once

#include <functional>

struct WindowsMessageBase {
	bool bTriggerCallback = false;

	inline void TriggerCallback(const std::function<void()>& cb) {
		if (!bTriggerCallback) { return; }
		bTriggerCallback = false;

		cb();
	}
};