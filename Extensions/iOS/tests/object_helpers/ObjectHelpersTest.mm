// No FindTheWay sources, headers or native library are used by these tests.
#include "ObjectSelection.h"
#include "ObjectCreation.h"
#include <stdexcept>
#include <cstdlib>
#define CHECK(x) do {if(!(x)) {NSLog(@"FAILED line %d: %s",__LINE__,#x);abort();}} while(0)
using namespace FusionIOS;
CObject* CreateFromPeer(CRun*,short);
int CountFromPeer(CRun*,int);

int main() {
    NSAutoreleasePool* pool=[[NSAutoreleasePool alloc] init];
    CRun* run=[[CRun alloc] init];
    run->rhOiList[0]->oilOi=71;run->rhOiList[1]->oilOi=131;run->rhOiList[2]->oilOi=511;
    run->rhOiList[0]->oilName=@"\uffff\uffffToken";run->rhOiList[1]->oilName=@"Peer";
    ObjectBase base(run);ObjectCreation creation(run);ObjectSelection selection(run);
    CCndExtension* condition=[[[CCndExtension alloc] init] autorelease];
    condition->parameters=@[@32768];[condition prepare];
    CHECK(ObjectBase::GetOil(condition,run)==-32768 && !ObjectSelection::IsNegated(condition));
    condition->pEvent->evtFlags2=EVFLAG2_NOT;CHECK(ObjectSelection::IsNegated(condition));
    CHECK(base.GetOiFromOiList(0)==71 && base.GetOiFromOiList(1)==131);
    CHECK(base.GetOiFromOiList(0x8000)==-1 && base.GetOiFromOiList(-1)==-1);
    CHECK(base.GetLPOIL(99)==nil && base.GetLPOIL((CObject*)nil)==nil);
    CHECK(base.OILHasObject(@"token") && base.OILHasObject(run->rhOiList[1]));
    CHECK(!base.OILHasObject(@"missing") && !ObjectBase::CompareObjectName((NSString*)nil,@"Token"));
    CHECK(creation.GetCreationOI(@"TOKEN")==71);
    ObjectCreation::CreateParams params;params.oi=71;params.x=3;params.y=5;
    CObject* a=creation.Create(params);CObject* b=creation.Create(params);CObject* c=CreateFromPeer(run,131);
    CHECK(a && a->hoNumber==0 && c->hoX==17 && c->hoY==23);
    CHECK(base.GetLPOIL(a)==run->rhOiList[0] && CountFromPeer(run,0)==2);
    a->hoIdentifier=b->hoIdentifier=0x41435456;c->hoIdentifier=123;
    selection.SelectAll(0);CHECK(selection.Selected(0) && selection.ObjectIsSelected(0));
    CHECK(selection.GetFirstObject(0)==b && selection.GetFirstObject(0,true)==b);
    selection.SelectOneObject(a);CHECK(selection.Selected(0) && selection.GetFirstObject(0,true)==a);
    CHECK(selection.GetNumberOfSelected(0)==1 && selection.GetNumberOfObject(0)==2);
    CHECK(selection.ObjectIsOfType(a,0) && !selection.ObjectIsOfType(a,1));
    auto selected=selection.SaveSelectedObject(run->rhOiList[0]);CHECK(selected.size()==1 && selected[0]==a);
    selection.SelectObjects(0,{a,a,c});CHECK(selection.GetNumberOfSelected(0)==1);
    selection.SelectObjects(0,{});CHECK(selection.ObjectIsSelected(0) && !selection.Selected(0));
    int visits=0;selection.ForEach(0,[&](CObject*){++visits;});CHECK(visits==0);
    --run->rhOiList[0]->oilEventCount;
    selection.ForEach(0,[&](CObject*){++visits;},ForEachFlag_SelectedOnly);CHECK(visits==0);
    selection.ForEach(0,[&](CObject*){++visits;},ForEachFlag_SelectedOnly|ForEachFlag_ForceAll);CHECK(visits==2);
    selection.SelectAll(0);b->hoFlags|=HOF_DESTROYED;visits=0;
    selection.ForEach(0,[&](CObject*){++visits;});CHECK(visits==1);
    CHECK(!selection.FilterObjects(0,true,[&](CObject*){return false;},false));
    CHECK(selection.GetNumberOfSelected(0)==1 && selection.GetFirstObject(0,true)==a);
    b->hoFlags&=~HOF_DESTROYED;
    short members[]={71,0,131,1,-1,-1};
    CQualToOiList* qualifier=[[[CQualToOiList alloc] init] autorelease];qualifier->nQoi=6;qualifier->qoiList=members;
    run->rhEvtProg->qualToOiList[0]=qualifier;
    selection.SelectAll(0x8000);CHECK(selection.GetNumberOfObject(-32768)==3 && selection.GetNumberOfSelected(0x8000)==3);
    CHECK(selection.ObjectIsOfType(c,0x8000) && selection.ObjectIsQualifier(0x8000));
    CHECK(selection.FilterObjects(-32768,false,[&](CObject* o){return o==a || o==c;}));
    CHECK(selection.GetNumberOfSelected(0)==1 && selection.GetNumberOfSelected(1)==1);
    selection.SelectAll(0);visits=0;
    selection.IterateObjectWithIdentifier(0x41435456,[&](CObject*){++visits;});CHECK(visits==2);
    // Snapshot iteration skips a slot whose creation ID changed in a callback.
    visits=0;selection.ForEach(0,[&](CObject* o){++visits;if(o==b) ++a->hoCreationId;});CHECK(visits==1);--a->hoCreationId;
    selection.SelectOneObject(a);selection.SelectNone(1);--run->rhOiList[2]->oilEventCount;
    CEventProgram* events=run->rhEvtProg;events->rh2ActionCount=7;events->rh2ActionLoopCount=8;events->rh2ActionOn=YES;
    auto scope=selection.SaveScope();
    ++events->rh2EventCount;selection.SelectAll(0x8000);selection.SelectAll(2);events->rh2ActionCount=90;
    selection.RestoreScope(scope);CHECK(events->rh2ActionCount==90);
    scope.RestoreActionState();CHECK(events->rh2ActionCount==7);
    CHECK(selection.GetFirstObject(0,true)==a && selection.GetNumberOfSelected(1)==0 && selection.ObjectIsSelected(1));
    CHECK(!selection.ObjectIsSelected(2));
    @try {
        selection.KeepScopeCall(true,[&] {
            selection.SelectAll(0x8000);events->rh2ActionCount=99;
            @throw [NSException exceptionWithName:@"Nested" reason:@"fixture" userInfo:nil];
        });
        CHECK(false);
    } @catch(NSException* e) { CHECK([[e name] isEqualToString:@"Nested"]); }
    CHECK(selection.GetNumberOfSelected(0)==1 && selection.GetNumberOfSelected(1)==0 && events->rh2ActionCount==7);
    // Apple's unified Objective-C++ exception runtime supports C++ exceptions.
    // The legacy GNU Objective-C runtime used by Linux fixtures does not.
#ifdef __APPLE__
    try {
        selection.KeepScopeCall(true,[&] {selection.SelectNone(0);throw std::runtime_error("fixture");});
        CHECK(false);
    } catch(const std::runtime_error&) {}
    CHECK(selection.GetNumberOfSelected(0)==1);
#endif
    selection.KeepScopeCall(false,[&]{selection.SelectNone(0);});CHECK(!selection.Selected(0));
    selection.SelectOneObject(a);scope=selection.SaveScope();++a->hoCreationId;selection.RestoreScope(scope);
    CHECK(selection.GetNumberOfSelected(0)==0);--a->hoCreationId;
    // Local creation parameters survive a nested creation on the same helper.
    CObject* outer=creation.OCCreateObject([&](ObjectCreation* helper,ObjectCreation::CreateParams& p) {
        p.oi=71;p.x=41;p.y=43;
        CObject* inner=helper->OCCreateObject([&](ObjectCreation*,ObjectCreation::CreateParams& q){q.oi=131;q.x=2;q.y=4;});
        CHECK(inner && inner->hoOi==131 && inner->hoX==2);
    });
    CHECK(outer && outer->hoOi==71 && outer->hoX==41 && outer->hoY==43);
    run->rhFrame->layers[0]->dwOptions=FLOPT_TOHIDE;params.layer=999;
    CHECK(creation.Create(params)->ros->hidden);
    params.layer=-1;CHECK(creation.Create(params)==nil);params.frameItem=0;
    CHECK(creation.Create(params)!=nil);
    ::CreateDuplicateParam source={};source.cdpOi=71;source.cdpHFII=-1;
    source.cdpPos.posOINUMParent=-1;source.cdpPos.posX=29;source.cdpPos.posY=31;source.cdpPos.posLayer=0;
    BOOL repeat=NO;ObjectCreation::CreateParams resolved;
    CHECK(creation.ResolvePosition(source,resolved,repeat) && resolved.x==29 && resolved.y==31 && resolved.oi==71);
    source.cdpPos.posOINUMParent=0;CHECK(!creation.ResolvePosition(source,resolved,repeat));
    selection.SelectAll(0);int before=selection.GetNumberOfObject(0);visits=0;
    selection.ForEach(0,[&](CObject*) {if(visits++==0) CHECK(CreateFromPeer(run,71)!=nil);});
    CHECK(visits==before && selection.GetNumberOfObject(0)==before+1);
    CHECK(run->physicsCalls==run->nextSlot);
    NSLog(@"Generic ObjectBase/Selection/Creation checks passed without a plugin core");
    [run release];[pool drain];
    return 0;
}
