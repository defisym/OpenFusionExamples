#pragma once
#include "ObjectBase.h"
#import "CQualToOiList.h"
#import "CEventProgram.h"
#import "CEvents.h"
#include <algorithm>
#include <vector>

namespace FusionIOS {
enum ForEachFlags { ForEachFlag_Default=0, ForEachFlag_ForceAll=1, ForEachFlag_SelectedOnly=2 };

class ObjectSelection : public ObjectBase {
    std::vector<short> Types(int oiList) const {
        std::vector<short> result;
        oiList=static_cast<short>(oiList);
        if(oiList==-1) return result;
        if(oiList>=0) {if(GetLPOIL(oiList)) result.push_back(oiList);}
        else {
            // The SDK provides no qualifier-array length. Accept Runtime event
            // parameters, not arbitrary external qualifier IDs.
            CQualToOiList* qualifier=run->rhEvtProg->qualToOiList[oiList&0x7fff];
            if(qualifier) for(int i=0;i+1<qualifier->nQoi;i+=2) {
                short type=qualifier->qoiList[i+1];
                if(type<0) break;
                if(GetLPOIL(type) && std::find(result.begin(),result.end(),type)==result.end()) result.push_back(type);
            }
        }
        return result;
    }
    std::vector<ObjectRef> SnapshotList(CObjInfo* info,bool selected,bool destroyed) const {
        std::vector<ObjectRef> result;
        int slot=info ? (selected ? info->oilListSelected : info->oilObject) : -1;
        int remaining=run->rhMaxObjects;
        while(slot>=0 && remaining-->0) {
            CObject* object=GetObject(slot);if(!object) break;
            if(destroyed || !(object->hoFlags&HOF_DESTROYED)) result.push_back({slot,object->hoCreationId});
            slot=selected ? object->hoNextSelected : object->hoNumNext;
        }
        return result;
    }
    void Assign(short type,const std::vector<ObjectRef>& refs) const {
        CObjInfo* info=GetLPOIL(type);if(!info) return;
        info->oilListSelected=-1;info->oilNumOfSelected=0;info->oilEventCount=run->rhEvtProg->rh2EventCount;
        CObject* previous=nil;
        for(auto ref:refs) {
            CObject* object=Resolve(ref);if(!object || object->hoOiList!=info) continue;
            if(previous) previous->hoNextSelected=object->hoNumber;else info->oilListSelected=object->hoNumber;
            object->hoNextSelected=-1;previous=object;++info->oilNumOfSelected;
        }
    }
public:
    using SelObj=std::vector<CObject*>;
    using Filter=std::function<bool(CObject*)>;
    using ForEachCallBack=std::function<void(CObject*)>;
    explicit ObjectSelection(CRun* runtime) : ObjectBase(runtime) {}
    static bool IsNegated(CCndExtension* condition) {
        LPEVT event=(LPEVT)((char*)condition->pParams[0]-CND_SIZE);
        return (event->evtFlags2&EVFLAG2_NOT)!=0;
    }

