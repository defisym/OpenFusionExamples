@echo off
setlocal
where py >nul 2>nul
if errorlevel 1 (
    python "%~dp0Fini.py" %*
) else (
    py -3 "%~dp0Fini.py" %*
)
exit /b %errorlevel%
