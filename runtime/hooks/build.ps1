# Build hercules_hook.dll and herc_inject.exe (x86, MSVC) into runtime/hooks/bin.
$ErrorActionPreference = "Stop"
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs = & $vswhere -latest -products * -property installationPath
$vcvars = Join-Path $vs "VC\Auxiliary\Build\vcvars32.bat"
$src = $PSScriptRoot
$bin = Join-Path $src "bin"
New-Item -ItemType Directory -Force $bin | Out-Null
$cmd = "`"$vcvars`" >nul && cd /d `"$bin`" && " +
       "cl /nologo /O2 /W3 /D_CRT_SECURE_NO_WARNINGS /LD /MT `"$src\hercules_hook.c`" /Fe:hercules_hook.dll user32.lib gdi32.lib && " +
       "cl /nologo /O2 /W3 /D_CRT_SECURE_NO_WARNINGS /MT `"$src\herc_inject.c`" /Fe:herc_inject.exe user32.lib gdi32.lib"
cmd /c $cmd
if ($LASTEXITCODE) { throw "build failed" }
Get-ChildItem $bin -Include *.dll, *.exe -Recurse | ForEach-Object { "$($_.Name) $($_.Length)" }
