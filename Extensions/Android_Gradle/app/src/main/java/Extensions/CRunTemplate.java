package Extensions;

import Actions.CActExtension;
import Conditions.CCndExtension;
import Expressions.CValue;

import RunLoop.CCreateObjectInfo;
import Services.CBinaryFile;

import org.jetbrains.annotations.NotNull;

public class CRunTemplate extends CRunExtension {
    // Define ACE ID here
    // Condition
    final int CND = 0;

    final int CND_LAST = 1;

    // Action
    final int ACT = 0;

    // Expression
    final int EXP = 0;

    // Expression return value
    CValue expRet;

    // Constructor
    public CRunTemplate() {
        expRet = new CValue(0);
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
            case CND: {
                // cnd.getParamExpString(rh, 0);

                return true;
            }
        }

        return false;
    }

    // Action Jump Table
    @Override
    public void action(int num, CActExtension act) {
        switch (num) {
            case ACT: {
                // act.getParamExpression(rh, 0);

                break;
            }
        }
    }

    // Expression Jump Table
    @Override
    public CValue expression(int num) {
        switch (num) {
            case EXP: {
                expRet.forceString("Null");

                // ho.getExpParam().getInt();

                return expRet;
            }
        }

        return new CValue(0);
    }
}
