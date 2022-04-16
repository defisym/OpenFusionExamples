package Extensions;

import Actions.CActExtension;
import Banks.CImage;
import Conditions.CCndExtension;
import Expressions.CValue;

import Frame.CLayer;
import OI.COCBackground;
import Objects.CExtension;
import Objects.CObject;
import RunLoop.CBackDrawPaste;
import RunLoop.CBkd2;
import RunLoop.CCreateObjectInfo;
import RunLoop.CRun;
import Services.CBinaryFile;

import Services.CRect;
import Sprites.CSpriteGen;
import _DeLib.FindTheWayClass;
import _DeLib.Coord;
import _DeLib.MapType;
import android.graphics.Bitmap;
import com.yourcompany.yourapplication.BuildConfig;
import org.jetbrains.annotations.NotNull;

import java.util.List;
import java.util.Vector;

import static RunLoop.CRun.OBSTACLE_TRANSPARENT;
import static _DeLib.FindTheWayClass.MAP_OBSTACLE;
import static _DeLib.FusionCommon.IDENTIFIER_ACTIVE;

public class CRunFindTheWay extends CRunExtension {
    // TODO DEBUG
    // private static final boolean DEBUG = true;
    private static final boolean DEBUG = false;

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

    // ObjectSelection* pSelect;

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
    // ObjectCreation* pOc;
    Vector<Coord> pObjZoc;
    String OnItZocName;

    // AOEClass* pAOE;
    // vector<AOEClass::coord>* pAOECoord;

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

        // FTW.ForceFind(new Coord(0,0),new Coord(0,0)
        // ,true,true
        // ,null,null,null
        // , FindTheWay.GetIgnoreFlag(true,true,true,true,true)
        // , FindTheWay::GetManhattanDistance);
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

            case CND_CONDITION_SMBC: {
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
        }

        return false;
    }

    // Action Jump Table
    @Override
    public void action(int num, CActExtension act) {
        switch (num) {
            // case ACT: {
            // // act.getParamExpression(rh, 0);
            //
            // break;
            // }

            case ACT_ACTION_C: {
                break;
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
            // case EXP: {
            // expRet.forceString("Null");
            //
            // // ho.getExpParam().getInt();
            //
            // return expRet;
            // }
        }

        return new CValue(0);
    }

    // Functions
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
