/* ddraw.dll proxy for Disney's Hercules (HERCULES.EXE, x86) - borderless 4K fullscreen.
 *
 * Placed next to HERCULES.EXE in working/game/, so the loader picks it before the system ddraw.
 * DirectDrawCreate is forwarded to the system ddraw.dll. On attach the proxy:
 *   - reads hercules_display.ini (same folder; env HERC_DISPLAY_MODE overrides `mode`, used by the
 *     trace tools), optionally makes the process per-monitor DPI aware;
 *   - keeps the game on its windowed GDI path: GFX_RequestFullscreen (EXE 0x6ec0) always stores 0,
 *     so the broken SetDisplayMode(320,240,8) switch (GFX_SwitchDisplayMode 0x6bb0) never runs;
 *   - redirects the game's Alt+Enter / "320x240 Fullscreen" command (GFX_CmdFullscreen 0x9990)
 *     to a borderless <-> windowed toggle;
 *   - IAT-patches HERCULES.EXE: CreateWindowExA (find the main frame), SetWindowPos and
 *     AdjustWindowRectEx (keep our window rectangle), StretchDIBits (scale the 320x240 8-bit
 *     frame into the configured rectangle, fill the bars), SetMenu, ShowCursor, and WINMM
 *     mciSendStringA when a CD track file exists (cd_audio.cpp: music without the disc), and with
 *     portable = on the ADVAPI32 registry calls for the game's HKLM key (served from an ini).
 * Game timing is untouched (its own 33 ms tick). Addresses are HERCULES.EXE RVAs + 0x400000.
 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ---------------- config ---------------- */
enum { MODE_BORDERLESS, MODE_WINDOWED, MODE_EXCLUSIVE };
enum { SCALE_INTEGER, SCALE_FIT, SCALE_STRETCH };
static struct {
    int mode, monitor, scale, linear, aspect169, window_scale, vsync, dpi_aware, hide_cursor, log, gpu, dev, art_dump, music_volume, portable, smooth;
    char art_dir[MAX_PATH], art_families[128];
    COLORREF border;
} cfg = { MODE_BORDERLESS, 0, SCALE_INTEGER, 0, 0, 3, 1, 1, 1, 0, 0, 0, RGB(0, 0, 0) };

static char g_dir[MAX_PATH];
static FILE *g_log;

static void logf_(const char *fmt, ...) {
    va_list ap;
    if (!g_log) return;
    va_start(ap, fmt); vfprintf(g_log, fmt, ap); va_end(ap);
    fputc('\n', g_log); fflush(g_log);
}

static void ini_str(const char *key, const char *def, char *out, int n) {
    char path[MAX_PATH], *p;
    _snprintf(path, MAX_PATH, "%s\\hercules_display.ini", g_dir);
    GetPrivateProfileStringA("display", key, def, out, n, path);
    if ((p = strchr(out, ';')) != NULL) *p = 0;           /* inline comment */
    for (p = out + strlen(out); p > out && (p[-1] == ' ' || p[-1] == '\t'); ) *--p = 0;
    for (p = out; *p == ' ' || *p == '\t'; p++) {}
    memmove(out, p, strlen(p) + 1);
}

static int ini_on(const char *key, int def) {
    char b[32];
    ini_str(key, def ? "on" : "off", b, sizeof b);
    return !_stricmp(b, "on") || !_stricmp(b, "1") || !_stricmp(b, "true") || !_stricmp(b, "yes");
}

