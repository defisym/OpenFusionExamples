#pragma once

#include "WindowsMessageBase.h"

struct MouseHandler :public WindowsMessageBase {
	std::vector<short> cmds;
	short curCmd = -1;

	inline void ReceiveCommand(const short cmd) {
		bTriggerCallback = true;
		cmds.push_back(cmd);
	}

	inline void TriggerCallback(const std::function<void()>& cb) {
		if (!bTriggerCallback) { return; }

		for(const auto& cmd: cmds) {
			curCmd = cmd;
			bTriggerCallback = true;
			static_cast<WindowsMessageBase*>(this)->TriggerCallback(cb);
		}

		curCmd = -1;
		cmds.clear();
		bTriggerCallback = false;
	}
};
