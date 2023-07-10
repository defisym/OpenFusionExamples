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

	using SelObj = std::vector<LPRO>;

	struct Scope {
		// State
		BYTE		rh2ActionLoop;
		BYTE 		rh2ActionOn;

		int		  	rh2ActionCount;
		int		  	rh2ActionLoopCount;

		// Objects
		std::map<LPOIL, SelObj> objects;

	public:
		explicit Scope(const LPRH rhPtr) {
			rh2ActionLoop = rhPtr->rh2.rh2ActionLoop;
			rh2ActionOn = rhPtr->rh2.rh2ActionOn;

			rh2ActionCount = rhPtr->rh2.rh2ActionCount;
			rh2ActionLoopCount = rhPtr->rh2.rh2ActionLoopCount;
		}

		inline void RestoreActionState(const LPRH rhPtr) const {
			// only restore rh2ActionLoop is enough
			rhPtr->rh2.rh2ActionLoop = rh2ActionLoop;
			rhPtr->rh2.rh2ActionOn = rh2ActionOn;

			rhPtr->rh2.rh2ActionCount = rh2ActionCount;
			rhPtr->rh2.rh2ActionLoopCount = rh2ActionLoopCount;
		}

		inline void AddSelObj(LPOIL pOil, const SelObj& selObj) {
			objects.emplace(pOil, selObj);
		}

		inline const auto& GetSelObj() const {
			return objects;
		}
	};	

	LPRH rhPtr;
	LPOBL ObjectList;
	LPOIL OiList;
	LPQOI QualToOiList;		// need update every time before using
	int oiListItemSize;

	inline bool FilterQualifierObjects(const LPRDATA rdPtr, const short oiList, const bool negate
		, const Filter& filterFunction
		, const bool selectDestroyed = true) {
		bool hasSelected = false;

		IterateQualifier(oiList, [&] (objInfoList* pOil, const short qoiOiList) {
			hasSelected |= FilterNonQualifierObjects(rdPtr, qoiOiList, negate
				, filterFunction
				, selectDestroyed);
			});

		return hasSelected;
	}

	inline bool FilterNonQualifierObjects(const LPRDATA rdPtr, const short oiList, const bool negate
		, const Filter& filterFunction
		, const bool selectDestroyed = true) const {
		const LPOIL pObjectInfo = GetLPOIL(oiList);

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

		IterateObject(pObjectInfo, [&] (const LPRO pObject, const short oblOffset) {
			const bool useObject = filterFunction(rdPtr, (LPRO)pObject) ^ negate;

			hasSelected |= useObject;

			if (useObject) {
				if (firstSelected == -1) {
					firstSelected = oblOffset;
				}

				if (previous != nullptr) {
					previous->hoNextSelected = oblOffset;
				}

				previous = reinterpret_cast<LPHO>(pObject);
				count++;
			}

			}, true, selectDestroyed);

		if (previous != nullptr) {
			previous->hoNextSelected = -1;
		}

		pObjectInfo->oilListSelected = static_cast<short>(firstSelected);
		pObjectInfo->oilNumOfSelected = count;

		return hasSelected;
	}

	inline void IterateQualifier(short oiList, const std::function<void(objInfoList*, short)>& f) {
		if (!ObjectIsQualifier(oiList)) {
			return;
		}

		oiList = oiList & static_cast<short>(0x7FFF);

		const LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return;
		}

		this->QualToOiList = rhPtr->rhQualToOiList;
		const auto CurrentQualToOiStart = reinterpret_cast<LPQOI>(reinterpret_cast<char*>(QualToOiList) + oiList);
		LPQOI CurrentQualToOi = CurrentQualToOiStart;

		if (CurrentQualToOi == nullptr) {
			return;
		}

		while (CurrentQualToOi->qoiOiList >= 0) {
			f(OiList + CurrentQualToOi->qoiOiList, CurrentQualToOi->qoiOiList);
			CurrentQualToOi = reinterpret_cast<LPQOI>(reinterpret_cast<char*>(CurrentQualToOi) + 4);

			if (CurrentQualToOi == nullptr) {
				break;
			}
		}
	}

	inline void IterateObject(const objInfoList* list
		, const std::function<void(LPRO, short)>& f
		, const bool selected = true
		, const bool selectDestroyed = false) const {
		short oblOffset = selected ? list->oilListSelected : list->oilObject;

		while (oblOffset >= 0) {
			const auto pObj = reinterpret_cast<RunObject*>(rhPtr->rhObjectList[oblOffset].oblOffset);

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
	explicit ObjectSelection(const LPRH rhPtr) {
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

#define OIL_GetParameter(rdPtr) ObjectSelection::GetOil(rdPtr)

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

	//Get Neg, must be called at first
	inline static bool IsNegated(const LPRDATA rdPtr) {
		// pParam points to 1st parameter, which should be of type object
		// Offset it if you have used second or other parameter for object selection using code below
		// LPEVP pParam2nd = (LPEVP)((LPBYTE)pParam + pParam->evpSize);
		LPEVP pParam = rdPtr->rHo.hoCurrentParam;
		const bool isNegated = reinterpret_cast<event*>(reinterpret_cast<LPBYTE>(rdPtr->rHo.hoCurrentParam) - CND_SIZE)->evtFlags2 & EVFLAG2_NOT;

		return isNegated;
	}

#define IsNegated(rdPtr) ObjectSelection::IsNegated(rdPtr)

	inline static bool IsDestroyed(const LPRO pObj) {
		return (static_cast<ushort>(pObj->roHo.hoFlags) & static_cast<ushort>(HOF_DESTROYED));
	}

#define IsDestroyed(pObj) ObjectSelection::IsDestroyed(pObj)

	//Get first obj
	inline LPRO GetFirstObject(const objInfoList* pOil, const bool selected = false) const {
		if (pOil->oilObject < 0) {
			return nullptr;
		}

		const auto offset = selected
			                    ? pOil->oilListSelected
			                    : pOil->oilObject;

		return offset < 0
			? nullptr
			: reinterpret_cast<LPRO>(ObjectList[offset].oblOffset);
	}

	inline LPRO GetFirstObject(const short oiList, const bool selected = false) const {
		return GetFirstObject(this->GetLPOIL(oiList), selected);
	}

	inline bool Selected(const objInfoList* pObjectInfo) const {
		return (pObjectInfo != nullptr)
			&& (pObjectInfo->oilEventCount == rhPtr->rh2.rh2EventCount)
			&& (pObjectInfo->oilListSelected > 0);
	}

	inline bool Selected(const short oiList) const {
		return Selected(this->GetLPOIL(oiList));
	}

	//Selects *all* objects of the given object-type
	inline void SelectAll(const short oiList) const {
		const LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return;
		}

		pObjectInfo->oilNumOfSelected = pObjectInfo->oilNObjects;
		pObjectInfo->oilListSelected = pObjectInfo->oilObject;
		pObjectInfo->oilEventCount = rhPtr->rh2.rh2EventCount;

		int i = pObjectInfo->oilObject;
		while (i >= 0) {
			const LPHO pObject = ObjectList[i].oblOffset;
			pObject->hoNextSelected = pObject->hoNumNext;
			i = pObject->hoNumNext;
		}
	}

	//Resets all objects of the given object-type
	inline void SelectNone(const short oiList) const {
		const LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return;
		}

		pObjectInfo->oilNumOfSelected = 0;
		pObjectInfo->oilListSelected = -1;
		pObjectInfo->oilEventCount = rhPtr->rh2.rh2EventCount;
	}

	//Resets the SOL and inserts only one given object
	inline void SelectOneObject(const LPRO object) const {
		// LPOIL pObjectInfo = object->roHo.hoOiList;
		const LPOIL pObjectInfo = GetLPOIL(object);

		if (pObjectInfo == nullptr) {
			return;
		}

		pObjectInfo->oilNumOfSelected = 1;
		pObjectInfo->oilEventCount = rhPtr->rh2.rh2EventCount;
		pObjectInfo->oilListSelected = object->roHo.hoNumber;
		ObjectList[object->roHo.hoNumber].oblOffset->hoNextSelected = -1;
	}

	//Resets the SOL and inserts the given list of objects
	inline void SelectObjects(const LPOIL pObjectInfo, const LPRO* objects, const size_t count) const {
		if (count == 0) {
			return;
		}
		
		if (pObjectInfo == nullptr) {
			return;
		}

		pObjectInfo->oilNumOfSelected = static_cast<int>(count);
		pObjectInfo->oilEventCount = rhPtr->rh2.rh2EventCount;

		short prevNumber = objects[0]->roHo.hoNumber;
		pObjectInfo->oilListSelected = prevNumber;

		for (size_t i = 1; i < count; i++) {
			const short currentNumber = objects[i]->roHo.hoNumber;
			ObjectList[prevNumber].oblOffset->hoNextSelected = currentNumber;
			prevNumber = currentNumber;
		}
		ObjectList[prevNumber].oblOffset->hoNextSelected = -1;
	}

	inline void SelectObjects(const short oiList, const LPRO* objects, const size_t count) const {
		SelectObjects(GetLPOIL(oiList), objects, count);
	}

	inline void SelectObjects(const LPOIL pObjectInfo, const std::vector<LPRO>& objects) const {
		if (objects.empty()) {
			return;
		}

		SelectObjects(pObjectInfo, objects.data(), objects.size());
	}

	inline void SelectObjects(const short oiList, const std::vector<LPRO>& objects) const {
		SelectObjects(GetLPOIL(oiList), objects);
	}

	//Run a custom filter on the SOL (via function callback)
	inline bool FilterObjects(const LPRDATA rdPtr, const short oiList, const bool negate
		, const Filter& filterFunction
		, const bool selectDestroyed = true) {
		const LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return false;
		}

		if (oiList & 0x8000) {
			return FilterQualifierObjects(rdPtr, static_cast<short>(oiList & 0x7FFF), negate
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
		const LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return 0;
		}

		if (oiList & 0x8000) {
			oiList &= 0x7FFF;	//Mask out the qualifier part
			int numberSelected = 0;

			this->QualToOiList = rhPtr->rhQualToOiList;
			const auto CurrentQualToOiStart = reinterpret_cast<LPQOI>(reinterpret_cast<char*>(QualToOiList) + oiList);
			LPQOI CurrentQualToOi = CurrentQualToOiStart;

			while (CurrentQualToOi->qoiOiList >= 0) {
				const LPOIL CurrentOi = GetLPOIL(CurrentQualToOi->qoiOiList);
				numberSelected += CurrentOi->oilNumOfSelected;
				CurrentQualToOi = reinterpret_cast<LPQOI>(reinterpret_cast<char*>(CurrentQualToOi) + 4);
			}

			return numberSelected;
		}
		else {
			return pObjectInfo->oilNumOfSelected;
		}
	}

	//Return the number of objects for the given object-type
	inline int GetNumberOfObject(short oiList) {
		const LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return 0;
		}

		if (oiList & 0x8000) {
			oiList &= 0x7FFF;	//Mask out the qualifier part
			int number = 0;

			this->QualToOiList = rhPtr->rhQualToOiList;
			const auto CurrentQualToOiStart = reinterpret_cast<LPQOI>(reinterpret_cast<char*>(QualToOiList) + oiList);
			LPQOI CurrentQualToOi = CurrentQualToOiStart;

			while (CurrentQualToOi->qoiOiList >= 0) {
				const LPOIL CurrentOi = GetLPOIL(CurrentQualToOi->qoiOiList);
				number += CurrentOi->oilNObjects;
				CurrentQualToOi = reinterpret_cast<LPQOI>(reinterpret_cast<char*>(CurrentQualToOi) + 4);
			}
			return number;
		}
		else {
			return pObjectInfo->oilNObjects;
		}
	}

	//Check if object is given type
	inline bool ObjectIsOfType(const LPRO object, short oiList) {
		if (oiList & 0x8000) {
			oiList &= 0x7FFF;	//Mask out the qualifier part

			this->QualToOiList = rhPtr->rhQualToOiList;
			const auto CurrentQualToOiStart = reinterpret_cast<LPQOI>(reinterpret_cast<char*>(QualToOiList) + oiList);
			LPQOI CurrentQualToOi = CurrentQualToOiStart;

			while (CurrentQualToOi->qoiOiList >= 0) {
				const LPOIL CurrentOi = GetLPOIL(CurrentQualToOi->qoiOiList);

				if (CurrentOi->oilOi == object->roHo.hoOi) {
					return true;
				}

				CurrentQualToOi = reinterpret_cast<LPQOI>(reinterpret_cast<char*>(CurrentQualToOi) + 4);
			}
			return false;
		}
		else {
			const LPOIL pObjectInfo = GetLPOIL(oiList);

			if (pObjectInfo == nullptr) {
				return false;
			}

			const short Oi = pObjectInfo->oilOi;

			return (object->roHo.hoOi == Oi);
		}
	}

	//Check if object is qualifier
	static inline bool ObjectIsQualifier(const short oiList) {
		return (oiList & 0x8000);
	}

	//Check if object is selected
	inline bool ObjectIsSelected(const objInfoList* list) const {
		if (list == nullptr) {
			return false;
		}

		return list->oilEventCount == rhPtr->rh2.rh2EventCount;
	}

	inline bool ObjectIsSelected(const short oiList) {
		if (!ObjectIsQualifier(oiList)) {
			return ObjectIsSelected(OiList + oiList);
		}
		else {
			bool selected = false;

			IterateQualifier(oiList, [&] (const objInfoList* list, short qoiOiList) {
				selected |= ObjectIsSelected(list);
				});

			return selected;
		}

		return false;
	}

	//For Each, used in action
	inline void ForEach(LPRDATA rdPtr, const short oiList, const ForEachCallBack& f, const char flag = 0x00) {
		const bool forceAll = flag & ForEachFlag_ForceAll;         // force iterate all
		const bool selectedOnly = flag & ForEachFlag_SelectedOnly; // only iterate selected

		auto iterateCall = [&] (const objInfoList* list) {
			if (forceAll) {
				IterateObject(list, [f] (const LPRO pObject, short oblOffset) {
					f(pObject);
					}, false);

				return;
			}

			if (list->oilEventCount == rhPtr->rh2.rh2EventCount) {
				// Object type already filtered, loop through selected list
				IterateObject(list, [f] (const LPRO pObject, short oblOffset) {
					f(pObject);
					}, true);
			}
			else if (!selectedOnly) {
				// Object type not filtered yet, loop through all objects instead
				IterateObject(list, [f] (const LPRO pObject, short oblOffset) {
					f(pObject);
					}, false);
			}
		};

		// Single object type
		if (oiList >= 0) {
			const LPOIL pObjectInfo = GetLPOIL(oiList);

			if (pObjectInfo == nullptr) {
				return;
			}

			iterateCall(OiList + oiList);
		}
		// Qualifier object type
		else if (oiList != -1) {
			IterateQualifier(oiList, [iterateCall] (const objInfoList* pOil, short qoiOiList) {
				iterateCall(pOil);
				});
		}
	}

	//Iterate given identifier
	inline void IterateObjectWithIdentifier(LPRDATA rdPtr, const DWORD identifier, const ForEachCallBack& callBack, const bool selected = false) const {
		IterateOiL([&] (const LPOIL pOil) {
			const LPRO pObject = GetFirstObject(pOil, selected);

			if (pObject == nullptr) {
				return;
			}

			const auto objIdentifier = pObject->roHo.hoIdentifier;

			if (objIdentifier == identifier) {
				IterateObject(pOil, [callBack] (const LPRO pObject, short oblOffset) {
					callBack(pObject);
					}, selected);
			}
			});
	}

	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	inline bool OILHasObject(std::wstring& objName) const {
		return OILHasObject(objName.c_str());
	}

	inline bool OILHasObject(const LPCWSTR pObjName) const {
		bool bHas = false;

		IterateOiL([&](const LPOIL pOil) {
			// oilName start with empty char
			const auto pCurName = [&]() {
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

	inline bool OILHasObject(const LPOIL pOil) const {
		bool bHas = false;

		IterateOiL([&] (const LPOIL _pOil) {
			if (pOil == _pOil) { bHas = true; }
			});

		return bHas;
	}

	inline auto SaveSelectedObject(const LPOIL pOil) const  {
		SelObj selObj;
		
		IterateObject(pOil, [&] (LPRO pObject, short oblOffset) {
			selObj.emplace_back(pObject);
		});

		return selObj;
	}
	
	// Android -> Events -> CEventProgram -> evt_SaveSelectedObjects
	inline auto SaveScope() const {
		Scope scope(rhPtr);

		const auto rhEvCount = rhPtr->rh2.rh2EventCount;

		IterateOiL([&] (const LPOIL pOil) {
			if (pOil->oilEventCount != rhEvCount) {
				return;
			}

			scope.AddSelObj(pOil, SaveSelectedObject(pOil));
		});

		return scope;
	}

	// Android -> Events -> CEventProgram -> evt_RestoreSelectedObjects
	inline void RestoreScope(const Scope& scope) const {
		for (const auto& [pOil, selObj] : scope.GetSelObj()) {
			if (!OILHasObject(pOil)) {
				continue;
			}

			SelectObjects(pOil, selObj);
		}
	}
};
