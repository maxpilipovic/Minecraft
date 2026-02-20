@echo off
setlocal

set PREMAKE_VERSION=5.0.0-beta2
set PREMAKE_DIR=vendor\bin\premake
set PREMAKE_EXE=%PREMAKE_DIR%\premake5.exe

if not exist "%PREMAKE_EXE%" (
    echo Premake5 not found. Please download it from:
    echo https://github.com/premake/premake-core/releases
    echo.
    echo Extract premake5.exe to: %PREMAKE_DIR%\
    echo.
    pause
    exit /b 1
)

echo Generating Visual Studio 2022 project files...
call "%PREMAKE_EXE%" vs2022

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Error: Failed to generate project files!
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Done! You can now open Minecraft.sln in Visual Studio.
echo.
pause
