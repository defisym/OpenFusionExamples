package _3rdLib;

import Application.CRunApp;
import Events.CEvent;
import Events.CEventProgram;
import Objects.CObject;
import Params.PARAM_OBJECT;
import RunLoop.CObjInfo;
import RunLoop.CRun;

import java.util.function.Consumer;

/** Shared OI helpers. OI list indices and creation OI handles are distinct. */
public class ObjectBaseClass {
    protected final CRunApp rhPtr;
    protected final CRun run;
    protected final CEventProgram eventProgram;

    public ObjectBaseClass(CRunApp runApp) {
        rhPtr = runApp;
        run = runApp.run;
        eventProgram = runApp.frame.evtProg;
    }

    protected void IterateOiL(Consumer<CObjInfo> callback) {
        for (int i = 0; i < run.rhMaxOI; ++i) {
            CObjInfo oil = GetLPOIL(i);
            if (oil != null) {
                callback.accept(oil);
            }
        }
    }

    public static short GetOil(CEvent evt, CRun run, int num) {
        return ((PARAM_OBJECT) evt.evtParams[num]).oiList;
    }

    public CObjInfo GetLPOIL(int oiList) {
        return oiList >= 0 && oiList < run.rhMaxOI && oiList < run.rhOiList.length
                ? run.rhOiList[oiList] : null;
    }

    public CObjInfo GetLPOIL(CObject object) {
        return object == null ? null : object.hoOiList;
    }

    public short GetOiFromOiList(int oiList) {
        if ((oiList & 0x8000) != 0) {
            return -1;
        }
        CObjInfo oil = GetLPOIL(oiList);
        return oil == null ? -1 : oil.oilOi;
    }

    public static boolean CompareObjectName(String objectName, String compareName) {
        if (objectName == null || compareName == null) {
            return false;
        }
        int start = 0;
        while (start < objectName.length() && objectName.charAt(start) == '\uffff') {
            ++start;
        }
        return objectName.substring(start).equalsIgnoreCase(compareName);
    }

    public static boolean CompareObjectName(CObjInfo oil, String compareName) {
        return oil != null && CompareObjectName(oil.oilName, compareName);
    }

    public CObjInfo GetOILByName(String objectName) {
        CObjInfo result = null;
        for (int i = 0; i < run.rhMaxOI; ++i) {
            CObjInfo oil = GetLPOIL(i);
            if (CompareObjectName(oil, objectName)) {
                result = oil;
            }
        }
        return result;
    }

    public boolean OILHasObject(String objectName) {
        return GetOILByName(objectName) != null;
    }

    public boolean OILHasObject(CObjInfo oil) {
        for (int i = 0; i < run.rhMaxOI; ++i) {
            if (oil != null && GetLPOIL(i) == oil) {
                return true;
            }
        }
        return false;
    }
}
