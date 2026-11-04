#pragma once
#import "CRun.h"
#import "CObject.h"
#import "CObjInfo.h"
#import "CCndExtension.h"
#import "CEvents.h"
#include <functional>

namespace FusionIOS {
// Runtime objects are borrowed. Use these helpers only on Fusion's thread.
struct ObjectRef { int slot; short generation; };

class ObjectBase {
protected:
    CRun* run;
    CObject* GetObject(int slot) const {
        return slot>=0 && slot<run->rhMaxObjects ? run->rhObjectList[slot] : nil;
    }
    CObject* Resolve(ObjectRef ref) const {
        CObject* object=GetObject(ref.slot);
        return object && object->hoCreationId==ref.generation ? object : nil;
    }
    void IterateOiL(const std::function<void(CObjInfo*)>& callback) const {
        for(int i=0;i<run->rhMaxOI;++i) if(run->rhOiList[i]) callback(run->rhOiList[i]);
    }
public:
    explicit ObjectBase(CRun* runtime) : run(runtime) {}
    static short GetOil(CCndExtension* condition,CRun* runtime,int index=0) {
        return [condition getParamObject:runtime withNum:index]->evp.evpW.evpW0;
    }
    CObjInfo* GetLPOIL(int oiList) const {
        return oiList>=0 && oiList<run->rhMaxOI ? run->rhOiList[oiList] : nil;
    }
    CObjInfo* GetLPOIL(CObject* object) const { return object ? object->hoOiList : nil; }
    short GetOiFromOiList(int oiList) const {
        CObjInfo* info=GetLPOIL(oiList);return info ? info->oilOi : -1;
    }
    static bool CompareObjectName(NSString* objectName,NSString* compareName) {
        if(!objectName || !compareName) return false;
        NSUInteger start=0;
        while(start<[objectName length] && [objectName characterAtIndex:start]==0xffff) ++start;
        return [[objectName substringFromIndex:start] caseInsensitiveCompare:compareName]==NSOrderedSame;
    }
    static bool CompareObjectName(CObjInfo* info,NSString* name) {
        return info && CompareObjectName(info->oilName,name);
    }
    CObjInfo* GetOILByName(NSString* name) const {
        CObjInfo* result=nil;
        IterateOiL([&](CObjInfo* info){if(CompareObjectName(info,name)) result=info;});
        return result;
    }
    bool OILHasObject(NSString* name) const { return GetOILByName(name)!=nil; }
    bool OILHasObject(CObjInfo* info) const {
        bool found=false;IterateOiL([&](CObjInfo* current){found=found || current==info;});
        return found;
    }
};
}
