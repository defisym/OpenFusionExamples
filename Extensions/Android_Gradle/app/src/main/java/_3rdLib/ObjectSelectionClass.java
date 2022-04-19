package _3rdLib;

// Origin version by Anders Riggelsen (Andos)

// Add For-Each support

import Application.CRunApp;
import Conditions.CCndExtension;
import Events.CEvent;
import Events.CEventProgram;
import Events.CQualToOiList;
import Objects.CObject;
import Params.PARAM_OBJECT;
import RunLoop.CObjInfo;
import RunLoop.CRun;

import java.util.Iterator;
import java.util.List;
import java.util.function.BiConsumer;
import java.util.function.Consumer;

import static Objects.CObject.HOF_DESTROYED;

// Usage
// boolean negated = ObjectSelectionClass.IsNegated(cnd);
// short oil = ObjectSelectionClass.GetOil(act, rh, 0);
// short oil = ObjectSelectionClass.GetOil(cnd, rh, 0);

public class ObjectSelectionClass {
    private final CRunApp rhPtr;
    private final CRun run;
    private final CEventProgram eventProgram;

    private final CObject[] ObjectList;
    private final CObjInfo[] OiList;
    private CQualToOiList[] QualToOiList;

    public static abstract class FilterClass {
        public abstract boolean Filter(Object rdPtr, CObject object);
    }

    public static abstract class ForEachCallBackClass {
        public abstract void ForEachCallBack(CObject object);
    }

    // Filter qualifier objects
    private boolean FilterQualifierObjects(Object rdPtr, int OiList, boolean negate, FilterClass filter) {
        QualToOiList = eventProgram.qualToOiList;
        CQualToOiList CurrentQualToOi = QualToOiList[OiList];

        boolean hasSelected = false;
        int i = 0;
        int length = CurrentQualToOi.qoiList.length;
        while (i < length) {
            int CurrentOi = CurrentQualToOi.qoiList[i + 1];

            hasSelected = (((hasSelected ? 1 : 0) |
                    (FilterNonQualifierObjects(rdPtr, CurrentOi, negate, filter) ? 1 : 0))) != 0;

            i += 2;
        }
        return hasSelected;
    }

    // Filter normal objects
    private boolean FilterNonQualifierObjects(Object rdPtr, int OiList, boolean negate, FilterClass filter) {
        CObjInfo pObjectInfo = this.OiList[OiList]; // GetOILFromOI(Oi);

        if (pObjectInfo == null) {
            return false;
        }

        if (pObjectInfo.oilEventCount != eventProgram.rh2EventCount) {
            SelectAll(OiList); // The SOL is invalid, must reset.
        }

        // If SOL is empty
        if (pObjectInfo.oilNumOfSelected <= 0) {
            return false;
        }

        boolean hasSelected = false;

        int firstSelected = -1;
        int count = 0;
        int current = pObjectInfo.oilListSelected;
        CObject previous = null;

        while ((current & 0x80000000) == 0) {
            CObject pObject = ObjectList[current];

            if (pObject == null) {
                continue;
            }

            boolean filterResult = filter.Filter(rdPtr, pObject);
            boolean useObject = ((filterResult ? 1 : 0) ^ (negate ? 1 : 0)) != 0;
            hasSelected = ((hasSelected ? 1 : 0) | (useObject ? 1 : 0)) != 0;

            if (useObject) {
                if (firstSelected == -1) {
                    firstSelected = current;
                }

                if (previous != null) {
                    previous.hoNextSelected = (short) current;
                }

                previous = pObject;
                count++;
            }
            current = pObject.hoNextSelected;
        }
        if (previous != null) {
            previous.hoNextSelected = -1;
        }

        pObjectInfo.oilListSelected = (short) firstSelected;
        pObjectInfo.oilNumOfSelected = count;

        return hasSelected;
    }

    public ObjectSelectionClass(CRunApp runApp) {
        rhPtr = runApp;
        run = rhPtr.run;
        eventProgram = rhPtr.frame.evtProg;
        ObjectList = run.rhObjectList; // get a pointer to the mmf object list
        OiList = run.rhOiList; // get a pointer to the mmf object info list
        QualToOiList = eventProgram.qualToOiList; // get a pointer to the mmf qualifier to Oi list
    }

    // Get oil
    public static short GetOil(CEvent evt, CRun rhPtr, int _num) {
        PARAM_OBJECT param = (PARAM_OBJECT) evt.evtParams[_num];
        return param.oiList;
    }