static void load_config(void) {
    char b[64];
    ini_str("mode", "borderless", b, sizeof b);
    { char e[32]; if (GetEnvironmentVariableA("HERC_DISPLAY_MODE", e, sizeof e)) strcpy(b, e); }  /* tools override */
    cfg.mode = !_stricmp(b, "windowed") ? MODE_WINDOWED : !_stricmp(b, "exclusive") ? MODE_EXCLUSIVE : MODE_BORDERLESS;
    ini_str("monitor", "primary", b, sizeof b);
    cfg.monitor = !_stricmp(b, "primary") ? 0 : atoi(b);
    ini_str("scale", "integer", b, sizeof b);
    cfg.scale = !_stricmp(b, "fit") ? SCALE_FIT : !_stricmp(b, "stretch") ? SCALE_STRETCH : SCALE_INTEGER;
    ini_str("filter", "nearest", b, sizeof b);
    cfg.linear = !_stricmp(b, "linear");
    ini_str("aspect", "4:3", b, sizeof b);
    cfg.aspect169 = !strcmp(b, "16:9");
    ini_str("border_color", "000000", b, sizeof b);
    { unsigned v = strtoul(b, NULL, 16); cfg.border = RGB((v >> 16) & 255, (v >> 8) & 255, v & 255); }
    ini_str("window_scale", "3", b, sizeof b);
    cfg.window_scale = atoi(b) > 0 ? atoi(b) : 3;
    cfg.vsync = ini_on("vsync", 1);
    cfg.dpi_aware = ini_on("dpi_aware", 1);
    cfg.hide_cursor = ini_on("hide_cursor", 1);
    cfg.log = ini_on("log", 0) || GetEnvironmentVariableA("HERC_DISPLAY_LOG", b, sizeof b);
    ini_str("renderer", "gdi", b, sizeof b);
    { char e[32]; if (GetEnvironmentVariableA("HERC_RENDERER", e, sizeof e)) strcpy(b, e); }
    cfg.gpu = !_stricmp(b, "gpu");
    cfg.dev = ini_on("dev", 0) || GetEnvironmentVariableA("HERC_DEV", b, sizeof b);
    ini_str("art_dir", "art", b, sizeof b);
    { char e[MAX_PATH]; if (GetEnvironmentVariableA("HERC_ART_DIR", e, sizeof e)) strcpy(b, _stricmp(e, "none") ? e : ""); }  /* tools; none = no art */
    if (b[0]) _snprintf(cfg.art_dir, MAX_PATH, "%s\\%s", g_dir, b);
    ini_str("art_families", "sprite,level,ui,screen,smooth", cfg.art_families, sizeof cfg.art_families);
    ini_str("music_volume", "100", b, sizeof b);
    cfg.music_volume = atoi(b);
    cfg.art_dump = ini_on("art_dump", 0) || GetEnvironmentVariableA("HERC_ART_DUMP", b, sizeof b);
    ini_str("smooth", "off", b, sizeof b);
    { char e[16]; if (GetEnvironmentVariableA("HERC_SMOOTH", e, sizeof e)) strcpy(b, e); }
    cfg.smooth = !_stricmp(b, "xbr");
    cfg.portable = ini_on("portable", 0) || GetEnvironmentVariableA("HERC_PORTABLE", b, sizeof b);
}

/* ---------------- monitor / rectangles ---------------- */
typedef struct { int want, n; RECT r; } MonFind;
static BOOL CALLBACK mon_cb(HMONITOR m, HDC dc, LPRECT r, LPARAM lp) {
    MonFind *f = (MonFind *)lp;
    (void)m; (void)dc;
    if (++f->n == f->want) { f->r = *r; return FALSE; }
    return TRUE;
}

static RECT monitor_rect(BOOL work) {
    RECT r = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    MONITORINFO mi = { sizeof mi };
    if (cfg.monitor > 0) {
        MonFind f = { cfg.monitor, 0, r };
        EnumDisplayMonitors(NULL, NULL, mon_cb, (LPARAM)&f);
        r = f.r;
    }
    if (GetMonitorInfoA(MonitorFromRect(&r, MONITOR_DEFAULTTOPRIMARY), &mi)) r = work ? mi.rcWork : mi.rcMonitor;
    return r;
}

/* Image rectangle inside a client area of cw x ch. */
static RECT image_rect(int cw, int ch) {
    RECT d;
    int w = cw, h = ch;
    if (cfg.scale == SCALE_INTEGER) {
        int n = min(cw / 320, ch / 240);
        if (n < 1) n = 1;
        w = 320 * n; h = 240 * n;
    } else if (cfg.scale == SCALE_FIT || !cfg.aspect169) {
        if (cfg.scale != SCALE_STRETCH) {
            if (cw * 3 > ch * 4) { h = ch; w = ch * 4 / 3; } else { w = cw; h = cw * 3 / 4; }
        }
    }
    d.left = (cw - w) / 2; d.top = (ch - h) / 2; d.right = d.left + w; d.bottom = d.top + h;
    return d;
}

/* ---------------- patching helpers ---------------- */
static void patch(DWORD addr, const void *bytes, int n) {
    DWORD old;
    VirtualProtect((void *)addr, n, PAGE_EXECUTE_READWRITE, &old);
    memcpy((void *)addr, bytes, n);
    VirtualProtect((void *)addr, n, old, &old);
    FlushInstructionCache(GetCurrentProcess(), (void *)addr, n);
}

