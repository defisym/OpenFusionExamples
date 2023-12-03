// ReSharper disable CppClangTidyClangDiagnosticShadow
// ReSharper disable CppClangTidyClangDiagnosticShadowField

#pragma once

// Origin version by Anders Riggelsen (Andos)

#include <functional>
#include <string>

#include "ccxhdr.h"
#include "EventBase.h"
#include "ObjectBase.h"

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

class ObjectCreation :public ObjectBase {
private:
	LPOI* ois;
	LPOBL oblPtr;
	CRunFrame* frame;
	RunFrameLayer* layerPtr;

	// For 2.5 HWA only:
	int additionalLayerSize = 8;

	int bufferSize = sizeof(event) + sizeof(eventParam) + sizeof(CreateDuplicateParam);
	char* buffer = nullptr;
	event* evt = nullptr;
	eventParam* creationParams = nullptr;
	CreateDuplicateParam* cdp = nullptr;

	CALLACTION_ROUTINE createObject;

	//Put the layer number in to a proper range
	inline int GetValidLayer(const int layer) const {		
		return (std::max)(-1,
			(std::min)(rhPtr->rhFrame->m_nLayers - 1, layer));
	}

	inline void ResetEVP() const {
		memset(buffer, 0, bufferSize);
		cdp->cdpHFII = static_cast<HFII>(rhPtr->rhNumberOi);
	}

public:	
	explicit ObjectCreation(const LPRDATA rdPtr)
		:ObjectBase(rdPtr->rHo.hoAdRunHeader) {
		ois = rhPtr->rhApp->m_ois;
		oblPtr = (LPOBL)rhPtr->rhObjectList;
		frame = (CRunFrame*)rhPtr->rhFrame;
		layerPtr = frame->m_pLayers;

//#ifdef HWABETA
//		additionalLayerSize = 8;
//#else
//		additionalLayerSize = 0;
//#endif
	
		// Create the event buffer (with plenty space):
		bufferSize = sizeof(event) + sizeof(eventParam) + sizeof(CreateDuplicateParam);
		buffer = new char[bufferSize];
		memset(buffer, 0, bufferSize);

		// The event that should be passed to the CreateObject routine		
		evt = reinterpret_cast<event*>(buffer);
		evt->evtCode = MAKELONG(0,0);
		
		// Resides at event + 14
		creationParams = reinterpret_cast<eventParam*>(buffer + ACT_SIZE);

		// The object creation parameters
		cdp = reinterpret_cast<CreateDuplicateParam*>(&creationParams->evp.evpW.evpW0);
		cdp->cdpHFII = static_cast<HFII>(rhPtr->rhNumberOi);

		// Call the routine
		const auto* tables = reinterpret_cast<CallTables*>(callRunTimeFunction(rdPtr, RFUNCTION_GETCALLTABLES, 0, 0));    // NOLINT(performance-no-int-to-ptr)
		createObject = tables->pActions[2];
	}

	~ObjectCreation(){
		delete buffer;
	}

	inline void OCCreateObject(const std::function<void(ObjectCreation* oc, CreateDuplicateParam*)>& updateParam){
		updateParam(this, cdp);
		cdp->cdpPos.posLayer = static_cast<short>(GetValidLayer(cdp->cdpPos.posLayer));
		
		if (cdp->cdpOi != -1) {
			createObject(evt);
		}
	}

	inline short GetCreationOI(const LPCWSTR objName) const {
		const auto pOil = GetOILByName(objName);

		return pOil != nullptr ? pOil->oilOi : static_cast<short>(-1);		
	}

	inline short GetCreationOI(const std::wstring& objName) const {
		return GetCreationOI(objName.c_str());
	}

	struct BKDParam {
		int x;
		int y;
		int nObstacleType;
		int nLayer;
	};

	inline void OCCreateBackdrop(const std::function<void(ObjectCreation* oc, LPOI& objOI, BKDParam& bkdParam)>& updateParam) {
		BKDParam bkdParam;
		LPOI objOI;
		
		updateParam(this, objOI, bkdParam);

		if (objOI == nullptr) {
			return;
		}

		const auto bkd = reinterpret_cast<LPBackdrop_OC>(objOI->oiOC);

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
	}

	inline LPOI GetBackdropOI(const LPCWSTR objName) const {
		// Find backdrop
		RunFrameLayer* layerPtr = this->layerPtr;

		for (int i = 0; i < frame->m_nLayers; ++i) 		{
			LPLO backdropPtr = (LPLO)(frame->m_los + layerPtr->nFirstLOIndex);

			for (DWORD j = 0; j < layerPtr->nBkdLOs; ++j) {
				const LPOI objOI = rhPtr->rhApp->m_ois[rhPtr->rhApp->m_oi_handle_to_index[backdropPtr->loOiHandle]];

				if (CompareObjectName(objOI->oiName, objName)) {
					return objOI;
				}

				backdropPtr++;
			}

			// TODO to check
			layerPtr = reinterpret_cast<RunFrameLayer*>(reinterpret_cast<char*>(++layerPtr) + additionalLayerSize);
		}

		return nullptr;
	}

	inline LPOI GetBackdropOI(const std::wstring& objName) const {
		return GetBackdropOI(objName.c_str());
	}

#define OCP_GetParameter(rdPtr) *(CreateDuplicateParam*)&(EventBase::GetEvtParam(rdPtr)->evp);
#define POS_GetParameter(rdPtr) *(PositionParam*)&(EventBase::GetEvtParam(rdPtr)->evp);
#define STO_GetParameter(rdPtr) *(ShootParam*)&(EventBase::GetEvtParam(rdPtr)->evp);

#define OCP_GetNthParameter(X) *(CreateDuplicateParam*)&(EventBase::GetNthEvtParam(rdPtr, X)->evp);
#define POS_GetNthParameter(X) *(PositionParam*)&(EventBase::GetNthEvtParam(rdPtr, X)->evp);
#define STO_GetNthParameter(X) *(ShootParam*)&(EventBase::GetNthEvtParam(rdPtr, X)->evp);
};