@echo off
rem Builds xbrz.dll (64-bit, matches the tools' Python) next to this file. Called by xbrz_index.py when missing.
setlocal
for /f "usebackq delims=" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -property installationPath`) do set VS=%%i
if not defined VS (echo Visual Studio with C++ tools not found & exit /b 1)
call "%VS%\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1
cd /d "%~dp0"
cl /nologo /O2 /EHsc /std:c++17 /LD xbrz.cpp wrap.cpp /Fe:xbrz.dll /Fo:%TEMP%\ || exit /b 1
del /q xbrz.exp xbrz.lib 2>nul
