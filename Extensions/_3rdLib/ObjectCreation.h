// ReSharper disable CppClangTidyClangDiagnosticShadow

#pragma once

// Origin version by Anders Riggelsen (Andos)

#include <functional>
#include <string>

#include "ccxhdr.h"

// usage
//ObjectCreation oc(rdPtr);
// 
//oc.OCCreateObject([&](ObjectCreation* oc, CreateDuplicateParam* cdp) {
//  ////PARAM_CREATE
//  *cdp = *(CreateDuplicateParam*)&(ObjectCreation::GetNthEvtParam(rdPtr, X)->evp);
// 
//	////PARAM_SYSCREATE
//	cdp->cdpOi = oc->GetCreationOI(L"Zoc");
// 
//	//cdp->cdpPos = *(PositionParam*)&(oc->GetNthEvtParam(rdPtr, 0)->evp);
//	//cdp->cdpPos.posLayer = 0;
//
//	//PARAM_EXPRESSION
//  cdp->cdpOi = oc->GetCreationOI(L"Zoc");
// 
//	cdp->cdpPos.posX = 50;
//	cdp->cdpPos.posY = 50;
//	cdp->cdpPos.posLayer = 0;
//	cdp->cdpPos.posOINUMParent = -1;
//	cdp->cdpPos.posFlags = 8;
//
//	});
//
//oc.OCCreateBackdrop([&](ObjectCreation* oc, LPOI& objOI, BKDParam& bkdParam) {
//	objOI = oc->GetBackdropOI(L"Backdrop");
//	bkdParam = BKDParam{ 0,0,0,0 };
//	});

struct BKDParam {
	int x;
	int y;
	int nObstacleType;
	int nLayer;
};

class ObjectCreation {
private:
	LPRH rhPtr;
	LPOBL ObjectList;
	LPOIL OiList;
	LPQOI QualToOiList;		// need to be updated each time before using

	// For 2.5 HWA only:
	int oiListItemSize = sizeof(objInfoList) + sizeof(LPVOID);

	LPOI* ois;
	LPOBL oblPtr;
	CRunFrame* frame;
	RunFrameLayer* layerPtr;

	// For 2.5 HWA only:
	int additionalLayerSize = 8;

	int bufferSize = sizeof(event) + sizeof(eventParam) + sizeof(CreateDuplicateParam);
	char* buffer=nullptr;
	event* evt=nullptr;
	eventParam* creationParams=nullptr;
	CreateDuplicateParam* cdp=nullptr;

	CALLACTION_ROUTINE createObject;

	//Put the layer number in to a proper range
	inline int GetValidLayer(int layer) const {		
		return (max(-1, min(rhPtr->rhFrame->m_nLayers - 1, layer)));
	}

	inline void ResetEVP() const {
		memset(buffer, 0, bufferSize);
		cdp->cdpHFII = (HFII)rhPtr->rhNumberOi;
	}

public:
	explicit ObjectCreation(LPRDATA rdPtr) {
		this->rhPtr = rdPtr->rHo.hoAdRunHeader;
		this->ObjectList = rhPtr->rhObjectList;		//get a pointer to the mmf object list
		this->OiList = rhPtr->rhOiList;				//get a pointer to the mmf object info list
		this->QualToOiList = rhPtr->rhQualToOiList;	//get a pointer to the mmf qualifier to Oi list

		ois = rhPtr->rhApp->m_ois;
		oblPtr = (LPOBL)rhPtr->rhObjectList;
		frame = (CRunFrame*)rhPtr->rhFrame;
		layerPtr = frame->m_pLayers;

//#ifdef HWABETA
//		additionalLayerSize = 8;
//#endif
//#ifndef HWABETA
//		additionalLayerSize = 0;
//#endif
//
//		//Only add the sizes to the runtime structures if they weren't compiled directly for those runtimes
//#ifndef UNICODE
//		if (rhPtr->rh4.rh4Mv->mvCallFunction(NULL, EF_ISUNICODE, 0, 0, 0))
//			oiListItemSize += 24;
//#endif
//#ifndef HWABETA
//		if (rhPtr->rh4.rh4Mv->mvCallFunction(NULL, EF_ISHWA, 0, 0, 0))
//			oiListItemSize += sizeof(LPVOID);
//#endif
	
		//Create the event buffer (with plenty space):
		bufferSize = sizeof(event) + sizeof(eventParam) + sizeof(CreateDuplicateParam);
		buffer = new char[bufferSize];
		memset(buffer, 0, bufferSize);

		//The event that should be passed to the CreateObject routine		
		evt = (event*)buffer;
		evt->evtCode = MAKELONG(0,0);
		
		//Resides at event+14
		creationParams = (eventParam*)(buffer+ACT_SIZE);

		//The object creation parameters
		cdp = (CreateDuplicateParam*)&creationParams->evp.evpW.evpW0;
		cdp->cdpHFII = (HFII)rhPtr->rhNumberOi;

		//Call the routine
		const auto* tables = (CallTables*)callRunTimeFunction(rdPtr, RFUNCTION_GETCALLTABLES, 0, 0);
		createObject = tables->pActions[2];
	}

