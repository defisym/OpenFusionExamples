package Extensions;

import Actions.CActExtension;
import Banks.CImage;
import Conditions.CCndExtension;
import Expressions.CValue;

import Frame.CLayer;
import Objects.CExtension;
import Objects.CObject;
import Params.*;
import RunLoop.CBackDrawPaste;
import RunLoop.CBkd2;
import RunLoop.CCreateObjectInfo;
import Services.CBinaryFile;

import Services.CRect;
import Sprites.CSpriteGen;
import _3rdLib.ObjectCreationClass;
import _3rdLib.ObjectSelectionClass;
import _DeLib.*;
import android.graphics.Bitmap;

import org.jetbrains.annotations.NotNull;

import java.util.Objects;
import java.util.Vector;
import java.util.function.Consumer;

import static RunLoop.CRun.OBSTACLE_TRANSPARENT;
import static _3rdLib.ObjectCreationClass.GetEvtParam;
import static _DeLib.FindTheWayClass.*;
import static _DeLib.FusionCommon.IDENTIFIER_ACTIVE;

public class CRunFindTheWay extends CRunExtension {
    // TODO DEBUG
    private static final boolean DEBUG = true;
    // private static final boolean DEBUG = false;

    // Define ACE ID here
    // Condition
    final int CND_CONDITION_SMBS = 0;
    final int CND_CONDITION_SMBB = 1;
    final int CND_CONDITION_SMBC = 2;
    final int CND_CONDITION_OSMBC = 3;

    final int CND_CONDITION_OPF = 4;
    final int CND_CONDITION_PA = 5;

    final int CND_CONDITION_OITP = 6;

    final int CND_CONDITION_MA = 7;
    final int CND_CONDITION_CMCAC = 8;

    final int CND_CONDITION_OAG = 9;
    final int CND_CONDITION_OAO = 10;
    final int CND_CONDITION_OAC = 11;

    final int CND_CONDITION_SMBP = 12;

    final int CND_CONDITION_CA = 13;
    final int CND_CONDITION_OITA = 14;
    final int CND_CONDITION_OITAA = 15;
    final int CND_CONDITION_AITA = 16;

    final int CND_CONDITION_OMC = 17;

    final int CND_CONDITION_OCOZ = 18;
    final int CND_CONDITION_ZV = 19;
    final int CND_CONDITION_ZAA = 20;
    final int CND_CONDITION_SA = 21;
    final int CND_CONDITION_OAOBJ = 22;
    final int CND_CONDITION_POAO = 23;

    final int CND_CONDITION_NOAG = 24;
    final int CND_CONDITION_NOAC = 25;

    final int CND_CONDITION_SMBA = 26;
    final int CND_CONDITION_SMBSF = 27;

    final int CND_LAST = 28;

    // Action
    final int ACT_ACTION_SM = 0;
    final int ACT_ACTION_C = 1;
    final int ACT_ACTION_ITP = 2;
    final int ACT_ACTION_SMBO = 3;
    final int ACT_ACTION_CM = 4;
    final int ACT_ACTION_SZBO = 5;
    final int ACT_ACTION_CZ = 6;
    final int ACT_ACTION_ITA = 7;
    final int ACT_ACTION_SUBO = 8;
    final int ACT_ACTION_CU = 9;
    final int ACT_ACTION_COZ = 10;
    final int ACT_ACTION_SS = 11;
    final int ACT_ACTION_COZBE = 12;
    final int ACT_ACTION_COZBN = 13;
    final int ACT_ACTION_CAO = 14;
    final int ACT_ACTION_CABNO = 15;
    final int ACT_ACTION_CAOE = 16;
    final int ACT_ACTION_CAOEBN = 17;
    final int ACT_ACTION_CG = 18;
    final int ACT_ACTION_SGS = 19;

    // Expression
    final int EXP_EXPRESSION_GITX = 0;
    final int EXP_EXPRESSION_GITY = 1;

    final int EXP_EXPRESSION_GS = 2;
    final int EXP_EXPRESSION_GSOP = 3;
    final int EXP_EXPRESSION_GSCOP = 4;

    final int EXP_EXPRESSION_GGC = 5;
    final int EXP_EXPRESSION_GRC = 6;

    final int EXP_EXPRESSION_GITI = 7;

    final int EXP_EXPRESSION_GMC = 8;
    final int EXP_EXPRESSION_GMB64 = 9;
    final int EXP_EXPRESSION_GMS = 10;

    final int EXP_EXPRESSION_GALR = 11;

    final int EXP_EXPRESSION_GIF = 12;

    final int EXP_EXPRESSION_GMTM = 13;
    final int EXP_EXPRESSION_GMTT = 14;
    final int EXP_EXPRESSION_GMTD = 15;
    final int EXP_EXPRESSION_GMCP = 16;
    final int EXP_EXPRESSION_GMCO = 17;
    final int EXP_EXPRESSION_GMCV = 18;

    final int EXP_EXPRESSION_GIGS = 19;

    // Expression return value
    CValue expRet;

    // Object's runtime data
    boolean isometric;

    FindTheWayClass pFTW;

    int itIndex;
    Coord itCoord;

    String pOnItCollisionName;
    String pOnItPathName;
    String pMapBase64Str;
    String pMapStr;

    ObjectSelectionClass pSelect;

    boolean isAttack;
    int areaSize;
    int areaPos;
    int extraRangeStartPos;

