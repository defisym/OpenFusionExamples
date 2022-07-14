package Extensions;

import Actions.CActExtension;
import Conditions.CCndExtension;
import Expressions.CValue;

import RunLoop.CCreateObjectInfo;
import Services.CBinaryFile;

import org.jetbrains.annotations.NotNull;

public class CRunFunc extends CRunExtension {
    // Define ACE ID here
    // Condition
    final int CND_CONDITION_OF = 0;

    final int CND_CONDITION_FHPA = 1;
    final int CND_CONDITION_FHRA = 2;

    final int CND_CONDITION_FHTP = 3;
    final int CND_CONDITION_CFHTP = 4;

    final int CND_CONDITION_FPAIN = 5;
    final int CND_CONDITION_FRAIN = 6;

    final int CND_CONDITION_FTPIN = 7;
    final int CND_CONDITION_CFTPIN = 8;

    final int CND_CONDITION_OITO = 9;
    final int CND_CONDITION_SA = 10;

    final int CND_LAST = 11;

    // Action
    final int ACT_ACTION_SR = 0;
    final int ACT_ACTION_PR = 1;

    final int ACT_ACTION_CF = 2;

    final int ACT_ACTION_T = 3;

    final int ACT_ACTION_STV = 4;
    final int ACT_ACTION_SCTV = 5;

    final int ACT_ACTION_SAR = 6;

    final int ACT_ACTION_SLI = 7;
    final int ACT_ACTION_SL = 8;

    final int ACT_ACTION_PASSRET = 9;
    final int ACT_ACTION_PASSRETV = 10;

    final int ACT_ACTION_STVV = 11;
    final int ACT_ACTION_SCTVV = 12;

    final int ACT_ACTION_SRV = 13;
    final int ACT_ACTION_PRV = 14;

    final int ACT_ACTION_ITO = 15;

    final int ACT_ACTION_SSGP = 16;
    final int ACT_ACTION_SVGP = 17;

    // Expression
    final int EXP_EXPRESSION_RV = 0;
    final int EXP_EXPRESSION_RS = 1;

    final int EXP_EXPRESSION_GVP = 2;
    final int EXP_EXPRESSION_GSP = 3;

    final int EXP_EXPRESSION_GVR = 4;
    final int EXP_EXPRESSION_GSR = 5;

    final int EXP_EXPRESSION_GRI = 6;

    final int EXP_EXPRESSION_GPS = 7;
    final int EXP_EXPRESSION_GRS = 8;

    final int EXP_EXPRESSION_TRV = 9;
    final int EXP_EXPRESSION_TRS = 10;

    final int EXP_EXPRESSION_GVTP = 11;
    final int EXP_EXPRESSION_GSTP = 12;

    final int EXP_EXPRESSION_GCFN = 13;

    final int EXP_EXPRESSION_GVCTP = 14;
    final int EXP_EXPRESSION_GSCTP = 15;

    final int EXP_EXPRESSION_GLI = 16;

    final int EXP_EXPRESSION_GVGP = 17;
    final int EXP_EXPRESSION_GSGP = 18;

    // Expression return value
    CValue expRet;

    // Constructor
    public CRunFunc() {
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
            case CND_CONDITION_OF: {
                // cnd.getParamExpString(rh, 0);

                return true;
            }
            case CND_CONDITION_FHPA: {
                return true;
            }
            case CND_CONDITION_FHRA: {
                return true;
            }
            case CND_CONDITION_FHTP: {
                return true;
            }
            case CND_CONDITION_CFHTP: {
                return true;
            }
            case CND_CONDITION_FPAIN: {
                return true;
            }
            case CND_CONDITION_FRAIN: {
                return true;
            }
            case CND_CONDITION_FTPIN: {
                return true;
            }
            case CND_CONDITION_CFTPIN: {
                return true;
            }
            case CND_CONDITION_OITO: {
                return true;
            }
            case CND_CONDITION_SA: {
                return true;
            }
        }

        return false;
    }

    // Action Jump Table
    @Override
    public void action(int num, CActExtension act) {
        switch (num) {
            case ACT_ACTION_SR: {
                // act.getParamExpression(rh, 0);

                break;
            }
            case ACT_ACTION_PR: {
                break;
            }
            case ACT_ACTION_CF: {
                break;
            }
            case ACT_ACTION_T: {
                break;
            }
            case ACT_ACTION_STV: {
                break;
            }
            case ACT_ACTION_SCTV: {
                break;
            }
            case ACT_ACTION_SAR: {
                break;
            }
            case ACT_ACTION_SLI: {
                break;
            }
            case ACT_ACTION_SL: {
                break;
            }
            case ACT_ACTION_PASSRET: {
                break;
            }
            case ACT_ACTION_PASSRETV: {
                break;
            }
            case ACT_ACTION_STVV: {
                break;
            }
            case ACT_ACTION_SCTVV: {
                break;
            }
            case ACT_ACTION_SRV: {
                break;
            }
            case ACT_ACTION_PRV: {
                break;
            }
            case ACT_ACTION_ITO: {
                break;
            }
            case ACT_ACTION_SSGP: {
                break;
            }
            case ACT_ACTION_SVGP: {
                break;
            }
        }
    }

    // Expression Jump Table
    @Override
    public CValue expression(int num) {
        switch (num) {
            case EXP_EXPRESSION_RV: {
                return expRet;
            }
            case EXP_EXPRESSION_RS: {
                expRet.forceString("Null");

                // ho.getExpParam().getInt();

                return expRet;
            }
            case EXP_EXPRESSION_GVP: {
                return expRet;
            }
            case EXP_EXPRESSION_GSP: {
                return expRet;
            }
            case EXP_EXPRESSION_GVR: {
                return expRet;
            }
            case EXP_EXPRESSION_GSR: {
                return expRet;
            }
            case EXP_EXPRESSION_GRI: {
                return expRet;
            }
            case EXP_EXPRESSION_GPS: {
                return expRet;
            }
            case EXP_EXPRESSION_GRS: {
                return expRet;
            }
            case EXP_EXPRESSION_TRV: {
                return expRet;
            }
            case EXP_EXPRESSION_TRS: {
                return expRet;
            }
            case EXP_EXPRESSION_GVTP: {
                return expRet;
            }
            case EXP_EXPRESSION_GSTP: {
                return expRet;
            }
            case EXP_EXPRESSION_GCFN: {
                return expRet;
            }
            case EXP_EXPRESSION_GVCTP: {
                return expRet;
            }
            case EXP_EXPRESSION_GSCTP: {
                return expRet;
            }
            case EXP_EXPRESSION_GLI: {
                return expRet;
            }
            case EXP_EXPRESSION_GVGP: {
                return expRet;
            }
            case EXP_EXPRESSION_GSGP: {
                return expRet;
            }
        }

        return new CValue(0);
    }
}
