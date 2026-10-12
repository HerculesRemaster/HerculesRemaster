# xBRZ (vendored source)

`xbrz.cpp`, `xbrz.h`, `config.h`: xBRZ by Zenju (HqMAME project), GPL-3.0, from PPSSPP `ext/xbrz`
(github.com/hrydgard/ppsspp, last commit touching `ext/xbrz`: `bb6164e79934661fe6dcd08c41504b6d80de7135`,
2026-07-24). Fetched 2026-10-12 with the user's OK (xBRZ route, `docs/authorization-scope.md`).

| file | sha256 |
|---|---|
| `xbrz.cpp` | `35283301d48ff518c8394c674195d086325697a51bf4d72213e636f1df213c7e` (as fetched) |
| `config.h` | `bbb7d3ab1c25851673a71c126188cc1f02dc483819f04af8a8241dac71e9fe1a` (as fetched) |
| `xbrz.h` | local change: includes `<cstdint>` directly instead of PPSSPP's `ppsspp_config.h` |

`wrap.cpp` exports `xbrz_scale` for `tools/hires/xbrz_index.py`. `build.bat` builds `xbrz.dll` (x64) with the
latest Visual Studio found by vswhere; `xbrz_index.py` runs it automatically when the DLL is missing. The DLL
is a build output (gitignored).

xBRZ is an offline asset tool here. It is not linked into or shipped with the game runtime.
