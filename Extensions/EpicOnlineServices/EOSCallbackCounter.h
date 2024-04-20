#pragma once

#include <atomic>

class CallbackCounter {
private:
	std::atomic<size_t> callbackCount = 0;

public:
	inline void CallCallback() { ++callbackCount; }
	inline void FinishCallback() { --callbackCount; }
	inline bool AllCallbackComplete() const { return callbackCount == 0; }
};

class CallbackCounterHelper {
public:
	CallbackCounter& callbackCounter;
	CallbackCounterHelper(CallbackCounter& cc) : callbackCounter(cc) {}
	~CallbackCounterHelper() {
		callbackCounter.FinishCallback();
	}
};