    Vector<Coord> pZoc;
    Vector<Coord> pAlly;
    Vector<Coord> pEnemy;
    Vector<Coord> pUnit;

    String pOnItAreaName;

    CObject pObject;
    ObjectCreationClass pOc;
    Vector<Coord> pObjZoc;
    String pOnItZocName;

    AOEClass pAOE;
    Vector<AOECoord> pAOECoord;

    // Constructor
    public CRunFindTheWay() {
        expRet = new CValue(0);
        itCoord = new Coord(0, 0);

        pZoc = new Vector<Coord>();
        pAlly = new Vector<Coord>();
        pEnemy = new Vector<Coord>();
        pUnit = new Vector<Coord>();

        pObjZoc = new Vector<Coord>();
    }

    // Fusion Funcs
    @Override
    public int getNumberOfConditions() {
        return CND_LAST;
    }

    @Override
    public boolean createRunObject(@NotNull final CBinaryFile file, final CCreateObjectInfo cob, final int version) {
        // Read Properties
        // file.readByte();
        // file.readInt();
        // file.readString(260);
        // ......

        this.isometric = (file.readByte() != 0);

        this.pObjZoc.ensureCapacity(8);

        pSelect = new ObjectSelectionClass(rh.rhApp);
        pOc = new ObjectCreationClass(rh.rhApp);

        // Return
        return true;
    }

    @Override
    public void destroyRunObject(boolean bFast) {
    }

    @Override
    public int handleRunObject() {
        return 0;
    }

    // ......

