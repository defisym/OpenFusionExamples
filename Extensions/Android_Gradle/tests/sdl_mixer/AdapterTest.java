package Extensions;
import _DeLib.SDLMixer;
import Runtime.MMFRuntime;
import Expressions.CValue;
import Actions.CActExtension;
import Conditions.CCndExtension;
public final class AdapterTest {
    private static void check(boolean ok,String message){if(!ok)throw new AssertionError(message);}
    private static CValue value(Object input){CValue v=new CValue(0);if(input instanceof String)v.forceString((String)input);else if(input instanceof Double)v.forceDouble((Double)input);else v.forceInt((Integer)input);return v;}
    private static void params(CRunSDL_Mixer ext,Object... args){CValue[] values=new CValue[args.length];for(int i=0;i<args.length;++i)values[i]=value(args[i]);ext.ho.setParams(values);}
    public static void main(String[] args){
        MMFRuntime.inst=new MMFRuntime();
        CRunSDL_Mixer ext=new CRunSDL_Mixer();ext.ho=new Objects.CExtension();ext.rh=new RunLoop.CRun();
        check(ext.createRunObject(null,null,1)&&ext.getNumberOfConditions()==7,"create and condition count");
        Object[][] conditions={{3,0},{},{3},{3,1},{},{3,0},{"binary"}};
        for(int id=0;id<conditions.length;++id){check(ext.condition(id,new CCndExtension(conditions[id])),"condition "+id);check(SDLMixer.op==id,"condition dispatch "+id);}
        Object[][] actions={{"tone","key",3,2,100},{"tone","key",3,2,100},{3,70,0},{3,100,1},{3,1},{3,0},{3,.375},{100},{3,.125,.625},{3,1,"{0,12}",4},{"tone","key"},{"tone"},{"tone","key"},{77},{3,255,0,0},{3,90,30,1},{3,40,0}};
        for(int id=0;id<actions.length;++id){ext.action(id,new CActExtension(actions[id]));check(SDLMixer.op==id,"action dispatch "+id);if(id==6)check(SDLMixer.a==.375,"fractional seek");if(id==8)check(SDLMixer.a==.125&&SDLMixer.b==.625,"fractional AB loop");if(id==9)check(!SDLMixer.exclusive&&SDLMixer.text.equals("{0,12}")&&SDLMixer.b==4,"score parameters");}
        Object[][] expressions={{3,0},{3,1},{3},{3},{"tone",0},{3},{"raw",100,10},{"access","binary",4,16},{"binary",4},{},{3,1},{3,0},{3,1},{3,0}};
        for(int id=0;id<expressions.length;++id){params(ext,expressions[id]);CValue result=ext.expression(id);check(ext.ho.parameters==expressions[id].length,"expression parameter consumption "+id);if(id==6)check(result.getString().isEmpty(),"raw pointer rejected");else if(id==8)check(result.getInt()==0,"native address rejected");else {check(SDLMixer.op==id,"expression dispatch "+id);if(id==2||id==3)check(result.getDouble()==id+.5,"double expression");}}
        ext.pauseRunObject();check(SDLMixer.suspended,"app pause");ext.continueRunObject();check(!SDLMixer.suspended,"app resume");ext.destroyRunObject(false);ext.destroyRunObject(false);check(!SDLMixer.closed,"instance destroy retains application playback");
        ext.rh.rhApp.appRunningState=Application.CRunApp.SL_QUIT; _DeLib.SessionsTestSweep.run(ext.rh.rhApp); check(SDLMixer.closed,"application quit releases playback");
        System.out.println("PASS: all 7 conditions, 17 actions and 14 expressions, numeric/string ABI and lifecycle dispatch");
    }
}
