@echo off
rem Keep this manifest in sync with FindTheWay Java/JNI, shared headers and build configuration.
setlocal EnableExtensions DisableDelayedExpansion
set "ProjectName=FindTheWay"
if not defined CompressPath set "CompressPath=C:\Program Files\7-Zip\7z.exe"
set "ParentPath=%~dp0.."
set "ToInstallPath=%ParentPath%\%ProjectName%\ToInstall\Files"
set "AndroidPath=%ParentPath%\Android_Gradle"
set "SrcPath=%AndroidPath%\app"
set "ContentPath=%AndroidPath%\_Release"
set "ReleasePath=%ToInstallPath%\Data\Runtime\Android"
if not defined FusionRoot set "FusionRoot=C:\Steam\steamapps\common\Clickteam Fusion 2.5"
set "FusionPath=%FusionRoot%\Data\Runtime\Android"

if not exist "%CompressPath%" goto PackageError
@echo Android: staging Java, JNI and shared C++ headers...
if exist "%ContentPath%" rmdir /s /q "%ContentPath%"
if exist "%ContentPath%" goto PackageError
mkdir "%ContentPath%" || goto PackageError

call :CopyFile "%SrcPath%\src\main\java\Extensions\CRunFindTheWay.java" "%ContentPath%\app\src\main\java\Extensions\CRunFindTheWay.java" || goto PackageError
call :CopyFile "%SrcPath%\src\main\java\_DeLib\AOEClass.java" "%ContentPath%\app\src\main\java\_DeLib\AOEClass.java" || goto PackageError
call :CopyFile "%SrcPath%\src\main\java\_DeLib\Coord.java" "%ContentPath%\app\src\main\java\_DeLib\Coord.java" || goto PackageError
call :CopyFile "%SrcPath%\src\main\java\_DeLib\FindTheWayClass.java" "%ContentPath%\app\src\main\java\_DeLib\FindTheWayClass.java" || goto PackageError
call :CopyFile "%SrcPath%\src\main\java\_DeLib\FusionUtilities.java" "%ContentPath%\app\src\main\java\_DeLib\FusionUtilities.java" || goto PackageError
call :CopyFile "%SrcPath%\src\main\java\_3rdLib\ObjectCreationClass.java" "%ContentPath%\app\src\main\java\_3rdLib\ObjectCreationClass.java" || goto PackageError
call :CopyFile "%SrcPath%\src\main\java\_3rdLib\ObjectSelectionClass.java" "%ContentPath%\app\src\main\java\_3rdLib\ObjectSelectionClass.java" || goto PackageError
call :CopyFile "%SrcPath%\src\main\cpp\CMakeLists.txt" "%ContentPath%\app\src\main\cpp\CMakeLists.txt" || goto PackageError
call :CopyFile "%SrcPath%\src\main\cpp\FindTheWayJNI.cpp" "%ContentPath%\app\src\main\cpp\FindTheWayJNI.cpp" || goto PackageError
call :CopyFile "%ParentPath%\_DeLib\FindTheWay.h" "%ContentPath%\app\src\main\cpp\shared\_DeLib\FindTheWay.h" || goto PackageError
call :CopyFile "%ParentPath%\_DeLib\AOE.h" "%ContentPath%\app\src\main\cpp\shared\_DeLib\AOE.h" || goto PackageError
call :CopyFile "%ParentPath%\_DeLib\MapData.h" "%ContentPath%\app\src\main\cpp\shared\_DeLib\MapData.h" || goto PackageError
call :CopyFile "%ParentPath%\_DeLib\FindTheWayFusion.h" "%ContentPath%\app\src\main\cpp\shared\_DeLib\FindTheWayFusion.h" || goto PackageError
call :CopyFile "%ParentPath%\_DeLib\GeneralDefinition.h" "%ContentPath%\app\src\main\cpp\shared\_DeLib\GeneralDefinition.h" || goto PackageError
call :CopyFile "%ParentPath%\_DeLib\StringTraits.h" "%ContentPath%\app\src\main\cpp\shared\_DeLib\StringTraits.h" || goto PackageError
call :CopyFile "%ParentPath%\_DeLib\ArithmeticTraits.h" "%ContentPath%\app\src\main\cpp\shared\_DeLib\ArithmeticTraits.h" || goto PackageError
call :CopyFile "%ParentPath%\_DeLib\StrNum.h" "%ContentPath%\app\src\main\cpp\shared\_DeLib\StrNum.h" || goto PackageError
call :CopyFile "%ParentPath%\_3rdLib\Base64.h" "%ContentPath%\app\src\main\cpp\shared\_3rdLib\Base64.h" || goto PackageError
call :CopyFile "%AndroidPath%\cmake\DeLibHelpers.cmake" "%ContentPath%\cmake\DeLibHelpers.cmake" || goto PackageError
call :CopyFile "%AndroidPath%\release.gradle" "%ContentPath%\app\build.gradle" || goto PackageError
call :CopyFile "%AndroidPath%\ReleaseSettings.gradle" "%ContentPath%\settings.gradle" || goto PackageError
call :CopyFile "%SrcPath%\proguard-rules.pro" "%ContentPath%\app\proguard-rules.txt" || goto PackageError
call :CopyFile "%AndroidPath%\FindTheWay-JNI.md" "%ContentPath%\FindTheWay-JNI.md" || goto PackageError
call :CopyFile "%ParentPath%\..\LICENSE" "%ContentPath%\FindTheWay-LICENSE.txt" || goto PackageError

