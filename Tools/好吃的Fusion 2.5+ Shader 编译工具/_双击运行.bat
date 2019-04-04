@echo off

title Fusion 2.5+ DX11 Shader 编译工具 - Custom By Defisym
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
echo Fusion 2.5+ DX11 Shader 编译工具
echo Custom By Defisym
echo.
echo Ver. 1.5.190404
echo ------------------------------------
echo.

cd /d %~dp0

:1

echo 请将需要编译的shader文件 (扩展名.hlsl) 拷贝至本文件夹根目录
echo 保证与批处理文件处于同一路径下
echo.
echo 模式【1】：输入文件名编译指定文件
echo.
echo 模式【2】：编译当前路径下全部文件
echo.
echo 模式【3】：退出程序
echo.

set /p Mode_Select=请输入模式（数字）：

set mode=0
if %Mode_Select% equ 1 goto :mode1
if %Mode_Select% equ 2 goto :mode2
if %Mode_Select% equ 3 (	
	endlocal
	exit /b	
)

cls
echo.
echo 模式错误！
echo.

goto :1


:mode1
set mode=1

cls
echo.
set /p FileName=请输入shader文件名 (不包含扩展名) ：
echo.

set error=0
goto :compile

:mode2
set mode=2

cls
echo.
echo 开始编译……
echo.

for /f  %%a in ('dir  /b "*.hlsl*" ') do (
	setlocal EnableDelayedExpansion
	
	set error=0	
	set FileName=%%~na
	call :compile

)

echo.
echo 编译结束
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
echo 正在编译DX11 非预乘 版本……

call :CompileShaderHLSL4 "%FileName%" ps ps_main

echo .
if %error% == 0 (
    echo DX11 非预乘 版本编译成功！
	echo.
) else (
    echo DX11 非预乘 版本编译错误！
	echo.
)

@rem *** compile premultiplied version
set FXCEXT=premultiplied.%FXCEXT%

echo ------------------------------------
echo.
echo 正在编译DX11 预乘 版本……

call :CompileShaderHLSL4 "%FileName%" ps ps_main_pm

echo .
if %error% == 0 (
    echo DX11 预乘 版本编译成功！
	echo.
) else (
    echo DX11 预乘 版本编译错误！
	echo.
)


if %mode% equ 1 (

	echo ------------------------------------
	echo 编译结束
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