    // Get LPOIL
    // Returns the object-info structure from a given object-type
    public CObjInfo GetLPOIL(int oiList) {
        for (int i = 0; i < run.rhMaxOI; ++i) {
            CObjInfo oil = OiList[i];
            if (oil.oilOi == oiList)
                return oil;
        }
        return null;
    }

    // Get Oi for creation
    public short GetOiFromOiList(int oiList) {
        CObjInfo pObjectInfo = GetLPOIL(oiList);

        if (pObjectInfo == null) {
            return -1;
        }

        // you cannot create qualifier
        if ((oiList & 0x8000) != 0) {
            return -1;
        }

        return pObjectInfo.oilOi;
    }

    // Get Neg, must be called at first
    public static boolean IsNegated(CCndExtension cnd) {
        return (cnd.evtFlags2 & CEvent.EVFLAG2_NOT) != 0;
    }

    // Selects *all* objects of the given object-type
    public void SelectAll(int oiList) {
        CObjInfo pObjectInfo = this.OiList[oiList]; // GetOILFromOI(Oi);
        if (pObjectInfo == null)
            return;
        pObjectInfo.oilNumOfSelected = pObjectInfo.oilNObjects;
        pObjectInfo.oilListSelected = pObjectInfo.oilObject;
        pObjectInfo.oilEventCount = eventProgram.rh2EventCount;

        int i = pObjectInfo.oilObject;
        while ((i & 0x80000000) == 0) {
            CObject pObject = run.rhObjectList[i];
            if (pObject == null)
                continue;
            pObject.hoNextSelected = pObject.hoNumNext;
            i = pObject.hoNumNext;
        }
        return;
    }

    // Resets all objects of the given object-type
    public void SelectNone(int oiList) {
        CObjInfo pObjectInfo = this.OiList[oiList]; // GetOILFromOI(Oi);
        if (pObjectInfo == null)
            return;
        pObjectInfo.oilNumOfSelected = 0;
        pObjectInfo.oilListSelected = -1;
        pObjectInfo.oilEventCount = eventProgram.rh2EventCount;
    }

    // Resets the SOL and inserts only one given object
    public void SelectOneObject(CObject object) {
        CObjInfo pObjectInfo = object.hoOiList;
        if (pObjectInfo == null)
            return;
        pObjectInfo.oilNumOfSelected = 1;
        pObjectInfo.oilEventCount = eventProgram.rh2EventCount;
        pObjectInfo.oilListSelected = object.hoNumber;
        CObject cObj = ObjectList[object.hoNumber];
        if (cObj != null)
            cObj.hoNextSelected = -1;
    }

    // Resets the SOL and inserts the given list of objects
    public void SelectObjects(int oiList, List<CObject> objects) {
        CObjInfo pObjectInfo = this.OiList[oiList]; // GetOILFromOI(Oi);

        if (pObjectInfo == null)
            return;

        pObjectInfo.oilNumOfSelected = objects.size();
        pObjectInfo.oilEventCount = eventProgram.rh2EventCount;

        Iterator<CObject> i = objects.iterator();

        if (!i.hasNext())
            return;

        int prevNumber = i.next().hoNumber;
        pObjectInfo.oilListSelected = (short) prevNumber;

        while (i.hasNext()) {
            int currentNumber = i.next().hoNumber;
            if (ObjectList[prevNumber] != null) {
                ObjectList[prevNumber].hoNextSelected = (short) currentNumber;
                prevNumber = currentNumber;
            }
        }
        if (ObjectList[prevNumber] != null)
            ObjectList[prevNumber].hoNextSelected = -1;
    }

    // Run a custom filter on the SOL (via function callback)
    public boolean FilterObjects(Object rdPtr, int oiList, boolean negate, FilterClass filter) {
        if ((oiList & 0x8000) != 0) {
            return ((FilterQualifierObjects(rdPtr, oiList & 0x7FFF, negate, filter) ? 1 : 0) ^ (negate ? 1 : 0)) != 0;
        }
        return ((FilterNonQualifierObjects(rdPtr, oiList & 0x7FFF, negate, filter) ? 1 : 0) ^ (negate ? 1 : 0)) != 0;
    }

