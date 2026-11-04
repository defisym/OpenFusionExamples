// Own API fixtures plus real Foundation and the actual C++ core/Runtime adapter.
#import "RuntimeAPI.h"
#import "CRunFindTheWay.h"
#include <stdlib.h>
#include "ObjectSelection.h"
#include "ObjectCreation.h"
#define CHECK(x) do { if (!(x)) { NSLog(@"FAILED line %d: %s",__LINE__,#x); abort(); } } while(0)
static BOOL condition(CRunFindTheWay* extension, int code, NSArray* params) {
    CCndExtension* cnd=[[[CCndExtension alloc] init] autorelease]; cnd->parameters=params;[cnd prepare];
    return [extension condition:code withCndExtension:cnd];
}
static void action(CRunFindTheWay* extension, int code, NSArray* params) {
    CActExtension* act=[[[CActExtension alloc] init] autorelease]; act->parameters=params;[act prepare];
    [extension action:code withActExtension:act];
}
static CValue* expression(CRunFindTheWay* extension, int code, NSArray* params) {
    extension->ho->parameters=params; extension->ho->nextParameter=0;
    CValue* result=[extension expression:code];
    CHECK(extension->ho->nextParameter==[params count]);
    return result;
}
typedef struct { CRunFindTheWay* ext; int mode, outer, inner; } Events;
static void event(void* context, int code) {
    Events* events=(Events*)context; CRunFindTheWay* ext=events->ext;
    if(code==17) return;
    if (condition(ext,code,@[@"inner"])) { ++events->inner; return; }
    CHECK(condition(ext,code,@[@"outer"])); ++events->outer;
    int x=[expression(ext,0,@[]) getInt], y=[expression(ext,1,@[]) getInt];
    if (events->outer==1 && events->mode==1) {
        action(ext,2,@[@"walk",@0,@"inner"]);
        CHECK([expression(ext,0,@[]) getInt]==x && [expression(ext,1,@[]) getInt]==y);
        CHECK(condition(ext,code,@[@"outer"]));
    }
    if (events->outer==1 && events->mode==2) CHECK(condition(ext,0,@[@3,@3]));
    if (events->mode==3) [ext destroyRunObject:NO];
}
static CRunFindTheWay* fresh(void) {
    CRunFindTheWay* ext=[[CRunFindTheWay alloc] init];ext->rh=[[[CRun alloc] init] autorelease];ext->ho=[[[CExtension alloc] init] autorelease];
    CFile* file=[[[CFile alloc] init] autorelease];CHECK([ext createRunObject:file withCOB:nil andVersion:1]);return ext;
}
static BOOL negatedCondition(CRunFindTheWay* ext,int code,NSArray* params) {
    CCndExtension* cnd=[[[CCndExtension alloc] init] autorelease];cnd->parameters=params;[cnd prepare];cnd->pEvent->evtFlags2=EVFLAG2_NOT;
    return [ext condition:code withCndExtension:cnd];
}
typedef struct {CRunFindTheWay* ext;int count,updates;BOOL destroy;} ObjectEvents;
static void objectEvent(void* context,int code) {
    ObjectEvents* state=(ObjectEvents*)context;CRunFindTheWay* ext=state->ext;CRun* run=ext->rh;
    ++run->rhEvtProg->rh2EventCount;run->rhEvtProg->rh2ActionCount=99;run->rhEvtProg->rh2ActionLoopCount=88;
    run->rhEvtProg->rh2ActionOn=NO;run->rhEvtProg->rh2ActionLoop=NO;
    if(code==17) {++state->updates;return;}
    CHECK(code==18);CHECK(condition(ext,18,@[@0,@1,@"zone"]));++state->count;
    if(state->destroy) [ext destroyRunObject:NO];
}
static void eventChecks(void) {
    CRunFindTheWay* ext=fresh();CRun* run=ext->rh;CHECK(condition(ext,0,@[@5,@5]));
    FusionIOS::ObjectCreation creation(run);FusionIOS::ObjectCreation::CreateParams p;p.oi=10;p.x=p.y=2;CObject* first=creation.Create(p);p.x=3;CObject* second=creation.Create(p);
    FusionIOS::ObjectSelection selection(run);selection.Replace(0,{{first->hoNumber,first->hoCreationId}});
    run->rhEvtProg->rh2ActionOn=YES;run->rhEvtProg->rh2ActionLoop=YES;run->rhEvtProg->rh2ActionCount=12;run->rhEvtProg->rh2ActionLoopCount=13;
    ObjectEvents events={ext,0,0,NO};ext->ho->event=objectEvent;ext->ho->context=&events;
    action(ext,12,@[@0,@"zone"]);CHECK(events.count==8);
    CHECK(run->rhOiList[0]->oilNumOfSelected==1 && run->rhOiList[0]->oilListSelected==first->hoNumber);
    CHECK(run->rhEvtProg->rh2ActionOn && run->rhEvtProg->rh2ActionLoop && run->rhEvtProg->rh2ActionCount==12 && run->rhEvtProg->rh2ActionLoopCount==13);
    action(ext,0,@[@2,@2,@255,@1,@0]);CHECK(condition(ext,10,@[@0,@0]));CHECK(events.updates==1);
    CHECK(run->rhOiList[0]->oilListSelected==first->hoNumber);
    selection.SelectAll(0);int visited=0;
    selection.ForEach(0,[&](CObject* object){++visited;if(object==second)++first->hoCreationId;});CHECK(visited==1);--first->hoCreationId;
    selection.Replace(0,{{first->hoNumber,first->hoCreationId}});events.destroy=YES;events.count=0;
    action(ext,12,@[@0,@"zone"]);CHECK(events.count==1);
    [ext release];
}
static void objectChecks(void) {
    CRunFindTheWay* ext=fresh();CRun* run=ext->rh;
    CHECK(condition(ext,0,@[@8,@8]));
    FusionIOS::ObjectCreation creation(run);FusionIOS::ObjectCreation::CreateParams p;p.oi=10;p.x=2;p.y=2;
    CObject* a=creation.Create(p);CHECK(a && a->hoNumber==0); // slot zero is success
    p.x=3;CObject* b=creation.Create(p);p.x=2;p.oi=11;CObject* c=creation.Create(p);
    CHECK(a && b && c && run->physicsCalls==3);
    run->rhFrame->layers[0]->dwOptions=FLOPT_TOHIDE;p.oi=12;p.layer=999;
    CObject* hidden=creation.Create(p);CHECK(hidden && hidden->ros->hidden);
    p.layer=-1;p.frameItem=-1;CHECK(!creation.Create(p));
    p.frameItem=0;CObject* sentinel=creation.Create(p);CHECK(sentinel && !sentinel->ros->hidden);
    run->rhFrame->layers[0]->dwOptions=FLOPT_VISIBLE;
    run->rhOiList[0]->oilName=@"\uffffToken";CHECK(creation.GetCreationOI(@"token")==10);CHECK(creation.GetCreationOI(@"missing")==-1);
    FusionIOS::ObjectSelection selection(run);selection.SelectAll(0);selection.SelectAll(1);
    CHECK(condition(ext,9,@[@0,@2,@2]));CHECK(run->rhOiList[0]->oilNumOfSelected==1 && run->rhOiList[0]->oilListSelected==0);
    CHECK(condition(ext,24,@[@0,@3,@2]));CHECK(!condition(ext,25,@[@0,@2,@2])); // no-object reads current SOL
    selection.SelectAll(0);CHECK(!negatedCondition(ext,11,@[@0,@2,@2]));
    CHECK(run->rhOiList[0]->oilNumOfSelected==1 && run->rhOiList[0]->oilListSelected==1);
    CHECK(condition(ext,21,@[@0]));CHECK(run->rhOiList[0]->oilNumOfSelected==2);
    b->hoFlags|=HOF_DESTROYED;CHECK(selection.Snapshot(0).size()==1);
    CHECK(condition(ext,11,@[@0,@3,@2]));CHECK(run->rhOiList[0]->oilListSelected==b->hoNumber);
    b->hoFlags&=~HOF_DESTROYED;selection.SelectAll(0);
    CHECK(condition(ext,22,@[@0,@1]));CHECK(run->rhOiList[0]->oilListSelected==0);
    selection.SelectAll(0);selection.SelectAll(1);
    short pairs[]={10,0,11,1};CQualToOiList* qualifier=[[[CQualToOiList alloc] init] autorelease];qualifier->nQoi=4;qualifier->qoiList=pairs;run->rhEvtProg->qualToOiList[0]=qualifier;
    CHECK(condition(ext,11,@[@(-32768),@2,@2]));CHECK(run->rhOiList[0]->oilNumOfSelected==1 && run->rhOiList[1]->oilNumOfSelected==1);
    selection.SelectNone(0);CHECK(selection.Snapshot(0).empty());
    --run->rhOiList[0]->oilEventCount;CHECK(selection.Snapshot(0).size()==2); // stale scope falls back to all
    selection.SelectAll(0);auto refs=selection.Snapshot(0);++a->hoCreationId;selection.Replace(0,refs);
    CHECK(run->rhOiList[0]->oilNumOfSelected==1);--a->hoCreationId;
    action(ext,3,@[a,@255,@1]);selection.SelectAll(0);CHECK(condition(ext,10,@[@0,@1]));CHECK(run->rhOiList[0]->oilListSelected==0);
    action(ext,4,@[@0,@1]);action(ext,8,@[a,@255]);selection.SelectAll(0);CHECK(condition(ext,19,@[@0]));CHECK(run->rhOiList[0]->oilListSelected==1);
    action(ext,9,@[@255]);action(ext,5,@[a,@1]);action(ext,6,@[]);
    action(ext,8,@[c,@0]);action(ext,8,@[b,@1]);action(ext,11,@[@1]);
    CHECK(condition(ext,13,@[@0,@0,@2,@2,@0,@3,@0,@9,@1,@2]));CHECK(condition(ext,16,@[]));
    CHECK([expression(ext,11,@[@2,@2,@0,@0,@3,@9,@0]) getInt]>=0);
    selection.SelectAll(0);(void)condition(ext,20,@[@0,@(-1)]);
    run->rhOiList[2]->oilName=@"Marker";run->rhMaxObjects=64;
    run->rhObjectList=(CObject**)realloc(run->rhObjectList,64*sizeof(CObject*));memset(run->rhObjectList+16,0,48*sizeof(CObject*));
    CreateDuplicateParam cp={};cp.cdpOi=12;cp.cdpHFII=-1;cp.cdpPos.posOINUMParent=-1;cp.cdpPos.posLayer=0;
    NSData* create=[NSData dataWithBytes:&cp length:sizeof(cp)];
    int previous=run->nextSlot;action(ext,10,@[a,create]);CHECK(run->nextSlot==previous+8);
    action(ext,13,@[a,@"Marker"]);CHECK(run->nextSlot==previous+16);
    action(ext,16,@[create,a,@0,@11,@31]);action(ext,17,@[@"Marker",a,@1,@21,@31]);
    action(ext,14,@[create]);action(ext,15,@[@"Marker"]);
    action(ext,7,@[@"area",@0]);CHECK(!condition(ext,16,@[]));
    action(ext,9,@[@(-1)]);action(ext,11,@[@0]);
    [ext destroyRunObject:NO];[ext release];
}
static void gridChecks(void) {
    CRunFindTheWay* ext=fresh();CRun* run=ext->rh;
    CHECK(condition(ext,0,@[@2,@2]));
    action(ext,19,@[@32,@10,@20]);
    action(ext,0,@[@1,@0,@255,@1,@0]);
    CObject* source=[[[CObject alloc] init] autorelease];source->hoType=2;
    source->hoImgXSpot=5;source->hoImgYSpot=7;
    source->roa=[[CRAni alloc] init];source->ros=[[CRSpr alloc] init];
    source->rov=[[CRVal alloc] init];source->rov->values=@[@31,@29];
    CAnimDir* frames=[[[CAnimDir alloc] init] autorelease];unsigned short handles[]={1,2,3};
    frames->adNumberOfFrame=3;frames->adFrames=handles;source->roa->raAnimDirOffset=frames;
    const int hotspots[][2]={{16,16},{0,16},{16,0}};
    for(int i=0;i<3;++i) {
        CImage* image=[[[CImage alloc] init] autorelease];image->width=image->height=32;
        image->xSpot=hotspots[i][0];image->ySpot=hotspots[i][1];
        [run->rhApp->imageBank->images setObject:image forKey:@(handles[i])];
    }
    // Expected Windows surface origins: cell center minus the source hotspot,
    // with R/B offsets only at internal obstacle edges, not the map boundary.
    const int expected[][3]={{1,21,29},{2,52,29},{1,21,61},{3,21,61},
                             {1,53,61},{2,53,61},{3,53,61}};
    for(int pass=0;pass<2;++pass) {
        action(ext,18,@[source,@1]);CHECK([run->backdrops count]==7);
        for(int i=0;i<7;++i) {
            CBkd2* backdrop=[run->backdrops objectAtIndex:i];
            CImage* image=[run->rhApp->imageBank getImageFromHandle:backdrop->img];
            CHECK(backdrop->img==expected[i][0]);
            // The Runtime's sprite subtracts the image hotspot; its visibility
            // rectangle separately subtracts CBkd2's stored hotspot.
            CHECK(backdrop->x-image->xSpot==expected[i][1]);
            CHECK(backdrop->y-image->ySpot==expected[i][2]);
            CHECK(backdrop->x-backdrop->spotX==expected[i][1]);
            CHECK(backdrop->y-backdrop->spotY==expected[i][2]);
        }
        [run->backdrops removeAllObjects];
        // Zero hotspots must retain the same Windows surface origins too.
        for(int i=0;i<3;++i) {
            CImage* image=[run->rhApp->imageBank getImageFromHandle:handles[i]];
            image->xSpot=image->ySpot=0;
        }
    }
    [ext destroyRunObject:NO];[ext release];
}
static void platformChecks(void) {
    CRunFindTheWay* ext=fresh();CRun* run=ext->rh;
    CHECK(condition(ext,2,@[@1,@0,@0,@0,@1,@1,@"collision"]));
    CHECK([expression(ext,8,@[@3,@3,@0,@1]) getInt]==255);
    CHECK([expression(ext,8,@[@2,@3,@0,@1]) getInt]==0);
    CObject* object=[[CObject alloc] init];object->hoNumber=0;object->hoCreationId=7;object->hoType=2;
    object->roa=[[CRAni alloc] init];CAnimDir* frames=[[[CAnimDir alloc] init] autorelease];unsigned short handles[]={1,2,3};frames->adNumberOfFrame=3;frames->adFrames=handles;object->roa->raAnimDirOffset=frames;
    object->ros=[[CRSpr alloc] init];run->rhObjectList[0]=object;
    CImage* image=[[[CImage alloc] init] autorelease];image->width=image->height=2;
    image->pixels=@[@0x00112233,@0x00445566,@0x00778899,@0x00aabbcc];
    unsigned int rawPixels[]={0xff112233,0xff445566,0xff778899,0xffaabbcc};
    image->data=rawPixels;image->dataLength=sizeof(rawPixels);image->bytesPrPixel=4;image->format=RGBA8888;image->bLineWidth=8;
    [run->rhApp->imageBank->images setObject:image forKey:@1];
    CHECK(condition(ext,26,@[@(7<<16),@0,@1,@0,@0]));
    CHECK([expression(ext,8,@[@0,@0,@0,@1]) getInt]==0x33);
    CHECK([expression(ext,8,@[@1,@1,@0,@2]) getInt]==0xbb);
    struct PixelCase {short format,bytes;unsigned raw;int r,g;BOOL premult;};
    const PixelCase cases[]={
        {RGBA8888,4,0x804080c8,200,128,NO}, {RGBA8888,4,0x80002040,128,64,YES},
        {RGB888,3,0x00112233,51,34,NO}, {RGBA4444,2,0xf84f,255,136,NO},
        {RGBA5551,2,0xf801,255,0,NO}, {RGB565,2,0xfbe0,255,125,NO}, {JPEG,4,0xff123456,86,52,NO}};
    unsigned char storage[16]={0};
    for(auto test:cases) {
        image->format=test.format;image->bytesPrPixel=test.bytes;image->bLineWidth=8;image->premultimage=test.premult;
        image->data=(unsigned*)storage;image->dataLength=16;
        for(int py=0;py<2;++py)for(int px=0;px<2;++px)memcpy(storage+py*8+px*test.bytes,&test.raw,test.bytes);
        CHECK(condition(ext,26,@[@(7<<16),@0,@1,@0,@0]));
        for(int py=0;py<2;++py)for(int px=0;px<2;++px) {
            CHECK([expression(ext,8,@[@(px),@(py),@0,@1]) getInt]==test.r);
            CHECK([expression(ext,8,@[@(px),@(py),@0,@2]) getInt]==test.g);
        }
    }
    image->dataLength=1;CHECK(!condition(ext,26,@[@(7<<16),@0,@1,@0,@0]));image->dataLength=16;
    fixtureFileImage=image;fixtureFileBytes=[NSData dataWithBytes:"png fixture" length:11];
    for(NSString* path in @[@"C:\\Project\\地图.png",@"maps/地图.png",@"/private/app/Documents/map.png",@"file:///private/app/Documents/map.png"]) {
        [run->rhApp->resources setObject:fixtureFileBytes forKey:path];
        CHECK(condition(ext,12,@[path,@1,@0,@0]));
        CHECK([run->rhApp->loadedPath isEqualToString:path]);
        CHECK([expression(ext,8,@[@1,@1,@0,@1]) getInt]==86);
    }
    fixtureFileImage=nil;fixtureFileBytes=nil;
    CHECK(condition(ext,0,@[@2,@2]));action(ext,18,@[object,@1]);CHECK([run->backdrops count]>0);
    CHECK(!condition(ext,27,@[@0,@1,@0,@0]));CHECK(!condition(ext,7,@[]));
    CHECK(!condition(ext,12,@[@"missing.png",@1,@0,@0]));
    [ext destroyRunObject:NO];[ext release];
}
int main(void) {
    NSAutoreleasePool* pool=[[NSAutoreleasePool alloc] init];
    CRunFindTheWay* ext=[[CRunFindTheWay alloc] init];
    ext->rh=[[[CRun alloc] init] autorelease]; ext->ho=[[[CExtension alloc] init] autorelease];
    CFile* file=[[[CFile alloc] init] autorelease];
    CHECK([ext createRunObject:file withCOB:nil andVersion:1]);
    CHECK([ext getNumberOfConditions]==29);
    CHECK(condition(ext,0,@[@4,@4]));
    CHECK(condition(ext,7,@[]));
    action(ext,0,@[@1,@1,@255,@1,@0]);
    CHECK([expression(ext,8,@[@1,@1,@0,@1]) getInt]==255);
    CHECK(condition(ext,8,@[@1,@1,@65536,@1,@0]));
    CHECK(condition(ext,8,@[@0,@0,@(-1),@1,@0]));
    CHECK(condition(ext,4,@[@0,@0,@3,@3,@9,@0,@1,@0,@0,@"walk"]));
    CHECK([expression(ext,3,@[@"walk"]) getInt]==7);
    CHECK([expression(ext,4,@[@"walk",@6,@0]) getInt]==3);
    NSString* encoded=[[[expression(ext,9,@[]) getString] copy] autorelease];
    CHECK(condition(ext,1,@[encoded]));
    CHECK([expression(ext,8,@[@1,@1,@0,@1]) getInt]==255);
    action(ext,4,@[@0,@1]);
    CHECK([expression(ext,8,@[@1,@1,@0,@1]) getInt]==0);
    action(ext,19,@[@32,@10,@20]);
    CHECK([expression(ext,6,@[@2,@1,@0]) getInt]==90);
    CHECK([expression(ext,6,@[@2,@1,@1]) getInt]==68);
    action(ext,19,@[@32,@(-1),@0]);
    CHECK([expression(ext,6,@[@2,@1,@0]) getInt]==79);
    CHECK([[expression(ext,10,@[@0,@0,@""]) getString] length]>0);
    CHECK([expression(ext,2,@[@0,@0,@3,@3,@9,@0,@1,@0,@0,@"walk"]) getInt]==7);
    Events events={ext,1,0,0}; ext->ho->event=event; ext->ho->context=&events;
    action(ext,2,@[@"walk",@0,@"outer"]);
    CHECK(events.outer==7 && events.inner==7);
    events.mode=2; events.outer=events.inner=0;
    action(ext,2,@[@"walk",@0,@"outer"]);
    CHECK(events.outer==7); // Snapshot survives replacing the native map.
    CHECK(condition(ext,4,@[@0,@0,@2,@2,@9,@0,@1,@0,@0,@"walk"]));
    events.mode=3; events.outer=0;
    action(ext,2,@[@"walk",@0,@"outer"]);
    CHECK(events.outer==1); // Destroying during iteration stops safely.
    [ext release];
    objectChecks();platformChecks();gridChecks();eventChecks();
    NSLog(@"Runtime adapter fixture checks passed");
    [pool drain]; return 0;
}
