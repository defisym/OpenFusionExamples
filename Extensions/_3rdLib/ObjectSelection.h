#pragma once

// Origin version by Anders Riggelsen (Andos)

// Add functional to support lambda
// Add For-Each support
// Add several handy funtions

#include <functional>
#include <vector>

#include "ccxhdr.h"

constexpr auto ForEachFlag_ForceAll = 0b00000001;
constexpr auto ForEachFlag_SelectedOnly = 0b00000010;

class ObjectSelection {
private:
	using Filter = std::function<bool(LPRDATA, LPRO)>;
	using ForEachCallBack = std::function<void(LPRO)>;

	LPRH rhPtr;
	LPOBL ObjectList;
	LPOIL OiList;
	LPQOI QualToOiList;		// need update every time before using
	int oiListItemSize;

	inline bool FilterQualifierObjects(LPRDATA rdPtr, short oiList, bool negate
		, Filter filterFunction
		, bool selectDestroyed = true) {
		bool hasSelected = false;

		IterateQualifier(oiList, [&] (objInfoList* pOil, short qoiOiList) {
			hasSelected |= FilterNonQualifierObjects(rdPtr, qoiOiList, negate
				, filterFunction
				, selectDestroyed);
			});

		return hasSelected;
	}

	inline bool FilterNonQualifierObjects(LPRDATA rdPtr, short oiList, bool negate
		, Filter filterFunction
		, bool selectDestroyed = true) {
		LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return false;
		}

		if (pObjectInfo->oilEventCount != rhPtr->rh2.rh2EventCount) {
			SelectAll(oiList);	//The SOL is invalid, must reset.
		}

		//If SOL is empty
		if (pObjectInfo->oilNumOfSelected <= 0) {
			return false;
		}

		bool hasSelected = false;

		int count = 0;
		int firstSelected = -1;

		LPHO previous = nullptr;

		IterateObject(pObjectInfo, [&] (LPRO pObject, short oblOffset) {
			bool useObject = filterFunction(rdPtr, (LPRO)pObject) ^ negate;

			hasSelected |= useObject;

			if (useObject) {
				if (firstSelected == -1) {
					firstSelected = oblOffset;
				}

				if (previous != nullptr) {
					previous->hoNextSelected = oblOffset;
				}

				previous = (LPHO)pObject;
				count++;
			}

			}, true, selectDestroyed);

		if (previous != nullptr) {
			previous->hoNextSelected = -1;
		}

		pObjectInfo->oilListSelected = firstSelected;
		pObjectInfo->oilNumOfSelected = count;

		return hasSelected;
	}

	inline void IterateQualifier(short oiList, std::function<void(objInfoList*, short)> f) {
		if (!ObjectIsQualifier(oiList)) {
			return;
		}

		oiList = oiList & 0x7FFF;

		LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return;
		}

		this->QualToOiList = rhPtr->rhQualToOiList;
		LPQOI CurrentQualToOiStart = (LPQOI)((char*)QualToOiList + oiList);
		LPQOI CurrentQualToOi = CurrentQualToOiStart;

		if (CurrentQualToOi == nullptr) {
			return;
		}

		while (CurrentQualToOi->qoiOiList >= 0) {
			f(OiList + CurrentQualToOi->qoiOiList, CurrentQualToOi->qoiOiList);
			CurrentQualToOi = (LPQOI)((char*)CurrentQualToOi + 4);

			if (CurrentQualToOi == nullptr) {
				break;
			}
		}
	}

	inline void IterateObject(objInfoList* list
		, std::function<void(LPRO, short)> f
		, bool selected = true
		, bool selectDestroyed = false) {
		short oblOffset = selected ? list->oilListSelected : list->oilObject;

		while (oblOffset >= 0) {
			RunObject* pObj = reinterpret_cast<RunObject*>(rhPtr->rhObjectList[oblOffset].oblOffset);

			if (pObj == nullptr) {
				continue;
			}

			if (selectDestroyed
				|| (pObj && !IsDestroyed(pObj))) {
				f(pObj, oblOffset);
			}

			oblOffset = selected ? pObj->roHo.hoNextSelected : pObj->roHo.hoNumNext;
		}
	}

	inline void IterateOiL(std::function<void(LPOIL)> callBack) {
		auto oiLSz = rhPtr->rhNumberOi;

		for (int i = 0; i < oiLSz; i++) {
			auto pCurOi = GetLPOIL(i);

			if (pCurOi == nullptr) {
				continue;
			}

			callBack(pCurOi);
		}
	}

