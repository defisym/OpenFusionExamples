package _DeLib;

import Application.CEmbeddedFile;
import Banks.CImage;
import Conditions.CCndExtension;
import Events.CEvent;
import Frame.CLayer;
import Objects.CExtension;
import Objects.CObject;
import Params.CParam;
import Params.PARAM_OBJECT;
import RunLoop.CBackDrawPaste;
import RunLoop.CBkd2;
import RunLoop.CRun;
import Services.CRect;
import Sprites.CSpriteGen;
import android.graphics.Bitmap;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Objects;
import java.util.Vector;

public class FusionUtilities {
    public static final int IDENTIFIER_ACTIVE = 1230131283;
    public static final int IDENTIFIER_STRING = 1415071060;
    public static final int IDENTIFIER_COUNTER = 1381256771;
    public static final int IDENTIFIER_LIVES = 1163282764;
    public static final int IDENTIFIER_QUESTION = IDENTIFIER_STRING;
    public static final int IDENTIFIER_SCORE = 1163019091;

    // something like s1=std::move(s2) but do not reset capacity
    public static <T> void move(Vector<T> s1, Vector<T> s2) {
        s1.clear();
        s1.addAll(s2);

        s2.clear();
    }

    public static <T> void swap(Vector<T> s1, Vector<T> s2) {
        Vector<T> tmp = new Vector<>(s1);

        s1.clear();
        s1.addAll(s2);

        s2.clear();
        s2.addAll(tmp);
    }

    public static CParam GetEvtParam(CEvent evt, CRun rhPtr, int _num) {
        return evt.evtParams[_num];
    }

    public static short GetOil(CEvent evt, CRun rhPtr, int _num) {
        PARAM_OBJECT param = (PARAM_OBJECT) evt.evtParams[_num];
        return param.oiList;
    }

    public static boolean IsNegated(CCndExtension cnd) {
        return (cnd.evtFlags2 & CEvent.EVFLAG2_NOT) != 0;
    }

