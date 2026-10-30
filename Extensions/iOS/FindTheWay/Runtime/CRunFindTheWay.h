#import "CRunExtension.h"

NS_ASSUME_NONNULL_BEGIN

@class CRun;
@class CRunExtension;
@class CActExtension;
@class CCndExtension;

@class CFile;
@class CValue;
@class CObject;
@class CCreateObjectInfo;

@interface CRunFindTheWay : CRunExtension {

}

-(int)getNumberOfConditions;
-(BOOL)createRunObject:(CFile*)file withCOB:(CCreateObjectInfo*)cob andVersion:(int)version;
-(int)handleRunObject;
-(void)destroyRunObject:(BOOL)bFast;

-(BOOL)condition:(int)num withCndExtension:(CCndExtension*)cnd;
-(void)action:(int)num withActExtension:(CActExtension*)act;
-(CValue*)expression:(int)num;

@end

NS_ASSUME_NONNULL_END
