@echo off
rem Build the P6 renderer spike (x64, MSVC) into runtime\prototypes\spike\bin.
setlocal enabledelayedexpansion
set "VSW=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set VS=
for /f "usebackq delims=" %%i in (`"!VSW!" -latest -products * -property installationPath`) do set "VS=%%i"
if not defined VS echo vswhere found no Visual Studio & exit /b 1
call "%VS%\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1
if not exist "%~dp0bin" mkdir "%~dp0bin"
cd /d "%~dp0bin"
cl /nologo /O2 /EHsc /std:c++17 /DUNICODE /D_UNICODE "%~dp0spike.cpp" /Fe:spike.exe /link /SUBSYSTEM:WINDOWS shell32.lib || exit /b 1
echo built %CD%\spike.exe
