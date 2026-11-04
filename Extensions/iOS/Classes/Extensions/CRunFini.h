#import "CRunExtension.h"

// Native pointer stays opaque to Objective-C importers and the exporter loader.
@interface CRunFini : CRunExtension {
    void* finiState;
    BOOL cf25p, allowRVforCS, keep, linked;
    int localizationFixed;
    NSString *currentSection, *currentItem, *sectionLoop, *itemLoop;
}
@end
