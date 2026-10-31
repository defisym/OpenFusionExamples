// Owned Runtime adapter. Windows ACE behavior with iOS Runtime representations.
#import "CRunFindTheWay.h"
#include "FTWObjectHelpers.h"
#import "CRun.h"
#import "CObject.h"
#import "CExtension.h"
#import "CCndExtension.h"
#import "CActExtension.h"
#import "CEventProgram.h"
#import "CEvents.h"
#include <algorithm>
#include <vector>

namespace {
short oil(CCndExtension* cnd, CRun* run, int index) {
    return [cnd getParamObject:run withNum:index]->evp.evpW.evpW0;
}
bool negated(CCndExtension* cnd) {
    // initialize() stores the first parameter immediately after the condition.
    LPEVT event=(LPEVT)((char*)cnd->pParams[0]-CND_SIZE);
    return (event->evtFlags2&EVFLAG2_NOT)!=0;
}
bool grid(FTWMap map,CObject* object,FTWCoord& point) {
    return object && ftw_map_convert_coord(map,{object->hoX,object->hoY},1,&point)==FTW_OK;
}
bool equal(FTWCoord a,FTWCoord b) { return a.x==b.x && a.y==b.y; }
bool contains(NSData* data,FTWCoord point) {
    const FTWCoord* values=(const FTWCoord*)[data bytes];
    for(NSUInteger i=0;i<[data length]/sizeof(FTWCoord);++i) if(equal(values[i],point)) return true;
    return false;
}
FTWiOS::ObjectRef ref(CObject* o) { return {o->hoNumber,o->hoCreationId}; }
}

