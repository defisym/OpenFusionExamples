// ReSharper disable CppClangTidyClangDiagnosticShadow
// ReSharper disable CppClangTidyClangDiagnosticShadowField

#pragma once

#ifdef _DEBUG
#include <cassert>
#endif

#include <functional>
#include <string>

#include "ccxhdr.h"
#include "GeneralDefinition.h"

class ObjectBase {
protected:
	LPRH rhPtr;
	LPOBL ObjectList;
	LPOIL OiList;
	LPQOI QualToOiList;		// need to be updated each time before using

	// For 2.5 HWA only:
	int oiListItemSize = sizeof(objInfoList);

	//Only add the sizes to the runtime structures if they weren't compiled directly for those runtimes
//#ifndef UNICODE
//		if (rhPtr->rh4.rh4Mv->mvCallFunction(NULL, EF_ISUNICODE, 0, 0, 0))
//			oiListItemSize += 24;
//#endif
//#ifndef HWABETA
//		if (rhPtr->rh4.rh4Mv->mvCallFunction(NULL, EF_ISHWA, 0, 0, 0))
//			oiListItemSize += sizeof(LPVOID);
//#endif

	inline void IterateOiL(const std::function<void(LPOIL)>& callBack) const {
		const auto oiLSz = rhPtr->rhNumberOi;

		for (int i = 0; i < oiLSz; i++) {
			const auto pCurOi = GetLPOIL(static_cast<short>(i));

			if (pCurOi == nullptr) {
				continue;
			}

			callBack(pCurOi);
		}
	}

public:
	explicit ObjectBase(const LPRDATA rdPtr)
		:ObjectBase(rdPtr->rHo.hoAdRunHeader) {
	}
	explicit ObjectBase(const LPRH rhPtr) {
		this->rhPtr = rhPtr;
		this->ObjectList = rhPtr->rhObjectList;		//get a pointer to the mmf object list
		this->OiList = rhPtr->rhOiList;				//get a pointer to the mmf object info list
		this->QualToOiList = rhPtr->rhQualToOiList;	//get a pointer to the mmf qualifier to Oi list
	}

	static inline bool CompareObjectName(const LPCWSTR pObjectName, const LPCWSTR pCompareName) {
		// oilName may start with empty char
		const auto pCurName = [&] () {
			auto pOilName = pObjectName;

			while (pOilName[0] == 65535) {
				pOilName++;
			}

			return pOilName;
			}();

			return StrEqu(pCompareName, pCurName);
	}

	static inline bool CompareObjectName(const LPOIL pOil, const LPCWSTR pCompareName) {
		return CompareObjectName(pOil->oilName, pCompareName);
	}

	//Get oil
	inline static short GetOil(const LPRDATA rdPtr, LPRO* pObj = nullptr) {
		// pParam points to 1st parameter, which should be of type object
		// Offset it if you have used second or other parameter for object selection using code below
		// LPEVP pParam2nd = (LPEVP)((LPBYTE)pParam + pParam->evpSize);
		const LPEVP pParam = rdPtr->rHo.hoCurrentParam;
		const auto oil = (short)pParam->evp.evpW.evpW0;

		// Don't forget to skip to next param
		const LPRO object = reinterpret_cast<LPRO>(CNC_GetParameter(rdPtr));

		if (pObj != nullptr) {
			*pObj = object;
		}

		return oil;
	}

#define OIL_GetParameter(rdPtr) ObjectBase::GetOil(rdPtr)

	// Get LPOIL
	inline LPOIL GetLPOIL(const LPRO object) const {
		return GetLPOIL(object->roHo.hoOi);
	}

	inline LPOIL GetLPOIL(const short oiList) const {
		return reinterpret_cast<LPOIL>(reinterpret_cast<char*>(OiList) + oiListItemSize * oiList);
	}

	// Get Oi for creation
	inline short GetOiFromOiList(const short oiList) const {
		const LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return -1;
		}

		// you cannot create qualifier
		if (oiList & 0x8000) {
			return -1;
		}

		return pObjectInfo->oilOi;
	}

	inline LPOIL GetOILByName(const std::wstring& objName) const {
		return GetOILByName(objName.c_str());
	}

	inline LPOIL GetOILByName(const LPCWSTR pObjName) const {
		LPOIL pOil = nullptr;

		IterateOiL([&] (const LPOIL _pOil) {
			if (CompareObjectName(_pOil, pObjName)) {
				pOil = _pOil;
			}
			});

		return pOil;
	}

	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	inline bool OILHasObject(std::wstring& objName) const {
		return OILHasObject(objName.c_str());
	}

	inline bool OILHasObject(const LPCWSTR pObjName) const {
		return GetOILByName(pObjName) != nullptr;
	}

	inline bool OILHasObject(const LPOIL pOil) const {
		bool bHas = false;

		IterateOiL([&] (const LPOIL _pOil) {
			if (pOil == _pOil) { bHas = true; }
			});

		return bHas;
	}
};