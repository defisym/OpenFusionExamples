#import "CRunFindTheWay.h"

#import "CRun.h"
#import "CExtension.h"
#import "CActExtension.h"
#import "CCndExtension.h"

#import "CFile.h"
#import "CValue.h"
#import "CObject.h"
#import "CCreateObjectInfo.h"

// Define A/C/E here
// Condition
#define CND                 0

#define CND_LAST            1

// Action
#define ACT                 0

// Expression
#define EXP                 0

@implementation CRunFindTheWay

// Fusion Funcs
-(int) getNumberOfConditions {
	return CND_LAST;
}

-(BOOL)createRunObject:(CFile*)file withCOB:(CCreateObjectInfo*)cob andVersion:(int)version {
	// Read Properties
    // [file readAInt];

    // Return
	return YES;
}

-(void)destroyRunObject:(BOOL)bFast {

}

-(int)handleRunObject {
	return 0;
}

// Jump Table
// Condition Jump Table
-(BOOL)condition:(int)num withCndExtension:(CCndExtension*)cnd {
	switch (num) {
		case CND: {
            // [cnd getParamOi:rh withNum:0];

            return YES;
        }
	}
	return NO;
}

// Action Jump Table
-(void)action:(int)num withActExtension:(CActExtension*)act {
	switch (num) {
		case ACT: {
			// [act getParamExpression:rh withNum:5]

			break;
		}
	}
}

// Expression Jump Table
-(CValue*)expression:(int)num {
	switch(num) {
		case EXP:{
            return [rh getTempValue:(int)1];
        }
	}

	return [rh getTempDouble:0];
}

@end
