Set ProjectName=Fini
Set CompressPath="C:\Program Files\7-Zip\7z.exe"

Set ParentPath=%~dp0\..\
Set ToInstallPath=%~dp0\..\%ProjectName%\ToInstall\Files\

@REM @echo Android...

@REM Set AndroidPath=%~dp0\..\Android_Gradle

@REM Set SrcPath=%AndroidPath%\app

@REM Set ContentPath=%AndroidPath%\_Release

@REM Set ReleasePath=%ToInstallPath%\Data\Runtime\Android

@REM Set FusionPath=C:\Steam\steamapps\common\Clickteam Fusion 2.5\Data\Runtime\Android

@REM @echo Delete...

@REM rem Clean Content Path
@REM for /r "%ContentPath%" %%F in (*.*) do (
@REM 	del "%%F" /f /s /q	
@REM )

@REM rmdir /s /q "%ContentPath%\build"
@REM rmdir /s /q "%ContentPath%\app"
@REM rmdir /s /q "%ContentPath%\libs"
@REM rmdir /s /q "%ContentPath%\src"

@REM @echo Copy...

@REM @REM Add content here
@REM echo f | xcopy "%SrcPath%\src\main\java\Extensions\CRun%ProjectName%.java" "%ContentPath%\app\src\main\java\Extensions\CRun%ProjectName%.java" /s /e

@REM @REM Use these commands if extra lib & build script are needed

@REM @REM xcopy "%SrcPath%\libs\*.*" "%ContentPath%\app\libs\" /s /e

@REM @REM copy "%AndroidPath%\release.gradle" "%ContentPath%\app\build.gradle"
@REM @REM copy "%AndroidPath%\ReleaseSettings.gradle" "%ContentPath%\settings.gradle"

@REM @REM del "%ContentPath%\app\libs\gson-2.8.6.jar" /f /s /q

@REM @echo Compress...

@REM del %AndroidPath%\%ProjectName%.zip /f /s /q

@REM %CompressPath% a -tzip %AndroidPath%\%ProjectName%.zip %ContentPath%\*

@REM @echo Install...

@REM del "%FusionPath%\%ProjectName%.zip" /f /s /q	
@REM copy "%AndroidPath%\%ProjectName%.zip" "%FusionPath%"

@REM del "%ReleasePath%\%ProjectName%.zip" /f /s /q	
@REM echo f | xcopy "%AndroidPath%\%ProjectName%.zip" "%ReleasePath%\%ProjectName%.zip" /s /e

@REM @echo Remove Temp...

@REM del %AndroidPath%\%ProjectName%.zip /f /s /q

@REM @echo HTML5

@REM Set HTML5Path=%~dp0\..\HTML5

@REM Set ReleasePath=%ToInstallPath%\Data\Runtime\Html5

@REM Set FusionPath=C:\Steam\steamapps\common\Clickteam Fusion 2.5\Data\Runtime\Html5

@REM del "%ReleasePath%\%ProjectName%.js" /f /s /q
@REM echo f | xcopy "%HTML5Path%\%ProjectName%.js" "%ReleasePath%\%ProjectName%.js" /s /e

@REM @echo Install...

@REM del "%FusionPath%\%ProjectName%.js" /f /s /q	
@REM copy "%HTML5Path%\%ProjectName%.js" "%FusionPath%"

@echo Make Release Package

set Date=%date:~2,8%

set Date=%Date:/=%

%CompressPath% a -tzip %ParentPath%\%ProjectName%_B%Date%.zip %ToInstallPath%\*
%CompressPath% d %ParentPath%\%ProjectName%_B%Date%.zip *.exe -r
%CompressPath% d %ParentPath%\%ProjectName%_B%Date%.zip *.001 -r
%CompressPath% d %ParentPath%\%ProjectName%_B%Date%.zip *.keep -r
%CompressPath% d %ParentPath%\%ProjectName%_B%Date%.zip *.DS_Store -r

copy "%ParentPath%\%ProjectName%_B%Date%.zip" "%ParentPath%\%ProjectName%_Release.zip"

@echo Stop JDK...

Powershell.exe -executionpolicy remotesigned -File  "F:\DEV\OpenFusionExamples\Tools\JDK Terminator\JDK_Terminator.ps1"

@REM @echo Wait...

@REM pause