@implementation CRunFindTheWay (Objects)
-(BOOL)objectCondition:(int)num withCndExtension:(CCndExtension*)cnd {
    FTWiOS::Selection selection(rh);
    short type=oil(cnd,rh,0); bool negate=negated(cnd);
    if(num==21) {selection.selectAll(type);return YES;}
    if(num==18) {
        (void)oil(cnd,rh,1);
        NSString* name=[cnd getParamExpString:rh withNum:2];
        return map && iterationKind==18 && [iterationName isEqualToString:name]
            && selection.filter(type,negate,[&](CObject* o){return o==sourceObject;});
    }
    int x=0,y=0,layer=0,mode=0; short other=-1;
    if(num==9 || num==11 || num==24 || num==25) {
        x=[cnd getParamExpression:rh withNum:1];y=[cnd getParamExpression:rh withNum:2];
    }
    if(num==10) layer=[cnd getParamExpression:rh withNum:1];
    if(num==20) mode=[cnd getParamExpression:rh withNum:1];
    if(num==22) other=oil(cnd,rh,1);
    if(!map) return NO;
    if(num==24 || num==25) {
        bool found=false;
        selection.forEach(type,[&](CObject* object) {
            FTWCoord point={object->hoX,object->hoY};
            if(num==25 || grid(map,object,point)) found=found || equal(point,{x,y});
        });
        // Desktop no-object predicates do not change the SOL; Fusion negates the result.
        return !found;
    }
    if(num==19) {
        selection.selectAll(type); std::vector<FTWiOS::ObjectRef> retained;
        selection.forEach(type,[&](CObject* object) {
            FTWCoord point;int cost=255;
            if(grid(map,object,point)) ftw_map_sample_cost(map,point.x,point.y,FTW_TERRAIN,&cost);
            if(cost!=255 && !contains(units,point)) retained.push_back(ref(object));
        });
        // Desktop vector overload leaves the SelectAll scope intact on no matches.
        if(!retained.empty()) selection.replace(type,retained);
        return !retained.empty();
    }
    if(num==23) {
        std::vector<FTWCoord> seen; std::vector<FTWiOS::ObjectRef> retained;
        selection.forEach(type,[&](CObject* object) {
            FTWCoord point;if(!grid(map,object,point)) return;
            bool duplicate=std::find_if(seen.begin(),seen.end(),[&](FTWCoord p){return equal(p,point);})!=seen.end();
            if(duplicate==negate) retained.push_back(ref(object));
            if(!duplicate) seen.push_back(point);
        });
        if(!retained.empty()) selection.replace(type,retained);
        return !retained.empty();
    }
    // Cost reads can dispatch OnMapUpdate. Finish them before the SOL predicate,
    // which must remain pure even when a nested event removes/replaces objects.
    struct Cost { FTWiOS::ObjectRef object; int cost; };
    std::vector<Cost> costs;
    if(num==10) {
        const auto refs=selection.snapshot(type,FTWiOS::IterateDefault,true);
        FTWMap original=map;
        for(auto r:refs) {
            CObject* object=rh->rhObjectList[r.slot];
            if(!object || object->hoCreationId!=r.generation || object->hoX<0 || object->hoY<0) continue;
            FTWCoord point;int cost=255;
            if(grid(original,object,point)) ftw_map_sample_cost(original,point.x,point.y,layer,&cost);
            if(!alive || map!=original) return NO;
            costs.push_back({r,cost});
        }
    }
    struct Position { CObject* object; FTWCoord point; };
    std::vector<Position> others;
    if(num==22) selection.forEach(other,[&](CObject* object) {
        FTWCoord point;if(grid(map,object,point)) others.push_back({object,point});
    });
    return selection.filter(type,negate,[&](CObject* object) {
        if(num==11) return object->hoX==x && object->hoY==y;
        if((num==9 || num==10) && (object->hoX<0 || object->hoY<0)) return false;
        FTWCoord point;if(!grid(map,object,point)) return false;
        if(num==9) return equal(point,{x,y});
        if(num==10) {
            const auto current=ref(object);
            for(auto cost:costs) if(cost.object.slot==current.slot && cost.object.generation==current.generation) return cost.cost==255;
            return false;
        }
        if(num==20) {
            for(NSUInteger i=0;i<[area count];++i)
                if(contains([area objectAtIndex:i],point)) return mode==-1 || ((mode==1)==(i>=attackStart));
            return false;
        }
        if(num==22) for(auto p:others) if(object!=p.object && equal(point,p.point)) return true;
        return false;
    });
}
-(NSData*)objectZone:(id)object {
    FTWCoord start; if(!map || !grid(map,(CObject*)object,start)) return nil;
    uint64_t count=0; if(ftw_generate_zoc(start,NULL,0,&count)!=FTW_OK) return nil;
    NSMutableData* data=[NSMutableData dataWithLength:count*sizeof(FTWCoord)];
    return ftw_generate_zoc(start,(FTWCoord*)[data mutableBytes],count,&count)==FTW_OK?data:nil;
}
-(void)createPoints:(NSData*)points withActExtension:(CActExtension*)act parameter:(int)param byName:(BOOL)byName {
    if(!map || !points) return;
    // Freeze real coordinates before creation can initialize other extensions.
    NSMutableData* converted=[NSMutableData dataWithData:points];
    FTWCoord* values=(FTWCoord*)[converted mutableBytes];
    for(NSUInteger i=0;i<[converted length]/sizeof(FTWCoord);++i)
        if(ftw_map_convert_coord(map,values[i],0,&values[i])!=FTW_OK) return;
    FTWiOS::Creation creation(rh);
    short oi=byName?creation.findOI([act getParamExpString:rh withNum:param]):-1;
    CreateDuplicateParam original={};
    if(!byName) original=*(LPCDP)&act->pParams[param]->evp.evpW.evpW0;
    [self retain];
    @try {
        for(NSUInteger i=0;i<[converted length]/sizeof(FTWCoord) && alive;++i) {
            FTWiOS::CreateParams p;
            if(byName) {p.oi=oi;p.x=(short)values[i].x;p.y=(short)values[i].y;p.layer=0;}
            else {
                CreateDuplicateParam local=original;
                local.cdpPos.posX=(short)values[i].x;local.cdpPos.posY=(short)values[i].y;
                BOOL repeat=NO;
                if(!read_Position(rh,&local.cdpPos,0x11,&p.x,&p.y,&p.direction,&repeat,&p.layer)) continue;
                p.oi=local.cdpOi;p.frameItem=local.cdpHFII;
                if(repeat) {act->pEvent->evtFlags|=ACTFLAGS_REPEAT;rh->rhEvtProg->rh2ActionLoop=YES;}
                else act->pEvent->evtFlags&=~ACTFLAGS_REPEAT;
            }
            creation.create(p);
        }
    } @finally { [self release]; }
}
-(void)objectAction:(int)num withActExtension:(CActExtension*)act {
    if(num==18) { [self createGridWithAct:act]; return; }
    CObject* object=nil;
    if(num==3 || num==5 || num==8 || num==10 || num==13) object=[act getParamObject:rh withNum:0];
    if(num==16 || num==17) object=[act getParamObject:rh withNum:1];
    if(num==3) {
        int cost=(unsigned char)[act getParamExpression:rh withNum:1], layer=[act getParamExpression:rh withNum:2];
        FTWCoord point;
        if(layer!=FTW_MAP && grid(map,object,point)) ftw_map_set_cost(map,point.x,point.y,cost,layer);
        return;
    }
    if(num==5) {
        BOOL center=[act getParamExpression:rh withNum:1]!=0;
        FTWCoord point;if(!grid(map,object,point)) return;
        NSData* points=center?[self objectZone:object]:[NSData dataWithBytes:&point length:sizeof(point)];
        const FTWCoord* p=(const FTWCoord*)[points bytes];
        for(NSUInteger i=0;i<[points length]/sizeof(FTWCoord);++i) if(!contains(zones,p[i])) [zones appendBytes:&p[i] length:sizeof(FTWCoord)];
        return;
    }
    if(num==8) {
        int type=[act getParamExpression:rh withNum:1]; FTWCoord point;
        if(!grid(map,object,point)) return;
        NSMutableData* list=type==0?allies:(type==1?enemies:(type==255?units:nil));
        [list appendBytes:&point length:sizeof(point)];return;
    }
    if(num==12) {
        short type=act->pParams[0]->evp.evpW.evpW0;
        NSString* name=[[[act getParamExpString:rh withNum:1] copy] autorelease];
        if(!map) return;
        FTWiOS::Selection selection(rh);
        [self retain];
        @try {
            selection.forEach(type,[&](CObject* source) {
                if(!alive) return;
                id previous=[sourceObject retain];sourceObject=source;
                [source retain];
                @try { [self iterate:[self objectZone:source] real:YES name:name kind:18]; }
                @finally { sourceObject=previous;[previous autorelease];[source release]; }
            });
        } @finally { [self release]; }
        return;
    }
    if(num==10 || num==13) { [self createPoints:[self objectZone:object] withActExtension:act parameter:1 byName:num==13];return; }
    if(num==14 || num==15) {
        if(map && areaPosition<[area count]) {
            uint64_t generation=areaGeneration;
            isAttack=!(areaPosition+1<attackStart);
            NSData* points=[[[area objectAtIndex:areaPosition] copy] autorelease];
            [self createPoints:points withActExtension:act parameter:0 byName:num==15];
            if(generation==areaGeneration) ++areaPosition;
        }
        return;
    }
    if(num==16 || num==17) {
        int dir=[act getParamExpression:rh withNum:2], type=[act getParamExpression:rh withNum:3],ignore=[act getParamExpression:rh withNum:4];
        FTWCoord start;if(!grid(map,object,start)) return;
        [self syncLists];uint64_t count=0;
        if(ftw_map_aoe(map,start,(unsigned)dir,(unsigned)type,(unsigned)ignore,NULL,0,&count)!=FTW_OK) return;
        NSMutableData* data=[NSMutableData dataWithLength:count*sizeof(FTWCoord)];
        if(ftw_map_aoe(map,start,(unsigned)dir,(unsigned)type,(unsigned)ignore,(FTWCoord*)[data mutableBytes],count,&count)==FTW_OK)
            [self createPoints:data withActExtension:act parameter:0 byName:num==17];
    }
}
@end
