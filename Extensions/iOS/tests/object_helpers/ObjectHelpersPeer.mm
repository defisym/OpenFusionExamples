// A second extension-like translation unit using the same generic headers.
#include "ObjectCreation.h"
#include "ObjectSelection.h"

CObject* CreateFromPeer(CRun* run,short oi) {
    FusionIOS::ObjectCreation creation(run);
    FusionIOS::ObjectCreation::CreateParams params;params.oi=oi;params.x=17;params.y=23;
    return creation.Create(params);
}
int CountFromPeer(CRun* run,int oiList) {
    return FusionIOS::ObjectSelection(run).GetNumberOfObject(oiList);
}
