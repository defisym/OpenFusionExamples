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

	inline bool PlatformOK() const {
		return pEU->state == EOSState::ConnectSuccess;
	}
};

// ------------
// template
// ------------

// Don't forget to add as friend class of EOSUtilities

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
//};