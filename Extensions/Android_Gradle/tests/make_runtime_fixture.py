"""Generate compile/test doubles, never production SDK replacements.
Only ACE dispatch, marshalling, event iteration and lifecycle are exercised here.
Backdrop calls only record anchor coordinates; no rendering is simulated.
Object selection/creation and native collision APIs deliberately fail if called.
"""
from pathlib import Path
import sys
root=Path(sys.argv[1])
files={
'androidx/annotation/NonNull.java': 'package androidx.annotation; public @interface NonNull {}',
'Expressions/CValue.java': '''package Expressions;
public class CValue {
 private int number; private String text="";
 public CValue(int value) {number=value;} public CValue(String value) {text=value;}
 public int getInt(){return number;} public String getString(){return text;}
 public void forceInt(int value){number=value;} public void forceString(String value){text=value;}
}''',
'RunLoop/CRun.java': '''package RunLoop;
public class CRun {
 public static final int OBSTACLE_TRANSPARENT=0;
 public Object rhApp=new Object(); public Frame rhFrame=new Frame();
 public static class Frame {public Services.CRect leVirtualRect=new Services.CRect();}
}''',
'RunLoop/CCreateObjectInfo.java':'package RunLoop; public class CCreateObjectInfo {}',
'Services/CRect.java':'package Services; public class CRect {public int left=0,top=0,right=32,bottom=32;}',
'Services/CBinaryFile.java':'package Services; public class CBinaryFile {public byte readByte(){return 0;}}',
'Banks/CImage.java':'package Banks; public class CImage {public int xSpot,ySpot; public int getXSpot(){return xSpot;} public int getYSpot(){return ySpot;}}',
'Objects/CObject.java':'''package Objects;
public class CObject {
 public int hoX,hoY; public Animation roa=new Animation(); public Values rov=new Values(); public Sprite ros=new Sprite();
 public static class Animation {public Direction raAnimDirOffset=new Direction();}
 public static class Direction {public short adNumberOfFrame=3; public int[] adFrames={0,1,2};}
 public static class Values {public Expressions.CValue[] rvValues={new Expressions.CValue(0),new Expressions.CValue(0)};}
 public static class Sprite {public int rsEffect,rsEffectParam;}
}''',
'Objects/CExtension.java':'''package Objects;
public class CExtension extends CObject {
 public RunLoop.CRun hoAdRunHeader;
 public java.util.function.IntConsumer listener=id->{};
 public java.util.ArrayDeque<Expressions.CValue> params=new java.util.ArrayDeque<>();
 public void generateEvent(int id,int p){listener.accept(id);}
 public Expressions.CValue getExpParam(){return params.remove();}
 public Bank imageBank=new Bank();
 public Bank getImageBank(){return imageBank;}
 public static class Bank {
  public Banks.CImage[] images={new Banks.CImage(),new Banks.CImage(),new Banks.CImage()};
  public Banks.CImage getImageFromHandle(int h){return images[h];}
 }
}''',
'Actions/CActExtension.java':'''package Actions;
public class CActExtension {
 public final Object[] params;
 public CActExtension(Object... values){params=values;}
 public int getParamExpression(RunLoop.CRun rh,int i){return ((Number)params[i]).intValue();}
 public String getParamExpString(RunLoop.CRun rh,int i){return (String)params[i];}
 public Objects.CObject getParamObject(RunLoop.CRun rh,int i){return (Objects.CObject)params[i];}
}''',
'Conditions/CCndExtension.java':'''package Conditions;
public class CCndExtension {
 public final Object[] params;
 public int reads;
 public CCndExtension(Object... values){params=values;}
 public int getParamExpression(RunLoop.CRun rh,int i){reads++;return ((Number)params[i]).intValue();}
 public String getParamExpString(RunLoop.CRun rh,int i){reads++;return (String)params[i];}
}''',
'Params/PARAM_CREATE.java': 'package Params; public class PARAM_CREATE {}',
'Extensions/CRunExtension.java':'''package Extensions;
public abstract class CRunExtension {
 public RunLoop.CRun rh=new RunLoop.CRun(); public Objects.CExtension ho=new Objects.CExtension();
 public CRunExtension(){ho.hoAdRunHeader=rh;}
 public abstract int getNumberOfConditions();
 public abstract boolean createRunObject(Services.CBinaryFile f,RunLoop.CCreateObjectInfo info,int version);
 public abstract void destroyRunObject(boolean fast);
 public abstract int handleRunObject();
 public abstract boolean condition(int n,Conditions.CCndExtension c);
 public abstract void action(int n,Actions.CActExtension a);
 public abstract Expressions.CValue expression(int n);
}''',
'_DeLib/FusionUtilities.java':'''package _DeLib;
public class FusionUtilities {
 public static final int IDENTIFIER_ACTIVE=1230131283;
 public static void CallEvent(Objects.CExtension ho,int id){ho.generateEvent(id,0);}
 public static boolean LPROValid(Objects.CObject o){return o!=null;}
 public static boolean LPROValid(Objects.CObject o,int identifier){return o!=null;}
 public static boolean colMaskTestPoint(Objects.CExtension ho,int x,int y,int layer,int plan){throw new UnsupportedOperationException("SDK collision fixture");}
 public static final java.util.List<int[]> backdrops=new java.util.ArrayList<>();
 public static void addBackdrop(Objects.CExtension ho,Banks.CImage image,int x,int y,int layer,int obstacle,int effect,int param){backdrops.add(new int[]{x,y,layer,image.getXSpot(),image.getYSpot()});}
}''',
'_3rdLib/ObjectSelectionClass.java':'''package _3rdLib;
public class ObjectSelectionClass {
 public ObjectSelectionClass(Object app){}
 public static boolean IsNegated(Conditions.CCndExtension c){return false;}
 public static short GetOil(Object evt,RunLoop.CRun rh,int i){throw new UnsupportedOperationException("SDK selection fixture");}
 public abstract static class FilterClass {public abstract boolean Filter(Object owner,Objects.CObject object);}
 public abstract static class ForEachCallBackClass {public abstract void ForEachCallBack(Objects.CObject object);}
 public boolean FilterObjects(Object owner,int oil,boolean negated,FilterClass fn){throw new UnsupportedOperationException("SDK selection fixture");}
 public void ForEach(int oil,ForEachCallBackClass fn){throw new UnsupportedOperationException("SDK selection fixture");}
 public int GetNumberOfObject(int oil){throw new UnsupportedOperationException("SDK selection fixture");}
 public int GetNumberOfSelected(int oil){throw new UnsupportedOperationException("SDK selection fixture");}
 public void SelectAll(int oil){throw new UnsupportedOperationException("SDK selection fixture");}
 public void SelectObjects(int oil,java.util.Vector<Objects.CObject> objects){throw new UnsupportedOperationException("SDK selection fixture");}
}''',
'_3rdLib/ObjectCreationClass.java':'''package _3rdLib;
public class ObjectCreationClass {
 public ObjectCreationClass(Object app){}
 public static Object GetEvtParam(Object evt,RunLoop.CRun rh,int i){throw new UnsupportedOperationException("SDK creation fixture");}
 public abstract static class CreationParamClass {public abstract void CreationParam(ObjectCreationClass oc,CreateDuplicateParam cdp);}
 public static class CreateDuplicateParam {
  public short oi;public int x,y,layer;
  public void AssignParam(RunLoop.CRun rh,Params.PARAM_CREATE param){throw new UnsupportedOperationException("SDK creation fixture");}
 }
 public void OCCreateObject(CreationParamClass fn){throw new UnsupportedOperationException("SDK creation fixture");}
 public short GetCreationOI(String name){throw new UnsupportedOperationException("SDK creation fixture");}
}'''
}
for name,text in files.items():
 p=root/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(text+'\n')
