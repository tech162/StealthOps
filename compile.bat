@echo off
REM ==========================================================
REM  compile.bat - builds StealthOps.exe with g++ (MinGW-w64)
REM  Double-click this file, or run  compile.bat  in a terminal
REM  that is open in the StealthOps folder.
REM ==========================================================

cd /d "%~dp0"

where g++ >nul 2>nul
if errorlevel 1 (
    echo.
    echo [ERROR] g++ was not found.
    echo Install MinGW-w64 and add its "bin" folder to PATH.
    echo See "Required software" in README.md.
    echo.
    pause
    exit /b 1
)

echo Compiling StealthOps...
g++ -std=c++17 -Wall -Iinclude src\main.cpp src\Character.cpp src\Player.cpp src\Guard.cpp src\Item.cpp src\Door.cpp src\Level.cpp src\Game.cpp -o StealthOps.exe

if errorlevel 1 (
    echo.
    echo [ERROR] Compilation failed. Read the messages above.
    echo.
    pause
    exit /b 1
)

echo.
echo [OK] Build finished: StealthOps.exe is now in this folder.
echo To play, type:  StealthOps.exe
echo.
pause
