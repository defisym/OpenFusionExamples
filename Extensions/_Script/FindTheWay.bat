Set ProjectName=FindTheWay
Set CompressPath="C:\Program Files\7-Zip\7z.exe"

Set ParentPath=%~dp0\..\
Set ToInstallPath=%~dp0\..\%ProjectName%\ToInstall\Files\

@echo Android...

Set AndroidPath=%~dp0\..\Android_Gradle

Set SrcPath=%AndroidPath%\app

Set ContentPath=%AndroidPath%\_Release

Set ReleasePath=%ToInstallPath%\Data\Runtime\Android

Set FusionPath=C:\Steam\steamapps\common\Clickteam Fusion 2.5\Data\Runtime\Android

@echo Delete...

rem Clean Content Path
for /r "%ContentPath%" %%F in (*.*) do (
	del "%%F" /f /s /q	
)

rmdir /s /q "%ContentPath%\build"
rmdir /s /q "%ContentPath%\app"
rmdir /s /q "%ContentPath%\libs"
rmdir /s /q "%ContentPath%\src"

@echo Copy...

@REM Add content here
echo f | xcopy "%SrcPath%\src\main\java\Extensions\CRun%ProjectName%.java" "%ContentPath%\app\src\main\java\Extensions\CRun%ProjectName%.java" /s /e
echo f | xcopy "%SrcPath%\src\main\java\_DeLib\Coord.java" "%ContentPath%\app\src\main\java\_DeLib\Coord.java" /s /e
echo f | xcopy "%SrcPath%\src\main\java\_DeLib\FindTheWayClass.java" "%ContentPath%\app\src\main\java\_DeLib\FindTheWayClass.java" /s /e
echo f | xcopy "%SrcPath%\src\main\java\_DeLib\FusionCommon.java" "%ContentPath%\app\src\main\java\_DeLib\FusionCommon.java" /s /e
echo f | xcopy "%SrcPath%\src\main\java\_DeLib\Heuristic.java" "%ContentPath%\app\src\main\java\_DeLib\Heuristic.java" /s /e
echo f | xcopy "%SrcPath%\src\main\java\_DeLib\MapType.java" "%ContentPath%\app\src\main\java\_DeLib\MapType.java" /s /e
echo f | xcopy "%SrcPath%\src\main\java\_DeLib\UpdateMapCallBack.java" "%ContentPath%\app\src\main\java\_DeLib\UpdateMapCallBack.java" /s /e

@REM remove debug mode

@REM set "source_1=private static final boolean DEBUG = true;"
@REM set "source_2=private static final boolean DEBUG = false;"
@REM set "source_1_1=// private static final boolean DEBUG = true;"
@REM set "source_2_1=// private static final boolean DEBUG = false;"
@REM set "source_del="

@REM del temp.java

@REM @echo off
@REM setlocal enabledelayedexpansion

@REM for /f %%i in (%SrcPath%\src\main\java\Extensions\CRun%ProjectName%.java) do (
@REM     set str=%%i
@REM 	@REM set rep=!str:%source_2%=%source_del%!
@REM 	set "rep=!str:%source_1%=%source_2%!"
	
@REM     echo !rep!>>temp.java
@REM )

@REM pause

@REM Use these commands if extra lib & build script are needed

@REM xcopy "%SrcPath%\libs\*.*" "%ContentPath%\app\libs\" /s /e

copy "%AndroidPath%\release.gradle" "%ContentPath%\app\build.gradle"
copy "%AndroidPath%\ReleaseSettings.gradle" "%ContentPath%\settings.gradle"

@REM del "%ContentPath%\app\libs\gson-2.8.6.jar" /f /s /q

@echo Compress...

del %AndroidPath%\%ProjectName%.zip /f /s /q

%CompressPath% a -tzip %AndroidPath%\%ProjectName%.zip %ContentPath%\*

@echo Install...

del "%FusionPath%\%ProjectName%.zip" /f /s /q	
copy "%AndroidPath%\%ProjectName%.zip" "%FusionPath%"

del "%ReleasePath%\%ProjectName%.zip" /f /s /q	
echo f | xcopy "%AndroidPath%\%ProjectName%.zip" "%ReleasePath%\%ProjectName%.zip" /s /e

@echo Remove Temp...

del %AndroidPath%\%ProjectName%.zip /f /s /q

@echo HTML5

Set HTML5Path=%~dp0\..\HTML5

Set ReleasePath=%ToInstallPath%\Data\Runtime\Html5

Set FusionPath=C:\Steam\steamapps\common\Clickteam Fusion 2.5\Data\Runtime\Html5

del "%ReleasePath%\%ProjectName%.js" /f /s /q
echo f | xcopy "%HTML5Path%\%ProjectName%.js" "%ReleasePath%\%ProjectName%.js" /s /e

@echo Install...

del "%FusionPath%\%ProjectName%.js" /f /s /q	
copy "%HTML5Path%\%ProjectName%.js" "%FusionPath%"

@echo Make Release Package

set Date=%date:~2,8%

set Date=%Date:/=%

%CompressPath% a -tzip %ParentPath%\%ProjectName%_B%Date%.zip %ToInstallPath%\*
%CompressPath% d %ParentPath%\%ProjectName%_B%Date%.zip *.001 -r
%CompressPath% d %ParentPath%\%ProjectName%_B%Date%.zip *.keep -r
%CompressPath% d %ParentPath%\%ProjectName%_B%Date%.zip *.DS_Store -r

copy "%ParentPath%\%ProjectName%_B%Date%.zip" "%ParentPath%\%ProjectName%_Release.zip"

@echo Stop JDK...

Powershell.exe -executionpolicy remotesigned -File  "F:\DEV\OpenFusionExamples\Tools\JDK Terminator\JDK_Terminator.ps1"

@REM @echo Wait...

@REM pause