public:
	ObjectSelection(LPRH rhPtr) {
		this->rhPtr = rhPtr;
		this->ObjectList = rhPtr->rhObjectList;		//get a pointer to the mmf object list
		this->OiList = rhPtr->rhOiList;				//get a pointer to the mmf object info list
		this->QualToOiList = rhPtr->rhQualToOiList;	//get a pointer to the mmf qualifier to Oi list

		//oiListItemSize = sizeof(objInfoList);
		oiListItemSize = sizeof(objInfoList) + sizeof(LPVOID);

//		//Only add the sizes to the runtime structures if they weren't compiled directly for those runtimes
//#ifndef UNICODE
//		if (rhPtr->rh4.rh4Mv->mvCallFunction(NULL, EF_ISUNICODE, 0, 0, 0))
//			oiListItemSize += 24;
//#endif
//#ifndef HWABETA
//		if (rhPtr->rh4.rh4Mv->mvCallFunction(NULL, EF_ISHWA, 0, 0, 0))
//			oiListItemSize += sizeof(LPVOID);
//#endif
	}

	//Get oil
	inline static short GetOil(LPRDATA rdPtr, LPRO* pObj = nullptr) {
		// pParam points to 1st parameter, which should be of type object
		// Offset it if you have used second or other parameter for object selection using code below
		// LPEVP pParam2nd = (LPEVP)((LPBYTE)pParam + pParam->evpSize);
		LPEVP pParam = rdPtr->rHo.hoCurrentParam;
		short oil = (short)pParam->evp.evpW.evpW0;

		// Don't forget to skip to next param
		LPRO object = (LPRO)CNC_GetParameter(rdPtr);

		if (pObj != nullptr) {
			*pObj = object;
		}

		return oil;
	}

#define OIL_GetParameter(rdPtr) ObjectSelection::GetOil(rdPtr)

	// Get LPOIL
	inline LPOIL GetLPOIL(short oiList) {
		return (LPOIL)((char*)OiList + oiListItemSize * oiList);
	}

	// Get Oi for creation
	inline short GetOiFromOiList(short oiList) {
		LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return -1;
		}

		// you cannot create qualifier
		if (oiList & 0x8000) {
			return -1;
		}

		return pObjectInfo->oilOi;
	}

	//Get Neg, must be called at first
	inline static bool IsNegated(LPRDATA rdPtr) {
		// pParam points to 1st parameter, which should be of type object
		// Offset it if you have used second or other parameter for object selection using code below
		// LPEVP pParam2nd = (LPEVP)((LPBYTE)pParam + pParam->evpSize);
		LPEVP pParam = rdPtr->rHo.hoCurrentParam;
		bool isNegated = ((event*)((LPBYTE)rdPtr->rHo.hoCurrentParam - CND_SIZE))->evtFlags2 & EVFLAG2_NOT;

		return isNegated;
	}

#define IsNegated(rdPtr) ObjectSelection::IsNegated(rdPtr)

	inline static bool IsDestroyed(LPRO pObj) {
		return (static_cast<ushort>(pObj->roHo.hoFlags) & static_cast<ushort>(HOF_DESTROYED));
	}

