@echo off
rem Build the x86 ddraw.dll proxy into runtime\display\bin (MSVC, vcvars32).
setlocal enabledelayedexpansion
set "VSW=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set VS=
for /f "usebackq delims=" %%i in (`"!VSW!" -latest -products * -property installationPath`) do set "VS=%%i"
if not defined VS echo vswhere found no Visual Studio & exit /b 1
call "%VS%\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 1
if not exist "%~dp0bin" mkdir "%~dp0bin"
cd /d "%~dp0bin"
cl /nologo /Brepro /O2 /W3 /EHsc /D_CRT_SECURE_NO_WARNINGS /LD /MT "%~dp0ddraw_proxy.c" "%~dp0gpu_renderer.cpp" "%~dp0cd_audio.cpp" /Fe:ddraw.dll /link /Brepro /DEF:"%~dp0ddraw.def" user32.lib gdi32.lib d3d11.lib d3dcompiler.lib winmm.lib ole32.lib advapi32.lib mfplat.lib mfreadwrite.lib mfuuid.lib || exit /b 1
echo built %CD%\ddraw.dll
