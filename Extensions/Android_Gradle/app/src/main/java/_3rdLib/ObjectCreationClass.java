package _3rdLib;

import Application.CRunApp;
import Events.CEvent;
import Frame.CLayer;
import OI.CObjectCommon;
import Objects.CObject;
import Params.CParam;
import Params.CPositionInfo;
import Params.PARAM_CREATE;
import RunLoop.CObjInfo;
import RunLoop.CRun;

import static _DeLib.FusionUtilities.GetPositionInfo;
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
//                 cdp.AssignParam(rh, param);
//
//                 cdp.x = realCoord.x;
//                 cdp.y = realCoord.y;
//             }
//         });
// }

public class ObjectCreationClass extends ObjectBaseClass {

    int GetValidLayer(int layer) {
        return (max(-1, min(run.rhFrame.nLayers - 1, layer)));
    }

    public static abstract class CreationParamClass {
        public abstract void CreationParam(ObjectCreationClass oc, CreateDuplicateParam cdp);
    }

    public static class CreateDuplicateParam {
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

        public CreateDuplicateParam(CRun rhPtr,PARAM_CREATE param) {
            AssignParam(rhPtr, param);
        }

        public void AssignParam(CRun rhPtr, PARAM_CREATE param){
            CPositionInfo pInfo = GetPositionInfo(rhPtr,param);

            this.HFII = param.cdpHFII;
            this.oi = param.cdpOi;

            this.x = pInfo.x;
            this.y = pInfo.y;
            this.dir = pInfo.dir;
            this.layer = pInfo.layer;
        }
    }

    public ObjectCreationClass(CRunApp runApp) {
        super(runApp);
    }

    public void OCCreateObject(CreationParamClass updateParam) {
        CreateDuplicateParam cdp = new CreateDuplicateParam();
        updateParam.CreationParam(this, cdp);

        cdp.layer = GetValidLayer(cdp.layer);

        if (cdp.oi != -1) {
            int number = run.f_CreateObject(cdp.HFII, cdp.oi, cdp.x, cdp.y, cdp.dir, (short) 0, cdp.layer, -1);

            if (number >= 0) {
                CObject pHo = run.rhObjectList[number];
                run.rhEvtProg.evt_AddCurrentObject(pHo);

                if (cdp.layer != -1 && (pHo.hoOEFlags & CObjectCommon.OEFLAG_SPRITES) != 0) {
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
        CObjInfo oil = GetOILByName(objName);
        return oil == null ? -1 : oil.oilOi;
    }

    public static CParam GetEvtParam(CEvent evt, CRun rhPtr, int _num) {
        return evt.evtParams[_num];
    }
}
