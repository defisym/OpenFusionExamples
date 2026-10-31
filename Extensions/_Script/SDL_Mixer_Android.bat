@echo off
setlocal EnableExtensions
py -3 --version >nul 2>nul
if not errorlevel 1 (
    py -3 "%~dp0SDL_Mixer.py" %*
    if errorlevel 1 exit /b 1
    exit /b 0
)
python "%~dp0SDL_Mixer.py" %*
exit /b %errorlevel%
