package _3rdLib;

// Origin version by Anders Riggelsen (Andos)
// Shared helpers follow Windows ObjectSelection; runtime structures remain Android-specific.

import Application.CRunApp;
import Conditions.CCndExtension;
import Events.CEvent;
import Events.CEventGroup;
import Events.CEventProgram;
import Events.CQualToOiList;
import Objects.CObject;
import RunLoop.CObjInfo;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.function.BiConsumer;
import java.util.function.Consumer;

import static Objects.CObject.HOF_DESTROYED;

public class ObjectSelectionClass extends ObjectBaseClass {
    public static final int ForEachFlag_Default = 0;
    public static final int ForEachFlag_ForceAll = 1;
    public static final int ForEachFlag_SelectedOnly = 2;

    public static abstract class FilterClass {
        public abstract boolean Filter(Object rdPtr, CObject object);
    }

    public static abstract class ForEachCallBackClass {
        public abstract void ForEachCallBack(CObject object);
    }

    public static class Scope {
        private final boolean rh2ActionLoop;
        private final boolean rh2ActionOn;
        private final int rh2ActionCount;
        private final int rh2ActionLoopCount;
        private final CEventGroup rhEventGroup;
        private final Map<CObjInfo, List<CObject>> objects = new LinkedHashMap<>();

        public Scope(CRunApp runApp) {
            CEventProgram program = runApp.frame.evtProg;
            rh2ActionLoop = program.rh2ActionLoop;
            rh2ActionOn = program.rh2ActionOn;
            rh2ActionCount = program.rh2ActionCount;
            rh2ActionLoopCount = program.rh2ActionLoopCount;
            rhEventGroup = program.rhEventGroup;
        }

        public void RestoreActionState(CRunApp runApp) {
            CEventProgram program = runApp.frame.evtProg;
            program.rh2ActionLoop = rh2ActionLoop;
            program.rh2ActionOn = rh2ActionOn;
            program.rh2ActionCount = rh2ActionCount;
            program.rh2ActionLoopCount = rh2ActionLoopCount;
            program.rhEventGroup = rhEventGroup;
        }

        public void AddSelObj(CObjInfo oil, List<CObject> selected) {
            if (!objects.containsKey(oil)) {
                objects.put(oil, new ArrayList<>(selected));
            }
        }

        public Map<CObjInfo, List<CObject>> GetSelObj() {
            return objects;
        }
    }

    public ObjectSelectionClass(CRunApp runApp) {
        super(runApp);
    }

    private CObject GetObject(int number) {
        return number >= 0 && number < run.rhObjectList.length ? run.rhObjectList[number] : null;
    }

    private void IterateQualifier(int oiList, BiConsumer<CObjInfo, Integer> callback) {
        if (oiList == -1 || !ObjectIsQualifier(oiList) || eventProgram.qualToOiList == null) {
            return;
        }
        int index = oiList & 0x7FFF;
        if (index >= eventProgram.qualToOiList.length) {
            return;
        }
        CQualToOiList qualifier = eventProgram.qualToOiList[index];
        if (qualifier == null || qualifier.qoiList == null) {
            return;
        }
        // Android stores pairs of OI handle, OI list index, rather than byte offsets.
        for (int i = 0; i + 1 < qualifier.qoiList.length; i += 2) {
            int member = qualifier.qoiList[i + 1];
            if (member < 0) {
                break;
            }
            CObjInfo oil = GetLPOIL(member);
            if (oil != null) {
                callback.accept(oil, member);
            }
        }
    }

    private void IterateObject(CObjInfo oil, Consumer<CObject> callback,
                               boolean selected, boolean selectDestroyed) {
        if (oil == null) {
            return;
        }
        int number = selected ? oil.oilListSelected : oil.oilObject;
        while (number >= 0) {
            CObject object = GetObject(number);
            if (object == null) {
                break; // A missing link cannot be followed.
            }
            if (selectDestroyed || (object.hoFlags & HOF_DESTROYED) == 0) {
                callback.accept(object);
            }
            number = selected ? object.hoNextSelected : object.hoNumNext;
        }
    }

    public static boolean IsNegated(CCndExtension cnd) {
        return (cnd.evtFlags2 & CEvent.EVFLAG2_NOT) != 0;
    }

    public CObject GetFirstObject(CObjInfo oil, boolean selected) {
        return oil == null || oil.oilObject < 0 ? null
                : GetObject(selected ? oil.oilListSelected : oil.oilObject);
    }

