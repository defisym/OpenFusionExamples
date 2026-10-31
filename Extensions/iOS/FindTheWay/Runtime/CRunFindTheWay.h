#import "CRunExtension.h"
#include "FindTheWayCore.h"
@class CFile, CCreateObjectInfo, CCndExtension, CActExtension, CValue;
@interface CRunFindTheWay : CRunExtension {
    FTWMap map;
    BOOL isometric, alive;
    int width, height, gridWidth, gridHeight, offsetX, offsetY;
    NSData* lastPath;
    NSString* iterationName;
    FTWCoord iterationCoord;
    int iterationIndex, iterationKind;
    unsigned warnedConditions, warnedActions, warnedExpressions;
    NSMutableData *allies, *enemies, *zones, *units;
    NSArray* area;
    uint64_t areaPosition, attackStart, areaGeneration;
    BOOL isAttack;
    id sourceObject;

}
-(int)getNumberOfConditions;
-(BOOL)createRunObject:(CFile*)file withCOB:(CCreateObjectInfo*)cob andVersion:(int)version;
-(int)handleRunObject;
-(void)destroyRunObject:(BOOL)bFast;
-(BOOL)condition:(int)num withCndExtension:(CCndExtension*)cnd;
-(void)action:(int)num withActExtension:(CActExtension*)act;
-(CValue*)expression:(int)num;
-(void)mapUpdated:(FTWMap)updated;
-(BOOL)configureGrid;
-(void)clearMap;
-(void)syncLists;
-(void)iterate:(NSData*)data real:(BOOL)real name:(NSString*)name kind:(int)kind;
@end
@interface CRunFindTheWay (Objects)
-(BOOL)objectCondition:(int)num withCndExtension:(CCndExtension*)cnd;
-(void)objectAction:(int)num withActExtension:(CActExtension*)act;
-(NSData*)objectZone:(id)object;
-(void)createPoints:(NSData*)points withActExtension:(CActExtension*)act parameter:(int)param byName:(BOOL)byName;
@end
@interface CRunFindTheWay (Platform)
-(BOOL)platformCondition:(int)num withCndExtension:(CCndExtension*)cnd;
-(void)createGridWithAct:(CActExtension*)act;
@end
