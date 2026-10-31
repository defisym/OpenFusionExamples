#import "CRunFindTheWay.h"
#import "CRun.h"
#import "CExtension.h"
#import "CActExtension.h"
#import "CCndExtension.h"
#import "CFile.h"
#import "CValue.h"
#if defined(__APPLE__)
#import <TargetConditionals.h>
#endif
#include <stdlib.h>

// The supplied exporter searches SRCROOT for libraries. Mach-O autolink selects
// the correct platform archive without changing global Xcode language settings.
#if defined(__APPLE__) && TARGET_OS_IPHONE
#if TARGET_OS_SIMULATOR
__asm__(".linker_option \"-lFindTheWayCore_iphonesimulator\"");
#else
__asm__(".linker_option \"-lFindTheWayCore_iphoneos\"");
#endif
__asm__(".linker_option \"-lc++\"");
__asm__(".linker_option \"-liconv\"");
#endif

#ifdef __cplusplus
extern "C"
#endif
void ftw_ios_generate_event(CRun*, CExtension*, int);
static void updated(void* context,FTWMap map) { [(CRunFindTheWay*)context mapUpdated:map]; }
@implementation CRunFindTheWay
-(int)getNumberOfConditions { return 29; }
-(BOOL)createRunObject:(CFile*)file withCOB:(CCreateObjectInfo*)cob andVersion:(int)version {
    if (ftw_abi_version()!=2) return NO;
    isometric=[file readAByte]!=0;
    gridWidth=gridHeight=1;
    allies=[[NSMutableData alloc] init]; enemies=[[NSMutableData alloc] init];
    zones=[[NSMutableData alloc] init]; units=[[NSMutableData alloc] init];
    attackStart=65536;
    alive=YES;
    iterationKind=-1;
    return YES;
}
-(void)clearMap {
    ftw_map_set_callback(map,NULL,NULL);
    ftw_map_release(map); map=0;
    [lastPath release]; lastPath=nil;
    width=height=0;
    [area release]; area=nil; areaPosition=0; ++areaGeneration;
}
-(void)destroyRunObject:(BOOL)fast {
    alive=NO;
    [self clearMap];
    [allies release]; allies=nil; [enemies release]; enemies=nil;
    [zones release]; zones=nil; [units release]; units=nil;
    [iterationName release]; iterationName=nil;
}
-(int)handleRunObject { return 0; }
-(void)mapUpdated:(FTWMap)changed {
    if(alive && changed==map) {
        [area release];area=nil;++areaGeneration;
        [self retain];
        @try { ftw_ios_generate_event(rh,ho,17); }
        @finally { [self release]; }
    }
}
-(BOOL)configureGrid {
    ftw_map_set_callback(map,updated,self);
    return map && ftw_map_set_grid(map,isometric,gridWidth,gridHeight,offsetX,offsetY)==FTW_OK;
}
-(void)syncLists {
    FTWLists lists={(const FTWCoord*)[allies bytes],(const FTWCoord*)[enemies bytes],(const FTWCoord*)[zones bytes],
        [allies length]/sizeof(FTWCoord),[enemies length]/sizeof(FTWCoord),[zones length]/sizeof(FTWCoord)};
    ftw_map_set_lists(map,&lists);
}
-(NSData*)pathNamed:(NSString*)name {
    uint64_t count=0;
    if(ftw_map_named_path(map,[name UTF8String],NULL,0,&count)!=FTW_OK || count>SIZE_MAX/sizeof(FTWCoord)) return nil;
    NSMutableData* data=[NSMutableData dataWithLength:(NSUInteger)count*sizeof(FTWCoord)];
    return ftw_map_named_path(map,[name UTF8String],(FTWCoord*)[data mutableBytes],count,&count)==FTW_OK?data:nil;
}
-(BOOL)findPath:(int*)values save:(NSString*)name {
    [lastPath release]; lastPath=nil;
    if (!map) return NO;
    FTWCoord start={values[0],values[1]}, goal={values[2],values[3]};
    if (values[8] && (ftw_map_convert_coord(map,start,1,&start)!=FTW_OK ||
                      ftw_map_convert_coord(map,goal,1,&goal)!=FTW_OK)) return NO;
    unsigned flags=(values[5]?FTW_FIND_DIAGONAL:0) | (values[6]?FTW_FIND_CHECK_CORNERS:0) |
                   (values[7]?FTW_FIND_FORCE:0);
    uint64_t count=0;
    [self syncLists];
    FTWMap original=map;
    if (ftw_map_find_ex(original,start,goal,flags,(unsigned)values[4],[name UTF8String],&count)!=FTW_OK || !alive || map!=original || !count || count>SIZE_MAX/sizeof(FTWCoord)) return NO;
    FTWCoord* points=(FTWCoord*)malloc((size_t)count*sizeof(FTWCoord));
    if (!points) return NO;
    if (ftw_map_copy_path(map,points,count,&count)==FTW_OK)
        lastPath=[[NSData alloc] initWithBytes:points length:(NSUInteger)count*sizeof(FTWCoord)];
    free(points);

    return lastPath!=nil;
}
-(void)iterate:(NSData*)data real:(BOOL)real name:(NSString*)name kind:(int)kind {
    if (!data || !alive) return;
    if (real) {
        NSMutableData* converted=[NSMutableData dataWithData:data];
        FTWCoord* points=(FTWCoord*)[converted mutableBytes];
        for (NSUInteger i=0;i<[data length]/sizeof(FTWCoord);++i)
            if (ftw_map_convert_coord(map,points[i],0,&points[i])!=FTW_OK) return;
        data=converted;
    }
    [self retain]; [data retain];
    NSString* oldName=[iterationName retain];
    FTWCoord oldCoord=iterationCoord;
    int oldIndex=iterationIndex, oldKind=iterationKind;
    [iterationName release]; iterationName=[name copy]; iterationKind=kind;
    @try {
        const FTWCoord* points=(const FTWCoord*)[data bytes];
        NSUInteger count=[data length]/sizeof(FTWCoord);
        for (NSUInteger i=0; i<count && alive; ++i) {
            FTWCoord point=points[i];
            iterationCoord=point;
            if (kind==6) iterationIndex=(int)i;
            ftw_ios_generate_event(rh,ho,kind);
        }
    } @finally {
        [iterationName release]; iterationName=alive?oldName:nil;
        if (!alive) [oldName release];
        iterationCoord=oldCoord; iterationIndex=oldIndex; iterationKind=oldKind;
        [data release]; [self release];
    }
}
-(void)warn:(int)num category:(NSString*)category bits:(unsigned*)bits {
    if (num>=0 && num<32 && !(*bits & (1u<<num))) {
        *bits|=1u<<num;
        NSLog(@"FindTheWay: %@ %d is not implemented in this iOS build",category,num);
    }
}
-(BOOL)condition:(int)num withCndExtension:(CCndExtension*)cnd {
    [self retain];
    @try {return [self evaluateCondition:num withCndExtension:cnd];}
    @finally {[self release];}
}
-(BOOL)evaluateCondition:(int)num withCndExtension:(CCndExtension*)cnd {
    if (num==0) {
        int w=[cnd getParamExpression:rh withNum:0], h=[cnd getParamExpression:rh withNum:1];
        [self clearMap];
        gridWidth=gridHeight=1;offsetX=offsetY=0;
        if (ftw_map_create(w,h,&map)!=FTW_OK) return NO;
        width=w; height=h;
        if (![self configureGrid]) { [self clearMap]; return NO; }
        return YES;
    }
    if (num==1) {
        NSString* text=[cnd getParamExpString:rh withNum:0];
        NSData* encoded=[text dataUsingEncoding:NSASCIIStringEncoding];
        [self clearMap];
        gridWidth=gridHeight=1;offsetX=offsetY=0;
        if (!encoded || ftw_map_create_serialized((const char*)[encoded bytes],[encoded length],&map)!=FTW_OK) return NO;
        ftw_map_dimensions(map,&width,&height);
        if (![self configureGrid]) { [self clearMap]; return NO; }
        return YES;
    }
    if (num==4) {
        int values[9]; for(int i=0;i<9;++i) values[i]=[cnd getParamExpression:rh withNum:i];
        return [self findPath:values save:[cnd getParamExpString:rh withNum:9]];
    }
    if (num==5) return map && [[self pathNamed:[cnd getParamExpString:rh withNum:0]] length]>0;
    if (num==3 || num==6 || num==28) return iterationKind==num && [iterationName isEqualToString:[cnd getParamExpString:rh withNum:0]];
    if (num==7) return map!=0;
    if (num==8) {
        FTWCoord point={[cnd getParamExpression:rh withNum:0],[cnd getParamExpression:rh withNum:1]};
        int expected=[cnd getParamExpression:rh withNum:2], layer=[cnd getParamExpression:rh withNum:3], cost=255;
        if ([cnd getParamExpression:rh withNum:4] && ftw_map_convert_coord(map,point,1,&point)!=FTW_OK) return NO;
        return ftw_map_sample_cost(map,point.x,point.y,layer,&cost)==FTW_OK &&
            ((expected==-1 && cost!=255) || (expected==65536 && cost==255) || cost==expected);
    }
    if (num==2 || num==12 || num==26 || num==27) return [self platformCondition:num withCndExtension:cnd];
    if (num==18 || num==19 || num==20 || num==21 || num==22 || num==23 || (num>=9 && num<=11) || num==24 || num==25)
        return [self objectCondition:num withCndExtension:cnd];
    if (num==13) {
        int v[10]; for(int i=0;i<10;++i) v[i]=[cnd getParamExpression:rh withNum:i];
        if(!map) return NO;
        FTWCoord start={v[2],v[3]};
        if(v[4] && ftw_map_convert_coord(map,start,1,&start)!=FTW_OK) return NO;
        FTWAreaOptions options={start,v[0],v[1],v[5],v[6],v[7],v[8],v[9]};
        uint64_t rings=0,attack=65536,count=0;
        [self syncLists];FTWMap original=map;
        if(ftw_map_area(original,&options,&rings,&attack)!=FTW_OK || !alive || map!=original || ftw_map_copy_area(map,NULL,0,&count)!=FTW_OK
            || count>SIZE_MAX/sizeof(FTWAreaPoint)) return NO;
        NSMutableData* points=[NSMutableData dataWithLength:count*sizeof(FTWAreaPoint)];
        if(ftw_map_copy_area(map,(FTWAreaPoint*)[points mutableBytes],count,&count)!=FTW_OK) return NO;
        NSMutableArray* result=[NSMutableArray arrayWithCapacity:rings];
        for(uint64_t i=0;i<rings;++i) [result addObject:[NSMutableData data]];
        const FTWAreaPoint* p=(const FTWAreaPoint*)[points bytes];
        for(uint64_t i=0;i<count;++i) [[result objectAtIndex:p[i].ring] appendBytes:&p[i].coord length:sizeof(FTWCoord)];
        [area release]; area=[result copy]; areaPosition=0; ++areaGeneration;
        attackStart=attack; isAttack=v[8]?NO:v[1]!=0; return YES;
    }
    if(num==14) return iterationKind==14 && [iterationName isEqualToString:[cnd getParamExpString:rh withNum:0]];
    if(num==15) return isAttack;
    if(num==16) return areaPosition<[area count];
    if(num==17) return YES;
    [self warn:num category:@"condition" bits:&warnedConditions];
    return NO;
}
-(void)action:(int)num withActExtension:(CActExtension*)act {
    [self retain];
    @try {[self evaluateAction:num withActExtension:act];}
    @finally {[self release];}
}
-(void)evaluateAction:(int)num withActExtension:(CActExtension*)act {
    if (num==0) {
        FTWCoord point={[act getParamExpression:rh withNum:0],[act getParamExpression:rh withNum:1]};
        int cost=(unsigned char)[act getParamExpression:rh withNum:2], layer=[act getParamExpression:rh withNum:3];
        BOOL real=[act getParamExpression:rh withNum:4]!=0;
        if (map && layer!=FTW_MAP && (!real || ftw_map_convert_coord(map,point,1,&point)==FTW_OK))
            ftw_map_set_cost(map,point.x,point.y,cost,layer);
        return;
    }
    if (num==4) {
        int cost=(unsigned char)[act getParamExpression:rh withNum:0], layer=[act getParamExpression:rh withNum:1];
        ftw_map_clear_costs(map,cost,layer);
        return;
    }
    if (num==19) {
        unsigned grid=(unsigned)[act getParamExpression:rh withNum:0];
        int x=[act getParamExpression:rh withNum:1], y=[act getParamExpression:rh withNum:2];
        if(!map) return;
        offsetX=x; offsetY=y;
        gridWidth=(grid&65535)?(int)(grid&65535):1;
        gridHeight=isometric?((grid>>16)?(int)(grid>>16):1):gridWidth;
        [self configureGrid]; return;
    }
    if (num==2) {
        NSString* pathName=[[[act getParamExpString:rh withNum:0] copy] autorelease];
        BOOL real=[act getParamExpression:rh withNum:1]!=0;
        NSString* name=[act getParamExpString:rh withNum:2];
        [self iterate:[self pathNamed:pathName] real:real name:name kind:6]; return;
    }
    if (num==20) {
        BOOL real=[act getParamExpression:rh withNum:0]!=0;
        NSString* name=[act getParamExpString:rh withNum:1];
        if (!map) return;
        NSMutableData* snapshot=[NSMutableData dataWithLength:(NSUInteger)width*(NSUInteger)height*sizeof(FTWCoord)];
        FTWCoord* points=(FTWCoord*)[snapshot mutableBytes];
        for(int y=0;y<height;++y) for(int x=0;x<width;++x) points[(NSUInteger)y*width+x]=(FTWCoord){x,y};
        [self iterate:snapshot real:real name:name kind:28]; return;
    }
    if(num==1) return; // Continue: force Fusion to evaluate an otherwise empty event.
    if(num==3 || num==5 || num==8 || num==10 || (num>=12 && num<=18)) {
        [self objectAction:num withActExtension:act]; return;
    }
    if(num==6) { [zones setLength:0]; return; }
    if(num==9) {
        int target=[act getParamExpression:rh withNum:0];
        if(target==-1 || target==0) [allies setLength:0];
        if(target==-1 || target==1) [enemies setLength:0];
        if(target==-1 || target==255) [units setLength:0];
        return;
    }
    if(num==11) { ftw_map_stash(map,[act getParamExpression:rh withNum:0]); return; }
    if(num==7) {
        NSString* name=[[[act getParamExpString:rh withNum:0] copy] autorelease];
        BOOL once=[act getParamExpression:rh withNum:1]!=0;
        if(!map) return;
        NSArray* snapshot=[[area copy] autorelease]; uint64_t generation=areaGeneration;
        uint64_t first=once?areaPosition:0, end=once?first+1:[snapshot count];
        [self retain];
        @try {
            for(uint64_t i=first;i<end && i<[snapshot count] && alive;++i) {
                isAttack=i>=attackStart;
                [self iterate:[snapshot objectAtIndex:i] real:NO name:name kind:14];
                if(generation==areaGeneration) ++areaPosition;
            }
        } @finally { [self release]; }
        return;
    }
    [self warn:num category:@"action" bits:&warnedActions];
}
-(CValue*)expression:(int)num {
    [self retain];
    @try {return [self evaluateExpression:num];}
    @finally {[self release];}
}
-(CValue*)evaluateExpression:(int)num {
    static const int counts[20]={0,0,10,1,3,3,3,0,4,0,3,7,5,0,0,0,0,0,1,2};
    int values[10]={0}; NSString* name=nil;
    if (num>=0 && num<20) for(int i=0;i<counts[num];++i) {
        CValue* value=[ho getExpParam];
        if ((num==2 && i==9) || ((num==3 || num==4) && i==0) || (num==10 && i==2))
            name=[[[value getString] copy] autorelease];
        else values[i]=[value getInt];
    }
    int result=0;
    switch(num) {
        case 0: result=iterationCoord.x; break;
        case 1: result=iterationCoord.y; break;
        case 2: result=[self findPath:values save:name]?(int)([lastPath length]/sizeof(FTWCoord)):65536; break;
        case 3: { NSData* data=[self pathNamed:name]; result=[data length]?(int)([data length]/sizeof(FTWCoord)):65536; break; }
        case 4: {
            NSData* data=[self pathNamed:name]; int step=values[1], type=values[2]; result=65536;
            if (step>=0 && (type==0 || type==1) && (NSUInteger)step<[data length]/sizeof(FTWCoord)) {
                FTWCoord point=((const FTWCoord*)[data bytes])[step]; result=type==0?point.x:point.y;
            } break;
        }
        case 5: case 6: {
            FTWCoord point={values[0],values[1]}; result=num==5?65536:INT32_MAX;
            if ((values[2]==0 || values[2]==1) && ftw_map_convert_coord(map,point,num==5,&point)==FTW_OK)
                result=values[2]==0?point.x:point.y;
            break;
        }
        case 7: result=iterationIndex; break;
        case 8: {
            FTWCoord point={values[0],values[1]}; result=255;
            if (!values[2] || ftw_map_convert_coord(map,point,1,&point)==FTW_OK)
                ftw_map_sample_cost(map,point.x,point.y,values[3],&result);
            break;
        }
        case 9: {
            uint64_t count=0;
            if (ftw_map_export(map,NULL,0,&count)!=FTW_OK || count>SIZE_MAX) return [rh getTempString:@"InvalidMap"];
            NSMutableData* data=[NSMutableData dataWithLength:(NSUInteger)count];
            if (ftw_map_export(map,(char*)[data mutableBytes],count,&count)!=FTW_OK) return [rh getTempString:@"InvalidMap"];
            NSString* text=[[[NSString alloc] initWithBytes:[data bytes] length:(NSUInteger)count encoding:NSASCIIStringEncoding] autorelease];
            return [rh getTempString:text];
        }
        case 10: {
            if(!map) return [rh getTempString:@"InvalidMap"];
            uint64_t count=0;
            if(ftw_map_display(map,values[0],values[1],[name UTF8String],NULL,0,&count)!=FTW_OK || count>SIZE_MAX) return [rh getTempString:@""];
            NSMutableData* data=[NSMutableData dataWithLength:count];
            if(ftw_map_display(map,values[0],values[1],[name UTF8String],(char*)[data mutableBytes],count,&count)!=FTW_OK) return [rh getTempString:@""];
            return [rh getTempString:[[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease]];
        }
        case 11: {
            FTWCoord start={values[0],values[1]};
            [self syncLists];
            if(!values[2] || ftw_map_convert_coord(map,start,1,&start)==FTW_OK)
                ftw_map_line_range(map,start,values[3],values[4],(unsigned)values[5],values[6],&result);
            break;
        }
        case 12: result=(values[0]?16:0)|(values[1]?8:0)|(values[2]?4:0)|(values[3]?2:0)|(values[4]?1:0); break;
        case 13: result=FTW_MAP; break;
        case 14: result=FTW_TERRAIN; break;
        case 15: result=FTW_DYNAMIC; break;
        case 16: result=0; break;
        case 17: result=255; break;
        case 18: result=values[0]<0?255:(values[0]>255?255:values[0]); break;
        case 19: {
            unsigned w=(unsigned)values[0]&65535, h=(unsigned)values[1]&65535;
            result=(int)((w?w:1)|((h?h:1)<<16)); break;
        }
        default: [self warn:num category:@"expression" bits:&warnedExpressions]; break;
    }
    return [rh getTempValue:result];
}
@end