    // Return the number of selected objects for the given object-type
    public int GetNumberOfSelected(int oiList) {
        if ((oiList & 0x8000) != 0) {
            oiList &= 0x7FFF; // Mask out the qualifier part
            int numberSelected = 0;

            QualToOiList = eventProgram.qualToOiList;
            CQualToOiList CurrentQualToOi = QualToOiList[oiList];

            int i = 0;
            int length = CurrentQualToOi.qoiList.length;
            while (i < length) {
                CObjInfo CurrentOi = GetLPOIL(CurrentQualToOi.qoiList[i + 1]);
                if (CurrentOi == null)
                    return 0;
                numberSelected += CurrentOi.oilNumOfSelected;
                i += 2;
            }
            return numberSelected;
        } else {
            CObjInfo pObjectInfo = this.OiList[oiList]; // GetOILFromOI(Oi);
            if (pObjectInfo == null)
                return 0;
            return pObjectInfo.oilNumOfSelected;
        }
    }

    // Return the number of objects for the given object-type
    public int GetNumberOfObject(int oiList) {
        if ((oiList & 0x8000) != 0) {
            oiList &= 0x7FFF; // Mask out the qualifier part
            int number = 0;

            QualToOiList = eventProgram.qualToOiList;
            CQualToOiList CurrentQualToOi = QualToOiList[oiList];

            int i = 0;
            int length = CurrentQualToOi.qoiList.length;
            while (i < length) {
                CObjInfo CurrentOi = GetLPOIL(CurrentQualToOi.qoiList[i + 1]);
                if (CurrentOi == null)
                    return 0;
                number += CurrentOi.oilNObjects;
                i += 2;
            }
            return number;
        } else {
            CObjInfo pObjectInfo = this.OiList[oiList]; // GetOILFromOI(Oi);
            if (pObjectInfo == null)
                return 0;
            return pObjectInfo.oilNObjects;
        }
    }

    // Check if object is given type
    public boolean ObjectIsOfType(CObject obj, int oiList) {
        if ((oiList & 0x8000) != 0) {
            oiList &= 0x7FFF; // Mask out the qualifier part

            QualToOiList = eventProgram.qualToOiList;
            CQualToOiList CurrentQualToOi = QualToOiList[oiList];

            int i = 0;
            while (i < CurrentQualToOi.qoiList.length) {
                CObjInfo CurrentOi = run.rhOiList[CurrentQualToOi.qoiList[i + 1]]; // GetOILFromOI(Oi);
                if (CurrentOi == null)
                    return false;
                if (CurrentOi.oilOi == obj.hoOi)
                    return true;
                i += 2;
            }
            return false;
        }
        return (obj.hoOi == run.rhOiList[oiList].oilOi);
    }

    public void ForEach(int oiList, ForEachCallBackClass f, boolean forceAll) {
        BiConsumer<CObjInfo, Boolean> iterate = (CObjInfo list, Boolean selected) -> {
            short num = selected ? list.oilListSelected : list.oilObject;

            while (num > 0) {
                CObject obj = run.rhObjectList[num];

                if (obj == null) {
                    break;
                }

                if ((obj.hoFlags & HOF_DESTROYED) == 0) {
                    f.ForEachCallBack(obj);
                }

                num = selected ? obj.hoNextSelected : obj.hoNumNext;
            }
        };

        Consumer<CObjInfo> iterateCall = (CObjInfo list) -> {
            if (forceAll) {
                iterate.accept(list, false);
                return;
            }

            if (list.oilEventCount == eventProgram.rh2EventCount) {
                // Object type already filtered, loop through selected list
                iterate.accept(list, true);
            } else {
                // Object type not filtered yet, loop through all objects instead
                iterate.accept(list, false);
            }
        };

        // Single object type
        if (oiList >= 0) {
            CObjInfo pObjectInfo = this.OiList[oiList];

            if (pObjectInfo == null) {
                return;
            }

            iterateCall.accept(pObjectInfo);
        } else if (oiList != -1) {
            oiList &= 0x7FFF;

            QualToOiList = eventProgram.qualToOiList;
            CQualToOiList CurrentQualToOi = QualToOiList[oiList];

            if (CurrentQualToOi == null) {
                return;
            }

            int i = 0;
            int length = CurrentQualToOi.qoiList.length;
            while (i < length) {
                CObjInfo CurrentOi = this.OiList[CurrentQualToOi.qoiList[i + 1]];

                if (CurrentOi == null) {
                    return;
                }

                iterateCall.accept(CurrentOi);

                i += 2;
            }

        }
        // Qualifier object type
    }

    public void ForEach(int oiList, ForEachCallBackClass f) {
        ForEach(oiList, f, false);
    }
}