static void *iat_hook(const char *dll, const char *fn, void *hook) {
    BYTE *base = (BYTE *)GetModuleHandleA(NULL);
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);
    IMAGE_IMPORT_DESCRIPTOR *d = (IMAGE_IMPORT_DESCRIPTOR *)(base +
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    for (; d->Name; d++) {
        IMAGE_THUNK_DATA *names, *iat;
        if (_stricmp((char *)(base + d->Name), dll)) continue;
        names = (IMAGE_THUNK_DATA *)(base + (d->OriginalFirstThunk ? d->OriginalFirstThunk : d->FirstThunk));
        iat = (IMAGE_THUNK_DATA *)(base + d->FirstThunk);
        for (; names->u1.AddressOfData; names++, iat++) {
            DWORD old;
            void *orig;
            if (names->u1.Ordinal & IMAGE_ORDINAL_FLAG) continue;
            if (strcmp((char *)((IMAGE_IMPORT_BY_NAME *)(base + names->u1.AddressOfData))->Name, fn)) continue;
            orig = (void *)iat->u1.Function;
            VirtualProtect(&iat->u1.Function, 4, PAGE_READWRITE, &old);
            iat->u1.Function = (DWORD)hook;
            VirtualProtect(&iat->u1.Function, 4, old, &old);
            return orig;
        }
    }
    logf_("IAT miss: %s!%s", dll, fn);
    return NULL;
}

/* ---------------- portable registry ----------------
 * Only HERCULES.EXE uses the registry (ENG1/ENG3/TITLE import no ADVAPI32), all on
 * HKLM\Software\Disney Interactive\Hercules\1.00:
 *   0x4a30  RegOpenKeyExA + RegQueryValueExA "Path" -> SetCurrentDirectoryA; no key = "Hercules is not
 *           installed" and exit (window setup 0x7670);
 *   0x4ce0  RegQueryValueExA "Config" (0x48 bytes) -> 0xb9e858; missing or invalid = defaults (0x4be0);
 *   0x4da0  RegCreateKeyExA + RegSetValueExA "Config" (REG_BINARY, 0x48 bytes).
 * With portable = on that key never reaches the registry: Path is this folder and Config is kept as hex in
 * hercules_settings.ini next to HERCULES.EXE, so the folder runs from anywhere without admin rights. */
