// Own API fixtures plus real Foundation and the actual C++ core/Runtime adapter.
#import "RuntimeAPI.h"
#import "CRunFindTheWay.h"
#include <stdlib.h>
#define CHECK(x) do { if (!(x)) { NSLog(@"FAILED line %d: %s",__LINE__,#x); abort(); } } while(0)
static CValue* makeValue(id value) {
    CValue* result=[[[CValue alloc] init] autorelease]; result->value=value; return result;
}
@implementation CValue
-(int)getInt { return [value intValue]; }
-(NSString*)getString { return [value isKindOfClass:[NSString class]]?value:[value description]; }
@end
@implementation CRun
-(CValue*)getTempValue:(int)value { return makeValue([NSNumber numberWithInt:value]); }
-(CValue*)getTempString:(NSString*)value { return makeValue(value); }
@end
@implementation CExtension
-(CValue*)getExpParam { return makeValue([parameters objectAtIndex:nextParameter++]); }
-(void)generateEvent:(int)code withParam:(int)param { if (event) event(context,code); }
@end
@implementation CRunExtension
@end
@implementation CFile
-(unsigned char)readAByte { return byte; }
@end
@implementation CCreateObjectInfo
@end
@implementation CCndExtension
-(int)getParamExpression:(CRun*)run withNum:(int)index { return [[parameters objectAtIndex:index] intValue]; }
-(NSString*)getParamExpString:(CRun*)run withNum:(int)index { return [parameters objectAtIndex:index]; }
@end
@implementation CActExtension
-(int)getParamExpression:(CRun*)run withNum:(int)index { return [[parameters objectAtIndex:index] intValue]; }
-(NSString*)getParamExpString:(CRun*)run withNum:(int)index { return [parameters objectAtIndex:index]; }
@end
static BOOL condition(CRunFindTheWay* extension, int code, NSArray* params) {
    CCndExtension* cnd=[[[CCndExtension alloc] init] autorelease]; cnd->parameters=params;
    return [extension condition:code withCndExtension:cnd];
}
static void action(CRunFindTheWay* extension, int code, NSArray* params) {
    CActExtension* act=[[[CActExtension alloc] init] autorelease]; act->parameters=params;
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
    action(ext,4,@[@1,@0]);
    CHECK([expression(ext,8,@[@1,@1,@0,@1]) getInt]==0);
    action(ext,19,@[@32,@10,@20]);
    CHECK([expression(ext,6,@[@2,@1,@0]) getInt]==90);
    CHECK([expression(ext,6,@[@2,@1,@1]) getInt]==68);
    action(ext,19,@[@32,@(-1),@0]);
    CHECK([expression(ext,6,@[@2,@1,@0]) getInt]==90);
    CHECK([[expression(ext,10,@[@0,@0,@""]) getString] length]==0);
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
    NSLog(@"Runtime adapter fixture checks passed");
    [pool drain]; return 0;
}
