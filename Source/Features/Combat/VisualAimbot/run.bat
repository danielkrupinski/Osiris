@echo off
cd /d "%~dp0zero_train"

echo ====================================
echo   TAC-HUD Visual Aimbot Launcher
echo ====================================
echo.
echo [1/2] Starting HUD overlay server...
start "TAC-HUD" "..\.venv\Scripts\pythonw.exe" overlay_server.py
timeout /t 2 /nobreak >nul

echo [2/2] Starting Aimbot...
call "..\.venv\Scripts\activate.bat"
python cs2_visual_aim.py %*

echo.
echo Shutting down...
taskkill /fi "WINDOWTITLE eq TAC-HUD" /f 2>nul
pause
