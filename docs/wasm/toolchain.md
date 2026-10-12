# Emscripten toolchain

Decision **D-W0-2**: the user approved installing the SDK on 2026-10-13. Emscripten **6.0.12** is installed at
`C:\Users\Thanh\emsdk`, outside the game folder, and permanently activated for the current Windows user.
The bundled tools are Node.js **24.19.0** and Python **3.13.3**. Existing user PATH entries were preserved.

Use version `6.0.12` for this project rather than the moving `latest` alias. Installation follows the
[official Emscripten instructions](https://emscripten.org/docs/getting_started/downloads.html).

## Use from PowerShell

Load the SDK environment into an existing terminal before building:

```powershell
& "$env:USERPROFILE\emsdk\emsdk_env.ps1"
emcc --version
em++ --version
```

The SDK directory and compiler directory are also registered in the user's persistent PATH. Restart the
terminal application to pick up those changes, or use the activation command above immediately.
The SDK uses `EMSDK_NODE` and `EMSDK_PYTHON` for its bundled runtimes; project Python tools still use
`.venv/Scripts/python.exe` as before.

## Install on another Windows machine

With Git and Python 3.8 or newer available, run:

```powershell
git clone --depth 1 https://github.com/emscripten-core/emsdk.git "$env:USERPROFILE\emsdk"
& "$env:USERPROFILE\emsdk\emsdk.bat" install 6.0.12
& "$env:USERPROFILE\emsdk\emsdk.bat" activate --permanent 6.0.12
& "$env:USERPROFILE\emsdk\emsdk_env.ps1"
```

## Installation check (2026-10-13)

`emcc` and `em++` reported version 6.0.12. C11 and C++17 programs compiled to WebAssembly and ran successfully
with the bundled Node.js. The C build also generated browser HTML using WebGL2 settings and a fixed 256 MB
heap. Browser execution of the game remains a later porting task.
