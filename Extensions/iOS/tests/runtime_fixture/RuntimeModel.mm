// Owned Runtime model shared by generic helper and extension tests.
#import "RuntimeAPI.h"
#include <stdlib.h>
#include <cstring>
CImage* fixtureFileImage=nil;NSData* fixtureFileBytes=nil;
static CValue* makeValue(id value) {
    CValue* result=[[[CValue alloc] init] autorelease]; result->value=value; return result;
}
@implementation CValue
-(int)getInt { return [value intValue]; }
-(NSString*)getString { return [value isKindOfClass:[NSString class]]?value:[value description]; }
@end
@implementation CRun
-(id)init {
    if((self=[super init])) {
        rhMaxObjects=16;rhMaxOI=3;rhObjectList=(CObject**)calloc(rhMaxObjects,sizeof(CObject*));rhOiList=(CObjInfo**)calloc(rhMaxOI,sizeof(CObjInfo*));
        rhEvtProg=[[CEventProgram alloc] init];rhEvtProg->rh2EventCount=1;
        rhEvtProg->qualToOiList=(CQualToOiList**)calloc(1,sizeof(CQualToOiList*));
        rhFrame=[[CRunFrame alloc] init];rhFrame->nLayers=1;rhFrame->layers=(CLayer**)calloc(1,sizeof(CLayer*));
        rhFrame->layers[0]=[[CLayer alloc] init];rhFrame->layers[0]->dwOptions=FLOPT_VISIBLE;
        rhFrame->leVirtualRect={0,0,8,8};
        rhApp=[[CRunApp alloc] init];rhApp->imageBank=[[CImageBank alloc] init];rhApp->imageBank->images=[[NSMutableDictionary alloc] init];
        backdrops=[[NSMutableArray alloc] init];rhApp->resources=[[NSMutableDictionary alloc] init];
        for(int i=0;i<rhMaxOI;++i) {rhOiList[i]=[[CObjInfo alloc] init];rhOiList[i]->oilOi=10+i;rhOiList[i]->oilType=2;rhOiList[i]->oilObject=-1;rhOiList[i]->oilListSelected=-1;}
    } return self;
}
-(void)dealloc {
    for(int i=0;i<rhMaxObjects;++i) [rhObjectList[i] release];free(rhObjectList);
    for(int i=0;i<rhMaxOI;++i) [rhOiList[i] release];free(rhOiList);
    free(rhEvtProg->qualToOiList);[rhEvtProg release];
    [rhFrame->layers[0] release];free(rhFrame->layers);[rhFrame release];
    [rhApp->resources release];[rhApp->loadedPath release];[rhApp->imageBank->images release];[rhApp->imageBank release];[rhApp release];[backdrops release];[super dealloc];
}
-(int)f_CreateObject:(short)frame withOIHandle:(short)oi andX:(int)x andY:(int)y andDir:(int)dir andFlags:(int)flags andLayer:(int)layer andNumCreation:(int)creation {
    int slot=nextSlot++;if(slot<0 || slot>=rhMaxObjects) return -1;
    int type=0;while(type<rhMaxOI && (!rhOiList[type] || rhOiList[type]->oilOi!=oi)) ++type;
    if(type==rhMaxOI) return -1;
    CObject* object=[[CObject alloc] init];object->hoCreationId=1;object->hoNumber=slot;object->hoOi=oi;object->hoOiList=rhOiList[type];
    object->hoX=x;object->hoY=y;object->hoType=2;object->hoOEFlags=OEFLAG_SPRITES;object->ros=[[CRSpr alloc] init];
    object->hoNumNext=rhOiList[type]->oilObject;rhOiList[type]->oilObject=slot;object->hoNextSelected=-1;++rhOiList[type]->oilNObjects;
    [rhObjectList[slot] release];rhObjectList[slot]=object;return slot;
}
-(CRunMvtPhysics*)GetPhysicMovement:(CObject*)object {return nullptr;}
-(void)addBackdrop2:(id)backdrop {[backdrops addObject:backdrop];[backdrop release];}
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
static void prepare(NSArray* params,NSMutableData** storage,LPEVP* pointers,LPEVT* event) {
    *storage=[[NSMutableData alloc] initWithLength:CND_SIZE+16*sizeof(FixtureParam)];
    *event=(LPEVT)[*storage mutableBytes];
    for(int i=0;i<16;++i) {
        pointers[i]=(LPEVP)((char*)[*storage mutableBytes]+CND_SIZE+i*sizeof(FixtureParam));
        if((NSUInteger)i<[params count]) {
            id value=[params objectAtIndex:i];
            if([value isKindOfClass:[NSNumber class]]) pointers[i]->evp.evpW.evpW0=[value shortValue];
            if([value isKindOfClass:[NSData class]]) memcpy(&pointers[i]->evp.evpW.evpW0,[value bytes],[value length]);
        }
    }
}
@implementation CCndExtension
-(void)prepare {prepare(parameters,&eventData,pParams,&pEvent);}
-(void)dealloc {[eventData release];[super dealloc];}
-(LPEVP)getParamObject:(CRun*)run withNum:(int)index {return pParams[index];}
-(int)getParamExpression:(CRun*)run withNum:(int)index { return [[parameters objectAtIndex:index] intValue]; }
-(NSString*)getParamExpString:(CRun*)run withNum:(int)index { return [parameters objectAtIndex:index]; }
@end
@implementation CActExtension
-(void)prepare {prepare(parameters,&eventData,pParams,&pEvent);}
-(void)dealloc {[eventData release];[super dealloc];}
-(CObject*)getParamObject:(CRun*)run withNum:(int)index {id value=[parameters objectAtIndex:index];return [value isKindOfClass:[CObject class]]?value:nil;}
-(int)getParamExpression:(CRun*)run withNum:(int)index { return [[parameters objectAtIndex:index] intValue]; }
-(NSString*)getParamExpString:(CRun*)run withNum:(int)index { return [parameters objectAtIndex:index]; }
@end
@implementation CObjInfo
@end
@implementation CQualToOiList
@end
@implementation CEventProgram
-(void)evt_AddCurrentObject:(CObject*)object {
    CObjInfo* info=object->hoOiList;
    object->hoNextSelected=info->oilEventCount==rh2EventCount?info->oilListSelected:-1;
    info->oilListSelected=object->hoNumber;info->oilNumOfSelected=1;info->oilEventCount=rh2EventCount;
}
@end
@implementation CRunFrame
-(BOOL)bkdCol_TestPoint:(int)x withY:(int)y andLayer:(int)layer andPlane:(int)plane {return x==3 && y==3;}
@end
@implementation CLayer
@end
@implementation CRunApp
-(NSData*)loadResourceData:(NSString*)path {[loadedPath release];loadedPath=[path copy];return [resources objectForKey:path];}
@end
@implementation CImageBank
-(CImage*)getImageFromHandle:(unsigned short)handle {return [images objectForKey:@(handle)];}
@end
@implementation CObject
-(void)dealloc {[roa release];[rov release];[ros release];[super dealloc];}
@end
@implementation CAnimDir
@end
@implementation CRAni
@end
@implementation CRSpr
-(id)init {if((self=[super init]))rsEffectShader=-1;return self;}
-(void)obHide {hidden=YES;}
@end
@implementation CRVal
-(CValue*)getValue:(int)index {return makeValue((NSUInteger)index<[values count]?[values objectAtIndex:index]:@0);}
@end
@implementation UIImage
+(UIImage*)imageWithContentsOfFile:(NSString*)path {return nil;}
+(UIImage*)imageNamed:(NSString*)path {return nil;}
+(UIImage*)imageWithData:(NSData*)data {return data==fixtureFileBytes?[[[UIImage alloc] init] autorelease]:nil;}
@end
@implementation CImage
+(CImage*)loadUIImage:(UIImage*)image {return image?[fixtureFileImage retain]:nil;}
-(int)getPixel:(int)x withY:(int)y {return [[pixels objectAtIndex:y*width+x] intValue];}
@end
@implementation COIList
-(id)getOIFromHandle:(short)handle {return nil;}
@end
@implementation CBkd2
-(int)checkOrCreateEffectIfNeeded:(CRunApp*)app andCOI:(id)oi {return -1;}
-(id)initWithCRun:(CRun*)run {return [super init];}
@end
void AddPhysicsAttractor(CRun* run,CObject* object) {++run->physicsCalls;}
BOOL read_Position(CRun* run,LPPOS pos,unsigned flags,int* x,int* y,int* dir,BOOL* repeat,int* layer) {
    if(pos->posOINUMParent!=-1) return NO;
    *x=pos->posX;*y=pos->posY;*dir=pos->posDir;*repeat=NO;*layer=pos->posLayer;return YES;
}
