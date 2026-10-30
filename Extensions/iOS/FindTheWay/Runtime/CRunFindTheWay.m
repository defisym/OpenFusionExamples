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

@implementation CRunFindTheWay
-(int)getNumberOfConditions { return 29; }
-(BOOL)createRunObject:(CFile*)file withCOB:(CCreateObjectInfo*)cob andVersion:(int)version {
    if (ftw_abi_version()!=1) return NO;
    isometric=[file readAByte]!=0;
    gridWidth=gridHeight=1;
    paths=[[NSMutableDictionary alloc] init];
    alive=YES;
    iterationKind=-1;
    return YES;
}
-(void)clearMap {
    ftw_map_release(map); map=0;
    [lastPath release]; lastPath=nil;
    [paths removeAllObjects];
    width=height=0;
}
-(void)destroyRunObject:(BOOL)fast {
    alive=NO;
    [self clearMap];
    [paths release]; paths=nil;
    [iterationName release]; iterationName=nil;
}
-(int)handleRunObject { return 0; }
-(BOOL)configureGrid {
    return map && ftw_map_set_grid(map,isometric,gridWidth,gridHeight,offsetX,offsetY)==FTW_OK;
}
-(NSData*)pathNamed:(NSString*)name {
    return [name length] ? [paths objectForKey:name] : lastPath;
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
    if (ftw_map_find_path(map,start,goal,flags,&count)!=FTW_OK || !count || count>SIZE_MAX/sizeof(FTWCoord)) return NO;
    FTWCoord* points=(FTWCoord*)malloc((size_t)count*sizeof(FTWCoord));
    if (!points) return NO;
    if (ftw_map_copy_path(map,points,count,&count)==FTW_OK)
        lastPath=[[NSData alloc] initWithBytes:points length:(NSUInteger)count*sizeof(FTWCoord)];
    free(points);
    if (lastPath && [name length]) [paths setObject:lastPath forKey:name];
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
            [ho generateEvent:kind withParam:0];
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
    if (num==0) {
        int w=[cnd getParamExpression:rh withNum:0], h=[cnd getParamExpression:rh withNum:1];
        [self clearMap];
        if (ftw_map_create(w,h,&map)!=FTW_OK) return NO;
        width=w; height=h;
        if (![self configureGrid]) { [self clearMap]; return NO; }
        return YES;
    }
    if (num==1) {
        NSString* text=[cnd getParamExpString:rh withNum:0];
        NSData* encoded=[text dataUsingEncoding:NSASCIIStringEncoding];
        [self clearMap];
        if (!encoded || ftw_map_create_serialized((const char*)[encoded bytes],[encoded length],&map)!=FTW_OK) return NO;
        NSArray* fields=[text componentsSeparatedByString:@":"];
        NSUInteger first=[text hasPrefix:@"FTW2:"]?1:0;
        width=[[fields objectAtIndex:first] intValue]; height=[[fields objectAtIndex:first+1] intValue];
        if (![self configureGrid]) { [self clearMap]; return NO; }
        return YES;
    }
    if (num==4) {
        int values[9]; for(int i=0;i<9;++i) values[i]=[cnd getParamExpression:rh withNum:i];
        return [self findPath:values save:[cnd getParamExpString:rh withNum:9]];
    }
    if (num==5) return map && [[self pathNamed:[cnd getParamExpString:rh withNum:0]] length]>0;
    if (num==6 || num==28) return iterationKind==num && [iterationName isEqualToString:[cnd getParamExpString:rh withNum:0]];
    if (num==7) return map!=0;
    if (num==8) {
        FTWCoord point={[cnd getParamExpression:rh withNum:0],[cnd getParamExpression:rh withNum:1]};
        int expected=[cnd getParamExpression:rh withNum:2], layer=[cnd getParamExpression:rh withNum:3], cost=255;
        if ([cnd getParamExpression:rh withNum:4] && ftw_map_convert_coord(map,point,1,&point)!=FTW_OK) return NO;
        return ftw_map_get_cost(map,point.x,point.y,layer,&cost)==FTW_OK &&
            ((expected==-1 && cost!=255) || (expected==65536 && cost==255) || cost==expected);
    }
    if (num==2 || num==12 || num==26 || num==27) [self clearMap];
    [self warn:num category:@"condition" bits:&warnedConditions];
    return NO;
}
-(void)action:(int)num withActExtension:(CActExtension*)act {
    if (num==0) {
        FTWCoord point={[act getParamExpression:rh withNum:0],[act getParamExpression:rh withNum:1]};
        int cost=[act getParamExpression:rh withNum:2], layer=[act getParamExpression:rh withNum:3];
        BOOL real=[act getParamExpression:rh withNum:4]!=0;
        if (map && layer!=FTW_MAP && (!real || ftw_map_convert_coord(map,point,1,&point)==FTW_OK))
            ftw_map_set_cost(map,point.x,point.y,cost,layer);
        return;
    }
    if (num==4) {
        int layer=[act getParamExpression:rh withNum:0], cost=[act getParamExpression:rh withNum:1];
        if (map && (layer==FTW_TERRAIN || layer==FTW_DYNAMIC))
            for (int y=0;y<height;++y) for (int x=0;x<width;++x)
                ftw_map_set_cost(map,x,y,cost,layer);
        return;
    }
    if (num==19) {
        unsigned grid=(unsigned)[act getParamExpression:rh withNum:0];
        int x=[act getParamExpression:rh withNum:1], y=[act getParamExpression:rh withNum:2];
        if (x<0 || y<0) { NSLog(@"FindTheWay: grid offsets must be nonnegative"); return; }
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
    [self warn:num category:@"action" bits:&warnedActions];
}
-(CValue*)expression:(int)num {
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
            FTWCoord point={values[0],values[1]}; result=65536;
            if ((values[2]==0 || values[2]==1) && ftw_map_convert_coord(map,point,num==5,&point)==FTW_OK)
                result=values[2]==0?point.x:point.y;
            break;
        }
        case 7: result=iterationIndex; break;
        case 8: {
            FTWCoord point={values[0],values[1]}; result=255;
            if (!values[2] || ftw_map_convert_coord(map,point,1,&point)==FTW_OK)
                ftw_map_get_cost(map,point.x,point.y,values[3],&result);
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
        case 10: [self warn:num category:@"expression" bits:&warnedExpressions]; return [rh getTempString:@""];
        case 12: result=(values[0]?16:0)|(values[1]?8:0)|(values[2]?4:0)|(values[3]?2:0)|(values[4]?1:0); break;
        case 13: result=FTW_MAP; break;
        case 14: result=FTW_TERRAIN; break;
        case 15: result=FTW_DYNAMIC; break;
        case 16: result=0; break;
        case 17: result=255; break;
        case 18: result=values[0]<0?0:(values[0]>255?255:values[0]); break;
        case 19: {
            unsigned w=(unsigned)values[0]&65535, h=(unsigned)values[1]&65535;
            result=(int)((w?w:1)|((h?h:1)<<16)); break;
        }
        default: [self warn:num category:@"expression" bits:&warnedExpressions]; break;
    }
    return [rh getTempValue:result];
}
@end