#define PORT_KEY ((HKEY)(ULONG_PTR)0x48455243)
static const char port_subkey[] = "Software\\Disney Interactive\\Hercules\\1.00";
typedef LSTATUS(WINAPI *RegOpenKeyExA_t)(HKEY, LPCSTR, DWORD, REGSAM, PHKEY);
typedef LSTATUS(WINAPI *RegCreateKeyExA_t)(HKEY, LPCSTR, DWORD, LPSTR, DWORD, REGSAM, const LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
typedef LSTATUS(WINAPI *RegQueryValueExA_t)(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
typedef LSTATUS(WINAPI *RegSetValueExA_t)(HKEY, LPCSTR, DWORD, DWORD, const BYTE *, DWORD);
typedef LSTATUS(WINAPI *RegCloseKey_t)(HKEY);
static RegOpenKeyExA_t o_RegOpenKeyExA;
static RegCreateKeyExA_t o_RegCreateKeyExA;
static RegQueryValueExA_t o_RegQueryValueExA;
static RegSetValueExA_t o_RegSetValueExA;
static RegCloseKey_t o_RegCloseKey;

static int port_is_key(HKEY root, LPCSTR sub) { return root == HKEY_LOCAL_MACHINE && sub && !_stricmp(sub, port_subkey); }
static void port_ini(char *path) { _snprintf(path, MAX_PATH, "%s\\hercules_settings.ini", g_dir); }

static LSTATUS WINAPI h_RegOpenKeyExA(HKEY root, LPCSTR sub, DWORD opt, REGSAM sam, PHKEY out) {
    if (!port_is_key(root, sub)) return o_RegOpenKeyExA(root, sub, opt, sam, out);
    *out = PORT_KEY;
    return ERROR_SUCCESS;
}
static LSTATUS WINAPI h_RegCreateKeyExA(HKEY root, LPCSTR sub, DWORD r, LPSTR cls, DWORD opt, REGSAM sam,
                                        const LPSECURITY_ATTRIBUTES sa, PHKEY out, LPDWORD disp) {
    if (!port_is_key(root, sub)) return o_RegCreateKeyExA(root, sub, r, cls, opt, sam, sa, out, disp);
    *out = PORT_KEY;
    if (disp) *disp = REG_OPENED_EXISTING_KEY;
    return ERROR_SUCCESS;
}
static LSTATUS port_return(const void *data, DWORD n, DWORD type, LPDWORD ptype, LPBYTE out, LPDWORD cb) {
    DWORD room = cb ? *cb : 0;
    if (ptype) *ptype = type;
    if (cb) *cb = n;
    if (!out) return ERROR_SUCCESS;
    if (room < n) return ERROR_MORE_DATA;
    memcpy(out, data, n);
    return ERROR_SUCCESS;
}
static LSTATUS WINAPI h_RegQueryValueExA(HKEY k, LPCSTR name, LPDWORD r, LPDWORD type, LPBYTE data, LPDWORD cb) {
    char path[MAX_PATH], hex[0x48 * 2 + 8];
    BYTE bin[0x48];
    int i;
    if (k != PORT_KEY) return o_RegQueryValueExA(k, name, r, type, data, cb);
    logf_("portable: query %s", name ? name : "(default)");
    if (name && !_stricmp(name, "Path")) {
        char dir[MAX_PATH + 1];
        _snprintf(dir, sizeof dir, "%s\\", g_dir);
        return port_return(dir, (DWORD)strlen(dir) + 1, REG_SZ, type, data, cb);
    }
    if (!name || _stricmp(name, "Config")) return ERROR_FILE_NOT_FOUND;
    port_ini(path);
    if (GetPrivateProfileStringA("registry", "Config", "", hex, sizeof hex, path) != 0x48 * 2) return ERROR_FILE_NOT_FOUND;
    for (i = 0; i < 0x48; i++) {
        unsigned v;
        if (sscanf(hex + i * 2, "%2x", &v) != 1) return ERROR_FILE_NOT_FOUND;
        bin[i] = (BYTE)v;
    }
    return port_return(bin, 0x48, REG_BINARY, type, data, cb);
}
static LSTATUS WINAPI h_RegSetValueExA(HKEY k, LPCSTR name, DWORD r, DWORD type, const BYTE *data, DWORD cb) {
    char path[MAX_PATH], hex[0x48 * 2 + 8];
    DWORD i;
    if (k != PORT_KEY) return o_RegSetValueExA(k, name, r, type, data, cb);
    if (!name || _stricmp(name, "Config") || cb != 0x48) return ERROR_SUCCESS;   /* the game writes nothing else */
    for (i = 0; i < cb; i++) sprintf(hex + i * 2, "%02x", data[i]);
    port_ini(path);
    logf_("portable: save Config");
    WritePrivateProfileStringA("registry", "Config", hex, path);
    return ERROR_SUCCESS;
}
static LSTATUS WINAPI h_RegCloseKey(HKEY k) { return k == PORT_KEY ? ERROR_SUCCESS : o_RegCloseKey(k); }

static void port_install(void) {
    o_RegOpenKeyExA = (RegOpenKeyExA_t)iat_hook("ADVAPI32.dll", "RegOpenKeyExA", h_RegOpenKeyExA);
    o_RegCreateKeyExA = (RegCreateKeyExA_t)iat_hook("ADVAPI32.dll", "RegCreateKeyExA", h_RegCreateKeyExA);
    o_RegQueryValueExA = (RegQueryValueExA_t)iat_hook("ADVAPI32.dll", "RegQueryValueExA", h_RegQueryValueExA);
    o_RegSetValueExA = (RegSetValueExA_t)iat_hook("ADVAPI32.dll", "RegSetValueExA", h_RegSetValueExA);
    o_RegCloseKey = (RegCloseKey_t)iat_hook("ADVAPI32.dll", "RegCloseKey", h_RegCloseKey);
    if (!o_RegOpenKeyExA) o_RegOpenKeyExA = RegOpenKeyExA;
    if (!o_RegCreateKeyExA) o_RegCreateKeyExA = RegCreateKeyExA;
    if (!o_RegQueryValueExA) o_RegQueryValueExA = RegQueryValueExA;
    if (!o_RegSetValueExA) o_RegSetValueExA = RegSetValueExA;
    if (!o_RegCloseKey) o_RegCloseKey = RegCloseKey;
    logf_("portable: game registry key served from %s\\hercules_settings.ini", g_dir);
}

/* ---------------- CD audio (cd_audio.cpp) ---------------- */
int cd_audio_init(const char *dir, int vol);
unsigned cd_audio_command(const char *cmd, char *ret, unsigned len);
void cd_log(const char *msg) { logf_("%s", msg); }
typedef MCIERROR(WINAPI *mciSendStringA_t)(LPCSTR, LPSTR, UINT, HWND);
static mciSendStringA_t o_mciSendStringA;
static MCIERROR WINAPI h_mciSendStringA(LPCSTR cmd, LPSTR ret, UINT len, HWND cb) {
    if (cmd && strstr(cmd, "cdaudio")) return cd_audio_command(cmd, ret, len);
    return o_mciSendStringA(cmd, ret, len, cb);
}

static LARGE_INTEGER g_freq, g_fpsT0;
static int g_frames;

/* ---------------- window management ---------------- */
typedef HWND (WINAPI *CreateWindowExA_t)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
typedef BOOL (WINAPI *SetWindowPos_t)(HWND, HWND, int, int, int, int, UINT);
typedef BOOL (WINAPI *AdjustWindowRectEx_t)(LPRECT, DWORD, BOOL, DWORD);
typedef int (WINAPI *StretchDIBits_t)(HDC, int, int, int, int, int, int, int, int, const void *, const BITMAPINFO *, UINT, DWORD);
typedef BOOL (WINAPI *SetMenu_t)(HWND, HMENU);
typedef int (WINAPI *ShowCursor_t)(BOOL);
static CreateWindowExA_t o_CreateWindowExA;
static SetWindowPos_t o_SetWindowPos;
static AdjustWindowRectEx_t o_AdjustWindowRectEx;
static StretchDIBits_t o_StretchDIBits;
static SetMenu_t o_SetMenu;
static ShowCursor_t o_ShowCursor;

static HWND g_main;
static HMENU g_menu;
static DWORD g_style, g_exstyle;
static WNDPROC g_prevProc;
static int g_cur;             /* current mode (borderless / windowed) */
static HRESULT (WINAPI *p_DwmFlush)(void);

static void apply_mode(void) {
    RECT m;
    if (!g_main) return;
    if (g_cur == MODE_BORDERLESS) {
        m = monitor_rect(FALSE);
        if (GetMenu(g_main)) { g_menu = GetMenu(g_main); o_SetMenu(g_main, NULL); }
        SetWindowLongA(g_main, GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN);
        SetWindowLongA(g_main, GWL_EXSTYLE, WS_EX_APPWINDOW);
        o_SetWindowPos(g_main, HWND_TOP, m.left, m.top, m.right - m.left, m.bottom - m.top,
                       SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOOWNERZORDER);
        logf_("borderless %ld,%ld %ldx%ld", m.left, m.top, m.right - m.left, m.bottom - m.top);
    } else {
        RECT r;
        int cw = 320 * cfg.window_scale, ch = 240 * cfg.window_scale;
        m = monitor_rect(TRUE);
        if (g_menu && !GetMenu(g_main)) o_SetMenu(g_main, g_menu);
        SetWindowLongA(g_main, GWL_STYLE, (g_style ? g_style : WS_OVERLAPPEDWINDOW) | WS_VISIBLE);
        SetWindowLongA(g_main, GWL_EXSTYLE, g_exstyle);
        r.left = 0; r.top = 0; r.right = cw; r.bottom = ch;
        o_AdjustWindowRectEx(&r, (DWORD)GetWindowLongA(g_main, GWL_STYLE), GetMenu(g_main) != NULL, g_exstyle);
        o_SetWindowPos(g_main, HWND_NOTOPMOST,
                       m.left + ((m.right - m.left) - (r.right - r.left)) / 2,
                       m.top + ((m.bottom - m.top) - (r.bottom - r.top)) / 2,
                       r.right - r.left, r.bottom - r.top, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
        logf_("windowed client %dx%d", cw, ch);
    }
    InvalidateRect(g_main, NULL, TRUE);
}

static LRESULT CALLBACK main_proc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_SETCURSOR && LOWORD(lp) == HTCLIENT && g_cur == MODE_BORDERLESS && cfg.hide_cursor) {
        SetCursor(NULL);
        return TRUE;
    }
    if (msg == WM_ERASEBKGND) {                      /* bars are painted with the frame */
        RECT r; GetClientRect(h, &r);
        { HBRUSH b = CreateSolidBrush(cfg.border); FillRect((HDC)wp, &r, b); DeleteObject(b); }
        return 1;
    }
    return CallWindowProcA(g_prevProc, h, msg, wp, lp);
}

static void attach_main(HWND h, DWORD style, DWORD exstyle) {
    g_main = h;
    g_style = style;
    g_exstyle = exstyle;
    g_prevProc = (WNDPROC)SetWindowLongA(h, GWL_WNDPROC, (LONG)main_proc);
    logf_("main window %p style %08lx ex %08lx", (void *)h, style, exstyle);
}

static HWND WINAPI h_CreateWindowExA(DWORD ex, LPCSTR cls, LPCSTR name, DWORD style, int x, int y, int w, int hgt,
                                     HWND parent, HMENU menu, HINSTANCE inst, LPVOID param) {
    HWND h = o_CreateWindowExA(ex, cls, name, style, x, y, w, hgt, parent, menu, inst, param);
    if (h && !g_main && !parent && (style & WS_CAPTION) == WS_CAPTION) {
        attach_main(h, style, ex);
        apply_mode();
    }
    return h;
}

/* The game sizes its own window (GFX_SetWindowClient 0x99a0, MFC frame code): keep ours. */
static BOOL WINAPI h_SetWindowPos(HWND h, HWND after, int x, int y, int cx, int cy, UINT f) {
    if (h == g_main && !(f & SWP_NOSIZE && f & SWP_NOMOVE)) {
        BOOL r = o_SetWindowPos(h, after, x, y, cx, cy, f | SWP_NOMOVE | SWP_NOSIZE);
        return r;
    }
    return o_SetWindowPos(h, after, x, y, cx, cy, f);
}

static BOOL WINAPI h_SetMenu(HWND h, HMENU m) {
    if (h == g_main && g_cur == MODE_BORDERLESS) { if (m) g_menu = m; return TRUE; }
    return o_SetMenu(h, m);
}

static int WINAPI h_ShowCursor(BOOL show) {
    if (g_cur == MODE_BORDERLESS && cfg.hide_cursor) return show ? 0 : -1;
    return o_ShowCursor(show);
}

static BOOL WINAPI h_AdjustWindowRectEx(LPRECT r, DWORD style, BOOL menu, DWORD ex) {
    return o_AdjustWindowRectEx(r, style, menu, ex);
}

/* ---------------- GPU renderer (gpu_renderer.cpp) ---------------- */
void gpu_draw_ot(void *ot);
int gpu_present(HWND h);
void gpu_set_dev(int on);
void gpu_set_smooth(int on);
void gpu_last_file(const char *name, int index);
void gpu_set_art(const char *dir, int dump, const char *families);
typedef int (__cdecl *FsOpen_t)(const char *, const char *);
static FsOpen_t o_FsOpen;
/* FS_Open (EXE 0x405130): remember the last archive file for the dev overlay. */
static int __cdecl h_FsOpen(const char *name, const char *mode) {
    char copy[160];
    int h;
    strncpy(copy, name ? name : "", sizeof copy - 1); copy[sizeof copy - 1] = 0;
    h = o_FsOpen(name, mode);
    gpu_last_file(copy, h - 1);
    return h;
}
typedef void (__cdecl *DrawOTag_t)(void *);
static DrawOTag_t o_DrawOTag;

static void *detour(DWORD target, void *hook, int stolen) {
    BYTE *tramp = (BYTE *)VirtualAlloc(NULL, 32, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    DWORD old;
    int i;
    if (*(BYTE *)target == 0xE9) {               /* already detoured by someone else: chain to its target */
        DWORD dest = target + 5 + *(DWORD *)(target + 1);
        tramp[0] = 0xE9;
        *(DWORD *)(tramp + 1) = dest - ((DWORD)tramp + 5);
    } else {
        memcpy(tramp, (void *)target, stolen);
        tramp[stolen] = 0xE9;
        *(DWORD *)(tramp + stolen + 1) = (target + stolen) - ((DWORD)tramp + stolen + 5);
    }
    VirtualProtect((void *)target, stolen, PAGE_EXECUTE_READWRITE, &old);
    *(BYTE *)target = 0xE9;
    *(DWORD *)(target + 1) = (DWORD)hook - (target + 5);
    for (i = 5; i < stolen; i++) ((BYTE *)target)[i] = 0x90;
    VirtualProtect((void *)target, stolen, old, &old);
    FlushInstructionCache(GetCurrentProcess(), (void *)target, stolen);
    return tramp;
}

/* GFX_DrawOTag (EXE 0x40ac60): collect the primitive list for the GPU, then let the original run. */
static void __cdecl h_DrawOTag(void *ot) {
    if (!*(volatile BYTE *)0x00ba01fc) gpu_draw_ot(ot);   /* g_skipRender: catch-up tick, nothing drawn */
    o_DrawOTag(ot);
}

/* ---------------- present ---------------- */

static int WINAPI h_StretchDIBits(HDC dc, int xd, int yd, int wd, int hd, int xs, int ys, int ws, int hs,
                                  const void *bits, const BITMAPINFO *bmi, UINT usage, DWORD rop) {
    RECT c, d;
    int r;
    if (!g_main || WindowFromDC(dc) != g_main || ws != 320 || hs != 240)
        return o_StretchDIBits(dc, xd, yd, wd, hd, xs, ys, ws, hs, bits, bmi, usage, rop);
    if (cfg.gpu && gpu_present(g_main)) {
        if (g_log) {
            LARGE_INTEGER t;
            QueryPerformanceCounter(&t);
            if (!g_frames++) g_fpsT0 = t;
            if (t.QuadPart - g_fpsT0.QuadPart >= 5 * g_freq.QuadPart) {
                logf_("gpu present %.1f fps", (g_frames - 1) / ((double)(t.QuadPart - g_fpsT0.QuadPart) / (double)g_freq.QuadPart));
                g_frames = 1; g_fpsT0 = t;
            }
        }
        return hs;
    }
    GetClientRect(g_main, &c);
    d = image_rect(c.right, c.bottom);
    SetStretchBltMode(dc, cfg.linear ? HALFTONE : COLORONCOLOR);
    if (cfg.linear) SetBrushOrgEx(dc, 0, 0, NULL);
    r = o_StretchDIBits(dc, d.left, d.top, d.right - d.left, d.bottom - d.top, xs, ys, ws, hs, bits, bmi, usage, rop);
    if (d.left > 0 || d.top > 0) {
        HBRUSH b = CreateSolidBrush(cfg.border);
        RECT bar;
        SetRect(&bar, 0, 0, d.left, c.bottom); FillRect(dc, &bar, b);
        SetRect(&bar, d.right, 0, c.right, c.bottom); FillRect(dc, &bar, b);
        SetRect(&bar, d.left, 0, d.right, d.top); FillRect(dc, &bar, b);
        SetRect(&bar, d.left, d.bottom, d.right, c.bottom); FillRect(dc, &bar, b);
        DeleteObject(b);
    }
    GdiFlush();
    if (cfg.vsync && p_DwmFlush) p_DwmFlush();
    if (g_log) {
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);
        if (!g_frames++) g_fpsT0 = t;
        if (t.QuadPart - g_fpsT0.QuadPart >= 5 * g_freq.QuadPart) {
            double s = (double)(t.QuadPart - g_fpsT0.QuadPart) / (double)g_freq.QuadPart;
            logf_("present %.1f fps, client %ldx%ld, image %ld,%ld %ldx%ld", (g_frames - 1) / s,
                  c.right, c.bottom, d.left, d.top, d.right - d.left, d.bottom - d.top);
            g_frames = 1; g_fpsT0 = t;
        }
    }
    return r;
}

/* GFX_CmdFullscreen (0x9990, menu 32800 / Alt+Enter) -> toggle borderless <-> windowed. */
static void __fastcall cmd_toggle(void *frame) {
    (void)frame;
    g_cur = g_cur == MODE_BORDERLESS ? MODE_WINDOWED : MODE_BORDERLESS;
    apply_mode();
}

/* ---------------- DirectDraw forward ---------------- */
static HMODULE g_sysddraw;
typedef HRESULT (WINAPI *DirectDrawCreate_t)(GUID *, void **, IUnknown *);

HRESULT WINAPI DirectDrawCreate(GUID *guid, void **dd, IUnknown *outer) {
    static DirectDrawCreate_t real;
    if (!real) {
        char path[MAX_PATH];
        UINT n = GetSystemWow64DirectoryA(path, MAX_PATH);
        if (!n) n = GetSystemDirectoryA(path, MAX_PATH);
        strcat(path, "\\ddraw.dll");
        g_sysddraw = LoadLibraryA(path);
        real = g_sysddraw ? (DirectDrawCreate_t)GetProcAddress(g_sysddraw, "DirectDrawCreate") : NULL;
        logf_("system ddraw %s -> %p (proxy base %p)", path, (void *)real, (void *)GetModuleHandleA("ddraw.dll"));
        if (!real) return E_FAIL;
    }
    return real(guid, dd, outer);
}

/* ---------------- attach ---------------- */
static void install(HINSTANCE self) {
    char *p;
    GetModuleFileNameA(self, g_dir, MAX_PATH);
    if ((p = strrchr(g_dir, '\\')) != NULL) *p = 0;
    load_config();
    if (cfg.log) {
        char path[MAX_PATH];
        _snprintf(path, MAX_PATH, "%s\\hercules_display.log", g_dir);
        g_log = fopen(path, "w");
    }
    QueryPerformanceFrequency(&g_freq);
    logf_("hercules ddraw proxy: mode=%d scale=%d linear=%d monitor=%d window_scale=%d vsync=%d dpi=%d",
          cfg.mode, cfg.scale, cfg.linear, cfg.monitor, cfg.window_scale, cfg.vsync, cfg.dpi_aware);
    if (cfg.portable) port_install();                /* before the EXE's entry point reads the key */
    if (cfg.mode == MODE_EXCLUSIVE) return;          /* original DirectDraw path, debugging only */

    if (cfg.dpi_aware) {
        typedef BOOL (WINAPI *SetCtx_t)(HANDLE);
        SetCtx_t f = (SetCtx_t)GetProcAddress(GetModuleHandleA("user32.dll"), "SetProcessDpiAwarenessContext");
        BOOL ok = f ? f((HANDLE)-4) : SetProcessDPIAware();   /* PER_MONITOR_AWARE_V2 */
        logf_("dpi awareness: %d", ok);
    }
    {
        HMODULE dwm = LoadLibraryA("dwmapi.dll");
        p_DwmFlush = dwm ? (HRESULT (WINAPI *)(void))GetProcAddress(dwm, "DwmFlush") : NULL;
    }
    g_cur = cfg.mode;

    /* Verify the EXE before patching code (HERCULES.EXE 1997-08-20 build). */
    if (memcmp((void *)0x406ec0, "\x8a\x44\x24\x04\xa2\xd0\xe3\x96\x00", 9) ||
        memcmp((void *)0x409990, "\x6a\x01\xe8\x29\xd5\xff\xff", 7)) {
        logf_("unexpected HERCULES.EXE code at 0x406ec0/0x409990, code patches skipped");
    } else {
        static const BYTE no_fs[] = { 0x30, 0xC0, 0xA2, 0xD0, 0xE3, 0x96, 0x00, 0x90, 0x90 }; /* xor al,al; mov [g_wantFullscreen],al */
        BYTE jmp[5] = { 0xE9 };
        patch(0x406ec0, no_fs, sizeof no_fs);
        *(DWORD *)(jmp + 1) = (DWORD)cmd_toggle - (0x409990 + 5);
        patch(0x409990, jmp, 5);
    }
    o_CreateWindowExA = (CreateWindowExA_t)iat_hook("USER32.dll", "CreateWindowExA", h_CreateWindowExA);
    o_SetWindowPos = (SetWindowPos_t)iat_hook("USER32.dll", "SetWindowPos", h_SetWindowPos);
    o_AdjustWindowRectEx = (AdjustWindowRectEx_t)iat_hook("USER32.dll", "AdjustWindowRectEx", h_AdjustWindowRectEx);
    o_SetMenu = (SetMenu_t)iat_hook("USER32.dll", "SetMenu", h_SetMenu);
    o_ShowCursor = (ShowCursor_t)iat_hook("USER32.dll", "ShowCursor", h_ShowCursor);
    o_StretchDIBits = (StretchDIBits_t)iat_hook("GDI32.dll", "StretchDIBits", h_StretchDIBits);
    if (cd_audio_init(g_dir, cfg.music_volume))
        o_mciSendStringA = (mciSendStringA_t)iat_hook("WINMM.dll", "mciSendStringA", h_mciSendStringA);
    if (cfg.gpu) {
        if (!memcmp((void *)0x40ac60, "\xa0\xfc\x01\xba\x00", 5)) {
            o_DrawOTag = (DrawOTag_t)detour(0x40ac60, h_DrawOTag, 5);
            logf_("gpu renderer: GFX_DrawOTag hooked");
            if (cfg.art_dir[0]) { gpu_set_art(cfg.art_dir, cfg.art_dump, cfg.art_families); logf_("art dir %s [%s]%s", cfg.art_dir, cfg.art_families, cfg.art_dump ? " (dump)" : ""); }
            {   /* last archive file: dev overlay and snapshot labels */
                char e[8]; int shots = GetEnvironmentVariableA("HERC_GPU_SHOT", e, sizeof e) != 0;
                if ((cfg.dev || shots) && !memcmp((void *)0x405130, "\x81\xec\x00\x04\x00\x00", 6))
                    o_FsOpen = (FsOpen_t)detour(0x405130, h_FsOpen, 6);
                if (cfg.dev) { gpu_set_dev(1); logf_("dev overlay on"); }
                if (cfg.smooth) { gpu_set_smooth(1); logf_("smoothing: xbr"); }
            }
        } else {
            cfg.gpu = 0;
            logf_("gpu renderer: unexpected code at 0x40ac60, staying on GDI");
        }
    }
    if (!o_CreateWindowExA) o_CreateWindowExA = CreateWindowExA;
    if (!o_SetWindowPos) o_SetWindowPos = SetWindowPos;
    if (!o_AdjustWindowRectEx) o_AdjustWindowRectEx = AdjustWindowRectEx;
    if (!o_SetMenu) o_SetMenu = SetMenu;
    if (!o_ShowCursor) o_ShowCursor = ShowCursor;
    if (!o_StretchDIBits) o_StretchDIBits = StretchDIBits;
}

/* log = on: name the module of the first access violation (the game's own handler only reports the address). */
static LONG CALLBACK av_logger(EXCEPTION_POINTERS *e) {
    static LONG once;
    if (e->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && !InterlockedExchange(&once, 1)) {
        HMODULE m = NULL; char name[MAX_PATH] = "?";
        void *a = e->ExceptionRecord->ExceptionAddress;
        if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)a, &m))
            GetModuleFileNameA(m, name, MAX_PATH);
        logf_("access violation at %p = %s+0x%x thread %lu (%s %p)", a, name, (unsigned)((char *)a - (char *)m), GetCurrentThreadId(),
              e->ExceptionRecord->ExceptionInformation[0] ? "write" : "read", (void *)e->ExceptionRecord->ExceptionInformation[1]);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

BOOL WINAPI DllMain(HINSTANCE h, DWORD reason, LPVOID r) {
    (void)r;
    if (reason == DLL_PROCESS_ATTACH) { DisableThreadLibraryCalls(h); install(h); if (g_log) AddVectoredExceptionHandler(1, av_logger); }
    if (reason == DLL_PROCESS_DETACH && g_log) { logf_("exit"); fclose(g_log); g_log = NULL; }
    return TRUE;
}
