#pragma once
#import <Foundation/Foundation.h>
#include <functional>
#include <vector>
@class CRun;
@class CObject;

namespace FTWiOS {
// Runtime-owned objects and run header are borrowed. Call only on the Fusion thread.
struct ObjectRef { int slot; short generation; };
enum IterateFlags { IterateDefault=0, IterateAll=1, IterateSelectedOnly=2 };
class Selection {
    CRun* run;
    std::vector<short> types(short oiList) const;
    CObject* resolve(ObjectRef object) const;
    void assign(short oiList, const std::vector<ObjectRef>& objects) const;
public:
    explicit Selection(CRun* runtime) : run(runtime) {}
    std::vector<ObjectRef> snapshot(short oiList, int flags=IterateDefault,
                                    bool includeDestroyed=false) const;
    void selectAll(short oiList) const;
    void selectNone(short oiList) const;
    // Explicit replacement: an empty input clears the SOL. Use selectNone when
    // porting callers whose desktop SelectObjects overload treats empty as no-op.
    void replace(short oiList, const std::vector<ObjectRef>& objects) const;
    void forEach(short oiList, const std::function<void(CObject*)>& callback,
                 int flags=IterateDefault) const;
    // Predicate must not generate events or mutate object lists. Return follows
    // Windows/Fusion negation convention: any retained object XOR negate.
    bool filter(short oiList, bool negate, const std::function<bool(CObject*)>& predicate,
                bool includeDestroyed=true) const;
};
struct CreateParams {
    short frameItem=-1, oi=-1;
    int x=0, y=0, direction=0, layer=0;
};
class Creation {
    CRun* run;
public:
    explicit Creation(CRun* runtime) : run(runtime) {}
    short findOI(NSString* name) const;
    // Resolved position parameters only; callers decode Fusion position params.
    // Returns Runtime-owned object, or nil on creation failure.
    CObject* create(const CreateParams& params) const;
};
}