@echo Compress Android source package...
if exist "%AndroidPath%\%ProjectName%.zip" del /f /q "%AndroidPath%\%ProjectName%.zip"
if exist "%AndroidPath%\%ProjectName%.zip" goto PackageError
pushd "%ContentPath%" || goto PackageError
"%CompressPath%" a -tzip "%AndroidPath%\%ProjectName%.zip" .\*
if errorlevel 1 (
    popd
    goto PackageError
)
popd
call :CopyFile "%AndroidPath%\%ProjectName%.zip" "%ReleasePath%\%ProjectName%.zip" || goto PackageError
if exist "%FusionPath%\" (
    call :CopyFile "%AndroidPath%\%ProjectName%.zip" "%FusionPath%\%ProjectName%.zip" || goto PackageError
) else (
    echo Fusion Android runtime directory not found; ToInstall package was created.
)
del /f /q "%AndroidPath%\%ProjectName%.zip"

@echo HTML5

Set HTML5Path=%~dp0\..\HTML5

Set ReleasePath=%ToInstallPath%\Data\Runtime\Html5

Set "FusionPath=%FusionRoot%\Data\Runtime\Html5"

Set H5ContentPath=%HTML5Path%\_Release

del "%ReleasePath%\%ProjectName%.js" /f /s /q

type "%HTML5Path%\_3rdLib\ObjectCreationClass.js">>"%H5ContentPath%\%ProjectName%.js"
type "%HTML5Path%\_3rdLib\ObjectSelectionClass.js">>"%H5ContentPath%\%ProjectName%.js"
type "%HTML5Path%\_DeLib\FusionUtilities.js">>"%H5ContentPath%\%ProjectName%.js"
type "%HTML5Path%\_DeLib\Coord.js">>"%H5ContentPath%\%ProjectName%.js"
type "%HTML5Path%\_DeLib\FindTheWayClass.js">>"%H5ContentPath%\%ProjectName%.js"
type "%HTML5Path%\_DeLib\AOEClass.js">>"%H5ContentPath%\%ProjectName%.js"

type "%HTML5Path%\%ProjectName%.js">>"%H5ContentPath%\%ProjectName%.js"

echo f | xcopy "%H5ContentPath%\%ProjectName%.js" "%ReleasePath%\%ProjectName%.js" /s /e

@echo Install...

del "%FusionPath%\%ProjectName%.js" /f /s /q	
copy "%HTML5Path%\%ProjectName%.js" "%FusionPath%"

@echo Make Release Package

for /f %%D in ('powershell -NoProfile -Command "Get-Date -Format yyyyMMdd"') do set "PackageDate=%%D"
if not defined PackageDate goto PackageError

"%CompressPath%" a -tzip "%ParentPath%\%ProjectName%_B%PackageDate%.zip" "%ToInstallPath%\*"
"%CompressPath%" d "%ParentPath%\%ProjectName%_B%PackageDate%.zip" *.001 -r
"%CompressPath%" d "%ParentPath%\%ProjectName%_B%PackageDate%.zip" *.keep -r
"%CompressPath%" d "%ParentPath%\%ProjectName%_B%PackageDate%.zip" *.DS_Store -r

copy "%ParentPath%\%ProjectName%_B%PackageDate%.zip" "%ParentPath%\%ProjectName%_Release.zip"


exit /b 0

:CopyFile
if not exist "%~1" (
    echo ERROR: Missing package input: %~1
    exit /b 1
)
if not exist "%~dp2" mkdir "%~dp2"
copy /y "%~1" "%~2" >nul
exit /b %errorlevel%

:PackageError
echo ERROR: FindTheWay packaging failed. No completed package should be used.
exit /b 1
