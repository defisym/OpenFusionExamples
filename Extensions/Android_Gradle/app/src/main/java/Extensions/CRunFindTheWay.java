package Extensions;

import Actions.CActExtension;
import Banks.CImage;
import Conditions.CCndExtension;
import Expressions.CValue;

import Objects.CExtension;
import Objects.CObject;
import Params.*;
import RunLoop.CCreateObjectInfo;
import Services.CBinaryFile;

import Services.CRect;
import android.graphics.Bitmap;

import android.graphics.Color;
import org.jetbrains.annotations.NotNull;

import java.util.Vector;
import java.util.Objects;
import java.util.function.Consumer;
import java.util.function.BiPredicate;

import _DeLib.*;
import _DeLib.Coord.*;
import _3rdLib.ObjectCreationClass;
import _3rdLib.ObjectSelectionClass;

import static _DeLib.Coord.VecContains;
import static _DeLib.FindTheWayClass.*;
import static _DeLib.FusionUtilities.*;
import static _3rdLib.ObjectSelectionClass.GetOil;
import static _3rdLib.ObjectCreationClass.GetEvtParam;
import static RunLoop.CRun.OBSTACLE_TRANSPARENT;

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
    Vector<Coord> pAOECoord;

    final int ALL = -1;
    final int ALLY = 0;
    final int ENEMY = 1;
    final int UNIT = 255;

    // Constructor
    public CRunFindTheWay() {
        expRet = new CValue(0);
        itCoord = new Coord(0, 0);

        pZoc = new Vector<Coord>();
        pAlly = new Vector<Coord>();
        pEnemy = new Vector<Coord>();
        pUnit = new Vector<Coord>();

        pObjZoc = new Vector<Coord>();

        pAOE = new AOEClass();
        pAOECoord = new Vector<Coord>();
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
                    this.pFTW.SetUpdateMapCallBack(this::UpdateMapCallBackFunc, ho);
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
                    this.pFTW.SetUpdateMapCallBack(this::UpdateMapCallBackFunc, ho);
                    this.pFTW.SetIsometric(this.isometric);
                } catch (Throwable e) {
                    return false;
                }

                if (DEBUG) {
                    String output = this.pFTW.OutPutMapStr(MapType.MAP, false, null);
                }

                return true;
            }
            case CND_CONDITION_SMBA: {
                this.pFTW = null;

                int fixed = cnd.getParamExpression(rh, 0);

                int frame = cnd.getParamExpression(rh, 1);

                int gridSize = cnd.getParamExpression(rh, 2);
                int gridOffsetX = cnd.getParamExpression(rh, 3);
                int gridOffsetY = cnd.getParamExpression(rh, 4);

                CObject object = LproFromFixed(ho, fixed);

                if (!LPROValid(object, IDENTIFIER_ACTIVE)) {
                    // must be active object
                    return false;
                }

                short nFrame = object.roa.raAnimDirOffset.adNumberOfFrame;

                if (frame >= (int) nFrame) {
                    return false;
                }

                short hImage = object.roa.raAnimDirOffset.adFrames[0];
                Bitmap image = GenerateBitmapFromHandle(ho, hImage);

                int picWidth = image.getWidth();
                int picHeight = image.getHeight();

                int width = FindTheWayClass.CalcMapWidth(picWidth, gridSize, false);
                int height = FindTheWayClass.CalcMapHeight(picHeight, gridSize, false);

                try {
                    this.pFTW = new FindTheWayClass(width, height);
                    this.pFTW.SetUpdateMapCallBack(this::UpdateMapCallBackFunc, ho);
                    this.pFTW.SetIsometric(false);
                } catch (Throwable e) {
                    return false;
                }

                this.pFTW.SetGridSize(gridSize, gridOffsetX, gridOffsetY);

                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        Coord realCoord = this.pFTW.GetRealCoord(new Coord(x, y));

                        int color = image.getPixel(realCoord.x, realCoord.y);

                        int costT = Color.red(color);
                        int costD = Color.green(color);

                        this.pFTW.SetMap(x, y, costT, MapType.TERRAIN, true);
                        this.pFTW.SetMap(x, y, costD, MapType.DYNAMIC, true);
                    }
                }

                this.pFTW.SetIsometric(this.isometric);

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
                    this.pFTW.SetUpdateMapCallBack(this::UpdateMapCallBackFunc, ho);
                    this.pFTW.SetIsometric(this.isometric);
                } catch (Throwable e) {
                    return false;
                }

                this.pFTW.SetGridSize(gridSize, gridOffsetX, gridOffsetY);

                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        this.itCoord = this.pFTW.GetRealCoord(new Coord(x, y));

                        if (eventIterate) {
                            CallEvent(ho, CND_CONDITION_OSMBC);
                        }
                        // Ref:
                        // https://community.clickteam.com/threads/59029-Accessing-backdrops-from-extensions
                        else if (!colMaskTestPoint(ho, this.itCoord.x, this.itCoord.y, baseLayer, 0)) {
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
            case CND_CONDITION_OAG: {
                boolean negated = ObjectSelectionClass.IsNegated(cnd);

                short oil = GetOil(cnd, rh, 0);
                int x = cnd.getParamExpression(rh, 1);
                int y = cnd.getParamExpression(rh, 2);

                if (this.pFTW == null) {
                    return false;
                }

                return this.pSelect.FilterObjects(this, oil, negated, new ObjectSelectionClass.FilterClass() {
                    @Override
                    public boolean Filter(Object rdPtr, CObject object) {
                        if (object.hoX < 0 || object.hoY < 0) {
                            return false;
                        } else {
                            Coord coord = new Coord(x, y);
                            Coord gridCoord = CRunFindTheWay.this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY));

                            return coord
                                    .isEqual(gridCoord);
                        }
                    }
                });
            }
            case CND_CONDITION_NOAG: {
                boolean negated = ObjectSelectionClass.IsNegated(cnd);

                short oil = GetOil(cnd, rh, 0);
                int x = cnd.getParamExpression(rh, 1);
                int y = cnd.getParamExpression(rh, 2);

                if (this.pFTW == null) {
                    return false;
                }

                try {
                    this.pSelect.ForEach(oil, new ObjectSelectionClass.ForEachCallBackClass() {
                        @Override
                        public void ForEachCallBack(CObject object) {
                            Coord objectCoord = CRunFindTheWay.this.pFTW
                                    .GetGridCoord(new Coord(object.hoX, object.hoY));

                            if (objectCoord.isEqual(new Coord(x, y))) {
                                throw new RuntimeException("Object found");
                            }
                        }
                    });
                } catch (Exception e) {
                    return false;
                }

                return true;
            }
            case CND_CONDITION_OAO: {
                boolean negated = ObjectSelectionClass.IsNegated(cnd);

                short oil = GetOil(cnd, rh, 0);
                MapType type = MapType.values()[cnd.getParamExpression(rh, 1)];

                if (this.pFTW == null) {
                    return false;
                }

                return this.pSelect.FilterObjects(this, oil, negated, new ObjectSelectionClass.FilterClass() {
                    @Override
                    public boolean Filter(Object rdPtr, CObject object) {
                        if (object.hoX < 0 || object.hoY < 0) {
                            return false;
                        } else {
                            Coord coord = CRunFindTheWay.this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY));

                            return CRunFindTheWay.this.pFTW.GetMap(coord.x, coord.y, type, true) == MAP_OBSTACLE;
                        }
                    }
                });
            }
            case CND_CONDITION_OAC: {
                boolean negated = ObjectSelectionClass.IsNegated(cnd);

                short oil = GetOil(cnd, rh, 0);
                int x = cnd.getParamExpression(rh, 1);
                int y = cnd.getParamExpression(rh, 2);

                if (this.pFTW == null) {
                    return false;
                }

                return this.pSelect.FilterObjects(this, oil, negated, new ObjectSelectionClass.FilterClass() {
                    @Override
                    public boolean Filter(Object rdPtr, CObject object) {
                        return object.hoX == x && object.hoY == y;
                    }
                });
            }
            case CND_CONDITION_NOAC: {
                boolean negated = ObjectSelectionClass.IsNegated(cnd);

                short oil = GetOil(cnd, rh, 0);
                int x = cnd.getParamExpression(rh, 1);
                int y = cnd.getParamExpression(rh, 2);

                if (this.pFTW == null) {
                    return false;
                }

                try {
                    this.pSelect.ForEach(oil, new ObjectSelectionClass.ForEachCallBackClass() {
                        @Override
                        public void ForEachCallBack(CObject object) {
                            if (object.hoX == x && object.hoY == y) {
                                throw new RuntimeException("Object found");
                            }
                        }
                    });
                } catch (RuntimeException e) {
                    return false;
                }

                return true;
            }
            case CND_CONDITION_CA: {
                boolean lineMode = (cnd.getParamExpression(rh, 0) != 0);
                boolean attack = (cnd.getParamExpression(rh, 1) != 0);

                int x = cnd.getParamExpression(rh, 2);
                int y = cnd.getParamExpression(rh, 3);

                boolean useRealCoord = (cnd.getParamExpression(rh, 4) != 0);

                int range = cnd.getParamExpression(rh, 5);
                int startRange = cnd.getParamExpression(rh, 6);

                int ignoreFlag = cnd.getParamExpression(rh, 7);

                boolean allRange = (cnd.getParamExpression(rh, 8) != 0);
                int allRangeAttackRange = cnd.getParamExpression(rh, 9);

                if (this.pFTW == null) {
                    return false;
                }

                this.isAttack = allRange ? false : attack;

                this.areaPos = 0;
                this.areaSize = 0;
                this.extraRangeStartPos = DEFAULT_EXTRARANGESTARTPOS;

                Coord gridCoord = new Coord(x, y);

                if (useRealCoord) {
                    gridCoord = this.pFTW.GetGridCoord(gridCoord);
                }

                if (!lineMode) {
                    this.pFTW.CalcArea(gridCoord, range, startRange, this.pAlly, this.pEnemy, this.pZoc, ignoreFlag,
                            attack, allRange, allRangeAttackRange, new UpdateCallBack<Integer>() {
                                @Override
                                public void CallBack(Integer v) {
                                    CRunFindTheWay.this.extraRangeStartPos = v;
                                }
                            }, false);
                } else {
                    this.pFTW.CalcLineArea(gridCoord, range, startRange, this.pAlly, this.pEnemy, this.pZoc, ignoreFlag,
                            attack);
                }

                this.areaSize = this.pFTW.GetArea().size();

                return true;
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
            case CND_CONDITION_ZV: {
                boolean negated = ObjectSelectionClass.IsNegated(cnd);

                short oil = GetOil(cnd, rh, 0);

                if (this.pFTW == null) {
                    return false;
                }

                Vector<CObject> objects = new Vector<>();

                this.pSelect.SelectAll(oil);
                objects.ensureCapacity(this.pSelect.GetNumberOfSelected(oil));

                this.pSelect.ForEach(oil, new ObjectSelectionClass.ForEachCallBackClass() {
                    @Override
                    public void ForEachCallBack(CObject object) {
                        Coord objectCoord = CRunFindTheWay.this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY));
                        // not overlap obstacle
                        if (CRunFindTheWay.this.pFTW.GetMap(objectCoord.x, objectCoord.y, MapType.TERRAIN,
                                true) != MAP_OBSTACLE
                                // not overlap unit
                                && !OverlapUnit(objectCoord)) {
                            objects.add(object);
                        }
                    }
                });

                this.pSelect.SelectObjects(oil, objects);

                return !objects.isEmpty();
            }
            case CND_CONDITION_ZAA: {
                boolean negated = ObjectSelectionClass.IsNegated(cnd);

                short oil = GetOil(cnd, rh, 0);
                int mode = cnd.getParamExpression(rh, 1);

                boolean atAttack = mode == 1; // 0 = Move Area, 1 = Attack Area
                boolean all = mode == -1;

                if (this.pFTW == null) {
                    return false;
                }

                Vector<Vector<Coord>> area = this.pFTW.GetArea();

                return this.pSelect.FilterObjects(this, oil, negated, new ObjectSelectionClass.FilterClass() {
                    @Override
                    public boolean Filter(Object rdPtr, CObject object) {
                        Coord objectCoord = CRunFindTheWay.this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY));

                        for (int it = 0; it < area.size(); it++) {
                            boolean isAttack = !(it < CRunFindTheWay.this.extraRangeStartPos);

                            if (VecContains(area.get(it), objectCoord)) {
                                return all
                                        || (atAttack && isAttack)
                                        || (!atAttack && !isAttack);
                            }
                        }

                        return false;
                    }
                });
            }
            case CND_CONDITION_OCOZ: {
                boolean negated = ObjectSelectionClass.IsNegated(cnd);

                short oil = GetOil(cnd, rh, 0);
                short oilZoc = GetOil(cnd, rh, 1);

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
            case CND_CONDITION_SA: {
                boolean negated = ObjectSelectionClass.IsNegated(cnd);

                short oil = GetOil(cnd, rh, 0);

                this.pSelect.SelectAll(oil);

                return true;
            }
            case CND_CONDITION_OAOBJ: {
                boolean negated = ObjectSelectionClass.IsNegated(cnd);

                short oilObjA = GetOil(cnd, rh, 0);
                short oilObjB = GetOil(cnd, rh, 1);

                if (this.pFTW == null) {
                    return false;
                }

                Vector<Coord> objects = new Vector<>();

                this.pSelect.ForEach(oilObjB, new ObjectSelectionClass.ForEachCallBackClass() {
                    @Override
                    public void ForEachCallBack(CObject object) {
                        objects.add(CRunFindTheWay.this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY)));
                    }
                });

                return this.pSelect.FilterObjects(this, oilObjA, negated, new ObjectSelectionClass.FilterClass() {
                    @Override
                    public boolean Filter(Object rdPtr, CObject object) {
                        return VecContains(objects,
                                CRunFindTheWay.this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY)));
                    }
                });
            }
            case CND_CONDITION_POAO: {
                boolean negated = ObjectSelectionClass.IsNegated(cnd);

                short oil = GetOil(cnd, rh, 0);

                if (this.pFTW == null) {
                    return false;
                }

                class tuple {
                    public final Coord coord;
                    public final CObject object;

                    tuple(Coord coord, CObject object) {
                        this.coord = coord;
                        this.object = object;
                    }
                }

                Vector<tuple> objects = new Vector<>();
                Vector<tuple> selected = new Vector<>();

                Vector<CObject> toSelect = new Vector<>();

                int oNum = this.pSelect.GetNumberOfObject(oil);

                objects.ensureCapacity(oNum);
                selected.ensureCapacity(oNum);
                toSelect.ensureCapacity(oNum);

                BiPredicate<Vector<tuple>, Coord> find = (Vector<tuple> fObjects, Coord objectCoord) -> {
                    for (tuple t : fObjects) {
                        if (t.coord.isEqual(objectCoord)) {
                            return true;
                        }
                    }

                    return false;
                };

                this.pSelect.ForEach(oil, new ObjectSelectionClass.ForEachCallBackClass() {
                    @Override
                    public void ForEachCallBack(CObject object) {
                        objects.add(new tuple(new Coord(object.hoX, object.hoY), object));
                    }
                });

                this.pSelect.ForEach(oil, new ObjectSelectionClass.ForEachCallBackClass() {
                    @Override
                    public void ForEachCallBack(CObject object) {
                        Coord objectCoord = CRunFindTheWay.this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY));

                        if (!find.test(selected, objectCoord)) {
                            selected.add(new tuple(objectCoord, object));
                        }
                    }
                });

                if (!negated) {
                    for (tuple t : selected) {
                        toSelect.add(t.object);
                    }
                } else {
                    for (tuple t : objects) {
                        if (!find.test(selected, t.coord)) {
                            toSelect.add(t.object);
                        }
                    }
                }

                this.pSelect.SelectObjects(oil, toSelect);

                return !toSelect.isEmpty();
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

                    CallEvent(ho, CND_CONDITION_OITP);
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
                } else if (!VecContains(this.pZoc, objectCoord)) {
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

                        CallEvent(ho, CND_CONDITION_OITA);
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
            case ACT_ACTION_CAO: {
                PARAM_CREATE param = (PARAM_CREATE) GetEvtParam(act, rh, 0);

                if (this.pFTW == null) {
                    return;
                }

                Vector<Vector<Coord>> area = this.pFTW.GetArea();

                if (this.areaPos < area.size()) {
                    this.isAttack = !(this.areaPos + 1 < this.extraRangeStartPos);

                    for (Coord it : area.get(this.areaPos)) {
                        Coord realCoord = this.pFTW.GetRealCoord(it);

                        this.pOc.OCCreateObject(new ObjectCreationClass.CreationParamClass() {
                            @Override
                            public void CreationParam(ObjectCreationClass oc,
                                    ObjectCreationClass.CreateDuplicateParam cdp) {
                                cdp.AssignParam(rh, param);

                                cdp.x = realCoord.x;
                                cdp.y = realCoord.y;
                            }
                        });
                    }

                    this.areaPos++;
                }

                return;
            }
            case ACT_ACTION_CABNO: {
                String objectName = act.getParamExpString(rh, 0);
                short Oi = this.pOc.GetCreationOI(objectName);

                if (this.pFTW == null) {
                    return;
                }

                Vector<Vector<Coord>> area = this.pFTW.GetArea();

                if (this.areaPos < area.size()) {
                    this.isAttack = !(this.areaPos + 1 < this.extraRangeStartPos);

                    for (Coord it : area.get(this.areaPos)) {
                        Coord realCoord = this.pFTW.GetRealCoord(it);

                        this.pOc.OCCreateObject(new ObjectCreationClass.CreationParamClass() {
                            @Override
                            public void CreationParam(ObjectCreationClass oc,
                                    ObjectCreationClass.CreateDuplicateParam cdp) {
                                cdp.oi = Oi;
                                cdp.layer = 0;

                                cdp.x = realCoord.x;
                                cdp.y = realCoord.y;
                            }
                        });
                    }

                    this.areaPos++;
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
                            cdp.AssignParam(rh, param);

                            cdp.x = realCoord.x;
                            cdp.y = realCoord.y;
                        }
                    });
                }

                return;
            }
            case ACT_ACTION_COZBE: {
                short oil = GetOil(act, rh, 0);
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

                            CallEvent(ho, CND_CONDITION_OCOZ);
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
                            cdp.layer = 0;
                        }
                    });
                }

                return;

            }
            case ACT_ACTION_CAOE: {
                PARAM_CREATE param = (PARAM_CREATE) GetEvtParam(act, rh, 0);

                CObject object = act.getParamObject(rh, 1);
                int dir = act.getParamExpression(rh, 2);
                int type = act.getParamExpression(rh, 3);

                int ignoreFlag = act.getParamExpression(rh, 4);

                boolean moveIgnoreZoc = (ignoreFlag & 0b10000) != 0; // Move through zoc
                boolean moveIgnoreAlly = (ignoreFlag & 0b01000) != 0; // Move through ally
                boolean moveIgnoreEnemy = (ignoreFlag & 0b00100) != 0; // Move through enemy
                boolean attackIgnoreAlly = (ignoreFlag & 0b00010) != 0; // Attack ally (e.g., heal)
                boolean attackIgnoreEnemy = (ignoreFlag & 0b00001) != 0; // Attack enemy

                if (!LPROValid(object)) {
                    return;
                }

                if (this.pFTW == null) {
                    return;
                }

                Coord objectCoord = this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY));
                Coord start = new Coord(objectCoord.x, objectCoord.y, true);

                this.pAOE.GetAOE(start, dir, type, this.pAOECoord);

                BiPredicate<Vector<Coord>, Coord> find = Coord::VecContains;

                for (Coord it : this.pAOECoord) {
                    Coord gridCoord = new Coord(it.x, it.y);
                    Coord realCoord = this.pFTW.GetRealCoord(gridCoord);

                    if (this.pFTW.GetMap(gridCoord.x, gridCoord.y, MapType.MAP, true) == MAP_OBSTACLE) {
                        continue;
                    }

                    if (!attackIgnoreAlly && find.test(this.pAlly, gridCoord)) {
                        continue;
                    }
                    if (!attackIgnoreEnemy && find.test(this.pEnemy, gridCoord)) {
                        continue;
                    }

                    this.pOc.OCCreateObject(new ObjectCreationClass.CreationParamClass() {
                        @Override
                        public void CreationParam(ObjectCreationClass oc,
                                ObjectCreationClass.CreateDuplicateParam cdp) {
                            cdp.AssignParam(rh, param);

                            cdp.x = realCoord.x;
                            cdp.y = realCoord.y;
                        }
                    });
                }

                return;
            }
            case ACT_ACTION_CAOEBN: {
                String objectName = act.getParamExpString(rh, 0);

                CObject object = act.getParamObject(rh, 1);
                int dir = act.getParamExpression(rh, 2);
                int type = act.getParamExpression(rh, 3);

                int ignoreFlag = act.getParamExpression(rh, 4);

                boolean moveIgnoreZoc = (ignoreFlag & 0b10000) != 0; // Move through zoc
                boolean moveIgnoreAlly = (ignoreFlag & 0b01000) != 0; // Move through ally
                boolean moveIgnoreEnemy = (ignoreFlag & 0b00100) != 0; // Move through enemy
                boolean attackIgnoreAlly = (ignoreFlag & 0b00010) != 0; // Attack ally (e.g., heal)
                boolean attackIgnoreEnemy = (ignoreFlag & 0b00001) != 0; // Attack enemy

                if (!LPROValid(object)) {
                    return;
                }

                if (this.pFTW == null) {
                    return;
                }

                short Oi = this.pOc.GetCreationOI(objectName);

                Coord objectCoord = this.pFTW.GetGridCoord(new Coord(object.hoX, object.hoY));
                Coord start = new Coord(objectCoord.x, objectCoord.y, true);

                this.pAOE.GetAOE(start, dir, type, this.pAOECoord);

                BiPredicate<Vector<Coord>, Coord> find = Coord::VecContains;

                for (Coord it : this.pAOECoord) {
                    Coord gridCoord = new Coord(it.x, it.y);
                    Coord realCoord = this.pFTW.GetRealCoord(gridCoord);

                    if (this.pFTW.GetMap(gridCoord.x, gridCoord.y, MapType.MAP, true) == MAP_OBSTACLE) {
                        continue;
                    }

                    if (!attackIgnoreAlly && find.test(this.pAlly, gridCoord)) {
                        continue;
                    }
                    if (!attackIgnoreEnemy && find.test(this.pEnemy, gridCoord)) {
                        continue;
                    }

                    this.pOc.OCCreateObject(new ObjectCreationClass.CreationParamClass() {
                        @Override
                        public void CreationParam(ObjectCreationClass oc,
                                ObjectCreationClass.CreateDuplicateParam cdp) {
                            cdp.oi = Oi;
                            cdp.layer = 0;

                            cdp.x = realCoord.x;
                            cdp.y = realCoord.y;
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

                            addBackdrop(ho, cImageLT, coord.x - hoX, coord.y - hoY, nLayer, nObstacleType, effect,
                                    effectParam);

                            if (this.pFTW.GetMap(x + 1, y, MapType.MAP, true) == MAP_OBSTACLE) {
                                addBackdrop(ho, cImageR, coord.x - hoX + (x + 1 == width ? 0 : rOffset), coord.y - hoY,
                                        nLayer,
                                        nObstacleType, effect, effectParam);
                            }

                            if (this.pFTW.GetMap(x, y + 1, MapType.MAP, true) == MAP_OBSTACLE) {
                                addBackdrop(ho, cImageB, coord.x - hoX, coord.y - hoY + (y + 1 == height ? 0 : bOffset),
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

    public boolean OverlapUnit(Coord objectCoord) {
        return VecContains(this.pUnit, objectCoord);
    }

    public void UpdateMapCallBackFunc(Object ho) {
        ((CExtension) ho).generateEvent(CND_CONDITION_OMC, 0);
    }
}
