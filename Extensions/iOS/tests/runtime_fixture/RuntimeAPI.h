#pragma once
#import <Foundation/Foundation.h>
@class CRun, CExtension;
@interface CValue : NSObject { @public id value; }
-(int)getInt;
-(NSString*)getString;
@end
@interface CRun : NSObject
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
@interface CCndExtension : NSObject { @public NSArray* parameters; }
-(int)getParamExpression:(CRun*)run withNum:(int)index;
-(NSString*)getParamExpString:(CRun*)run withNum:(int)index;
@end
@interface CActExtension : NSObject { @public NSArray* parameters; }
-(int)getParamExpression:(CRun*)run withNum:(int)index;
-(NSString*)getParamExpString:(CRun*)run withNum:(int)index;
@end
