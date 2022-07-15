package _DeLib;

public class FuncParam {
    public enum DataType {
        VALUE,
        STRING,
    }

    public double val= 0.0;
    public String str="";

    public DataType type = DataType.VALUE;
    public boolean converted = false;
    public boolean isNumber = false;
    public boolean isNumberChecked = false;

    public FuncParam(float val){
        this.val=val;
        this.str="";

        this.type=DataType.VALUE;
        this.converted=false;
        this.isNumber=true;
        this.isNumberChecked=false;
    }
    public FuncParam(String str){
        this.val=0.0;
        this.str=str;

        this.type=DataType.STRING;
        this.converted=false;
        this.isNumber=false;
        this.isNumberChecked=false;
    }

    public static void Data_StoV(FuncParam data){
        if(data.type.compareTo(DataType.STRING)==0
        &&!data.converted){
            try {
                data.val = Double.parseDouble(data.str);
            }finally {
                data.val=0.0;
            }

            data.converted=true;
        }
    }
    public static void Data_VtoS(FuncParam data){
        if(data.type.compareTo(DataType.VALUE)==0
                &&!data.converted){
            data.str=String.valueOf(data.val);

            data.converted=true;
        }
    }

    boolean DataIsNum(FuncParam data){
        if(!data.isNumberChecked){
            data.isNumberChecked=true;
            try {
                Double.parseDouble(data.str);
                data.isNumber=true;
            }finally {
                data.isNumber=false;
            }
        }

        return data.isNumber;
    }
}
