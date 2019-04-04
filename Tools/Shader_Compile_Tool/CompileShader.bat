@echo off

title Fusion 2.5+ DX11 Shader Compile Tool - Custom By Defisym
@rem mode con cols=72 lines=20
mode con cols=96
color 3F

:Init

setlocal
set error=0

@rem set FXCOPTS=/nologo /WX /Ges /Zi /Zpc /Qstrip_reflect /Qstrip_debug
set FXCOPTS=/nologo /WX /Ges /Qstrip_reflect /Qstrip_debug
set FXCOPTSNS=/nologo /Ges /Qstrip_reflect /Qstrip_debug

set PCFXC=fxc\fxc.exe
set OUTPUT_PATH=%~dp0\Shader_Output\

:0

cls
echo ------------------------------------
echo Fusion 2.5+ DX11 Shader Compile Tool
echo Custom By Defisym
echo.
echo Ver. 1.5.190404
echo ------------------------------------
echo.

cd /d %~dp0

:1

echo Please copy the shader file (.hlsl) that needs to be compiled
echo to the root directory of this folder.
echo Guaranteed to be in the same path as the batch file.
echo.
echo Mode [1] : Enter the file name to compile the specified file
echo.
echo Mode [2] : Compile all files in the current path
echo.
echo Mode [3] : Exit
echo.

set /p Mode_Select=Please input the mode(1/2/3):

set mode=0
if %Mode_Select% equ 1 goto :mode1
if %Mode_Select% equ 2 goto :mode2
if %Mode_Select% equ 3 (	
	endlocal
	exit /b	
)

cls
echo.
echo Mode error!
echo.

goto :1


:mode1
set mode=1

cls
echo.
set /p FileName=Please enter the name of the shader file (without the extension):
echo.

set error=0
goto :compile

:mode2
set mode=2

cls
echo.
echo Compiling¡­¡­
echo.

for /f  %%a in ('dir  /b "*.hlsl*" ') do (
	
	set error=0	
	set FileName=%%~na
	call :compile

)

echo.
echo Compile finished
echo.

echo.
@pause

goto :0

@rem ------------------------------------------------
@rem Compile

:compile

@rem *** compile non-premultiplied version
set FXCEXT=fxc

echo ------------------------------------
echo.
echo Compiling DX11 Non-premultiplied version

call :CompileShaderHLSL4 "%FileName%" ps ps_main

echo .
if %error% == 0 (
    echo DX11 Non-premultiplied version compile success!
	echo.
) else (
    echo DX11 Non-premultiplied version compile failed!
	echo.
)

@rem *** compile premultiplied version
set FXCEXT=premultiplied.%FXCEXT%

echo ------------------------------------
echo.
echo Compiling DX11 premultiplied version

call :CompileShaderHLSL4 "%FileName%" ps ps_main_pm

echo .
if %error% == 0 (
    echo DX11 premultiplied version compile success!
	echo.
) else (
    echo DX11 premultiplied version compile failed!
	echo.
)


if %mode% equ 1 (

	echo ------------------------------------
	echo Compile finished
	echo ------------------------------------
	echo.
	
	@pause
	goto :mode1
)

if %mode% equ 2 (	
	exit /b
)

@rem ------------------------------------------------
@rem CompileShader

:CompileShader
@rem set fxc=%PCFXC% %1.fx %FXCOPTS% /T%2_4_0_level_9_1 /E%3 /FhCompiled\%1_%3.inc /FdCompiled\%1_%3.pdb /Vn%1_%3
set fxc=%PCFXC% "%~1.fx11" %FXCOPTS% /T%2_4_0_level_9_1 /E%3 /Fo"%~1.%FXCEXT%"
echo.
echo %fxc%
%fxc% || set error=1
exit /b

:CompileShaderSM4
@rem set fxc=%PCFXC% %1.fx %FXCOPTS% /T%2_4_0 /E%3 /FhCompiled\%1_%3.inc /FdCompiled\%1_%3.pdb /Vn%1_%3
set fxc=%PCFXC% "%~1.fx11" %FXCOPTS% /T%2_4_0 /E%3 /Fo"%~1.%FXCEXT%"
echo.
echo %fxc%
%fxc% || set error=1
exit /b

:CompileShaderHLSL
@rem set fxc=%PCFXC% %1.hlsl %FXCOPTS% /T%2_4_0_level_9_1 /E%3 /FhCompiled\%1_%3.inc /FdCompiled\%1_%3.pdb /Vn%1_%3
set fxc=%PCFXC% "%~1.hlsl" %FXCOPTS% /T%2_4_0_level_9_1 /E%3 /Fo"%~1.%FXCEXT%"
echo.
echo %fxc%
%fxc% || set error=1
exit /b

:CompileShaderHLSL4
@rem set fxc=%PCFXC% %1.hlsl %FXCOPTS% /T%2_4_0_level_9_1 /E%3 /FhCompiled\%1_%3.inc /FdCompiled\%1_%3.pdb /Vn%1_%3
@rem set fxc=%PCFXC% "%~1.hlsl" %FXCOPTS% /T%2_4_0 /E%3 /Fo"%~1.%FXCEXT%"
set fxc=%PCFXC% "%~1.hlsl" %FXCOPTS% /T%2_4_0 /E%3 /Fo"%OUTPUT_PATH%%~1.%FXCEXT%"

echo.
echo %fxc%
%fxc% || set error=1
exit /b

:CompileShaderHLSL4ns
@rem set fxc=%PCFXC% %1.hlsl %FXCOPTS% /T%2_4_0_level_9_1 /E%3 /FhCompiled\%1_%3.inc /FdCompiled\%1_%3.pdb /Vn%1_%3
set fxc=%PCFXC% "%~1.hlsl" %FXCOPTSNS% /T%2_4_0 /E%3 /Fo"%~1.%FXCEXT%"
echo.
echo %fxc%
%fxc% || set error=1
exit /b



@rem ------------------------------------------------
@rem Hello from Dalian China