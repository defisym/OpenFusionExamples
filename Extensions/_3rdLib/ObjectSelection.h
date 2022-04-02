#pragma once

// Origin version by Anders Riggelsen (Andos)

// Add functional to support lambda
#include <functional>
#include <vector>

#include "ccxhdr.h"

class ObjectSelection {
private:
	using Filter = std::function<bool(LPRDATA, LPRO)>;
	using ForEachCallBack = std::function<void(LPRO)>;

	LPRH rhPtr;
	LPOBL ObjectList;
	LPOIL OiList;
	LPQOI QualToOiList;
	int oiListItemSize;

	inline LPOIL GetLPOIL(short oiList) {
		return (LPOIL)(((char*)OiList) + oiListItemSize * oiList);
	}

	inline bool FilterQualifierObjects(LPRDATA rdPtr, short oiList, bool negate, Filter filterFunction) {
		LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return false;
		}

		bool hasSelected = false;
		this->QualToOiList = rhPtr->rhQualToOiList;
		LPQOI CurrentQualToOiStart = (LPQOI)((char*)QualToOiList + oiList);
		LPQOI CurrentQualToOi = CurrentQualToOiStart;

		if (CurrentQualToOi == nullptr) {
			return false;
		}

		while (CurrentQualToOi->qoiOiList >= 0) {
			hasSelected |= FilterNonQualifierObjects(rdPtr, CurrentQualToOi->qoiOiList, negate, filterFunction);
			CurrentQualToOi = (LPQOI)((char*)CurrentQualToOi + 4);
			
			if (CurrentQualToOi == nullptr) {
				break;
			}
		}

		return hasSelected;
	}
	
	inline bool FilterNonQualifierObjects(LPRDATA rdPtr, short oiList, bool negate, Filter filterFunction) {
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

		int firstSelected = -1;
		int count = 0;
		int current = pObjectInfo->oilListSelected;
		LPHO previous = nullptr;

		while (current >= 0) {
			LPHO pObject = ObjectList[current].oblOffset;
			bool useObject = filterFunction(rdPtr, (LPRO)pObject) ^ negate;

			hasSelected |= useObject;

			if (useObject) {
				if (firstSelected == -1) {
					firstSelected = current;
				}

				if (previous != nullptr) {
					previous->hoNextSelected = current;
				}

				previous = pObject;
				count++;
			}
			current = pObject->hoNextSelected;
		}
		if (previous != nullptr) {
			previous->hoNextSelected = -1;
		}

		pObjectInfo->oilListSelected = firstSelected;
		pObjectInfo->oilNumOfSelected = count;

		return hasSelected;
	}

public:
	ObjectSelection(LPRH rhPtr) {
		this->rhPtr = rhPtr;
		this->ObjectList = rhPtr->rhObjectList;		//get a pointer to the mmf object list
		this->OiList = rhPtr->rhOiList;				//get a pointer to the mmf object info list
		this->QualToOiList = rhPtr->rhQualToOiList;	//get a pointer to the mmf qualifier to Oi list

		oiListItemSize = sizeof(objInfoList);

		//Only add the sizes to the runtime structures if they weren't compiled directly for those runtimes
#ifndef UNICODE
		if (rhPtr->rh4.rh4Mv->mvCallFunction(NULL, EF_ISUNICODE, 0, 0, 0))
			oiListItemSize += 24;
#endif
#ifndef HWABETA
		if (rhPtr->rh4.rh4Mv->mvCallFunction(NULL, EF_ISHWA, 0, 0, 0))
			oiListItemSize += sizeof(LPVOID);
#endif
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
	inline bool FilterObjects(LPRDATA rdPtr, short oiList, bool negate, Filter filterFunction) {
		LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return false;
		}

		if (oiList & 0x8000) {
			return FilterQualifierObjects(rdPtr, oiList & 0x7FFF, negate, filterFunction) ^ negate;
		}
		else {
			return FilterNonQualifierObjects(rdPtr, oiList, negate, filterFunction) ^ negate;
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
		LPOIL pObjectInfo = GetLPOIL(oiList);

		if (pObjectInfo == nullptr) {
			return false;
		}

		short Oi = pObjectInfo->oilOi;

		if (oiList & 0x8000) {
			oiList &= 0x7FFF;	//Mask out the qualifier part
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
			return (object->roHo.hoOi == Oi);
		}
	}
	
	//For Each, used in action
	inline void ForEach(LPRDATA rdPtr, short oiList, ForEachCallBack f, bool forceAll = false) {
		auto iterate = [&](objInfoList* list, bool selected = true) {
			short num = selected ? list->oilListSelected : list->oilObject;

			while (num >= 0) {
				RunObject* obj = reinterpret_cast<RunObject*>(rhPtr->rhObjectList[num].oblOffset);
				
				if (obj && !(static_cast<ushort>(obj->roHo.hoFlags) & static_cast<ushort>(HOF_DESTROYED))) {
					f(obj);
				}

				num = selected ? obj->roHo.hoNextSelected : obj->roHo.hoNumNext;
			}
		};

		auto iterateCall = [&](objInfoList* list) {
			if (forceAll) {
				iterate(list, false);
				return;
			}

			if(list->oilEventCount == rhPtr->rh2.rh2EventCount){
				// Object type already filtered, loop through selected list
				iterate(list, true);
			}
			else {
				// Object type not filtered yet, loop through all objects instead
				iterate(list, false);
			}
		};

		// Single object type
		if (oiList >= 0) {
			iterateCall(OiList + oiList);
		}
		// Qualifier object type
		else if (oiList != -1) {
			auto qualifier_iterator = reinterpret_cast<qualToOi*>(reinterpret_cast<unsigned char*>(QualToOiList) + (oiList & 0x7FFF));

			while (qualifier_iterator->qoiOiList >= 0) {
				iterateCall(OiList + qualifier_iterator->qoiOiList);
				qualifier_iterator = reinterpret_cast<qualToOi*>(reinterpret_cast<unsigned char*>(qualifier_iterator) + 4);
			}
		}
	}
};
