#pragma once

#include "EOSCallbackCounter.h"
#include "EOSUtilities.h"

class PlatformBase {
protected:
	EOSUtilities* pEU = nullptr;
	CallbackCounter callbackCounter;

public:
	explicit PlatformBase(EOSUtilities* pEU) {
		this->pEU = pEU;
	}
	virtual ~PlatformBase() = default;

	// init platform, usually need to query data first
	virtual inline void PlatformInit() = 0;
	// query data
	virtual inline void PlatformQuery() = 0;
	// handle update task if needed
	virtual inline void PlatformUpdate() {}

	inline bool PlatformOK() const {
		return pEU->PlatformOK();
	}

	inline bool AllCallbackComplete() const {
		return callbackCounter.AllCallbackComplete();
	}
};

// ------------
// template
// ------------

// Step:
// 1. Copy template
// 2. Add as friend class of EOSUtilities
// 3. Create / Refresh in GlobalData
// 4. Add include info in EOSPlatformInclude.h

//#include "EOSPlatformBase.h"
//
//class EOSStat :private PlatformBase {
//private:
//	using CallbackType = std::function<void(EOSStat*)>;
//	inline const static CallbackType defaultCb = [] (EOSStat*) {};
//
//public:
//	explicit EOSStat(EOSUtilities* pEU) : PlatformBase(pEU) {}
//	~EOSStat() override = default;
//	inline void PlatformInit() override {}
//	inline void PlatformQuery() override {}
//};