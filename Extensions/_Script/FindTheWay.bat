@echo off
setlocal EnableExtensions
rem One entry point. Python prepares/verifies all Android dependencies and publishes the packages.
py -3 --version >nul 2>nul
if not errorlevel 1 (
    py -3 "%~dp0FindTheWay.py"
    if errorlevel 1 goto Failed
    exit /b 0
)
python --version >nul 2>nul
if errorlevel 1 (
    echo ERROR: Python 3.10 or newer is required. Install Python and run this batch again.
    goto Failed
)
python "%~dp0FindTheWay.py"
if errorlevel 1 goto Failed
exit /b 0
:Failed
echo ERROR: Packaging failed. See Android_Gradle\FindTheWay-package.log for details.
pause
exit /b 1
