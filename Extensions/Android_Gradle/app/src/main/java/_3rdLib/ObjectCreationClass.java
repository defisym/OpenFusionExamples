package _3rdLib;

import Application.CRunApp;
import Events.CEvent;
import Events.CEventProgram;
import Events.CQualToOiList;
import Frame.CLayer;
import OI.COI;
import OI.CObjectCommon;
import Objects.CObject;
import Params.CParam;
import Params.PARAM_OBJECT;
import RunLoop.CObjInfo;
import RunLoop.CRun;

import static java.lang.Math.max;
import static java.lang.Math.min;

// Usage

// PARAM_CREATE param = (PARAM_CREATE) GetEvtParam(act, rh, 1);
// 
// PARAM_POSITION param = (PARAM_POSITION) ObjectCreationClass.GetEvtParam(act, rh, 1);
// CPositionInfo pInfo = new CPositionInfo();
// param.read_Position(rhPtr, 0x11, pInfo)
// 
// Oc.OCCreateObject(new ObjectCreationClass.CreationParamClass() {
//     @Override
//     public void CreationParam(ObjectCreationClass oc,
//             ObjectCreationClass.CreateDuplicateParam cdp) {
//                 cdp.HFII = param.cdpHFII;
//                 cdp.oi = param.cdpOi;
//                 cdp.x = realCoord.x;
//                 cdp.y = realCoord.y;
//                 cdp.layer = param.posLayer;
//                 // cdp.layer = 0;
//             }
//         });
// }

public class ObjectCreationClass {
    private final CRunApp rhPtr;
    private final CRun run;
    private final CEventProgram eventProgram;

    private final CObject[] ObjectList;
    private final CObjInfo[] OiList;
    private final CQualToOiList[] QualToOiList;

    int GetValidLayer(int layer) {
        return (max(-1, min(run.rhFrame.nLayers - 1, layer)));
    }

    public static abstract class CreationParamClass {
        public abstract void CreationParam(ObjectCreationClass oc, CreateDuplicateParam cdp);
    }

    public class CreateDuplicateParam {
        public short HFII;
        public short oi;
        public int x;
        public int y;
        public int dir;
        public int layer;

        public CreateDuplicateParam() {
            this.HFII = -1;
            this.oi = 0;
            this.x = 0;
            this.y = 0;
            this.dir = 0;
            this.layer = 0;
        }

        public CreateDuplicateParam(short HFII, short oi, int x, int y, int dir, int layer) {
            this.HFII = HFII;
            this.oi = oi;
            this.x = x;
            this.y = y;
            this.dir = dir;
            this.layer = layer;
        }
    }

    CreateDuplicateParam cdp;

    public ObjectCreationClass(CRunApp runApp) {
        rhPtr = runApp;
        run = rhPtr.run;
        eventProgram = rhPtr.frame.evtProg;
        ObjectList = run.rhObjectList; // get a pointer to the mmf object list
        OiList = run.rhOiList; // get a pointer to the mmf object info list
        QualToOiList = eventProgram.qualToOiList; // get a pointer to the mmf qualifier to Oi list
    }

    public void OCCreateObject(CreationParamClass updateParam) {
        cdp = new CreateDuplicateParam();
        updateParam.CreationParam(this, cdp);

        cdp.layer = GetValidLayer(cdp.layer);

        if (cdp.oi != -1) {
            int number = run.f_CreateObject(cdp.HFII, cdp.oi, cdp.x, cdp.y, cdp.dir, (short) 0, cdp.layer, -1);

            if (number > 0) {
                CObject pHo = ObjectList[number];
                run.rhEvtProg.evt_AddCurrentObject(pHo);

                if ((pHo.hoOEFlags & CObjectCommon.OEFLAG_SPRITES) != 0) {
                    // Hide object if layer hidden
                    CLayer pLayer = run.rhFrame.layers[cdp.layer];
                    if ((pLayer.dwOptions & (CLayer.FLOPT_TOHIDE | CLayer.FLOPT_VISIBLE)) != CLayer.FLOPT_VISIBLE) {
                        pHo.ros.obHide();
                    }
                }
            }

        }
    }

    public short GetCreationOI(String objName) {
        COI oiPtr;

        // Iterate OI list
        for (oiPtr = run.rhApp.OIList.getFirstOI(); oiPtr != null; oiPtr = run.rhApp.OIList.getNextOI()) {
            if (oiPtr.oiType >= 2) {
                if (oiPtr.oiName.equalsIgnoreCase(objName)) {
                    return oiPtr.oiHandle;
                }
            }
        }

        // No object was found
        return -1;
    }

    public static CParam GetEvtParam(CEvent evt, CRun rhPtr, int _num) {
        return evt.evtParams[_num];
    }
}
