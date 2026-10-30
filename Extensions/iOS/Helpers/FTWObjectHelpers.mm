#include "FTWObjectHelpers.h"
#import "CRun.h"
#import "CRunApp.h"
#import "CRunFrame.h"
#import "CObject.h"
#import "CObjInfo.h"
#import "CQualToOiList.h"
#import "CEventProgram.h"
#import "COI.h"
#import "COIList.h"
#import "CLayer.h"
#import "CObjectCommon.h"
#import "CRSpr.h"
#import "CBox2D.h"
#import "CEvents.h"
#include <algorithm>
#include <stdexcept>

namespace FTWiOS {
std::vector<short> Selection::types(short oiList) const {
    std::vector<short> result;
    if (oiList == -1) return result;
    if (oiList >= 0) {
        if (oiList < run->rhMaxOI && run->rhOiList[oiList]) result.push_back(oiList);
    } else {
        // OI-list parameters originate from validated Runtime events. The SDK
        // exposes no qualifier-array length; arbitrary external IDs are invalid.
        CQualToOiList* qualifier = run->rhEvtProg->qualToOiList[oiList & 0x7fff];
        if (qualifier) for (int i=0; i+1<qualifier->nQoi; i+=2) {
            short type=qualifier->qoiList[i+1];
            if (type>=0 && type<run->rhMaxOI && run->rhOiList[type]
                && std::find(result.begin(),result.end(),type)==result.end())
                result.push_back(type);
        }
    }
    return result;
}
CObject* Selection::resolve(ObjectRef ref) const {
    if (ref.slot<0 || ref.slot>=run->rhMaxObjects) return nil;
    CObject* object=run->rhObjectList[ref.slot];
    return object && object->hoCreationId==ref.generation ? object : nil;
}
std::vector<ObjectRef> Selection::snapshot(short oiList, int flags, bool destroyed) const {
    std::vector<ObjectRef> result;
    for (short type : types(oiList)) {
        CObjInfo* info=run->rhOiList[type];
        bool selected=!(flags & IterateAll)
            && info->oilEventCount==run->rhEvtProg->rh2EventCount;
        if (!selected && !(flags & IterateAll) && (flags & IterateSelectedOnly)) continue;
        int slot=selected ? info->oilListSelected : info->oilObject;
        int remaining=run->rhMaxObjects; // Bound traversal even if a list is corrupt.
        while (slot>=0 && slot<run->rhMaxObjects && remaining-->0) {
            CObject* object=run->rhObjectList[slot];
            if (!object) break;
            if (destroyed || !(object->hoFlags & HOF_DESTROYED))
                result.push_back({slot,object->hoCreationId});
            slot=selected ? object->hoNextSelected : object->hoNumNext;
        }
    }
    return result;
}
void Selection::assign(short type, const std::vector<ObjectRef>& refs) const {
    CObjInfo* info=run->rhOiList[type];
    CObject* previous=nil;
    info->oilListSelected=-1;
    info->oilNumOfSelected=0;
    info->oilEventCount=run->rhEvtProg->rh2EventCount;
    for (const auto& ref : refs) {
        CObject* object=resolve(ref);
        if (!object || object->hoOiList!=info) continue;
        if (previous) previous->hoNextSelected=object->hoNumber;
        else info->oilListSelected=object->hoNumber;
        object->hoNextSelected=-1;
        previous=object;
        ++info->oilNumOfSelected;
    }
}
void Selection::replace(short oiList, const std::vector<ObjectRef>& refs) const {
    // Remove duplicate slots before rebuilding linked SOLs.
    std::vector<ObjectRef> unique;
    for (const auto& ref : refs) {
        if (std::find_if(unique.begin(),unique.end(),[&](const ObjectRef& value) {
            return value.slot==ref.slot;
        })==unique.end()) unique.push_back(ref);
    }
    for (short type : types(oiList)) assign(type,unique);
}
void Selection::selectNone(short oiList) const { replace(oiList,{}); }
void Selection::selectAll(short oiList) const {
    replace(oiList,snapshot(oiList,IterateAll,true));
}
void Selection::forEach(short oiList, const std::function<void(CObject*)>& callback, int flags) const {
    const auto refs=snapshot(oiList,flags);
    for (const auto& ref : refs) {
        CObject* object=resolve(ref);
        if (object && !(object->hoFlags & HOF_DESTROYED)) callback(object);
    }
}
bool Selection::filter(short oiList, bool negate,
                        const std::function<bool(CObject*)>& predicate, bool destroyed) const {
    const auto typeList=types(oiList);
    if (typeList.empty()) return false;
    std::vector<ObjectRef> retained;
    for (const auto& ref : snapshot(oiList,IterateDefault,destroyed)) {
        CObject* object=resolve(ref);
        if (object && (predicate(object)!=negate)) retained.push_back(ref);
    }
    for (short type : typeList) assign(type,retained);
    return !retained.empty()!=negate;
}
short Creation::findOI(NSString* name) const {
    if (!name) return -1;
    // Current frame OIL, not the application's entire OI bank. Preserve the
    // mobile case-insensitive comparison and skip Windows' name prefix marker.
    for (int index=0; index<run->rhMaxOI; ++index) {
        CObjInfo* info=run->rhOiList[index];
        if (!info || info->oilType<2 || !info->oilName) continue;
        NSString* current=info->oilName;
        NSUInteger prefix=0;
        while (prefix<[current length] && [current characterAtIndex:prefix]==0xffff) ++prefix;
        if ([[current substringFromIndex:prefix] caseInsensitiveCompare:name]==NSOrderedSame)
            return info->oilOi;
    }
    return -1;
}
CObject* Creation::create(const CreateParams& params) const {
    if (params.oi<0 || (params.frameItem==-1 && params.layer<0)) return nil;
    int layer=std::max(-1,std::min(params.layer,run->rhFrame->nLayers-1));
    int slot=[run f_CreateObject:params.frameItem withOIHandle:params.oi
        andX:params.x andY:params.y andDir:params.direction andFlags:0
        andLayer:layer andNumCreation:-1];
    if (slot<0 || slot>=run->rhMaxObjects) return nil;
    CObject* object=run->rhObjectList[slot];
    if (!object) return nil;
    [run->rhEvtProg evt_AddCurrentObject:object];
    AddPhysicsAttractor(run,object);
    CRunMvtPhysics* physics=[run GetPhysicMovement:object];
    if (physics) physics->CreateBody(object);
    else if (run->rh4Box2DBase) run->rh4Box2DBase->pAddNormalObject(run->rh4Box2DBase,object);
    if (layer>=0 && (object->hoOEFlags & OEFLAG_SPRITES)) {
        CLayer* target=run->rhFrame->layers[layer];
        if ((target->dwOptions & (FLOPT_TOHIDE|FLOPT_VISIBLE))!=FLOPT_VISIBLE)
            [object->ros obHide];
    }
    return object;
}
}
