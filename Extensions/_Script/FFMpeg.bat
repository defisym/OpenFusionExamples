@echo off
setlocal EnableExtensions
py -3 --version >nul 2>nul
if not errorlevel 1 (
    py -3 "%~dp0FFMpeg.py" %*
    if errorlevel 1 exit /b 1
    exit /b 0
)
python "%~dp0FFMpeg.py" %*
exit /b %errorlevel%