    public CObject GetFirstObject(CObjInfo oil) {
        return GetFirstObject(oil, false);
    }

    public CObject GetFirstObject(int oiList, boolean selected) {
        return GetFirstObject(GetLPOIL(oiList), selected);
    }

    public CObject GetFirstObject(int oiList) {
        return GetFirstObject(oiList, false);
    }

    // Selected means there is a current, nonempty SOL; ObjectIsSelected also accepts empty SOLs.
    public boolean Selected(CObjInfo oil) {
        return ObjectIsSelected(oil) && oil.oilListSelected >= 0;
    }

    public boolean Selected(int oiList) {
        return Selected(GetLPOIL(oiList));
    }

    public static boolean ObjectIsQualifier(int oiList) {
        return (oiList & 0x8000) != 0;
    }

    public boolean ObjectIsSelected(CObjInfo oil) {
        return oil != null && oil.oilEventCount == eventProgram.rh2EventCount;
    }

    public boolean ObjectIsSelected(int oiList) {
        if (!ObjectIsQualifier(oiList)) {
            return ObjectIsSelected(GetLPOIL(oiList));
        }
        boolean[] selected = {false};
        IterateQualifier(oiList, (oil, index) -> selected[0] |= ObjectIsSelected(oil));
        return selected[0];
    }

    public void SelectAll(int oiList) {
        CObjInfo oil = GetLPOIL(oiList);
        if (oil == null) {
            return;
        }
        oil.oilNumOfSelected = oil.oilNObjects;
        oil.oilListSelected = oil.oilObject;
        oil.oilEventCount = eventProgram.rh2EventCount;
        IterateObject(oil, object -> object.hoNextSelected = object.hoNumNext, false, true);
    }

    public void SelectNone(int oiList) {
        SelectObjects(GetLPOIL(oiList), new ArrayList<>());
    }

    public void SelectOneObject(CObject object) {
        if (object != null) {
            List<CObject> objects = new ArrayList<>();
            objects.add(object);
            SelectObjects(GetLPOIL(object), objects);
        }
    }

    public void SelectObjects(int oiList, List<CObject> objects) {
        SelectObjects(GetLPOIL(oiList), objects);
    }

    public void SelectObjects(CObjInfo oil, List<CObject> objects) {
        if (oil == null) {
            return;
        }
        oil.oilEventCount = eventProgram.rh2EventCount;
        oil.oilNumOfSelected = 0;
        oil.oilListSelected = -1;
        CObject previous = null;
        for (CObject object : objects) {
            // A callback can delete objects or reuse their slots before a scope is restored.
            if (object == null || GetObject(object.hoNumber) != object || object.hoOiList != oil) {
                continue;
            }
            if (previous == null) {
                oil.oilListSelected = object.hoNumber;
            } else {
                previous.hoNextSelected = object.hoNumber;
            }
            previous = object;
            ++oil.oilNumOfSelected;
        }
        if (previous != null) {
            previous.hoNextSelected = -1;
        }
    }

    private boolean FilterNonQualifierObjects(Object rdPtr, int oiList, boolean negate,
                                               FilterClass filter, boolean selectDestroyed) {
        CObjInfo oil = GetLPOIL(oiList);
        if (oil == null) {
            return false;
        }
        if (!ObjectIsSelected(oil)) {
            SelectAll(oiList);
        }
        if (oil.oilNumOfSelected <= 0) {
            return false;
        }
        List<CObject> selected = new ArrayList<>();
        IterateObject(oil, object -> {
            if (filter.Filter(rdPtr, object) ^ negate) {
                selected.add(object);
            }
        }, true, selectDestroyed);
        SelectObjects(oil, selected);
        return oil.oilNumOfSelected != 0;
    }

    public boolean FilterObjects(Object rdPtr, int oiList, boolean negate, FilterClass filter) {
        return FilterObjects(rdPtr, oiList, negate, filter, true);
    }

    public boolean FilterObjects(Object rdPtr, int oiList, boolean negate,
                                 FilterClass filter, boolean selectDestroyed) {
        if (oiList == -1) {
            return false;
        }
        if (ObjectIsQualifier(oiList)) {
            boolean[] selected = {false};
            IterateQualifier(oiList, (oil, index) -> selected[0] |=
                    FilterNonQualifierObjects(rdPtr, index, negate, filter, selectDestroyed));
            return selected[0] ^ negate;
        }
        if (GetLPOIL(oiList) == null) {
            return false;
        }
        return FilterNonQualifierObjects(rdPtr, oiList, negate, filter, selectDestroyed) ^ negate;
    }

