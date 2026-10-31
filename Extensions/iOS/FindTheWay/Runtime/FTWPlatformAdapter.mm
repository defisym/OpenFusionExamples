// Platform-only I/O. Algorithms and sampling semantics follow Windows Main.cpp.
#import "CRunFindTheWay.h"
#import "CRun.h"
#import "CRunApp.h"
#import "CRunFrame.h"
#import "CObject.h"
#import "CCndExtension.h"
#import "CActExtension.h"
#import "CImage.h"
#import "CImageBank.h"
#import "COIList.h"
#import "CRAni.h"
#import "CAnimDir.h"
#import "CRVal.h"
#import "CRSpr.h"
#import "CValue.h"
#import "CBkd2.h"
#import "CEvents.h"
#import <UIKit/UIKit.h>
#include <algorithm>
#include <limits.h>
#include <vector>
#include <cstring>

namespace {
// Read logical CPU colors, not rendered/premultiplied display colors. CImage's
// getPixel() is used only to reload CPU data if the Runtime discarded it.
bool readCostPixel(CImage* image,int x,int y,int& terrain,int& dynamic) {
    if(x<0 || y<0 || x>=image->width || y>=image->height || !image->data) return false;
    unsigned bytes=image->bytesPrPixel;
    if(bytes<2 || bytes>4) return false;
    uint64_t pitch=image->bLineWidth>0?(uint64_t)image->bLineWidth:(uint64_t)image->width*bytes;
    uint64_t offset=(uint64_t)y*pitch+(uint64_t)x*bytes;
    if(image->dataLength && (offset>image->dataLength || bytes>image->dataLength-offset)) return false;
    const unsigned char* p=(const unsigned char*)image->data+offset;
    unsigned r=0,g=0,a=255;uint16_t packed=0;
    if(bytes==2) std::memcpy(&packed,p,sizeof(packed));
    switch(image->format) {
        case RGBA8888: case JPEG:
            if(bytes!=4) return false;r=p[0];g=p[1];a=p[3];break;
        case RGB888:
            r=p[0];g=p[1];break;
        case RGBA4444:
            r=((packed>>12)&15)*17;g=((packed>>8)&15)*17;a=(packed&15)*17;break;
        case RGBA5551:
            r=((packed>>11)&31)*255/31;g=((packed>>6)&31)*255/31;a=(packed&1)*255;break;
        case RGB565:
            r=((packed>>11)&31)*255/31;g=((packed>>5)&63)*255/63;break;
        default:return false;
    }
    if(image->premultimage && a && a!=255) {r=std::min(255u,(r*255+a/2)/a);g=std::min(255u,(g*255+a/2)/a);}
    terrain=(int)r;dynamic=(int)g;return true;
}
}

