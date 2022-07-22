package Extensions;

import Actions.CActExtension;
import Conditions.CCndExtension;
import Expressions.CValue;
import RunLoop.CCreateObjectInfo;
import Services.CBinaryFile;
import androidx.annotation.NonNull;

import static _DeLib.FusionUtilities.*;

public class CRunAndroidUtilities extends CRunExtension {
    // Define ACE ID here
    // Condition
    final int CND_ISEMBED = 0;

    final int CND_LAST = 1;

    // Action
    final int ACT_BINARYFILEEXTRACT_SETTINGS = 0;
    final int ACT_EXTRACTBINARYFILE = 1;

    // Expression
    final int EXP_GETBINARYFILEPATH = 0;

    // Expression return value
    CValue expRet;

    // Binary file settings
    String BinaryFileExtractDir = "";
    String BinaryFileExtractName = "";

    // Constructor
    public CRunAndroidUtilities() {
        expRet = new CValue(0);
    }

    // Fusion Funcs
    @Override
    public int getNumberOfConditions() {
        return CND_LAST;
    }

    @Override
    public boolean createRunObject(@NonNull final CBinaryFile file, final CCreateObjectInfo cob, final int version) {
        // Read Properties
        // file.readByte();
        // file.readInt();
        // file.readString(260);
        // ......

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
            case CND_ISEMBED: {
                return IsEmbed(ho, cnd.getParamExpString(rh, 0));
            }
        }

        return false;
    }

    // Action Jump Table
    @Override
    public void action(int num, CActExtension act) {
        switch (num) {
            case ACT_BINARYFILEEXTRACT_SETTINGS: {
                this.BinaryFileExtractDir = act.getParamExpString(rh, 0);
                this.BinaryFileExtractName = act.getParamExpString(rh, 1);

                break;
            }
            case ACT_EXTRACTBINARYFILE: {
                ExtractEmbedFile(ho, act.getParamExpString(rh, 0), BinaryFileExtractDir, BinaryFileExtractName);

                break;
            }
        }
    }

    // Expression Jump Table
    @Override
    public CValue expression(int num) {
        switch (num) {
            case EXP_GETBINARYFILEPATH: {
                expRet.forceString("Not Binary File");

                String SrcPath = ho.getExpParam().getString();
                if (IsEmbed(ho, SrcPath)) {
                    GetExtractPath(ho, SrcPath, BinaryFileExtractDir, BinaryFileExtractName);
                }
                return expRet;
            }
        }

        return new CValue(0);
    }
}