    // Jump Table
    // Condition Jump Table
    @Override
    public boolean condition(int num, CCndExtension cnd) {
        switch (num) {
            case CND_CONDITION_SMBS: {
                this.pFTW = null;

                int width = cnd.getParamExpression(rh, 0);
                int height = cnd.getParamExpression(rh, 1);

                try {
                    this.pFTW = new FindTheWayClass(width, height);
                    this.pFTW.SetUpdateMapCallBack(this::UpdateMapCallBackFunc, (Object) ho);
                    this.pFTW.SetIsometric(this.isometric);
                } catch (Throwable e) {
                    return false;
                }

                if (DEBUG) {
                    String output = this.pFTW.OutPutMapStr(MapType.MAP, false, null);
                }

                return true;
            }
            case CND_CONDITION_SMBB: {
                this.pFTW = null;

                String base64 = cnd.getParamExpString(rh, 0);

                try {
                    this.pFTW = new FindTheWayClass(base64);
                    this.pFTW.SetUpdateMapCallBack(this::UpdateMapCallBackFunc, (Object) ho);
                    this.pFTW.SetIsometric(this.isometric);
                } catch (Throwable e) {
                    return false;
                }

                if (DEBUG) {
                    String output = this.pFTW.OutPutMapStr(MapType.MAP, false, null);
                }

                return true;
            }
            case CND_CONDITION_SMBC: {
                this.pFTW = null;

                int gridSize = cnd.getParamExpression(rh, 0);
                int gridOffsetX = cnd.getParamExpression(rh, 1);
                int gridOffsetY = cnd.getParamExpression(rh, 2);

                boolean eventIterate = (cnd.getParamExpression(rh, 3) != 0);
                int baseLayer = cnd.getParamExpression(rh, 4) - 1;
                MapType type = MapType.values()[cnd.getParamExpression(rh, 5)];

                this.pOnItCollisionName = cnd.getParamExpString(rh, 6);

                if (type == MapType.MAP) {
                    return false;
                }

                CRect frameRect = ho.hoAdRunHeader.rhFrame.leVirtualRect;

                int frameWidth = frameRect.right - frameRect.left;
                int frameHeight = frameRect.bottom - frameRect.top;

                int width = FindTheWayClass.CalcMapWidth(frameWidth, gridSize, this.isometric);
                int height = FindTheWayClass.CalcMapHeight(frameHeight, gridSize, this.isometric);

                try {
                    this.pFTW = new FindTheWayClass(width, height);
                    this.pFTW.SetUpdateMapCallBack(this::UpdateMapCallBackFunc, (Object) ho);
                    this.pFTW.SetIsometric(this.isometric);
                } catch (Throwable e) {
                    return false;
                }

                this.pFTW.SetGridSize(gridSize, gridOffsetX, gridOffsetY);

                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        this.itCoord = this.pFTW.GetRealCoord(new Coord(x, y));

                        if (eventIterate) {
                            CallEvent(CND_CONDITION_OSMBC);
                        }
                        // Ref:
                        // https://community.clickteam.com/threads/59029-Accessing-backdrops-from-extensions
                        else if (!colMaskTestPoint(this.itCoord.x, this.itCoord.y, baseLayer, 0)) {
                            this.pFTW.SetMap(x, y, MAP_OBSTACLE, type, true);
                        }
                    }
                }

                if (DEBUG) {
                    String output = this.pFTW.OutPutMapStr(MapType.MAP, false, null);
                }

                return true;
            }
            case CND_CONDITION_OSMBC: {
                String iterateName = cnd.getParamExpString(rh, 0);
                return Objects.equals(this.pOnItCollisionName, iterateName);
            }
            case CND_CONDITION_OPF: {
                int startX = cnd.getParamExpression(rh, 0);
                int startY = cnd.getParamExpression(rh, 1);

                int destinationX = cnd.getParamExpression(rh, 2);
                int destinationY = cnd.getParamExpression(rh, 3);

                int ignoreFlag = cnd.getParamExpression(rh, 4);

                boolean diagonal = (cnd.getParamExpression(rh, 5) != 0);
                boolean checkDiagonalCorner = (cnd.getParamExpression(rh, 6) != 0);

                boolean forceFind = (cnd.getParamExpression(rh, 7) != 0);
                boolean useRealCoord = (cnd.getParamExpression(rh, 8) != 0);

                String saveName = cnd.getParamExpString(rh, 9);

                if (this.pFTW == null) {
                    return false;
                }

                FindPath(new Coord(startX, startY), new Coord(destinationX, destinationY), ignoreFlag, diagonal,
                        checkDiagonalCorner, forceFind, useRealCoord, saveName);

                if (DEBUG) {
                    String output = this.pFTW.OutPutMapStr(MapType.MAP, false, null);
                }

                return this.pFTW.GetPathValidity(null);
            }
            case CND_CONDITION_PA: {
                String pathName = cnd.getParamExpString(rh, 0);

                if (this.pFTW == null) {
                    return false;
                }

                return this.pFTW.GetPathValidity(pathName);
            }
            case CND_CONDITION_OITP: {
                String iterateName = cnd.getParamExpString(rh, 0);

                return Objects.equals(this.pOnItPathName, iterateName);
            }
            case CND_CONDITION_MA: {
                return this.pFTW != null;
            }
            case CND_CONDITION_CMCAC: {
                int x = cnd.getParamExpression(rh, 0);
                int y = cnd.getParamExpression(rh, 1);

                int cost = cnd.getParamExpression(rh, 2);
                MapType type = MapType.values()[cnd.getParamExpression(rh, 3)];

                boolean useRealCoord = (cnd.getParamExpression(rh, 4) != 0);

                if (this.pFTW == null) {
                    return false;
                }

                Coord gridCoord = new Coord(x, y);

                if (useRealCoord) {
                    gridCoord = this.pFTW.GetGridCoord(gridCoord);
                }

                int mapCost = this.pFTW.GetMap(gridCoord.x, gridCoord.y, type, true);

                if (cost == -1 && mapCost != MAP_OBSTACLE) {
                    // Check for path
                    return true;
                } else if (cost == 65536 && mapCost == MAP_OBSTACLE) { // Check for obstacle
                    return true;
                } else if (cost == mapCost) { // Check accurate cost
                    return true;
                } else {
                    return false;
                }
            }
            case CND_CONDITION_OITA: {
                String iterateName = cnd.getParamExpString(rh, 0);

                return Objects.equals(this.pOnItAreaName, iterateName);
            }
            case CND_CONDITION_OITAA: {
                return this.isAttack;
            }
            case CND_CONDITION_AITA: {
                return this.areaPos < this.areaSize;
            }
            case CND_CONDITION_OMC: {
                return true;
            }
            case CND_CONDITION_OCOZ: {
                boolean negated = ObjectSelectionClass.IsNegated(cnd);

                short oil = ObjectSelectionClass.GetOil(cnd, rh, 0);
                short oilZoc = ObjectSelectionClass.GetOil(cnd, rh, 1);

                String iterateName = cnd.getParamExpString(rh, 2);

                if (this.pFTW == null) {
                    return false;
                }

                Coord itGridCoord = this.pFTW.GetGridCoord(this.itCoord);

                return Objects.equals(this.pOnItZocName, iterateName)
                        && this.pSelect.FilterObjects(this, oil, negated, new ObjectSelectionClass.FilterClass() {
                            @Override
                            public boolean Filter(Object rdPtr, CObject object) {
                                return object == CRunFindTheWay.this.pObject;
                            }
                        });
            }
        }

        return false;
    }

    // Action Jump Table
    @Override
    public void action(int num, CActExtension act) {
        switch (num) {
            case ACT_ACTION_SM: {
                int x = act.getParamExpression(rh, 0);
                int y = act.getParamExpression(rh, 1);

                int cost = act.getParamExpression(rh, 2);
                MapType type = MapType.values()[act.getParamExpression(rh, 3)];

                boolean useRealCoord = (act.getParamExpression(rh, 4) != 0);

                if (this.pFTW == null) {
                    return;
                }

                if (type == MapType.MAP) {
                    return;
                }

                Coord gridCoord = new Coord(x, y);

                if (useRealCoord) {
                    gridCoord = this.pFTW.GetGridCoord(gridCoord);
                }

                this.pFTW.SetMap(gridCoord.x, gridCoord.y, cost, type, true);

                return;
            }
            case ACT_ACTION_SMBO: {
                CObject object = act.getParamObject(rh, 0);
                int cost = act.getParamExpression(rh, 1);
                MapType type = MapType.values()[act.getParamExpression(rh, 2)];

                if (!LPROValid(object, 0)) {
                    return;
                }

                if (this.pFTW == null) {
                    return;
                }
                if (type == MapType.MAP) {
                    return;
                }

                Coord gridCoord = this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY));
                this.pFTW.SetMap(gridCoord.x, gridCoord.y, cost, type, true);

                return;
            }
            case ACT_ACTION_CM: {
                int cost = act.getParamExpression(rh, 0);
                MapType type = MapType.values()[act.getParamExpression(rh, 1)];

                if (this.pFTW == null) {
                    return;
                }
                if (type == MapType.MAP) {
                    return;
                }

                this.pFTW.ClearMap(type, cost, true);

                return;
            }
            case ACT_ACTION_C: {
                return; // Force fusion to evaluate conditions, as empty events will be skipped
            }
            case ACT_ACTION_ITP: {
                String pathName = act.getParamExpString(rh, 0);
                boolean useRealCoord = (act.getParamExpression(rh, 1) != 0);

                this.pOnItPathName = act.getParamExpString(rh, 2);

                if (this.pFTW == null) {
                    return;
                }

                String pPathName = Objects.equals(pathName, "") ? null : pathName;

                if (!this.pFTW.GetPathValidity(pPathName)) {
                    return;
                }

                this.itIndex = 0;
                int totalSteps = this.pFTW.GetDistance(pPathName);

                for (int step = 0; step < totalSteps; step++) {
                    Coord coord = new Coord(this.pFTW.GetCoordAtPath(step, CoordType.X, pPathName),
                            this.pFTW.GetCoordAtPath(step, CoordType.Y, pPathName));

                    this.itCoord = useRealCoord ? this.pFTW.GetRealCoord(coord) : coord;
                    this.itIndex = step;

                    CallEvent(CND_CONDITION_OITP);
                }

                return;
            }
            case ACT_ACTION_SZBO: {
                CObject object = act.getParamObject(rh, 0);
                boolean center = (act.getParamExpression(rh, 1) != 0);

                if (!LPROValid(object, 0)) {
                    return;
                }

                if (this.pFTW == null) {
                    return;
                }

                Coord objectCoord = this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY));

                if (center) {
                    this.pFTW.GenerateZoc(objectCoord, this.pZoc, true, true);
                } else if (!this.pZoc.contains(objectCoord)) {
                    this.pZoc.add(objectCoord);
                }

                return;
            }
            case ACT_ACTION_CZ: {
                this.pZoc.clear();

                return;
            }
            case ACT_ACTION_ITA: {
                this.pOnItAreaName = act.getParamExpString(rh, 0);
                boolean once = (act.getParamExpression(rh, 1) != 0);

                if (this.pFTW == null) {
                    return;
                }

                Vector<Vector<Coord>> area = this.pFTW.GetArea();

                Consumer<Vector<Coord>> loop = (Vector<Coord> s) -> {
                    for (Coord it_c : s) {
                        this.itCoord = it_c;

                        CallEvent(CND_CONDITION_OITA);
                    }
                };

                if (once) {
                    if (this.areaPos < area.size()) {
                        this.isAttack = !(this.areaPos < this.extraRangeStartPos);

                        loop.accept(area.get(this.areaPos));

                        this.areaPos++;
                    }
                } else {
                    for (int it = 0; it < area.size(); it++) {
                        this.isAttack = !(it < this.extraRangeStartPos);

                        loop.accept(area.get(it));

                        this.areaPos++;
                    }
                }

                return;
            }
            case ACT_ACTION_SUBO: {
                CObject object = act.getParamObject(rh, 0);
                int type = act.getParamExpression(rh, 1);

                if (!LPROValid(object, 0)) {
                    return;
                }

                if (this.pFTW == null) {
                    return;
                }

                Coord objectCoord = this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY));

                final int ALLY = 0;
                final int ENEMY = 1;
                final int UNIT = 2;

                switch (type) {
                    case ALLY: {
                        this.pAlly.add(objectCoord);
                        break;
                    }
                    case ENEMY: {
                        this.pEnemy.add(objectCoord);
                        break;
                    }
                    case UNIT: {
                        this.pUnit.add(objectCoord);
                        break;
                    }
                }

                return;
            }
            case ACT_ACTION_CU: {
                int target = act.getParamExpression(rh, 0);

                final int ALL = -1;
                final int ALLY = 0;
                final int ENEMY = 1;
                final int UNIT = 2;

                switch (target) {
                    case ALL: {
                        this.pAlly.clear();
                        this.pEnemy.clear();
                        this.pUnit.clear();
                        break;
                    }
                    case ALLY: {
                        this.pAlly.clear();
                        break;
                    }
                    case ENEMY: {
                        this.pEnemy.clear();
                        break;
                    }
                    case UNIT: {
                        this.pUnit.clear();
                        break;
                    }
                }

                return;
            }
            case ACT_ACTION_SS: {
                boolean enable = act.getParamExpression(rh, 0) != 0;

                if (this.pFTW == null) {
                    return;
                }

                this.pFTW.SetStash(enable);

                if (!enable) {
                    this.pFTW.ClearStash();
                }

                return;
            }
            case ACT_ACTION_COZ: {
                CObject object = act.getParamObject(rh, 0);
                PARAM_CREATE param = (PARAM_CREATE) GetEvtParam(act, rh, 1);

                // CPositionInfo pInfo = new CPositionInfo();
                // param.read_Position(rh,0x11,pInfo);

                if (!LPROValid(object, 0)) {
                    return;
                }

                if (this.pFTW == null) {
                    return;
                }

                Coord objectCoord = this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY));
                this.pFTW.GenerateZoc(objectCoord, this.pObjZoc, false, true);

                for (Coord it : this.pObjZoc) {
                    Coord realCoord = this.pFTW.GetRealCoord(it);

                    this.pOc.OCCreateObject(new ObjectCreationClass.CreationParamClass() {
                        @Override
                        public void CreationParam(ObjectCreationClass oc,
                                ObjectCreationClass.CreateDuplicateParam cdp) {
                            cdp.HFII = param.cdpHFII;
                            cdp.oi = param.cdpOi;
                            cdp.x = realCoord.x;
                            cdp.y = realCoord.y;
                            cdp.layer = param.posLayer;
                        }
                    });
                }

                return;
            }
            case ACT_ACTION_COZBE: {
                short oil = ObjectSelectionClass.GetOil(act, rh, 0);
                this.pOnItZocName = act.getParamExpString(rh, 1);

                if (this.pFTW == null) {
                    return;
                }

                this.pSelect.ForEach(oil, new ObjectSelectionClass.ForEachCallBackClass() {
                    @Override
                    public void ForEachCallBack(CObject object) {
                        Coord objectCoord = pFTW.GetGridCoord(new Coord(object.hoX, object.hoY));

                        CRunFindTheWay.this.pObject = object;
                        CRunFindTheWay.this.pFTW.GenerateZoc(objectCoord, CRunFindTheWay.this.pObjZoc, false, true);

                        for (Coord it : CRunFindTheWay.this.pObjZoc) {
                            CRunFindTheWay.this.itCoord = CRunFindTheWay.this.pFTW.GetRealCoord(it);

                            CallEvent(CND_CONDITION_OCOZ);
                        }

                        CRunFindTheWay.this.pObject = null;
                    }
                });

                return;
            }
            case ACT_ACTION_COZBN: {
                CObject object = act.getParamObject(rh, 0);
                String objectName = act.getParamExpString(rh, 1);

                if (!LPROValid(object, 0)) {
                    return;
                }

                if (this.pFTW == null) {
                    return;
                }

                short Oi = this.pOc.GetCreationOI(objectName);

                Coord objectCoord = this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY));
                this.pFTW.GenerateZoc(objectCoord, this.pObjZoc, false, true);

                for (Coord it : this.pObjZoc) {
                    Coord realCoord = this.pFTW.GetRealCoord(it);

                    this.pOc.OCCreateObject(new ObjectCreationClass.CreationParamClass() {
                        @Override
                        public void CreationParam(ObjectCreationClass oc,
                                ObjectCreationClass.CreateDuplicateParam cdp) {
                            cdp.oi = Oi;
                            cdp.x = realCoord.x;
                            cdp.y = realCoord.y;
                            cdp.layer = 1;
                        }
                    });
                }

                return;

            }
            case ACT_ACTION_CG: {
                CObject object = act.getParamObject(rh, 0);
                int nLayer = act.getParamExpression(rh, 1) - 1;

                if (!LPROValid(object, IDENTIFIER_ACTIVE)) { // must be active object
                    return;
                }

                int nObstacleType = OBSTACLE_TRANSPARENT;
                short nFrame = object.roa.raAnimDirOffset.adNumberOfFrame;

                int rOffset = object.rov.rvValues[0].getInt();
                int bOffset = object.rov.rvValues[1].getInt();

                if (0 >= nFrame) {
                    return;
                }

                short hImageLT = object.roa.raAnimDirOffset.adFrames[0];
                CImage cImageLT = ho.getImageBank().getImageFromHandle(hImageLT);

                if (1 >= nFrame) {
                    return;
                }

                short hImageR = object.roa.raAnimDirOffset.adFrames[1];
                CImage cImageR = ho.getImageBank().getImageFromHandle(hImageR);

                if (2 >= nFrame) {
                    return;
                }

                short hImageB = object.roa.raAnimDirOffset.adFrames[2];
                CImage cImageB = ho.getImageBank().getImageFromHandle(hImageB);

                int hoX = 0;
                int hoY = 0;

                hoX = -cImageLT.getXSpot();
                hoY = -cImageLT.getYSpot();

                if (DEBUG) {
                    // no hotspot offset when debugging
                    hoX = 0;
                    hoY = 0;
                }

                int effect = object.ros.rsEffect;
                int effectParam = object.ros.rsEffectParam;

                if (this.pFTW == null) {
                    return;
                }

                int width = this.pFTW.GetMapWidth();
                int height = this.pFTW.GetMapHeight();

                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        if (this.pFTW.GetMap(x, y, MapType.MAP, true) != MAP_OBSTACLE) {
                            Coord coord = this.pFTW.GetRealCoord(new Coord(x, y));

                            addBackdrop(cImageLT, coord.x - hoX, coord.y - hoY, nLayer, nObstacleType, effect,
                                    effectParam);

                            if (this.pFTW.GetMap(x + 1, y, MapType.MAP, true) == MAP_OBSTACLE) {
                                addBackdrop(cImageR, coord.x - hoX + (x + 1 == width ? 0 : rOffset), coord.y - hoY,
                                        nLayer,
                                        nObstacleType, effect, effectParam);
                            }

                            if (this.pFTW.GetMap(x, y + 1, MapType.MAP, true) == MAP_OBSTACLE) {
                                addBackdrop(cImageB, coord.x - hoX, coord.y - hoY + (y + 1 == height ? 0 : bOffset),
                                        nLayer,
                                        nObstacleType, effect, effectParam);
                            }
                        }
                    }
                }

                break;
            }
            case ACT_ACTION_SGS: {
                int gridSize = act.getParamExpression(rh, 0);
                int gridOffsetX = act.getParamExpression(rh, 1);
                int gridOffsetY = act.getParamExpression(rh, 2);

                if (this.pFTW == null) {
                    return;
                }

                this.pFTW.SetGridSize(gridSize, gridOffsetX, gridOffsetY);

                break;
            }
        }
    }

    // Expression Jump Table
    @Override
    public CValue expression(int num) {
        switch (num) {
            case EXP_EXPRESSION_GITX: {
                expRet.forceInt(this.itCoord.x);
                return expRet;
            }
            case EXP_EXPRESSION_GITY: {
                expRet.forceInt(this.itCoord.y);
                return expRet;
            }
            case EXP_EXPRESSION_GITI: {
                expRet.forceInt(this.itIndex);
                return expRet;
            }
            case EXP_EXPRESSION_GS: {
                int startX = ho.getExpParam().getInt();
                int startY = ho.getExpParam().getInt();

                int destinationX = ho.getExpParam().getInt();
                int destinationY = ho.getExpParam().getInt();

                int ignoreFlag = ho.getExpParam().getInt();

                boolean diagonal = ho.getExpParam().getInt() != 0;
                boolean checkDiagonalCorner = ho.getExpParam().getInt() != 0;

                boolean forceFind = ho.getExpParam().getInt() != 0;
                boolean useRealCoord = ho.getExpParam().getInt() != 0;

                String saveName = ho.getExpParam().getString();

                if (this.pFTW == null) {
                    expRet.forceInt(STEP_UNREACHABLE);
                    return expRet;
                }

                FindPath(new Coord(startX, startY), new Coord(destinationX, destinationY), ignoreFlag, diagonal,
                        checkDiagonalCorner, forceFind, useRealCoord, saveName);

                if (DEBUG) {
                    String output = this.pFTW.OutPutMapStr(MapType.MAP, false, null);
                }

                expRet.forceInt(this.pFTW.GetDistance(null));
                return expRet;
            }
            case EXP_EXPRESSION_GSOP: {
                String pathName = ho.getExpParam().getString();
                if (this.pFTW == null) {
                    expRet.forceInt(STEP_UNREACHABLE);
                    return expRet;
                }

                String pPathName = Objects.equals(pathName, "") ? null : pathName;

                expRet.forceInt(this.pFTW.GetDistance(pPathName));
                return expRet;
            }
            case EXP_EXPRESSION_GSCOP: {
                String pathName = ho.getExpParam().getString();
                int step = ho.getExpParam().getInt();
                CoordType type = CoordType.values()[ho.getExpParam().getInt()];

                if (this.pFTW == null) {
                    expRet.forceInt(COORD_INVALID);
                    return expRet;
                }

                String pPathName = Objects.equals(pathName, "") ? null : pathName;

                expRet.forceInt(this.pFTW.GetCoordAtPath(step, type, pPathName));
                return expRet;
            }
            case EXP_EXPRESSION_GGC: {
                int coordX = ho.getExpParam().getInt();
                int coordY = ho.getExpParam().getInt();
                CoordType type = CoordType.values()[ho.getExpParam().getInt()];

                if (this.pFTW == null) {
                    expRet.forceInt(COORD_INVALID);
                    return expRet;
                }

                expRet.forceInt(type == CoordType.X
                        ? this.pFTW.GetGridCoord(new Coord(coordX, coordY)).x
                        : this.pFTW.GetGridCoord(new Coord(coordX, coordY)).y);
                return expRet;
            }

            case EXP_EXPRESSION_GRC: {
                int coordX = ho.getExpParam().getInt();
                int coordY = ho.getExpParam().getInt();
                CoordType type = CoordType.values()[ho.getExpParam().getInt()];

                if (this.pFTW == null) {
                    expRet.forceInt(COORD_INVALID);
                    return expRet;
                }

                expRet.forceInt(type == CoordType.X
                        ? this.pFTW.GetRealCoord(new Coord(coordX, coordY)).x
                        : this.pFTW.GetRealCoord(new Coord(coordX, coordY)).y);
                return expRet;
            }

            case EXP_EXPRESSION_GMC: {
                int x = ho.getExpParam().getInt();
                int y = ho.getExpParam().getInt();

                boolean useRealCoord = ho.getExpParam().getInt() != 0;
                MapType type = MapType.values()[ho.getExpParam().getInt()];

                if (this.pFTW == null) {
                    expRet.forceInt(COORD_INVALID);
                    return expRet;
                }

                Coord gridCoord = new Coord(x, y);

                if (useRealCoord) {
                    gridCoord = this.pFTW.GetGridCoord(gridCoord);
                }

                expRet.forceInt(this.pFTW.GetMap(gridCoord.x, gridCoord.y, type, true));
                return expRet;
            }
            case EXP_EXPRESSION_GMB64: {
                // if(this.pFTW==null){
                // expRet.forceString("");
                // return expRet;
                // }

                this.pMapBase64Str = this.pFTW == null ? "InvalidMap" : this.pFTW.GetMap();

                expRet.forceString(this.pMapBase64Str);
                return expRet;
            }
            case EXP_EXPRESSION_GMS: {
                MapType type = MapType.values()[ho.getExpParam().getInt()];
                boolean showPath = ho.getExpParam().getInt() != 0;

                String pathName = ho.getExpParam().getString();

                // if(this.pFTW==null){
                // expRet.forceString("");
                // return expRet;
                // }

                String pPathName = Objects.equals(pathName, "") ? null : pathName;

                this.pMapStr = this.pFTW == null ? "InvalidMap" : this.pFTW.OutPutMapStr(type, showPath, pPathName);

                expRet.forceString(this.pMapStr);
                return expRet;
            }
            case EXP_EXPRESSION_GALR: {
                int x = ho.getExpParam().getInt();
                int y = ho.getExpParam().getInt();

                boolean useRealCoord = ho.getExpParam().getInt() != 0;

                int dir = ho.getExpParam().getInt();
                int range = ho.getExpParam().getInt();

                int ignoreFlag = ho.getExpParam().getInt();

                boolean attack = ho.getExpParam().getInt() != 0;

                if (this.pFTW == null) {
                    expRet.forceInt(0);
                    return expRet;
                }

                Coord gridCoord = new Coord(x, y);

                if (useRealCoord) {
                    gridCoord = this.pFTW.GetGridCoord(gridCoord);
                }

                expRet.forceInt(this.pFTW.GetAbleLineRange(gridCoord, range, dir, this.pAlly, this.pEnemy, this.pZoc,
                        ignoreFlag, attack));
                return expRet;
            }
            case EXP_EXPRESSION_GIF: {
                boolean moveIgnoreZoc = ho.getExpParam().getInt() != 0;
                boolean moveIgnoreAlly = ho.getExpParam().getInt() != 0;
                boolean moveIgnoreEnemy = ho.getExpParam().getInt() != 0;
                boolean attackIgnoreAlly = ho.getExpParam().getInt() != 0;
                boolean attackIgnoreEnemy = ho.getExpParam().getInt() != 0;

                expRet.forceInt(GetIgnoreFlag(moveIgnoreZoc, moveIgnoreAlly, moveIgnoreEnemy, attackIgnoreAlly,
                        attackIgnoreEnemy));
                return expRet;
            }
            case EXP_EXPRESSION_GMTM: {
                expRet.forceInt(MapType.MAP.ordinal());
                return expRet;
            }
            case EXP_EXPRESSION_GMTT: {
                expRet.forceInt(MapType.TERRAIN.ordinal());
                return expRet;
            }
            case EXP_EXPRESSION_GMTD: {
                expRet.forceInt(MapType.DYNAMIC.ordinal());
                return expRet;
            }
            case EXP_EXPRESSION_GMCP: {
                expRet.forceInt(MAP_PATH);
                return expRet;
            }
            case EXP_EXPRESSION_GMCO: {
                expRet.forceInt(MAP_OBSTACLE);
                return expRet;
            }
            case EXP_EXPRESSION_GMCV: {
                int cost = ho.getExpParam().getInt();

                expRet.forceInt(Range(cost));
                return expRet;
            }
            case EXP_EXPRESSION_GIGS: {
                int isoGridWidth = ho.getExpParam().getInt();
                int isoGridHeight = ho.getExpParam().getInt();

                expRet.forceInt(GetIsometricGridSize(isoGridWidth, isoGridHeight));
                return expRet;
            }
        }

        return new CValue(0);
    }

    // Functions
    public void FindPath(Coord start, Coord destination, int ignoreFlag, boolean diagonal, boolean checkDiagonalCorner,
            boolean forceFind, boolean useRealCoord, String saveName) {
        if (useRealCoord) {
            start = this.pFTW.GetGridCoord(start);
            destination = this.pFTW.GetGridCoord(destination);
        }

        if (!forceFind) {
            this.pFTW.Find(start, destination, diagonal, checkDiagonalCorner, this.pAlly, this.pEnemy, this.pZoc,
                    ignoreFlag, FindTheWayClass::GetManhattanDistance);
        } else {
            this.pFTW.ForceFind(start, destination, diagonal, checkDiagonalCorner, this.pAlly, this.pEnemy, this.pZoc,
                    ignoreFlag, FindTheWayClass::GetManhattanDistance);
        }

        if (!Objects.equals(saveName, "")) {
            this.pFTW.SaveLastPath(saveName);
        }
    }

    public void UpdateMapCallBackFunc(Object ho) {
        ((CExtension) ho).generateEvent(CND_CONDITION_OMC, 0);
    }

    public void CallEvent(int event) {
        ho.generateEvent(event, 0);
    }

    public boolean colMaskTestPoint(int x, int y, int layer, int plan) {
        if (ho.hoAdRunHeader.rhFrame.bkdCol_TestPoint(x - ho.hoAdRunHeader.rhWindowX, y - ho.hoAdRunHeader.rhWindowY,
                layer, plan)) {
            return false;
        }
        return true;
    }

    public static boolean LPROValid(CObject object, int identifier) {
        if (object == null) {
            return false;
        }

        if (identifier != 0) {
            return object.hoIdentifier == identifier;
        }

        return true; // need not to check identifier
    }

    // public short getBKDHandle(CImage cImage, int x, int y, int nLayer, int
    // typeObst, int dwEffect, int dwEffectParam){
    // // Duplique
    // int[] mImage = cImage.getRawPixels();
    //
    // if (mImage == null) {
    // return -1;
    // }
    //
    // Bitmap bImage = Bitmap.createBitmap(cImage.getWidth(), cImage.getHeight(),
    // cImage.getFormat());
    // bImage.setPixels(mImage, 0, cImage.getWidth(), 0, 0, cImage.getWidth(),
    // cImage.getHeight());
    //
    // short handle = ho.hoAdRunHeader.rhApp.imageBank.addImage(bImage, (short)
    // cImage.getXSpot(),
    // (short) cImage.getYSpot(), (short) cImage.getXAP(), (short) cImage.getYAP(),
    // true);
    //
    // // Ajoute a la liste
    // CBkd2 toadd = new CBkd2();
    // toadd.img = handle;
    // toadd.loHnd = 0;
    // toadd.oiHnd = 0;
    // toadd.x = x;
    // toadd.y = y;
    // toadd.width = cImage.getWidth();
    // toadd.height = cImage.getHeight();
    // toadd.nLayer = (short) nLayer;
    // toadd.inkEffect = dwEffect;
    // toadd.inkEffectParam = dwEffectParam;
    // toadd.colMode = CSpriteGen.CM_BITMAP;
    // toadd.obstacleType = (short) typeObst; // a voir
    // for (int ns = 0; ns < 4; ns++) {
    // toadd.pSpr[ns] = null;
    // }
    // ho.hoAdRunHeader.addBackdrop2(toadd);
    //
    // return handle;
    // }
    //
    // public void addBackdropNC(CImage cImage, short handle, int x, int y, int
    // nLayer, int typeObst, int dwEffect, int dwEffectParam) {
    // // Add paste routine
    // if ((ho.hoAdRunHeader.rhFrame.layers[0].dwOptions
    // & (CLayer.FLOPT_TOHIDE | CLayer.FLOPT_VISIBLE)) == CLayer.FLOPT_VISIBLE) {
    //// if (nLayer == 0 && (ho.hoAdRunHeader.rhFrame.layers[0].dwOptions
    //// & (CLayer.FLOPT_TOHIDE | CLayer.FLOPT_VISIBLE)) == CLayer.FLOPT_VISIBLE) {
    // CBackDrawPaste paste;
    // paste = new CBackDrawPaste();
    // paste.img = handle;
    // paste.x = x;
    // paste.y = y;
    // paste.typeObst = (short) typeObst;
    // paste.inkEffect = dwEffect;
    // paste.inkEffectParam = dwEffectParam;
    // ho.hoAdRunHeader.addBackDrawRoutine(paste);
    //
    // // Redraw sprites that intersect with the rectangle
    // CRect rc = new CRect();
    // rc.left = x - ho.hoAdRunHeader.rhWindowX;
    // rc.top = y - ho.hoAdRunHeader.rhWindowY;
    // rc.right = rc.left + cImage.getWidth();
    // rc.bottom = rc.top + cImage.getHeight();
    // ho.hoAdRunHeader.spriteGen.activeSprite(null, CSpriteGen.AS_REDRAW_RECT, rc);
    // }
    // }

    public void addBackdrop(CImage cImage, int x, int y, int nLayer, int typeObst, int dwEffect, int dwEffectParam) {
        // Duplique
        int[] mImage = cImage.getRawPixels();

        if (mImage == null) {
            return;
        }

        Bitmap bImage = Bitmap.createBitmap(cImage.getWidth(), cImage.getHeight(), cImage.getFormat());
        bImage.setPixels(mImage, 0, cImage.getWidth(), 0, 0, cImage.getWidth(), cImage.getHeight());

        short handle = ho.hoAdRunHeader.rhApp.imageBank.addImage(bImage, (short) cImage.getXSpot(),
                (short) cImage.getYSpot(), (short) cImage.getXAP(), (short) cImage.getYAP(), true);

        // Ajoute a la liste
        CBkd2 toadd = new CBkd2();
        toadd.img = handle;
        toadd.loHnd = 0;
        toadd.oiHnd = 0;
        toadd.x = x;
        toadd.y = y;
        toadd.width = cImage.getWidth();
        toadd.height = cImage.getHeight();
        toadd.nLayer = (short) nLayer;
        toadd.inkEffect = dwEffect;
        toadd.inkEffectParam = dwEffectParam;
        toadd.colMode = CSpriteGen.CM_BITMAP;
        toadd.obstacleType = (short) typeObst; // a voir
        for (int ns = 0; ns < 4; ns++) {
            toadd.pSpr[ns] = null;
        }
        ho.hoAdRunHeader.addBackdrop2(toadd);

        // Add paste routine
        if ((ho.hoAdRunHeader.rhFrame.layers[0].dwOptions
                & (CLayer.FLOPT_TOHIDE | CLayer.FLOPT_VISIBLE)) == CLayer.FLOPT_VISIBLE) {
            // if (nLayer == 0 && (ho.hoAdRunHeader.rhFrame.layers[0].dwOptions
            // & (CLayer.FLOPT_TOHIDE | CLayer.FLOPT_VISIBLE)) == CLayer.FLOPT_VISIBLE) {
            CBackDrawPaste paste;
            paste = new CBackDrawPaste();
            paste.img = handle;
            paste.x = x;
            paste.y = y;
            paste.typeObst = (short) typeObst;
            paste.inkEffect = dwEffect;
            paste.inkEffectParam = dwEffectParam;
            ho.hoAdRunHeader.addBackDrawRoutine(paste);

            // Redraw sprites that intersect with the rectangle
            CRect rc = new CRect();
            rc.left = x - ho.hoAdRunHeader.rhWindowX;
            rc.top = y - ho.hoAdRunHeader.rhWindowY;
            rc.right = rc.left + cImage.getWidth();
            rc.bottom = rc.top + cImage.getHeight();
            ho.hoAdRunHeader.spriteGen.activeSprite(null, CSpriteGen.AS_REDRAW_RECT, rc);
        }
    }
}
