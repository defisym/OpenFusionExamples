#pragma once

#include <atomic>

enum class CallbackType {
	
};

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
	CallbackCounterHelper(const CallbackCounterHelper&) = delete;
	CallbackCounterHelper(CallbackCounterHelper&&) = delete;
	CallbackCounterHelper& operator=(const CallbackCounterHelper&) = delete;
	CallbackCounterHelper& operator=(CallbackCounterHelper&&) = delete;

	CallbackCounter& callbackCounter;
	explicit CallbackCounterHelper(CallbackCounter& cc) : callbackCounter(cc) {}
	~CallbackCounterHelper() {
		callbackCounter.FinishCallback();
	}
};