    std::vector<ObjectRef> Snapshot(int oiList,int flags=ForEachFlag_Default,bool destroyed=false) const {
        std::vector<ObjectRef> result;
        for(short type:Types(oiList)) {
            CObjInfo* info=GetLPOIL(type);
            bool selected=!(flags&ForEachFlag_ForceAll) && ObjectIsSelected(info);
            if(!selected && !(flags&ForEachFlag_ForceAll) && (flags&ForEachFlag_SelectedOnly)) continue;
            auto refs=SnapshotList(info,selected,destroyed);result.insert(result.end(),refs.begin(),refs.end());
        }
        return result;
    }
    // Explicit replacement also clears empty SOLs. Generation IDs protect scope
    // restore and iteration against deletion/slot reuse during callbacks.
    void Replace(int oiList,const std::vector<ObjectRef>& refs) const {
        std::vector<ObjectRef> unique;
        for(auto ref:refs) if(std::find_if(unique.begin(),unique.end(),[&](ObjectRef r){return r.slot==ref.slot;})==unique.end()) unique.push_back(ref);
        for(short type:Types(oiList)) Assign(type,unique);
    }
    void SelectAll(int oiList) const { Replace(oiList,Snapshot(oiList,ForEachFlag_ForceAll,true)); }
    void SelectNone(int oiList) const { Replace(oiList,{}); }
    void SelectObjects(int oiList,const SelObj& objects) const {
        std::vector<ObjectRef> refs;
        for(CObject* object:objects) if(object && GetObject(object->hoNumber)==object) refs.push_back({object->hoNumber,object->hoCreationId});
        Replace(oiList,refs);
    }
    void SelectObjects(CObjInfo* info,const SelObj& objects) const {
        if(!info) return;
        for(short type=0;type<run->rhMaxOI;++type) if(GetLPOIL(type)==info) {SelectObjects(type,objects);return;}
    }
    void SelectOneObject(CObject* object) const { if(object) SelectObjects(object->hoOiList,{object}); }
    CObject* GetFirstObject(CObjInfo* info,bool selected=false) const {
        return info ? GetObject(selected ? info->oilListSelected : info->oilObject) : nil;
    }
    CObject* GetFirstObject(int oiList,bool selected=false) const { return GetFirstObject(GetLPOIL(oiList),selected); }
    static bool ObjectIsQualifier(int oiList) { return (oiList&0x8000)!=0; }
    bool ObjectIsSelected(CObjInfo* info) const { return info && info->oilEventCount==run->rhEvtProg->rh2EventCount; }
    bool ObjectIsSelected(int oiList) const {
        for(short type:Types(oiList)) if(ObjectIsSelected(GetLPOIL(type))) return true;return false;
    }
    bool Selected(CObjInfo* info) const { return ObjectIsSelected(info) && info->oilListSelected>=0; }
    bool Selected(int oiList) const { return Selected(GetLPOIL(oiList)); }
    int GetNumberOfSelected(int oiList) const {
        int count=0;for(short type:Types(oiList)) count+=GetLPOIL(type)->oilNumOfSelected;return count;
    }
    int GetNumberOfObject(int oiList) const {
        int count=0;for(short type:Types(oiList)) count+=GetLPOIL(type)->oilNObjects;return count;
    }
    bool ObjectIsOfType(CObject* object,int oiList) const {
        if(object) for(short type:Types(oiList)) if(GetLPOIL(type)->oilOi==object->hoOi) return true;return false;
    }
    void ForEach(int oiList,const ForEachCallBack& callback,int flags=ForEachFlag_Default) const {
        for(auto ref:Snapshot(oiList,flags)) {CObject* object=Resolve(ref);if(object && !(object->hoFlags&HOF_DESTROYED)) callback(object);}
    }
    // Predicates must not dispatch events or mutate object lists.
    bool FilterObjects(int oiList,bool negate,const Filter& predicate,bool destroyed=true) const {
        auto types=Types(oiList);if(types.empty()) return false;
        std::vector<ObjectRef> retained;
        for(auto ref:Snapshot(oiList,ForEachFlag_Default,destroyed)) {
            CObject* object=Resolve(ref);if(object && predicate(object)!=negate) retained.push_back(ref);
        }
        for(short type:types) Assign(type,retained);
        return !retained.empty()!=negate;
    }
    void IterateObjectWithIdentifier(int identifier,const ForEachCallBack& callback,bool selected=false) const {
        IterateOiL([&](CObjInfo* info){
            CObject* first=GetFirstObject(info,selected);
            if(first && first->hoIdentifier==identifier) for(auto ref:SnapshotList(info,selected,false)) {
                CObject* object=Resolve(ref);if(object && !(object->hoFlags&HOF_DESTROYED)) callback(object);
            }
        });
    }
    SelObj SaveSelectedObject(CObjInfo* info) const {
        SelObj result;for(auto ref:SnapshotList(info,true,false)) result.push_back(Resolve(ref));return result;
    }
    class Scope {
        friend class ObjectSelection;
        CRun* run;
        int count,action,loop;
        BOOL on,looping;
        LPEVT start;
        struct Saved {short type;CObjInfo* info;int stamp;std::vector<ObjectRef> objects;};
        std::vector<Saved> saved;
    public:
        explicit Scope(CRun* runtime) : run(runtime) {
            CEventProgram* e=run->rhEvtProg;count=e->rh2EventCount;action=e->rh2ActionCount;loop=e->rh2ActionLoopCount;
            on=e->rh2ActionOn;looping=e->rh2ActionLoop;start=e->rh4ActionStart;
        }
        void RestoreActionState() const {
            CEventProgram* e=run->rhEvtProg;e->rh2ActionCount=action;e->rh2ActionLoopCount=loop;
            e->rh2ActionOn=on;e->rh2ActionLoop=looping;e->rh4ActionStart=start;
        }
    };
    void SaveScope(Scope& scope) const {
        scope.saved.clear();
        for(short type=0;type<run->rhMaxOI;++type) if(CObjInfo* info=GetLPOIL(type)) {
            int stamp=info->oilEventCount;
            scope.saved.push_back({type,info,stamp,stamp==scope.count ? SnapshotList(info,true,true) : std::vector<ObjectRef>{}});
        }
    }
    Scope SaveScope() const { Scope scope(run);SaveScope(scope);return scope; }
    void RestoreScope(const Scope& scope) const {
        for(const auto& state:scope.saved) if(GetLPOIL(state.type)==state.info) {
            if(state.stamp==scope.count) Replace(state.type,state.objects);
            else state.info->oilEventCount=state.stamp;
        }
    }
    void KeepScopeCall(bool keepScope,const std::function<void()>& callback) const {
        if(!keepScope) {callback();return;}
        const auto scope=SaveScope();
        @try { callback(); }
        @finally { scope.RestoreActionState();RestoreScope(scope); }
    }
};
inline void KeepScopeCall(const ObjectSelection* selection,bool keepScope,const std::function<void()>& callback) {
    if(selection) selection->KeepScopeCall(keepScope,callback);else callback();
}
}
