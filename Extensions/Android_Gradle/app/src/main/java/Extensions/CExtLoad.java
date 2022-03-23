/* Copyright (c) 1996-2013 Clickteam
 *
 * This source code is part of the Android exporter for Clickteam Multimedia Fusion 2.
 * 
 * Permission is hereby granted to any person obtaining a legal copy 
 * of Clickteam Multimedia Fusion 2 to use or modify this source code for 
 * debugging, optimizing, or customizing applications created with 
 * Clickteam Multimedia Fusion 2.  Any other use of this source code is prohibited.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
//----------------------------------------------------------------------------------
//
// CEXTLOADER: Chargement des extensions
//
//----------------------------------------------------------------------------------
package Extensions;

import Runtime.Log;
import Runtime.MMFRuntime;
import Services.CFile;

public class CExtLoad
{
	short handle;
	String name;
	String subType;

	public CExtLoad()
	{
	}

	public void loadInfo(CFile file)
	{
		long debut = file.getFilePointer();

		short size = (short) Math.abs (file.readAShort());
		handle = file.readAShort();
		file.skipBytes(12);
		name = file.readAString();
		int pos = name.lastIndexOf('.');
		name = name.substring(0, pos);
		int index = name.indexOf('-');
		while (index > 0)
		{
			name = name.substring(0, index) + '_' + name.substring(index + 1);
			index = name.indexOf('-');
		}
		index = name.indexOf(' ');
		while (index > 0)
		{
			name = name.substring(0, index) + '_' + name.substring(index + 1);
			index = name.indexOf(' ');
		}
		subType = file.readAString();

		file.seek(debut + size);
	}

	public CRunExtension loadRunObject()
	{
		CRunExtension object=null;

		if (MMFRuntime.nativeExtensions.contains ("CRun" + name))
		{
			object = new CRunNativeExtension ("CRun" + name);
		}
		else if (name.compareToIgnoreCase("Android")==0)
		{
			object=new CRunAndroid();
		}
		else if (name.compareToIgnoreCase("OUYA")==0)
		{
			object=new CRunOUYA();
		}
		else
		{
			// STARTCUT
			// STARTCUT
			// STARTCUT
			if (name.compareToIgnoreCase("TapTap")==0)
			{
				object=new CRunTapTap();
			}


//			if (name.compareToIgnoreCase("clickteam_movement_controller")==0)
//			{
//				object=new CRunclickteam_movement_controller();
//			}
//			if (name.compareToIgnoreCase("kcarray")==0)
//			{
//				object=new CRunKcArray();
//			}
//			if (name.compareToIgnoreCase("Layer")==0)
//			{
//				object=new CRunLayer();
//			}
//
//			if (name.compareToIgnoreCase("KcBoxA")==0)
//			{
//				object=new CRunKcBoxA();
//			}
//			if (name.compareToIgnoreCase("KcBoxB")==0)
//			{
//				object=new CRunKcBoxB();
//			}
//			if (name.compareToIgnoreCase("parser")==0)
//			{
//				object=new CRunparser();
//			}
//			if (name.compareToIgnoreCase("Accelerometer")==0)
//			{
//				object=new CRunAccelerometer();
//			}
//			if (name.compareToIgnoreCase("kcbutton")==0)
//			{
//				object=new CRunKcButton();
//			}
//			if (name.compareToIgnoreCase("KcEdit")==0)
//			{
//				object=new CRunkcedit();
//			}
//			if (name.compareToIgnoreCase("KcList")==0)
//			{
//				object=new CRunkclist();
//			}
//			if (name.compareToIgnoreCase("KcCombo")==0)
//			{
//				object=new CRunkccombo();
//			}
//			if (name.compareToIgnoreCase("kchisc")==0)
//			{
//				object=new CRunkchisc();
//			}
//			if (name.compareToIgnoreCase("kcfile")==0)
//			{
//				object=new CRunkcfile();
//			}
//			if (name.compareToIgnoreCase("KcClock")==0)
//			{
//				object=new CRunkcclock();
//			}
			if (name.compareToIgnoreCase("kcpica")==0)
			{
				object=new CRunkcpica();
			}
//			if (name.compareToIgnoreCase("AdvDir")==0)
//			{
//				object=new CRunAdvDir();
//			}
//			if (name.compareToIgnoreCase("FontPacker")==0)
//			{
//				object=new CRunFontPacker();
//			}
//			if (name.compareToIgnoreCase("ACamera")==0)
//			{
//				object=new CRunACamera();
//			}
//			if (name.compareToIgnoreCase("DlgMsgBox")==0)
//			{
//				object=new CRunDlgMsgBox();
//			}
//			//if (name.compareToIgnoreCase("AndroidEmail")==0)
//			//{
//			//	object=new CRunAndroidEmail();
//			//}
//			//if (name.compareToIgnoreCase("EmailSender")==0)
//			//{
//			//	object=new CRunEmailSender();
//			//}
//			//if (name.compareToIgnoreCase("OneSignalApp")==0)
//			//{
//			//	object=new CRunOneSignalApp();
//			//}
//
//			if (name.compareToIgnoreCase("MultipleTouch")==0)
//			{
//				object=new CRunMultipleTouch();
//			}
//			if (name.compareToIgnoreCase("JoystickControl")==0)
//			{
//				object=new CRunJoystickControl();
//			}
//			if (name.compareToIgnoreCase("ActiveBackdrop")==0)
//			{
//				object=new CRunActiveBackdrop();
//			}
//			if (name.compareToIgnoreCase("curl")==0)
//			{
//				object=new CRunCurl();
//			}
			if (name.compareToIgnoreCase("kcini")==0)
			{
				object=new CRunkcini();
			}
//			if (name.compareToIgnoreCase("kcwctrl")==0)
//			{
//				object=new CRunkcwctrl();
//			}
//			if (name.compareToIgnoreCase("CalcRect")==0)
//			{
//				object=new CRunCalcRect();
//			}
//			if (name.compareToIgnoreCase("Easing")==0)
//			{
//				object=new CRunEasing();
//			}
//			if (name.compareToIgnoreCase("StringTokenizer")==0)
//			{
//				object=new CRunStringTokenizer();
//			}
//			if (name.compareToIgnoreCase("Platform")==0)
//			{
//				object=new CRunPlatform();
//			}
//			if (name.compareToIgnoreCase("WebView2")==0)
//			{
//				object=new CRunWebView2();
//			}
//			if (name.compareToIgnoreCase("AndroidUpdater")==0)
//			{
//				object=new CRunAndroidUpdater();
//			}
//			if (name.compareToIgnoreCase("AndroidSplash")==0)
//			{
//				object=new CRunAndroidSplash();
//			}
//			if (name.compareToIgnoreCase("Oldini")==0)
//			{
//				object=new CRunOldini();
//			}
//			if (name.compareToIgnoreCase("GMaps")==0)
//			{
//				object=new CRunGMaps();
//			}
//			if (name.compareToIgnoreCase("AdMob")==0)
//			{
//				object=new CRunAdMob();
//			}
//			if (name.compareToIgnoreCase("LeadBolt")==0)
//			{
//				object=new CRunLeadBolt();
//			}
//			if (name.compareToIgnoreCase("Location")==0)
//			{
//				object=new CRunLocation();
//			}
			if (name.compareToIgnoreCase("Get")==0)
			{
				object=new CRunGet();
			}
//			if (name.compareToIgnoreCase("InputType")==0)
//			{
//				object=new CRunInputType();
//			}
//			if (name.compareToIgnoreCase("MTRandom")==0)
//			{
//				object=new CRunMTRandom();
//			}
//			if (name.compareToIgnoreCase("AndroidPlus")==0)
//			{
//				object=new CRunAndroidPlus();
//			}
//			if (name.compareToIgnoreCase("AndroidDialog")==0)
//			{
//				object=new CRunAndroidDialog();
//			}
//			if (name.compareToIgnoreCase("InAppAndroid")==0)
//			{
//				object=new CRunInAppAndroid();
//			}
//			if (name.compareToIgnoreCase("Inventory")==0)
//			{
//				object=new CRunInventory();
//			}
//			if (name.compareToIgnoreCase("FIRETV")==0)
//			{
//				object=new CRunFIRETV();
//			}
//
//			if (name.compareToIgnoreCase("NEXUSTV")==0)
//			{
//				object=new CRunNEXUSTV();
//			}
//			if (name.compareToIgnoreCase("JSON_Object")==0)
//			{
//				object=new CRunJSON_Object();
//			}
//
//			if (name.compareToIgnoreCase("GetLocale")==0)
//			{
//				object=new CRunGetLocale();
//			}
//
//			if (name.compareToIgnoreCase("Lacewing")==0)
//			{
//				object=new CRunLacewing();
//			}
//
//			if (name.compareToIgnoreCase("VideoAndroid")==0)
//			{
//				object=new CRunVideoAndroid();
//			}
//
//			//if (name.compareToIgnoreCase("AVideoSurface")==0)
//			//{
//			//	object=new CRunAVideoSurface();
//			//}
//
//			if (name.compareToIgnoreCase("CreateByName")==0)
//			{
//				object=new CRunCreateByName();
//			}
//
//			if (name.compareToIgnoreCase("ScreenZoom")==0)
//			{
//				object=new CRunScreenZoom();
//			}
//
//			if (name.compareToIgnoreCase("ChartBoost")==0)
//			{
//				object=new CRunChartBoost();
//			}
//
//			if (name.compareToIgnoreCase("EncoderX")==0)
//			{
//				object=new CRunEncoderX();
//			}
//
//			if (name.compareToIgnoreCase("Scan")==0)
//			{
//				object=new CRunScan();
//			}
//
//			if (name.compareToIgnoreCase("GPGConnect")==0)
//			{
//				object=new CRunGPGConnect();
//			}
//
//			if (name.compareToIgnoreCase("Perspective")==0)
//			{
//				object=new CRunPerspective();
//			}
//
//			if (name.compareToIgnoreCase("GPGCloud")==0)
//			{
//				object=new CRunGPGCloud();
//			}
//
//
//			if (name.compareToIgnoreCase("GPGTurnbase")==0)
//			{
//				object=new CRunGPGTurnbase();
//			}
//
//
//			if (name.compareToIgnoreCase("GPGAchievement")==0)
//			{
//				object=new CRunGPGAchievement();
//			}
//
//
//			if (name.compareToIgnoreCase("GPGLeaderboard")==0)
//			{
//				object=new CRunGPGLeaderboard();
//			}
//
//
//			if (name.compareToIgnoreCase("GPGPlayers")==0)
//			{
//				object=new CRunGPGPlayers();
//			}
//
//			if (name.compareToIgnoreCase("GPGQuests")==0)
//			{
//				object=new CRunGPGQuests();
//			}
//
//			if (name.compareToIgnoreCase("Box2DBase")==0)
//			{
//				object=new CRunBox2DBase();
//			}
//			if (name.compareToIgnoreCase("Box2DFan")==0)
//			{
//				object=new CRunBox2DFan();
//			}
//			if (name.compareToIgnoreCase("Box2DMagnet")==0)
//			{
//				object=new CRunBox2DMagnet();
//			}
//			if (name.compareToIgnoreCase("Box2DTreadmill")==0)
//			{
//				object=new CRunBox2DTreadmill();
//			}
//			if (name.compareToIgnoreCase("Box2DParticules")==0)
//			{
//				object=new CRunBox2DParticules();
//			}
//			if (name.compareToIgnoreCase("Box2DRopeAndChain")==0)
//			{
//				object=new CRunBox2DRopeAndChain();
//			}
//            if (name.compareToIgnoreCase("Box2DGround")==0)
//            {
//                object=new CRunBox2DGround();
//            }
//            if (name.compareToIgnoreCase("Box2DJoint")==0)
//            {
//                object=new CRunBox2DJoint();
//            }
			// ENDCUT
		}

		if (object!=null)
		{
			Log.Log("Created extension: " + name);
			return object;
		}

		Log.Log("*** MISSING EXTENSION: " + name);
		return null;
	}
}