	~ObjectCreation(){
		delete buffer;
	}

	inline void OCCreateObject(std::function<void(ObjectCreation* oc, CreateDuplicateParam*)> updateParam){
		updateParam(this, cdp);
		cdp->cdpPos.posLayer = (short)GetValidLayer(cdp->cdpPos.posLayer);
		
		if (cdp->cdpOi != -1) {
			createObject(evt);
		}
	}

	inline short GetCreationOI(LPCWSTR objName) const {
		//Iterate OI list
		objInfoList* list = rhPtr->rhOiList;
		const int num = rhPtr->rhNumberOi - 1;

		for (int i = 0; i < num; i++) {
			objInfoList info = *(objInfoList*)(((char*)list) + i * oiListItemSize);

			// oilName may start with empty char
			const auto pCurName = [&] () {
				auto pOilName = info.oilName;

				while (pOilName[0] == 65535) {
					pOilName++;
				}

				return pOilName;
				}();

			if (StrEqu(objName, pCurName)) {
				return info.oilOi;
			}
		}

		//No object was found
		return -1;
	}

	inline short GetCreationOI(const std::wstring& objName) const {
		return GetCreationOI(objName.c_str());
	}

	inline void OCCreateBackdrop(std::function<void(ObjectCreation* oc, LPOI& objOI, BKDParam& bkdParam)> updateParam) {
		BKDParam bkdParam;
		LPOI objOI;
		
		updateParam(this, objOI, bkdParam);

		if (objOI == nullptr) {
			return;
		}

		const auto bkd = (LPBackdrop_OC)objOI->oiOC;

		cSurface imageSurface;
		LockImageSurface(rhPtr->rhIdAppli, bkd->ocImage, imageSurface, LOCKIMAGE_READBLITONLY);

		rhPtr->rh4.rh4Mv->mvAddBackdrop(
			&imageSurface,
			bkdParam.x,
			bkdParam.y,
			objOI->oiHdr.oiInkEffect,
			objOI->oiHdr.oiInkEffectParam,
			bkdParam.nObstacleType,
			bkdParam.nLayer
		);

		UnlockImageSurface(imageSurface);
		return;
	}

	inline LPOI GetBackdropOI(LPCWSTR objName) const {
		// Find backdrop
		RunFrameLayer* layerPtr = this->layerPtr;

		for (int i = 0; i < frame->m_nLayers; ++i) 		{
			LPLO backdropPtr = (LPLO)(frame->m_los + layerPtr->nFirstLOIndex);

			for (int j = 0; j < (int)layerPtr->nBkdLOs; ++j) {
				LPOI objOI = rhPtr->rhApp->m_ois[rhPtr->rhApp->m_oi_handle_to_index[backdropPtr->loOiHandle]];

				// oilName may start with empty char
				const auto pCurName = [&] () {
					auto pOilName = objOI->oiName;

					while (pOilName[0] == 65535) {
						pOilName++;
					}

					return pOilName;
				}();

				if (StrEqu(objName, pCurName)) {
					return objOI;
				}

				backdropPtr++;
			}

			layerPtr = (RunFrameLayer*)(((char*)layerPtr++) + additionalLayerSize);
		}

		return nullptr;
	}

	inline LPOI GetBackdropOI(const std::wstring& objName) const {
		return GetBackdropOI(objName.c_str());
	}

	// must be called at first, starts from 0
	inline static eventParam* GetNthEvtParam(LPRDATA rdPtr, size_t index = 0) {
		eventParam* param = rdPtr->rHo.hoCurrentParam;
		param = reinterpret_cast<eventParam*>(reinterpret_cast<unsigned char*>(param) + param->evpSize * index);

		return param;
	}
	
	#define OCP_GetNthParameter(X) *(CreateDuplicateParam*)&(ObjectCreation::GetNthEvtParam(rdPtr, X)->evp);
	#define POS_GetNthParameter(X) *(PositionParam*)&(ObjectCreation::GetNthEvtParam(rdPtr, X)->evp);
	#define STO_GetNthParameter(X) *(ShootParam*)&(ObjectCreation::GetNthEvtParam(rdPtr, X)->evp);

	// normally get current param then read next
	inline static eventParam* GetEvtParam(LPRDATA rdPtr) {
		eventParam* param = rdPtr->rHo.hoCurrentParam;
		CNC_GetParameter(rdPtr);

		return param;
	}

	#define OCP_GetParameter(rdPtr) *(CreateDuplicateParam*)&(ObjectCreation::GetEvtParam(rdPtr)->evp);
	#define POS_GetParameter(rdPtr) *(PositionParam*)&(ObjectCreation::GetEvtParam(rdPtr)->evp);
	#define STO_GetParameter(rdPtr) *(ShootParam*)&(ObjectCreation::GetEvtParam(rdPtr)->evp);
};