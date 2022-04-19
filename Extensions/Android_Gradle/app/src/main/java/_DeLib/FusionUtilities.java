package _DeLib;

import Banks.CImage;
import Objects.CExtension;
import Objects.CObject;
import RunLoop.CRun;
import android.graphics.Bitmap;

public class FusionUtilities {
    public static boolean LPROValid(CObject object, int identifier) {
        if (object == null) {
            return false;
        }

        if (identifier != 0) {
            return object.hoIdentifier == identifier;
        }

        return true; // need not to check identifier
    }

    public static int FixedVal(CObject object){
        return (object.hoCreationId<<16)|( ((int)object.hoNumber)&0xFFFF );
    }

    public static CObject LproFromFixed(CExtension rhPtr, int fixed){
        CObject[] ObjectList= rhPtr.hoAdRunHeader.rhObjectList;
        int id=0x0000FFFF & fixed;

        return ObjectList[id];
    }

    public static Bitmap GenerateBitmapFromHandle(CExtension rhPtr,short hImage){
        CImage cImage = rhPtr.getImageBank().getImageFromHandle(hImage);

        return GenerateBitmap(cImage);
    }

    public static Bitmap GenerateBitmap(CImage cImage){
        int[] mImage = cImage.getRawPixels();
        Bitmap.Config conf=cImage.getFormat();

        Bitmap image = Bitmap.createBitmap(cImage.getWidth(), cImage.getHeight(), conf);
        image.setPixels(mImage, 0, cImage.getWidth(), 0, 0, cImage.getWidth(), cImage.getHeight());

        return image;
    }

    public enum ColorType{
        R,
        G,
        B,
        A
    }
    public static int GetColor(Bitmap.Config conf,ColorType type,int color){
        switch (conf){
            case ALPHA_8:{
                return color;
            }
            case RGB_565:{
                switch (type){
                    case R:{
                        return (color>>11)&0x1F;
                    }
                    case G:{
                        return (color>>5)&0x3F;
                    }
                    case B:{
                        return color&0x1F;
                    }
                }
            }
            case ARGB_8888:{
                switch (type){
                    case R:{
                        return (color)&0xFF;
                    }
                    case G:{
                        return (color>>8)&0xFF;
                    }
                    case B:{
                        return (color>>16)&0xFF;
                    }
                    case A:{
                        return (color>>24)&0xFF;
                    }
                }
            }
//            case RGBA_F16:{
//                switch (type){
//                    case R:{
//                        return color&0xFFFF;
//                    }
//                    case G:{
//                        return (color>>16)&0xFFFF;
//                    }
//                    case B:{
//                        return (color>>32)&0xFFFF;
//                    }
//                    case A:{
//                        return (color>>48)&0xFFFF;
//                    }
//                }
//            }
            case HARDWARE:{
                return -1;
            }
        }

        return -1;
    }
}