@implementation CRunFindTheWay (Platform)
-(BOOL)mapFromImage:(CImage*)image grid:(unsigned)packed x:(int)x y:(int)y {
    if(!image || image->width<=0 || image->height<=0) return NO;
    (void)[image getPixel:0 withY:0]; // Restore purged CPU pixels through the SDK.
    if(!image->data) return NO;
    int size=(packed&65535)?(packed&65535):1;
    if(ftw_map_create(image->width/size,image->height/size,&map)!=FTW_OK) return NO;
    ftw_map_dimensions(map,&width,&height);
    gridWidth=size;gridHeight=size;offsetX=x;offsetY=y;
    // Windows samples image maps in orthogonal space, then applies isometric mode.
    if(ftw_map_set_grid(map,0,size,size,x,y)!=FTW_OK) {[self clearMap];return NO;}
    for(int gy=0;gy<height;++gy) for(int gx=0;gx<width;++gx) {
        FTWCoord point;
        if(ftw_map_convert_coord(map,{gx,gy},0,&point)!=FTW_OK || point.x<0 || point.y<0 || point.x>=image->width || point.y>=image->height) continue;
        int terrain=0,dynamic=0;
        if(!readCostPixel(image,point.x,point.y,terrain,dynamic)) {[self clearMap];return NO;}
        ftw_map_set_cost(map,gx,gy,terrain,FTW_TERRAIN);
        ftw_map_set_cost(map,gx,gy,dynamic,FTW_DYNAMIC);
    }
    gridHeight=isometric?std::max(size/2,1):size;
    if(![self configureGrid]) {[self clearMap];return NO;}
    return YES;
}
-(BOOL)platformCondition:(int)num withCndExtension:(CCndExtension*)cnd {
    [self clearMap];
    if(num==27) {
        // A Windows cSurface pointer is not an iOS resource identifier or pointer.
        for(int i=0;i<4;++i) (void)[cnd getParamExpression:rh withNum:i];
        NSLog(@"FindTheWay: Windows cSurface pointer import is unavailable on iOS; use a picture or Active image");
        return NO;
    }
    if(num==12) {
        NSString* path=[[[cnd getParamExpString:rh withNum:0] copy] autorelease];
        unsigned packed=(unsigned)[cnd getParamExpression:rh withNum:1];
        int x=[cnd getParamExpression:rh withNum:2], y=[cnd getParamExpression:rh withNum:3];
        if(![path length]) return NO;
        // Preserve Fusion's path normalization, sandbox/resource precedence,
        // embedded-file lookup and security-scoped file URL handling.
        NSData* bytes=[rh->rhApp loadResourceData:path];
        UIImage* source=bytes?[UIImage imageWithData:bytes]:nil;
        CImage* image=source?[CImage loadUIImage:source]:nil;
        BOOL result=[self mapFromImage:image grid:packed x:x y:y];
        [image release];return result;
    }
    if(num==26) {
        unsigned fixed=(unsigned)[cnd getParamExpression:rh withNum:0];
        int frame=[cnd getParamExpression:rh withNum:1];
        unsigned packed=(unsigned)[cnd getParamExpression:rh withNum:2];
        int x=[cnd getParamExpression:rh withNum:3], y=[cnd getParamExpression:rh withNum:4];
        int slot=(int)(fixed&65535);
        CObject* object=slot<rh->rhMaxObjects?rh->rhObjectList[slot]:nil;
        if(!object || (unsigned short)object->hoCreationId!=(fixed>>16) || (object->hoFlags&HOF_DESTROYED) || object->hoType!=2
            || !object->roa || !object->roa->raAnimDirOffset || frame<0 || frame>=object->roa->raAnimDirOffset->adNumberOfFrame) return NO;
        CImage* image=[rh->rhApp->imageBank getImageFromHandle:object->roa->raAnimDirOffset->adFrames[frame]];
        return [self mapFromImage:image grid:packed x:x y:y];
    }
    unsigned packed=(unsigned)[cnd getParamExpression:rh withNum:0];
    int x=[cnd getParamExpression:rh withNum:1], y=[cnd getParamExpression:rh withNum:2];
    BOOL events=[cnd getParamExpression:rh withNum:3]!=0;
    int layer=[cnd getParamExpression:rh withNum:4]-1, type=[cnd getParamExpression:rh withNum:5];
    NSString* name=[[[cnd getParamExpString:rh withNum:6] copy] autorelease];
    if(type<1 || type>2 || rh->rhFrame->nLayers<=0 || layer<-1 || layer>=rh->rhFrame->nLayers) return NO;
    gridWidth=(packed&65535)?(int)(packed&65535):1;
    gridHeight=isometric?((packed>>16)?(int)(packed>>16):1):gridWidth;offsetX=x;offsetY=y;
    int frameWidth=rh->rhFrame->leVirtualRect.right-rh->rhFrame->leVirtualRect.left;
    int frameHeight=rh->rhFrame->leVirtualRect.bottom-rh->rhFrame->leVirtualRect.top;
    int w=frameWidth/gridWidth,h=frameHeight/gridWidth;
    if(isometric) {int sum=std::min((int64_t)frameWidth*2/gridWidth,(int64_t)frameHeight*2/gridHeight);h=sum/2;w=h+(sum%2==0);}
    if(ftw_map_create(w,h,&map)!=FTW_OK) return NO;
    width=w;height=h;
    if(![self configureGrid]) {[self clearMap];return NO;}
    if(events) {
        NSMutableData* snapshot=[NSMutableData dataWithLength:(NSUInteger)w*h*sizeof(FTWCoord)];
        FTWCoord* points=(FTWCoord*)[snapshot mutableBytes];
        for(int gy=0;gy<h;++gy) for(int gx=0;gx<w;++gx) points[(NSUInteger)gy*w+gx]={gx,gy};
        [self iterate:snapshot real:YES name:name kind:3];
    } else {
        FTWMap original=map;
        for(int gy=0;gy<h && alive && map==original;++gy) for(int gx=0;gx<w && alive && map==original;++gx) {
            FTWCoord point;
            if(ftw_map_convert_coord(original,{gx,gy},0,&point)==FTW_OK && [rh->rhFrame bkdCol_TestPoint:point.x-rh->rhWindowX withY:point.y-rh->rhWindowY andLayer:layer andPlane:0])
                ftw_map_set_cost(original,gx,gy,255,type);
        }
    }
    return map!=0;
}
-(void)createGridWithAct:(CActExtension*)act {
    CObject* source=[act getParamObject:rh withNum:0];
    int layer=[act getParamExpression:rh withNum:1]-1;
    if(!map || !source || (source->hoFlags&HOF_DESTROYED) || source->hoType!=2 || !source->roa || !source->roa->raAnimDirOffset
        || !source->ros || layer<0 || layer>=rh->rhFrame->nLayers) return;
    CAnimDir* frames=source->roa->raAnimDirOffset;
    int r=source->rov?[[source->rov getValue:0] getInt]:0,b=source->rov?[[source->rov getValue:1] getInt]:0;
    int hotX=source->hoImgXSpot,hotY=source->hoImgYSpot;
    int effect=source->ros->rsEffect,param=source->ros->rsEffectParam,shader=source->ros->rsEffectShader;
    short sourceOI=source->hoOi;
    std::vector<unsigned short> images(frames->adFrames,frames->adFrames+frames->adNumberOfFrame);
    auto add=[&](int frame,int x,int y) {
        if((size_t)frame>=images.size()) return;
        CBkd2* backdrop=[[CBkd2 alloc] initWithCRun:rh];
        backdrop->img=images[frame];backdrop->x=x;backdrop->y=y;
        // Backdrop coordinates are top-left; no image hotspot is subtracted twice.
        backdrop->spotX=0;backdrop->spotY=0;backdrop->nLayer=(short)layer;
        backdrop->obstacleType=OBSTACLE_TRANSPARENT;backdrop->colMode=0;
        backdrop->inkEffect=effect;backdrop->inkEffectParam=param;
        if(shader!=-1) backdrop->bkdEffectShader=[backdrop checkOrCreateEffectIfNeeded:rh->rhApp andCOI:[rh->rhApp->OIList getOIFromHandle:sourceOI]];
        [rh addBackdrop2:backdrop]; // Runtime consumes ownership, including duplicates.
    };
    FTWMap original=map;
    for(int gy=0;gy<height && alive && map==original;++gy) for(int gx=0;gx<width && alive && map==original;++gx) {
        int cost=255;ftw_map_sample_cost(original,gx,gy,FTW_MAP,&cost);
        if(!alive || map!=original) return;
        if(cost==255) continue;
        FTWCoord point;if(ftw_map_convert_coord(original,{gx,gy},0,&point)!=FTW_OK) continue;
        add(0,point.x-hotX,point.y-hotY);
        cost=255;ftw_map_sample_cost(original,gx+1,gy,FTW_MAP,&cost);
        if(cost==255) add(1,point.x-hotX+(gx+1==width?0:r),point.y-hotY);
        cost=255;ftw_map_sample_cost(original,gx,gy+1,FTW_MAP,&cost);
        if(cost==255) add(2,point.x-hotX,point.y-hotY+(gy+1==height?0:b));
    }
}
@end
