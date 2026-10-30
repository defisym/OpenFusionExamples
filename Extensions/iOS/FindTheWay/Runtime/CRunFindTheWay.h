#import "CRunExtension.h"
#include "FindTheWayCore.h"
@class CFile, CCreateObjectInfo, CCndExtension, CActExtension, CValue;
@interface CRunFindTheWay : CRunExtension {
    FTWMap map;
    BOOL isometric, alive;
    int width, height, gridWidth, gridHeight, offsetX, offsetY;
    NSMutableDictionary* paths;
    NSData* lastPath;
    NSString* iterationName;
    FTWCoord iterationCoord;
    int iterationIndex, iterationKind;
    unsigned warnedConditions, warnedActions, warnedExpressions;
}
-(int)getNumberOfConditions;
-(BOOL)createRunObject:(CFile*)file withCOB:(CCreateObjectInfo*)cob andVersion:(int)version;
-(int)handleRunObject;
-(void)destroyRunObject:(BOOL)bFast;
-(BOOL)condition:(int)num withCndExtension:(CCndExtension*)cnd;
-(void)action:(int)num withActExtension:(CActExtension*)act;
-(CValue*)expression:(int)num;
@end
