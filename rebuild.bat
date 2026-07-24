@echo off

echo ==========================
echo    Rebuilding Project
echo ==========================
echo.
:: Detect make command
where make >nul 2>&1
if not errorlevel 1 (
    set MAKE=make
    goto :build
)

where mingw32-make >nul 2>&1
if not errorlevel 1 (
    set MAKE=mingw32-make
    goto :build
)


echo [ERROR] no make tool found!
echo Please install GNU Make / MinGW-w64 a and add it to PATH.
pause
exit /b 1

:build
echo Using %MAKE%
echo.


echo Cleaning previous build...
%MAKE% clean >nul 2>&1

echo Building project...
%MAKE% all

if errorlevel 1 (
    echo.
    echo ==========================
    echo      Build Failed!
    echo ==========================
    pause
    exit /b
)

echo.
echo ==========================
echo    Build Successful!
echo ==========================
echo.

echo Starting application...
echo.

build\playlist.exe

pause