#define IsDestroyed(pObj) ObjectSelection::IsDestroyed(pObj)

	//Get first obj
	inline LPRO GetFirstObject(objInfoList* pOil, bool selected = false) {
		if (pOil->oilObject < 0) {
			return nullptr;
		}

		auto offset = selected
			? pOil->oilListSelected
			: pOil->oilObject;

		return offset < 0
			? nullptr
			: (LPRO)ObjectList[offset].oblOffset;
	}

	inline LPRO GetFirstObject(short oiList, bool selected = false) {
		return GetFirstObject(this->GetLPOIL(oiList), selected);
	}

	inline bool Selected(objInfoList* pObjectInfo) {
		return (pObjectInfo != nullptr)
			&& (pObjectInfo->oilEventCount = rhPtr->rh2.rh2EventCount)
			&& (pObjectInfo->oilListSelected > 0);
	}

	inline bool Selected(short oiList) {
		return Selected(this->GetLPOIL(oiList));
	}

	//Selects *all* objects of the given object-type
	inline void SelectAll(short oiList) {
		const LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return;
		}

		pObjectInfo->oilNumOfSelected = pObjectInfo->oilNObjects;
		pObjectInfo->oilListSelected = pObjectInfo->oilObject;
		pObjectInfo->oilEventCount = rhPtr->rh2.rh2EventCount;

		int i = pObjectInfo->oilObject;
		while (i >= 0) {
			LPHO pObject = ObjectList[i].oblOffset;
			pObject->hoNextSelected = pObject->hoNumNext;
			i = pObject->hoNumNext;
		}
	}

	//Resets all objects of the given object-type
	inline void SelectNone(short oiList) {
		LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return;
		}

		pObjectInfo->oilNumOfSelected = 0;
		pObjectInfo->oilListSelected = -1;
		pObjectInfo->oilEventCount = rhPtr->rh2.rh2EventCount;
	}

	//Resets the SOL and inserts only one given object
	inline void SelectOneObject(LPRO object) {
		// LPOIL pObjectInfo = object->roHo.hoOiList;
		LPOIL pObjectInfo = GetLPOIL(object->roHo.hoOi);

		if (pObjectInfo == nullptr) {
			return;
		}

		pObjectInfo->oilNumOfSelected = 1;
		pObjectInfo->oilEventCount = rhPtr->rh2.rh2EventCount;
		pObjectInfo->oilListSelected = object->roHo.hoNumber;
		ObjectList[object->roHo.hoNumber].oblOffset->hoNextSelected = -1;
	}

	//Resets the SOL and inserts the given list of objects
	inline void SelectObjects(short oiList, const LPRO* objects, size_t count) {
		if (count == 0) {
			return;
		}

		LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return;
		}

		pObjectInfo->oilNumOfSelected = count;
		pObjectInfo->oilEventCount = rhPtr->rh2.rh2EventCount;

		short prevNumber = objects[0]->roHo.hoNumber;
		pObjectInfo->oilListSelected = prevNumber;

		for (size_t i = 1; i < count; i++) {
			short currentNumber = objects[i]->roHo.hoNumber;
			ObjectList[prevNumber].oblOffset->hoNextSelected = currentNumber;
			prevNumber = currentNumber;
		}
		ObjectList[prevNumber].oblOffset->hoNextSelected = -1;
	}
	inline void SelectObjects(short oiList, const std::vector<LPRO> objects) {
		if (objects.empty()) {
			return;
		}

		SelectObjects(oiList, &objects[0], objects.size());
	}

	//Run a custom filter on the SOL (via function callback)
	inline bool FilterObjects(LPRDATA rdPtr, short oiList, bool negate
		, Filter filterFunction
		, bool selectDestroyed = true) {
		LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return false;
		}

		if (oiList & 0x8000) {
			return FilterQualifierObjects(rdPtr, oiList & 0x7FFF, negate
				, filterFunction
				, selectDestroyed) ^ negate;
		}
		else {
			return FilterNonQualifierObjects(rdPtr, oiList, negate
				, filterFunction
				, selectDestroyed) ^ negate;
		}
	}

	//Return the number of selected objects for the given object-type
	inline int GetNumberOfSelected(short oiList) {
		LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return 0;
		}

		if (oiList & 0x8000) {
			oiList &= 0x7FFF;	//Mask out the qualifier part
			int numberSelected = 0;

			this->QualToOiList = rhPtr->rhQualToOiList;
			LPQOI CurrentQualToOiStart = (LPQOI)((char*)QualToOiList + oiList);
			LPQOI CurrentQualToOi = CurrentQualToOiStart;

			while (CurrentQualToOi->qoiOiList >= 0) {
				LPOIL CurrentOi = GetLPOIL(CurrentQualToOi->qoiOiList);
				numberSelected += CurrentOi->oilNumOfSelected;
				CurrentQualToOi = (LPQOI)((char*)CurrentQualToOi + 4);
			}
			return numberSelected;
		}
		else {
			LPOIL pObjectInfo = GetLPOIL(oiList);
			return pObjectInfo->oilNumOfSelected;
		}
	}

	//Return the number of objects for the given object-type
	inline int GetNumberOfObject(short oiList) {
		LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return 0;
		}

		if (oiList & 0x8000) {
			oiList &= 0x7FFF;	//Mask out the qualifier part
			int number = 0;

			this->QualToOiList = rhPtr->rhQualToOiList;
			LPQOI CurrentQualToOiStart = (LPQOI)((char*)QualToOiList + oiList);
			LPQOI CurrentQualToOi = CurrentQualToOiStart;

			while (CurrentQualToOi->qoiOiList >= 0) {
				LPOIL CurrentOi = GetLPOIL(CurrentQualToOi->qoiOiList);
				number += CurrentOi->oilNObjects;
				CurrentQualToOi = (LPQOI)((char*)CurrentQualToOi + 4);
			}
			return number;
		}
		else {
			LPOIL pObjectInfo = GetLPOIL(oiList);
			return pObjectInfo->oilNObjects;
		}
	}

	//Check if object is given type
	inline bool ObjectIsOfType(LPRO object, short oiList) {
		if (oiList & 0x8000) {
			oiList &= 0x7FFF;	//Mask out the qualifier part

			this->QualToOiList = rhPtr->rhQualToOiList;
			LPQOI CurrentQualToOiStart = (LPQOI)((char*)QualToOiList + oiList);
			LPQOI CurrentQualToOi = CurrentQualToOiStart;

			while (CurrentQualToOi->qoiOiList >= 0) {
				LPOIL CurrentOi = GetLPOIL(CurrentQualToOi->qoiOiList);

				if (CurrentOi->oilOi == object->roHo.hoOi) {
					return true;
				}

				CurrentQualToOi = (LPQOI)((char*)CurrentQualToOi + 4);
			}
			return false;
		}
		else {
			LPOIL pObjectInfo = GetLPOIL(oiList);

			if (pObjectInfo == nullptr) {
				return false;
			}

			short Oi = pObjectInfo->oilOi;

			return (object->roHo.hoOi == Oi);
		}
	}

	//Check if object is qualifier
	inline bool ObjectIsQualifier(short oiList) {
		return (oiList & 0x8000);
	}

	//Check if object is selected
	inline bool ObjectIsSelected(objInfoList* list) {
		if (list == nullptr) {
			return false;
		}

		return list->oilEventCount == rhPtr->rh2.rh2EventCount;
	}

	inline bool ObjectIsSelected(short oiList) {
		if (!ObjectIsQualifier(oiList)) {
			return ObjectIsSelected(OiList + oiList);
		}
		else {
			bool selected = false;

			IterateQualifier(oiList, [&] (objInfoList* list, short qoiOiList) {
				selected |= ObjectIsSelected(list);
				});

			return selected;
		}

		return false;
	}

	//For Each, used in action
	inline void ForEach(LPRDATA rdPtr, short oiList, ForEachCallBack f, char flag = 0x00) {
		bool forceAll = flag & ForEachFlag_ForceAll;			// force iterate all
		bool selectedOnly = flag & ForEachFlag_SelectedOnly;		// only iterate selected

		auto iterateCall = [&] (objInfoList* list) {
			if (forceAll) {
				IterateObject(list, [f] (LPRO pObject, short oblOffset) {
					f(pObject);
					}, false);

				return;
			}

			if (list->oilEventCount == rhPtr->rh2.rh2EventCount) {
				// Object type already filtered, loop through selected list
				IterateObject(list, [f] (LPRO pObject, short oblOffset) {
					f(pObject);
					}, true);
			}
			else if (!selectedOnly) {
				// Object type not filtered yet, loop through all objects instead
				IterateObject(list, [f] (LPRO pObject, short oblOffset) {
					f(pObject);
					}, false);
			}
		};

		// Single object type
		if (oiList >= 0) {
			LPOIL pObjectInfo = GetLPOIL(oiList);

			if (pObjectInfo == nullptr) {
				return;
			}

			iterateCall(OiList + oiList);
		}
		// Qualifier object type
		else if (oiList != -1) {
			IterateQualifier(oiList, [iterateCall] (objInfoList* pOil, short qoiOiList) {
				iterateCall(pOil);
				});
		}
	}

	//Iterate given identifier
	inline void IterateObjectWithIdentifier(LPRDATA rdPtr, const int identifier, ForEachCallBack callBack, bool selected = false) {
		IterateOiL([&] (LPOIL pOil) {
			LPRO pObject = GetFirstObject(pOil, selected);

			if (pObject == nullptr) {
				return;
			}

			auto objIdentifier = pObject->roHo.hoIdentifier;

			if (objIdentifier == identifier) {
				IterateObject(pOil, [callBack] (LPRO pObject, short oblOffset) {
					callBack(pObject);
					}, selected);
			}
			});
	}

	inline bool OILHasObject(const std::wstring& objName) {
		return OILHasObject(objName.c_str());
	}

	inline bool OILHasObject(LPCWSTR pObjName) {
		bool bHas = false;

		IterateOiL([&](LPOIL pOil) {
			// oilName start with empty char
			auto pCurName = [&]() {
				auto pOilName = pOil->oilName;

				while (pOilName[0] == 65535) {
					pOilName++;
				}

				return pOilName;
			}();

			if (StrEqu(pObjName, pCurName)) {
				bHas = true;
			}
			});

		return bHas;
	}

	//TODO Save scope
	inline auto SaveScope() {
		auto rhEvCount = rhPtr->rh2.rh2EventCount;

		auto pOiL = rhPtr->rhOiList;
		auto oiLSz = rhPtr->rhNumberOi;

		for (int i = 0; i < oiLSz; i++) {
			auto pCurOi = GetLPOIL(i);

			if (pCurOi == nullptr) {
				continue;
			}

			auto evCount = pCurOi->oilEventCount;
		}
	}
};
