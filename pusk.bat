@echo off
cls
setlocal

set "WORK_DIR=C:\Users\user\work_space\Laboratory_work\AAR-2025\Files"
set "LIB_DIR=C:\Users\user\work_space\Laboratory_work\AAR-2025\AAR-2025\Debug"
set "MSVC_BIN=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x86"

set "LIB=%LIB_DIR%;C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\lib\x86;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x86;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\ucrt\x86"

cd /d "%WORK_DIR%"

"%MSVC_BIN%\ml.exe" /c /coff /Zd /Zi in.txt.asm
if %errorlevel% neq 0 (
    echo.
    exit /b
)

"%MSVC_BIN%\link.exe" /subsystem:console /entry:main in.txt.obj StaticLib.lib /out:result.exe
if %errorlevel% neq 0 (
    echo.
    exit /b
)

if exist result.exe (
    result.exe
)