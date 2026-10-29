package Extensions;
import Actions.CActExtension;
import Conditions.CCndExtension;
import Expressions.CValue;
import Services.CBinaryFile;
import java.util.*;

/** Real extension and JNI facade, with explicit runtime API doubles only at the Fusion boundary. */
public final class AdapterDispatchTest {
 private static int count;
 private static void check(boolean value,String name){count++;if(!value)throw new AssertionError(name);}
 private static void exp(CRunFindTheWay ext,Object... values){
  for(Object value:values)ext.ho.params.add(value instanceof String?new CValue((String)value):new CValue(((Number)value).intValue()));
 }
 public static void main(String[] args){
  CRunFindTheWay ext=new CRunFindTheWay();
  check(ext.createRunObject(new CBinaryFile(),null,0),"creation");
  check(ext.getNumberOfConditions()==29,"condition count includes ID28");
  check(!ext.condition(7,new CCndExtension()),"initial map unavailable");
  exp(ext,1,2,0);check(ext.expression(6).getInt()==Integer.MAX_VALUE,"missing real-coordinate sentinel");
  exp(ext,0,0,0,0);check(ext.expression(8).getInt()==255,"missing cost sentinel");
  check(ext.condition(0,new CCndExtension(3,2)),"create by size");
  ext.action(0,new CActExtension(1,0,255,1,0));
  exp(ext,1,0,0,0);check(ext.expression(8).getInt()==255,"terrain to combined map");
  int[] iterations={0};
  ext.ho.listener=id->{
   if(id==28){
    check(ext.condition(28,new CCndExtension("map")),"named map condition");
    int i=iterations[0]++;
    check(ext.expression(0).getInt()==i%3 && ext.expression(1).getInt()==i/3,"row-major iteration coords");
   }
  };
  ext.action(20,new CActExtension(0,"map"));check(iterations[0]==6,"new map action ID20");
  check(ext.condition(4,new CCndExtension(0,0,2,1,9,0,1,0,0,"saved")),"path condition");
  check(ext.condition(5,new CCndExtension("")),"empty path selects last result");
  exp(ext,"saved");int length=ext.expression(3).getInt();check(length==4,"saved path expression");
  int[] steps={0};
  ext.ho.listener=id->{if(id==6){check(ext.condition(6,new CCndExtension("path")),"path callback name");steps[0]++;}};
  ext.action(2,new CActExtension("saved",0,"path"));check(steps[0]==length,"path snapshot iteration");
  int[] outer={0},inner={0};
  ext.ho.listener=id->{
   if(id==28){
    if(ext.condition(28,new CCndExtension("outer"))){
     int x=ext.itCoord.x,y=ext.itCoord.y,index=ext.itIndex;
     if(outer[0]++==0)ext.action(20,new CActExtension(0,"inner"));
     check(ext.condition(28,new CCndExtension("outer")),"nested name restored");
     check(ext.itCoord.x==x&&ext.itCoord.y==y&&ext.itIndex==index,"nested coordinate restored");
    }else if(ext.condition(28,new CCndExtension("inner")))inner[0]++;
   }
  };
  ext.action(20,new CActExtension(0,"outer"));check(outer[0]==6&&inner[0]==6,"nested map iteration");
  ext.ho.listener=id->{};
  for(int id:new int[]{12,26,27}){
   CCndExtension c=id==12?new CCndExtension("image.png",16,0,0):id==26?new CCndExtension(1,0,16,0,0):new CCndExtension(0,16,0,0);
   check(!ext.condition(id,c),"unsupported texture condition returns false");
   check(c.reads==(id==26?5:4),"unsupported texture condition consumes parameters");
   check(!ext.condition(7,new CCndExtension()),"failed map creation clears previous map");
  }
  int[] collision={0};
  ext.ho.listener=id->{if(id==3){
   check(ext.condition(3,new CCndExtension("collision")),"collision event name");collision[0]++;
   ext.action(0,new CActExtension(ext.itCoord.x,ext.itCoord.y,255,1,1));
  }};
  check(ext.condition(2,new CCndExtension(16,0,0,1,1,1,"collision")),"event driven collision map");
  check(collision[0]==4,"four collision cells");
  ext.ho.listener=id->{};
  exp(ext,0,0,0,0);check(ext.expression(8).getInt()==255,"collision callback cost write");
  check(!ext.condition(0,new CCndExtension(0,1)),"invalid dimensions fail condition");
  check(ext.condition(0,new CCndExtension(2,2)),"recreate");
  _DeLib.FindTheWayClass map=ext.pFTW;
  ext.destroyRunObject(false);ext.destroyRunObject(false);
  check(ext.pFTW==null,"extension destruction clears handle");
  boolean closed=false;try{map.GetMap();}catch(IllegalStateException e){closed=true;}
  check(closed,"extension destruction releases native resource");
  System.out.println("PASS: "+count+" Fusion adapter checks (runtime API fixture)");
 }
}
