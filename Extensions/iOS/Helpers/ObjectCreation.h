#pragma once
#include "ObjectBase.h"
#import "CRunApp.h"
#import "CRunFrame.h"
#import "CEventProgram.h"
#import "CObjectCommon.h"
#import "CLayer.h"
#import "CRSpr.h"
#import "CBox2D.h"
#import "CEvents.h"
#include <algorithm>

namespace FusionIOS {
class ObjectCreation : public ObjectBase {
public:
    struct CreateParams {
        short frameItem=-1,oi=-1;
        int x=0,y=0,direction=0,layer=0;
    };
    explicit ObjectCreation(CRun* runtime) : ObjectBase(runtime) {}
    short GetCreationOI(NSString* name) const {
        CObjInfo* info=GetOILByName(name);return info && info->oilType>=2 ? info->oilOi : -1;
    }
    int GetValidLayer(int layer) const { return std::max(-1,std::min(layer,run->rhFrame->nLayers-1)); }
    // Position decoding remains SDK-specific; never copy a Windows layout.
    bool ResolvePosition(const ::CreateDuplicateParam& source,CreateParams& result,BOOL& repeat) const {
        auto local=source;
        if(!read_Position(run,&local.cdpPos,0x11,&result.x,&result.y,&result.direction,&repeat,&result.layer)) return false;
        result.oi=local.cdpOi;result.frameItem=local.cdpHFII;return true;
    }
    CObject* Create(const CreateParams& params) const {
        if(params.oi<0 || (params.frameItem==-1 && params.layer<0)) return nil;
        int layer=GetValidLayer(params.layer);
        int slot=[run f_CreateObject:params.frameItem withOIHandle:params.oi
            andX:params.x andY:params.y andDir:params.direction andFlags:0 andLayer:layer andNumCreation:-1];
        CObject* object=GetObject(slot);if(!object) return nil;
        [run->rhEvtProg evt_AddCurrentObject:object];
        AddPhysicsAttractor(run,object);
        CRunMvtPhysics* physics=[run GetPhysicMovement:object];
        if(physics) physics->CreateBody(object);
        else if(run->rh4Box2DBase) run->rh4Box2DBase->pAddNormalObject(run->rh4Box2DBase,object);
        if(layer>=0 && (object->hoOEFlags&OEFLAG_SPRITES)) {
            CLayer* target=run->rhFrame->layers[layer];
            if((target->dwOptions&(FLOPT_TOHIDE|FLOPT_VISIBLE))!=FLOPT_VISIBLE) [object->ros obHide];
        }
        return object;
    }
    CObject* OCCreateObject(const std::function<void(ObjectCreation*,CreateParams&)>& update) {
        CreateParams params;update(this,params);return Create(params);
    }
};
}