    public static boolean LPROValid(CObject object) {
        return LPROValid(object, 0);
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

    public static int FixedVal(CObject object) {
        return (object.hoCreationId << 16) | (((int) object.hoNumber) & 0xFFFF);
    }

    public static CObject LproFromFixed(CExtension rhPtr, int fixed) {
        CObject[] ObjectList = rhPtr.hoAdRunHeader.rhObjectList;
        int id = 0x0000FFFF & fixed;

        return ObjectList[id];
    }

    public static Bitmap GenerateBitmapFromHandle(CExtension rhPtr, short hImage) {
        CImage cImage = rhPtr.getImageBank().getImageFromHandle(hImage);

        return GenerateBitmap(cImage);
    }

    public static Bitmap GenerateBitmap(CImage cImage) {
        int[] mImage = cImage.getRawPixels();

        if (mImage == null) {
            return null;
        }

        Bitmap image = Bitmap.createBitmap(cImage.getWidth(), cImage.getHeight(), cImage.getFormat());
        image.setPixels(mImage, 0, cImage.getWidth(), 0, 0, cImage.getWidth(), cImage.getHeight());

        return image;
    }

    public static void addBackdrop(CExtension rhPtr, CImage cImage, int x, int y, int nLayer, int typeObst,
            int dwEffect, int dwEffectParam) {
        // Duplique
        Bitmap bImage = GenerateBitmap(cImage);

        short handle = rhPtr.hoAdRunHeader.rhApp.imageBank.addImage(bImage, (short) cImage.getXSpot(),
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
        rhPtr.hoAdRunHeader.addBackdrop2(toadd);

        // Add paste routine
        if ((rhPtr.hoAdRunHeader.rhFrame.layers[0].dwOptions
                & (CLayer.FLOPT_TOHIDE | CLayer.FLOPT_VISIBLE)) == CLayer.FLOPT_VISIBLE) {
            // if (nLayer == 0 && (rhPtr.hoAdRunHeader.rhFrame.layers[0].dwOptions
            // & (CLayer.FLOPT_TOHIDE | CLayer.FLOPT_VISIBLE)) == CLayer.FLOPT_VISIBLE) {
            CBackDrawPaste paste;
            paste = new CBackDrawPaste();
            paste.img = handle;
            paste.x = x;
            paste.y = y;
            paste.typeObst = (short) typeObst;
            paste.inkEffect = dwEffect;
            paste.inkEffectParam = dwEffectParam;
            rhPtr.hoAdRunHeader.addBackDrawRoutine(paste);

            // Redraw sprites that intersect with the rectangle
            CRect rc = new CRect();
            rc.left = x - rhPtr.hoAdRunHeader.rhWindowX;
            rc.top = y - rhPtr.hoAdRunHeader.rhWindowY;
            rc.right = rc.left + cImage.getWidth();
            rc.bottom = rc.top + cImage.getHeight();
            rhPtr.hoAdRunHeader.spriteGen.activeSprite(null, CSpriteGen.AS_REDRAW_RECT, rc);
        }
    }

    // false = obstacle
    public static boolean colMaskTestPoint(CExtension rhPtr, int x, int y, int layer, int plan) {
        return !rhPtr.hoAdRunHeader.rhFrame.bkdCol_TestPoint(x - rhPtr.hoAdRunHeader.rhWindowX,
                y - rhPtr.hoAdRunHeader.rhWindowY,
                layer, plan);
    }

    public static void CallEvent(CExtension rhPtr, int event) {
        rhPtr.generateEvent(event, 0);
    }

    public static void PushEvent(CExtension rhPtr, int event) {
        rhPtr.pushEvent(event, 0);
    }

    public static boolean IsEmbed(CExtension rhPtr, String FilePath) {
        return rhPtr.hoAdRunHeader.rhApp.getEmbeddedFile(FilePath) != null;
    }

    public static String GetExtractPath(CExtension rhPtr, String SrcPath, String DirName, String FileName) {
        if (Objects.equals(DirName, "")) {
            DirName = rhPtr.getControlsContext().getFilesDir().getAbsolutePath() + "/";
        }
        if (Objects.equals(FileName, "")) {
            String temp = SrcPath.trim();
            FileName = temp.substring(temp.lastIndexOf("\\") + 1);
        }

        return DirName + FileName;
    }

    public static boolean ExtractEmbedFile(CExtension rhPtr, String SrcPath, String DirName, String FileName) {
        if (!IsEmbed(rhPtr, SrcPath)) {
            return false;
        }

        CEmbeddedFile embed = rhPtr.hoAdRunHeader.rhApp.getEmbeddedFile(SrcPath);

        if (Objects.equals(DirName, "")) {
            DirName = rhPtr.getControlsContext().getFilesDir().getAbsolutePath() + "/";
        }

        try {
            // 创建文件夹
            File Dir = new File(DirName);
            // 不存在创建
            if (!Dir.exists()) {
                if (!Dir.mkdir()) {
                    throw (new Exception("Failed to create dir"));
                }
            }
            // 下载后的文件名
            if (Objects.equals(FileName, "")) {
                String temp = SrcPath.trim();
                FileName = temp.substring(temp.lastIndexOf("\\") + 1);
            }

            String fileName = DirName + FileName;
            File file = new File(fileName);
            if (file.exists()) {
                if (!file.delete()) {
                    throw (new Exception("Failed to delete file"));
                }
            }

            InputStream is = embed.getInputStream();

            // 创建字节流
            byte[] bs = new byte[1024];
            int len;
            OutputStream os = Files.newOutputStream(Paths.get(fileName));
            // 写数据
            while ((len = is.read(bs)) != -1) {
                os.write(bs, 0, len);
            }
            // 完成后关闭流
            os.close();
            is.close();

            return true;

        } catch (Exception e) {
            e.printStackTrace();

            return false;
        }
    }

    public enum ColorType {
        R,
        G,
        B,
        A
    }

    public static int GetColor(Bitmap.Config conf, ColorType type, int color) {
        switch (conf) {
            case ALPHA_8: {
                return color;
            }
            case RGB_565: {
                switch (type) {
                    case R: {
                        return (color >> 11) & 0x1F;
                    }
                    case G: {
                        return (color >> 5) & 0x3F;
                    }
                    case B: {
                        return color & 0x1F;
                    }
                    default: {
                        return -1;
                    }
                }
            }
            case ARGB_8888: {
                switch (type) {
                    case R: {
                        return (color) & 0xFF;
                    }
                    case G: {
                        return (color >> 8) & 0xFF;
                    }
                    case B: {
                        return (color >> 16) & 0xFF;
                    }
                    case A: {
                        return (color >> 24) & 0xFF;
                    }
                }
            }
            // case RGBA_F16:{
            // switch (type){
            // case R:{
            // return color&0xFFFF;
            // }
            // case G:{
            // return (color>>16)&0xFFFF;
            // }
            // case B:{
            // return (color>>32)&0xFFFF;
            // }
            // case A:{
            // return (color>>48)&0xFFFF;
            // }
            // }
            // }
            case HARDWARE: {
                return -1;
            }
        }

        return -1;
    }
}
