// ReSharper disable CppClangTidyClangDiagnosticShadow
// ReSharper disable CppClangTidyClangDiagnosticShadowField

#pragma once

#include "ccxhdr.h"

class EventBase{
public:
	//Get Neg, must be called at first
	inline static bool IsNegated(const LPRDATA rdPtr) {
		// pParam points to 1st parameter, which should be of type object
		// Offset it if you have used second or other parameter for object selection using code below
		// LPEVP pParam2nd = (LPEVP)((LPBYTE)pParam + pParam->evpSize);
		const LPEVP pParam = rdPtr->rHo.hoCurrentParam;
		const bool isNegated = reinterpret_cast<event*>(reinterpret_cast<LPBYTE>(pParam) - CND_SIZE)->evtFlags2 & EVFLAG2_NOT;

		return isNegated;
	}

#define IsNegated(rdPtr) EventBase::IsNegated(rdPtr)

	inline static bool IsDestroyed(const LPRO pObj) {
		return (static_cast<ushort>(pObj->roHo.hoFlags) & static_cast<ushort>(HOF_DESTROYED));
	}

#define IsDestroyed(pObj) EventBase::IsDestroyed(pObj)

	// normally get current param then read next
	inline static eventParam* GetEvtParam(const LPRDATA rdPtr) {
		eventParam* param = rdPtr->rHo.hoCurrentParam;
		CNC_GetParameter(rdPtr);

		return param;
	}

	// must be called at first, starts from 0
	inline static eventParam* GetNthEvtParam(const LPRDATA rdPtr, const size_t index = 0) {
		eventParam* param = rdPtr->rHo.hoCurrentParam;
		param = reinterpret_cast<eventParam*>(reinterpret_cast<unsigned char*>(param) + param->evpSize * index);

		return param;
	}
};