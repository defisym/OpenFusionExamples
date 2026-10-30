import Application.CRunApp;
import Events.CEventGroup;
import Events.CEventProgram;
import Events.CQualToOiList;
import Frame.CLayer;
import Frame.CRunFrame;
import Objects.CObject;
import RunLoop.CObjInfo;
import RunLoop.CRun;
import _3rdLib.ObjectCreationClass;
import _3rdLib.ObjectSelectionClass;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class ObjectHelpersTest {
    private final CRunApp app = new CRunApp();
    private final CRun run = new CRun();
    private final CEventProgram program = new CEventProgram();
    private final ObjectSelectionClass selection;
    private final ObjectCreationClass creation;
    private final CObject zero, one, two;
    private static int checks;

    private ObjectHelpersTest() {
        app.run = run;
        app.frame = new CRunFrame();
        app.frame.evtProg = program;
        run.rhFrame = app.frame;
        run.rhEvtProg = program;
        for (int i = 0; i < 2; ++i) {
            CObjInfo oil = new CObjInfo();
            oil.oilOi = (short) (40 + i * 10); // Handles deliberately differ from list indices.
            oil.oilName = i == 0 ? "\uffff\uffffPlayer" : "Enemy";
            run.rhOiList[i] = oil;
        }
        zero = addObject(0, 0);
        one = addObject(1, 0);
        two = addObject(2, 1);
        zero.hoNumNext = 1;
        run.rhOiList[0].oilObject = 0;
        run.rhOiList[1].oilObject = 2;
        CQualToOiList qualifier = new CQualToOiList();
        qualifier.qoiList = new short[]{40, 0, 50, 1, -1, -1};
        program.qualToOiList[0] = qualifier;
        selection = new ObjectSelectionClass(app);
        creation = new ObjectCreationClass(app);
    }

    private CObject addObject(int number, int oiList) {
        CObject object = new CObject();
        object.hoNumber = (short) number;
        object.hoOiList = run.rhOiList[oiList];
        object.hoOi = object.hoOiList.oilOi;
        object.hoIdentifier = 123;
        ++object.hoOiList.oilNObjects;
        run.rhObjectList[number] = object;
        return object;
    }

    private static void check(boolean condition, String message) {
        ++checks;
        if (!condition) throw new AssertionError(message);
    }

    private List<Integer> each(int oil, int flags) {
        List<Integer> result = new ArrayList<>();
        selection.ForEach(oil, new ObjectSelectionClass.ForEachCallBackClass() {
            @Override public void ForEachCallBack(CObject object) { result.add((int) object.hoNumber); }
        }, flags);
        return result;
    }

    private static ObjectSelectionClass.FilterClass filter(final boolean value) {
        return new ObjectSelectionClass.FilterClass() {
            @Override public boolean Filter(Object rdPtr, CObject object) { return value; }
        };
    }

    private void lookupAndIteration() {
        check(selection.GetLPOIL(0) == run.rhOiList[0], "GetLPOIL takes a list index");
        check(selection.GetLPOIL(zero) == run.rhOiList[0], "object OIL lookup");
        check(selection.GetOiFromOiList(0) == 40, "creation uses OI handle");
        check(selection.GetOiFromOiList(0x8000) == -1, "cannot create qualifier");
        check(selection.GetLPOIL(-1) == null && selection.GetLPOIL(99) == null, "invalid OIL lookup");
        check(creation.GetCreationOI("player") == 40, "creation shares prefixed name lookup");
        check(creation.GetCreationOI("missing") == -1, "missing creation OI");
        check(selection.OILHasObject("PLAYER"), "case insensitive name lookup");
        check(each(0, 0).equals(Arrays.asList(0, 1)), "default iteration includes slot zero");
        check(each(0, 2).isEmpty(), "SelectedOnly skips unfiltered type");
        check(each(0, 3).equals(Arrays.asList(0, 1)), "ForceAll has precedence");
        check(each(0x8000, 0).equals(Arrays.asList(0, 1, 2)), "unsigned qualifier");
        check(each((short) 0x8000, 0).equals(Arrays.asList(0, 1, 2)), "signed qualifier");
        check(each(-1, 0).isEmpty() && each(99, 0).isEmpty(), "invalid iteration");
        selection.SelectOneObject(zero);
        check(selection.Selected(0) && selection.ObjectIsSelected(0x8000), "selected slot zero");
        check(selection.GetFirstObject(0, true) == zero, "first selected object");
        check(each(0, 2).equals(Collections.singletonList(0)), "SelectedOnly uses SOL");
        check(each(0, 1).equals(Arrays.asList(0, 1)), "ForceAll ignores SOL");
        List<CObject> legacy = new ArrayList<>();
        selection.ForEach(0, new ObjectSelectionClass.ForEachCallBackClass() {
            @Override public void ForEachCallBack(CObject object) { legacy.add(object); }
        }, false);
        check(legacy.equals(Collections.singletonList(zero)), "legacy boolean overload");
        selection.SelectNone(0);
        check(selection.ObjectIsSelected(0) && !selection.Selected(0), "empty filtered SOL");
        check(each(0, 0).isEmpty(), "empty SOL does not fall back to all");
        selection.SelectAll(0);
        selection.SelectAll(1);
        check(selection.GetNumberOfSelected(0x8000) == 3, "qualifier selected count uses indices");
        check(selection.GetNumberOfObject(0x8000) == 3, "qualifier total count uses indices");
        check(selection.ObjectIsOfType(two, 0x8000) && !selection.ObjectIsOfType(two, 0), "qualifier type");
        one.hoFlags = CObject.HOF_DESTROYED;
        check(each(0, 1).equals(Collections.singletonList(0)), "ForEach excludes destroyed objects");
        List<CObject> identifierObjects = new ArrayList<>();
        selection.IterateObjectWithIdentifier(null, 123, new ObjectSelectionClass.ForEachCallBackClass() {
            @Override public void ForEachCallBack(CObject object) { identifierObjects.add(object); }
        });
        check(identifierObjects.equals(Arrays.asList(zero, two)), "identifier traversal");
        program.qualToOiList[0].qoiList = new short[]{50, 1};
        check(selection.GetNumberOfObject(0x8000) == 1, "refresh qualifier mapping");
        run.rhObjectList[0] = null;
        selection.SelectAll(0); // Missing links must terminate, never hang.
        check(each(0, 1).isEmpty(), "missing object link terminates");
    }

    private void filters() {
        one.hoFlags = CObject.HOF_DESTROYED;
        check(selection.FilterObjects(null, 0, false, filter(true)), "default filter includes destroyed");
        check(selection.GetNumberOfSelected(0) == 2, "default destroyed selection count");
        check(selection.FilterObjects(null, 0, false, filter(true), false), "filter excludes destroyed");
        check(selection.GetNumberOfSelected(0) == 1 && selection.GetFirstObject(0, true) == zero,
                "destroyed filter updates SOL");
        selection.SelectAll(0);
        check(!selection.FilterObjects(null, 0, true, filter(false), false), "negated condition return");
        check(selection.GetNumberOfSelected(0) == 1, "destroyed exclusion is outside negation");
        ++program.rh2EventCount;
        check(!selection.FilterObjects(null, 0x8000, true, filter(false)), "qualifier negation return");
        check(selection.GetNumberOfSelected(0x8000) == 3, "all qualifier members filtered");
        check(!selection.FilterObjects(null, 0x8000, false, filter(false)), "reject all qualifier members");
        check(selection.GetNumberOfSelected(0x8000) == 0, "reject all empties every SOL");
        check(selection.FilterObjects(null, 0, true, filter(true)), "negated empty SOL condition");
        check(!selection.FilterObjects(null, -1, true, filter(true)), "invalid condition rejected");
    }

    private void scopes() {
        selection.SelectOneObject(zero);
        selection.SelectNone(1);
        program.rh2ActionOn = true;
        program.rh2ActionLoop = true;
        program.rh2ActionCount = 15;
        program.rh2ActionLoopCount = 3;
        program.rhEventGroup = new CEventGroup();
        CEventGroup group = program.rhEventGroup;
        selection.KeepScopeCall(true, () -> {
            selection.SelectOneObject(one);
            selection.SelectOneObject(two);
            program.rh2ActionCount = 16;
            selection.KeepScopeCall(true, () -> selection.SelectNone(0));
            check(selection.GetFirstObject(0, true) == one, "nested scope restores inner selection");
            program.rh2ActionLoop = false;
            program.rh2ActionOn = false;
            program.rh2ActionLoopCount = 0;
            program.rhEventGroup = null;
            ++program.rh2EventCount;
        });
        check(selection.GetFirstObject(0, true) == zero, "scope restores original selection");
        check(selection.ObjectIsSelected(1) && selection.GetNumberOfSelected(1) == 0,
                "scope restores empty selection");
        check(program.rh2ActionCount == 15 && program.rh2ActionLoopCount == 3
                && program.rh2ActionOn && program.rh2ActionLoop && program.rhEventGroup == group,
                "scope restores action state");
        try {
            selection.KeepScopeCall(true, () -> {
                selection.SelectOneObject(one);
                throw new IllegalStateException("test");
            });
            throw new AssertionError("exception should propagate");
        } catch (IllegalStateException expected) {
            check(selection.GetFirstObject(0, true) == zero, "exception restores scope");
        }
        selection.KeepScopeCall(false, () -> selection.SelectOneObject(one));
        check(selection.GetFirstObject(0, true) == one, "disabled scope keeps callback changes");
        ObjectSelectionClass.Scope saved = selection.SaveScope();
        run.rhObjectList[1] = new CObject();
        selection.RestoreScope(saved);
        check(selection.GetNumberOfSelected(0) == 0, "scope does not select a reused slot");
        selection.SelectOneObject(zero);
        saved = selection.SaveScope();
        CObjInfo oldOil = run.rhOiList[0];
        run.rhOiList[0] = new CObjInfo();
        selection.RestoreScope(saved);
        check(!selection.OILHasObject(oldOil), "scope ignores removed OIL");
        ObjectSelectionClass.KeepScopeCall(null, true, () -> checks++);
    }

    private void creation() {
        run.result = 0;
        zero.hoOEFlags = OI.CObjectCommon.OEFLAG_SPRITES;
        app.frame.layers[0].dwOptions = CLayer.FLOPT_TOHIDE;
        creation.OCCreateObject(new ObjectCreationClass.CreationParamClass() {
            @Override public void CreationParam(ObjectCreationClass oc, ObjectCreationClass.CreateDuplicateParam param) {
                param.oi = oc.GetCreationOI("Player");
                param.layer = -10;
            }
        });
        check(program.current == zero, "creation selects slot zero");
        check(run.lastLayer == -1, "lower layer bound");
        check(!zero.ros.hidden, "default layer is not used as array index");
        creation.OCCreateObject(new ObjectCreationClass.CreationParamClass() {
            @Override public void CreationParam(ObjectCreationClass oc, ObjectCreationClass.CreateDuplicateParam param) {
                param.oi = 40;
                param.layer = 0;
            }
        });
        check(zero.ros.hidden, "hidden layer hides created sprite");
        creation.OCCreateObject(new ObjectCreationClass.CreationParamClass() {
            @Override public void CreationParam(ObjectCreationClass oc, ObjectCreationClass.CreateDuplicateParam outer) {
                outer.oi = 40;
                outer.x = 100;
                outer.layer = 100;
                oc.OCCreateObject(new ObjectCreationClass.CreationParamClass() {
                    @Override public void CreationParam(ObjectCreationClass innerOC, ObjectCreationClass.CreateDuplicateParam inner) {
                        inner.oi = 50;
                        inner.x = 200;
                    }
                });
            }
        });
        check(run.lastOi == 40 && run.lastX == 100 && run.lastLayer == 1,
                "nested creation preserves outer params and clamps layer");
        program.current = null;
        run.result = -1;
        creation.OCCreateObject(new ObjectCreationClass.CreationParamClass() {
            @Override public void CreationParam(ObjectCreationClass oc, ObjectCreationClass.CreateDuplicateParam param) {
                param.oi = 40;
            }
        });
        check(program.current == null, "failed creation is not selected");
        run.lastOi = -2;
        creation.OCCreateObject(new ObjectCreationClass.CreationParamClass() {
            @Override public void CreationParam(ObjectCreationClass oc, ObjectCreationClass.CreateDuplicateParam param) {
                param.oi = -1;
            }
        });
        check(run.lastOi == -2, "missing OI is not created");
    }

    public static void main(String[] args) {
        new ObjectHelpersTest().lookupAndIteration();
        new ObjectHelpersTest().filters();
        new ObjectHelpersTest().scopes();
        new ObjectHelpersTest().creation();
        System.out.println("Object helper regressions passed: " + checks + " checks (Java 8)");
    }
}
