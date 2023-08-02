#pragma once

#include "EOSUtilities.h"

class PlatformBase {
protected:
	EOSUtilities* pEU = nullptr;

public:
	explicit PlatformBase(EOSUtilities* pEU) {
		this->pEU = pEU;
	}
	virtual ~PlatformBase() = default;

	virtual inline void PlatformInit() = 0;
	virtual inline void PlatformUpdate() = 0;

	inline bool PlatformOK() const {
		return pEU->PlatformOK();
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
//	inline void PlatformUpdate() override {}
//};