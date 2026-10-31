#pragma once
#import <Foundation/Foundation.h>
@class CRun, CExtension, CObject,CObjInfo,CEventProgram,CRunFrame,CRunApp;
#ifdef __cplusplus
struct CRunMvtPhysics;struct FTWFixturePhysics;
#endif
typedef struct { int left,top,right,bottom; } CRect;
typedef struct {short posOINUMParent,posFlags,posX,posY,posSlope,posAngle;int posDir;short posTypeParent,posOiList,posLayer;} PositionParam;
typedef PositionParam* LPPOS;
typedef struct { PositionParam cdpPos;short cdpHFII,cdpOi;unsigned cdpFree;} CreateDuplicateParam;
typedef CreateDuplicateParam* LPCDP;
typedef struct FTWFixtureParam {short evpCode;union {struct {short evpW0,evpW1;char tail[64];} evpW;} evp;} *LPEVP;
typedef struct FTWFixtureEvent {unsigned char evtFlags,evtFlags2;char rest[14];} *LPEVT;
#define CND_SIZE 16
#define EVFLAG2_NOT 1
#define ACTFLAGS_REPEAT 1
@interface CValue : NSObject { @public id value; }
-(int)getInt;
-(NSString*)getString;
@end
@interface CRun : NSObject {
@public CObject** rhObjectList;CObjInfo** rhOiList;int rhMaxObjects,rhMaxOI,rhWindowX,rhWindowY;
    CEventProgram* rhEvtProg;CRunFrame* rhFrame;CRunApp* rhApp;NSMutableArray* backdrops;
    int nextSlot,physicsCalls;
#ifdef __cplusplus
    FTWFixturePhysics* rh4Box2DBase;
#endif
}
-(int)f_CreateObject:(short)frame withOIHandle:(short)oi andX:(int)x andY:(int)y andDir:(int)dir andFlags:(int)flags andLayer:(int)layer andNumCreation:(int)creation;
#ifdef __cplusplus
-(CRunMvtPhysics*)GetPhysicMovement:(CObject*)object;
#endif
-(void)addBackdrop2:(id)backdrop;
-(CValue*)getTempValue:(int)value;
-(CValue*)getTempString:(NSString*)value;
@end
@interface CExtension : NSObject {
@public NSArray* parameters; NSUInteger nextParameter;
    void (*event)(void*,int); void* context;
}
-(CValue*)getExpParam;
-(void)generateEvent:(int)code withParam:(int)param;
@end
@interface CRunExtension : NSObject { @public CRun* rh; CExtension* ho; }
@end
@interface CFile : NSObject { @public unsigned char byte; }
-(unsigned char)readAByte;
@end
@interface CCreateObjectInfo : NSObject
@end
@interface CCndExtension : NSObject { @public NSArray* parameters;NSMutableData* eventData;LPEVP pParams[16];LPEVT pEvent; }
-(LPEVP)getParamObject:(CRun*)run withNum:(int)index;
-(void)prepare;
-(int)getParamExpression:(CRun*)run withNum:(int)index;
-(NSString*)getParamExpString:(CRun*)run withNum:(int)index;
@end
@interface CActExtension : NSObject { @public NSArray* parameters;NSMutableData* eventData;LPEVP pParams[16];LPEVT pEvent; }
-(CObject*)getParamObject:(CRun*)run withNum:(int)index;
-(void)prepare;
-(int)getParamExpression:(CRun*)run withNum:(int)index;
-(NSString*)getParamExpString:(CRun*)run withNum:(int)index;
@end

// Owned model of the Runtime fields used by object/platform adapters, not SDK code.
@class COIList,CObject,CObjInfo,CQualToOiList,CEventProgram,CRunFrame,CRunApp,CImageBank,CImage,CRAni,CAnimDir,CRVal,CRSpr,CLayer;
#define OBSTACLE_TRANSPARENT 4
#define HOF_DESTROYED 1
#define OEFLAG_SPRITES 2
#define FLOPT_VISIBLE 1
#define FLOPT_TOHIDE 2
@interface CObjInfo:NSObject { @public short oilOi,oilType;NSString* oilName;int oilEventCount,oilListSelected,oilNumOfSelected,oilObject; }
@end
@interface CQualToOiList:NSObject { @public short nQoi;short* qoiList; }
@end
@interface CEventProgram:NSObject { @public CQualToOiList** qualToOiList;int rh2EventCount,rh2ActionCount,rh2ActionLoopCount;BOOL rh2ActionOn,rh2ActionLoop;LPEVT rh4ActionStart; }
-(void)evt_AddCurrentObject:(CObject*)object;
@end
@interface CRunFrame:NSObject { @public int nLayers;CLayer** layers;CRect leVirtualRect; }
-(BOOL)bkdCol_TestPoint:(int)x withY:(int)y andLayer:(int)layer andPlane:(int)plane;
@end
@interface CLayer:NSObject { @public int dwOptions; }
@end
@interface CImageBank:NSObject { @public NSMutableDictionary* images; }
-(CImage*)getImageFromHandle:(unsigned short)handle;
@end
@interface CRunApp:NSObject { @public CImageBank* imageBank;COIList* OIList;NSMutableDictionary* resources;NSString* loadedPath; }
-(NSData*)loadResourceData:(NSString*)path;
@end
@interface CObject:NSObject {
@public short hoCreationId,hoNumber,hoOi,hoType,hoFlags,hoOEFlags,hoNextSelected,hoNumNext;
    int hoX,hoY,hoImgXSpot,hoImgYSpot;CObjInfo* hoOiList;CRAni* roa;CRVal* rov;CRSpr* ros;
}
@end
@interface CAnimDir:NSObject { @public short adNumberOfFrame;unsigned short* adFrames; }
@end
@interface CRAni:NSObject { @public CAnimDir* raAnimDirOffset; }
@end
@interface CRSpr:NSObject { @public int rsEffect,rsEffectParam,rsEffectShader;BOOL hidden; }
-(void)obHide;
@end
@interface CRVal:NSObject { @public NSArray* values; }
-(CValue*)getValue:(int)index;
@end
@interface UIImage:NSObject
+(UIImage*)imageWithContentsOfFile:(NSString*)path;
+(UIImage*)imageNamed:(NSString*)path;
+(UIImage*)imageWithData:(NSData*)data;
@end
enum {RGBA8888,RGBA4444,RGBA5551,RGB888,RGB565,JPEG};
@interface CImage:NSObject { @public int width,height;NSArray* pixels;unsigned int* data;NSUInteger dataLength;short bytesPrPixel,format;int bLineWidth;BOOL premultimage; }
+(CImage*)loadUIImage:(UIImage*)image;
-(int)getPixel:(int)x withY:(int)y;
@end
@interface CBkd2:NSObject {
@public unsigned short img;int x,y,spotX,spotY,nLayer,obstacleType,colMode,inkEffect,inkEffectParam,bkdEffectShader;
}
-(id)initWithCRun:(CRun*)run;
-(int)checkOrCreateEffectIfNeeded:(CRunApp*)app andCOI:(id)oi;
@end
#ifdef __cplusplus
struct CRunMvtPhysics { int* calls;void CreateBody(CObject*){if(calls)++*calls;} };
struct FTWFixturePhysics {void (*pAddNormalObject)(FTWFixturePhysics*,CObject*);};
extern "C" {
#endif
void AddPhysicsAttractor(CRun*,CObject*);
BOOL read_Position(CRun*,LPPOS,unsigned,int*,int*,int*,BOOL*,int*);
#ifdef __cplusplus
}
#endif

@interface COIList:NSObject
-(id)getOIFromHandle:(short)handle;
@end