    public int GetNumberOfSelected(int oiList) {
        if (ObjectIsQualifier(oiList)) {
            int[] count = {0};
            IterateQualifier(oiList, (oil, index) -> count[0] += oil.oilNumOfSelected);
            return count[0];
        }
        CObjInfo oil = GetLPOIL(oiList);
        return oil == null ? 0 : oil.oilNumOfSelected;
    }

    public int GetNumberOfObject(int oiList) {
        if (ObjectIsQualifier(oiList)) {
            int[] count = {0};
            IterateQualifier(oiList, (oil, index) -> count[0] += oil.oilNObjects);
            return count[0];
        }
        CObjInfo oil = GetLPOIL(oiList);
        return oil == null ? 0 : oil.oilNObjects;
    }

    public boolean ObjectIsOfType(CObject object, int oiList) {
        if (object == null) {
            return false;
        }
        if (ObjectIsQualifier(oiList)) {
            boolean[] matches = {false};
            IterateQualifier(oiList, (oil, index) -> matches[0] |= oil.oilOi == object.hoOi);
            return matches[0];
        }
        CObjInfo oil = GetLPOIL(oiList);
        return oil != null && oil.oilOi == object.hoOi;
    }

    public void ForEach(int oiList, ForEachCallBackClass callback, int flag) {
        boolean forceAll = (flag & ForEachFlag_ForceAll) != 0;
        boolean selectedOnly = (flag & ForEachFlag_SelectedOnly) != 0;
        Consumer<CObjInfo> iterate = oil -> {
            if (oil == null) {
                return;
            }
            if (forceAll) {
                IterateObject(oil, callback::ForEachCallBack, false, false);
            } else if (ObjectIsSelected(oil)) {
                IterateObject(oil, callback::ForEachCallBack, true, false);
            } else if (!selectedOnly) {
                IterateObject(oil, callback::ForEachCallBack, false, false);
            }
        };
        if (ObjectIsQualifier(oiList)) {
            IterateQualifier(oiList, (oil, index) -> iterate.accept(oil));
        } else {
            iterate.accept(GetLPOIL(oiList));
        }
    }

    // Preserve the existing Android boolean overload.
    public void ForEach(int oiList, ForEachCallBackClass callback, boolean forceAll) {
        ForEach(oiList, callback, forceAll ? ForEachFlag_ForceAll : ForEachFlag_Default);
    }

    public void ForEach(int oiList, ForEachCallBackClass callback) {
        ForEach(oiList, callback, ForEachFlag_Default);
    }

    public void IterateObjectWithIdentifier(Object rdPtr, int identifier,
                                            ForEachCallBackClass callback, boolean selected) {
        IterateOiL(oil -> {
            CObject first = GetFirstObject(oil, selected);
            if (first != null && first.hoIdentifier == identifier) {
                IterateObject(oil, callback::ForEachCallBack, selected, false);
            }
        });
    }

    public void IterateObjectWithIdentifier(Object rdPtr, int identifier, ForEachCallBackClass callback) {
        IterateObjectWithIdentifier(rdPtr, identifier, callback, false);
    }

    public List<CObject> SaveSelectedObject(CObjInfo oil) {
        List<CObject> selected = new ArrayList<>();
        IterateObject(oil, selected::add, true, false);
        return selected;
    }

    public void SaveScope(Scope scope) {
        IterateOiL(oil -> {
            if (ObjectIsSelected(oil)) {
                scope.AddSelObj(oil, SaveSelectedObject(oil));
            }
        });
    }

    public Scope SaveScope() {
        Scope scope = new Scope(rhPtr);
        SaveScope(scope);
        return scope;
    }

    public void RestoreScope(Scope scope) {
        for (Map.Entry<CObjInfo, List<CObject>> entry : scope.GetSelObj().entrySet()) {
            if (OILHasObject(entry.getKey())) {
                SelectObjects(entry.getKey(), entry.getValue());
            }
        }
    }

    public void KeepScopeCall(boolean keepScope, Runnable callback) {
        if (!keepScope) {
            callback.run();
            return;
        }
        Scope scope = SaveScope();
        try {
            callback.run();
        } finally {
            scope.RestoreActionState(rhPtr);
            RestoreScope(scope);
        }
    }

    public static void KeepScopeCall(ObjectSelectionClass selection, boolean keepScope, Runnable callback) {
        if (selection == null || !keepScope) {
            callback.run();
        } else {
            selection.KeepScopeCall(true, callback);
        }
    }
}
