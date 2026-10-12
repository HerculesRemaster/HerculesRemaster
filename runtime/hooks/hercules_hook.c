/* hercules_hook.dll - in-process tracer for HERCULES.EXE (x86, injected by herc_inject.exe).
 *
 * Hooks (HERCULES.EXE RVAs, base 0x400000, names from analysis/annotations/p2.json):
 *   FS_Open 0x5130, FS_Read 0x5210, FS_Seek 0x52c0, FS_Close 0x51f0  -> archive access + caller
 *   RT_PCopen 0xa210, RT_PCread 0xa2f0                                -> DLL caller of file access
 *   AUD_DecodeAdpcm 0x2410                                            -> ESF in/out pair dump
 *   VRAM upload 0xa710 (slot 0x204)                                   -> image upload rect + pixels
 *   RT_PollInput 0xbcf0 (slot 0x258, once per tick)                   -> tick counter on every event
 *   AUD_StartVoice 0xf940                                             -> sound-play events
 *   IAT of HERCULES.EXE: CreateFileA / SetFilePointer / ReadFile       -> OS-level file trace ("os_*")
 *   eng1.dll 0x3cd0 (draw one object's sprite frame)                  -> "anim" on frame change:
 *       object, bank (+0x22, name from bank table 0x5cee0 stride 0x14), frame (+0x34), x/y (+0x0c/+0x0e)
 * "dll" = first return address on the stack that lies in a game DLL (stack scan, heuristic).
 * Options (environment):
 *   HERC_HOOK_DIR       output folder (required): resource-usage.jsonl, pairs/
 *   HERC_HOOK_WINDOWED  1 = never request fullscreen (0x6ec0 stores 0)
 *   HERC_HOOK_ACTIVE    1 = keep the "active" flag (0xba0238) at 2 so the game runs unfocused
 *   HERC_HOOK_NOINPUT   1 = RT_ReadPadBits (0x8040) returns 0 (no stray keyboard input)
 *   HERC_HOOK_PAIRS     max number of image uploads to dump (default 16; ESF: first 4)
 *   HERC_HOOK_VRAM_TICKS comma list of ticks: dump the emulated VRAM (EXE 0x96e6e0, 1 MiB,
 *                       layout of the upload 0xa710) to vram_<tick>.bin at those ticks
 *   HERC_HOOK_HERO      1 = log the hero every tick while eng1.dll runs ("hero": x, y, z, vx +0xa8,
 *                       target +0xac, vy +0xb8, up flag +0x95, facing +0x94, state (0x72108), frame +0x34)
 *   HERC_HOOK_FRAMES    file: append every presented 320x240 frame (StretchDIBits, IAT) as
 *                       {"FRM0", u32 tick, double ms, RGBQUAD[256], u8[320*240] top-down}
 * Identity events (full-game S1.02; fields: docs/full-game/gameplay/difficulty.md):
 *   level_load / level_palette  on FS_Open of MAP_PSX.BIN / MAP_PSX.CIF: engine, loader (stack caller),
 *                       path, normalized path (O:\ -> M:\GRAFIX\MAPS\), archive index + ID, game state
 *                       level (+0), start (+1), restart (+2), lives (+0xc), difficulty (+0xe), password (+0x12)
 *   module_load         after MOD_LoadModule (EXE 0x5c20): the module now loaded
 *   state               per tick when a game-state identity byte changes (level, start, restart, mode,
 *                       lives, difficulty, saved difficulty, password) plus health (+0x24)
 *   spawn / despawn     ENG1 object create 0x27db0 (x, y, z, kind) / free 0x27260
 *   palette             per tick when the frame palette (FNV of the 256 BITMAPINFO colours) or the
 *                       frame brightness (EXE 0x436a0c, GFX_FrameBrightness) changes
 *   HERC_HOOK_DIFFICULTY 0/1/2: options preset. Before each config apply (EXE 0x4ac0, which copies
 *                       config byte 0xb9e864 into game state +0xe) the byte is set to this value, the
 *                       same variable the F2 "Game Play" dialog edits; the registry is not touched
 *   input.jsonl         (in HERC_HOOK_DIR) the pad bits RT_PollInput (EXE 0xbcf0) delivered, one line
 *                       per change: {"tick", "bits"}; bits = ~u16 at *(0xa8e78c)+2 (PSX active-low)
 *   HERC_HOOK_KEEPCONFIG 1 = let the game save its options to the registry. Default: traced runs never
 *                       save (EXE 0x4da0 -> ret), so presets/passwords never change the player's settings
 *   HERC_HOOK_WATCH     memory watch, logged per tick on change ("watch" events): comma list of
 *                       name=module+rva[@offset]:size, module = exe|title|eng1|eng3, @offset dereferences
 *                       the pointer at rva first, size 1|2|4 (e.g. slot0=title+0x2a278@0x34:2)
 *   audio               (S5) interface-table wrappers (EXE table 0x4361b8, patched before any module
 *                       copies it): slot 0x06c AUD_LoadLevelSonix(name), 0x070 AUD_LoadMusic, 0x074
 *                       AUD_LoadSfx, 0x0d4/0x0d8 AUD_StartMusic, 0x0dc AUD_StopMusic -> "audio" events
 *   HERC_HOOK_PAD_HANDOVER 1 (with HERC_HOOK_NOINPUT 0): after the last scripted tick, restore RT_ReadPadBits
 *                       so the player takes over (event "pad_handover"); input.jsonl records the play for replay
 *   HERC_HOOK_PAD       scripted pad (replaces RT_ReadPadBits 0x8040): "from-to:hexbits,..." in ticks,
 *                       e.g. "400-404:0800" presses Start (S) on ticks 400..404
 *   HERC_HOOK_LEVEL     test harness: the first ENGINE1/ENGINE3 request after the first scripted press
 *                       (MOD_RequestModule 0x5cd0) starts level N instead: game state +0 = N, +1/+2 = 0
 *                       (start index), engine ENGINE3 for levels 2, 8, 10, else ENGINE1
 *   HERC_HOOK_START     DEBUG ONLY: on the first ENG1 MAP_PSX.BIN open (after MOD_Tick zeroed game state
 *                       +1/+2, before HERO_Init) set the start and restart index to N, once ("start_forced")
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

static CRITICAL_SECTION g_cs;
static FILE *g_log;
static char g_dir[MAX_PATH];
static LARGE_INTEGER g_freq, g_t0;
static int g_maxPairs = 16, g_imgPairs, g_sfxPairs, g_scene;   /* pair caps reset per scene (S1.07) */
static void new_scene(void) { g_scene++; g_imgPairs = g_sfxPairs = 0; }
static u32 pal_hash(void) {
    const u8 *bmi = *(const u8 **)0x0096e3f4; u32 k = 2166136261u;
    if (bmi) for (int i = 0; i < 1024; i++) k = (k ^ bmi[0x28 + i]) * 16777619u;
    return k;
}
static volatile u32 g_tick;

/* game globals */
#define G_FS_INDEX ((u32 *)0x009177d0)   /* {id, offset, size} x 1024 */
#define G_FS_POS   ((u32 *)0x0091a7d0)
#define G_ACTIVE   ((volatile u8 *)0x00ba0238)
#define G_SKIP     ((volatile u8 *)0x00ba01fc)
#define G_ESF_HDR  ((u8 *)0x00a99838)

static double now_ms(void) {
    LARGE_INTEGER t; QueryPerformanceCounter(&t);
    return (double)(t.QuadPart - g_t0.QuadPart) * 1000.0 / (double)g_freq.QuadPart;
}

static void logf_(const char *fmt, ...) {
    va_list ap;
    if (!g_log) return;
    EnterCriticalSection(&g_cs);
    fprintf(g_log, "{\"t\":%.1f,\"tick\":%u,", now_ms(), g_tick);
    va_start(ap, fmt); vfprintf(g_log, fmt, ap); va_end(ap);
    fputs("}\n", g_log);
    fflush(g_log);
    LeaveCriticalSection(&g_cs);
}

static void json_str(char *out, size_t n, const char *s) {
    size_t j = 0;
    for (; s && *s && j + 2 < n; s++) {
        if (*s == '\\' || *s == '"') out[j++] = '\\';
        out[j++] = (*s >= 0x20 && *s < 0x7f) ? *s : '?';
    }
    out[j] = 0;
}

static u32 rva(void *ret) {
    u32 a = (u32)ret;
    HMODULE m = NULL;
    /* module that owns the address first: a relocated DLL (ENG1 loads around 0x01170000 when its base is taken)
       lies inside 0x400000..0x10000000, so the EXE range test alone gave wrong DLL offsets (sfx_request callers) */
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCSTR)ret, &m) && m) return a - (u32)m;
    if (a >= 0x400000 && a < 0x10000000) return a - 0x400000;
    return a;
}

static const char *module_of(void *ret) {
    static char name[MAX_PATH];
    HMODULE m = NULL;
    char *p;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCSTR)ret, &m) || !m) return "?";
    GetModuleFileNameA(m, name, MAX_PATH);
    p = strrchr(name, '\\');
    return p ? p + 1 : name;
}

/* First return address on the stack (above `frame`) inside a loaded game DLL. */
static char g_dllCaller[96];
static const char *dll_caller(void *frame) {
    u32 *sp = (u32 *)frame;
    static const char *names[3] = { "title.dll", "eng1.dll", "eng3.dll" };
    HMODULE mods[3]; u32 size[3];
    u32 *top = (u32 *)((NT_TIB *)NtCurrentTeb())->StackBase;     /* never read past the stack */
    for (int k = 0; k < 3; k++) {
        mods[k] = GetModuleHandleA(names[k]); size[k] = 0;
        if (mods[k]) {
            const u8 *b = (const u8 *)mods[k];
            size[k] = ((const IMAGE_NT_HEADERS *)(b + ((const IMAGE_DOS_HEADER *)b)->e_lfanew))->OptionalHeader.SizeOfImage;
        }
    }
    for (int i = 0; i < 1024 && sp + i < top; i++) {
        u32 v = sp[i];
        for (int k = 0; k < 3; k++) {
            u32 b = (u32)mods[k];
            if (b && v > b + 0x1000 && v < b + size[k]) {   /* inside the image, so ins[-6..-1] is mapped */
                const u8 *ins = (const u8 *)v;
                if (ins[-5] == 0xE8 || ins[-2] == 0xFF || ins[-3] == 0xFF || ins[-6] == 0xFF) {
                    _snprintf(g_dllCaller, sizeof g_dllCaller, "%s+0x%X", names[k], v - b);
                    return g_dllCaller;
                }
            }
        }
    }
    return "-";
}

static void write_file(const char *rel, const void *data, size_t n) {
    char path[MAX_PATH];
    FILE *f;
    _snprintf(path, MAX_PATH, "%s\\%s", g_dir, rel);
    f = fopen(path, "wb");
    if (f) { fwrite(data, 1, n, f); fclose(f); }
}

/* ---- detours: 5-byte JMP at entry, trampoline = stolen bytes + JMP back ---- */
static void *detour(u32 target, void *hook, int stolen) {
    u8 *tramp = (u8 *)VirtualAlloc(NULL, 32, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    DWORD old;
    if (*(u8 *)target == 0xE9) {               /* already detoured by someone else: chain to its target */
        u32 dest = target + 5 + *(u32 *)(target + 1);
        tramp[0] = 0xE9;
        *(u32 *)(tramp + 1) = dest - ((u32)tramp + 5);
    } else {
        memcpy(tramp, (void *)target, stolen);
        tramp[stolen] = 0xE9;
        *(u32 *)(tramp + stolen + 1) = (target + stolen) - ((u32)tramp + stolen + 5);
    }
    VirtualProtect((void *)target, stolen, PAGE_EXECUTE_READWRITE, &old);
    *(u8 *)target = 0xE9;
    *(u32 *)(target + 1) = (u32)hook - (target + 5);
    for (int i = 5; i < stolen; i++) ((u8 *)target)[i] = 0x90;
    VirtualProtect((void *)target, stolen, old, &old);
    FlushInstructionCache(GetCurrentProcess(), (void *)target, stolen);
    return tramp;
}

static void patch(u32 addr, const u8 *bytes, int n) {
    DWORD old;
    VirtualProtect((void *)addr, n, PAGE_EXECUTE_READWRITE, &old);
    memcpy((void *)addr, bytes, n);
    VirtualProtect((void *)addr, n, old, &old);
    FlushInstructionCache(GetCurrentProcess(), (void *)addr, n);
}

/* ---- recent reads, to link uploads/decodes back to archive bytes ---- */
typedef struct { u8 *buf; u32 len; int handle; u32 pos; } Read;
static Read g_reads[128];
static int g_readN;
static Read g_big[2048];  /* reads >= 4 KiB, newest overwrite oldest */
static int g_bigN;
static int g_lastOpenHandle;
static char g_lastOpenName[260];

static void remember_read(u8 *buf, u32 len, int h, u32 pos) {
    Read *r = &g_reads[g_readN++ % 128];
    r->buf = buf; r->len = len; r->handle = h; r->pos = pos;
    if (len >= 4096) { Read *b = &g_big[g_bigN++ % 2048]; *b = *r; }
}

static const Read *find_read(const u8 *p) {
    for (int i = 0; i < 128; i++) {
        const Read *r = &g_reads[(g_readN - 1 - i + 1280) % 128];
        if (r->buf && p >= r->buf && p < r->buf + r->len) return r;
    }
    for (int i = 0; i < 2048; i++) {
        const Read *r = &g_big[(g_bigN - 1 - i + 2048 * 16) % 2048];
        if (r->buf && p >= r->buf && p < r->buf + r->len) return r;
    }
    return NULL;
}

/* ---- FS hooks ---- */
typedef int (__cdecl *FsOpen)(const char *, const char *);
typedef u32 (__cdecl *FsRead)(void *, u32, u32, int);
typedef void (__cdecl *FsSeek)(int, int, int);
typedef int (__cdecl *FsClose)(int);
typedef int (__cdecl *PcOpen)(const char *, int);
typedef int (__cdecl *PcRead)(int, void *, int);
static FsOpen o_FsOpen; static FsRead o_FsRead; static FsSeek o_FsSeek; static FsClose o_FsClose;
static PcOpen o_PcOpen; static PcRead o_PcRead;
static void *g_pcCaller; /* DLL return address of the PCopen/PCread in progress */

static const char *cur_module(void) {
    return GetModuleHandleA("eng1.dll") ? "ENG1" : GetModuleHandleA("eng3.dll") ? "ENG3" : GetModuleHandleA("title.dll") ? "TITLE" : "-";
}

static void log_identity(const char *ev, const char *name, int h) {
    const u8 *gs = (const u8 *)0x00615c98;
    char norm[300], esc[400];
    if ((name[0] == 'O' || name[0] == 'o') && name[1] == ':' && name[2] == '\\')
        _snprintf(norm, sizeof norm, "M:\\GRAFIX\\MAPS\\%s", name + 3);
    else
        _snprintf(norm, sizeof norm, "%s", name);
    json_str(esc, sizeof esc, norm);
    logf_("\"ev\":\"%s\",\"engine\":\"%s\",\"loader\":\"%s\",\"path\":\"%s\",\"index\":%d,\"id\":\"%08X\","
          "\"level\":%u,\"start\":%u,\"restart\":%u,\"lives\":%u,\"difficulty\":%u,\"password\":%u",
          ev, cur_module(), dll_caller((void *)&name), esc, h - 1, h > 0 ? G_FS_INDEX[(h - 1) * 3] : 0,
          gs[0], gs[1], gs[2], gs[0xc], gs[0xe], gs[0x12]);
}

static int g_start = -1;                        /* HERC_HOOK_START (debug): start index forced once */

static int __cdecl h_FsOpen(const char *name, const char *mode) {
    char esc[300];
    void *ret = _ReturnAddress();
    int h;
    char raw[300];
    json_str(esc, sizeof esc, name);           /* before: FS_NormalizePath uses a static buffer */
    strncpy(raw, name, sizeof raw - 1); raw[sizeof raw - 1] = 0;
    h = o_FsOpen(name, mode);
    g_lastOpenHandle = h;
    strncpy(g_lastOpenName, esc, sizeof g_lastOpenName - 1);
    logf_("\"ev\":\"open\",\"name\":\"%s\",\"handle\":%d,\"index\":%d,\"id\":\"%08X\",\"offset\":%u,\"size\":%u,"
          "\"caller\":\"%s+0x%X\",\"pc_caller\":\"%s+0x%X\",\"dll\":\"%s\"",
          esc, h, h - 1, G_FS_INDEX[(h - 1) * 3], G_FS_INDEX[(h - 1) * 3 + 1], G_FS_INDEX[(h - 1) * 3 + 2],
          module_of(ret), rva(ret), g_pcCaller ? module_of(g_pcCaller) : "-", g_pcCaller ? rva(g_pcCaller) : 0,
          dll_caller(&name));
    if (StrStrIA(raw, "MAP_PSX.BIN")) {
        if (g_start >= 0 && GetModuleHandleA("eng1.dll")) {   /* after MOD_Tick zeroed +1/+2, before HERO_Init */
            u8 *gs = (u8 *)0x00615c98;
            gs[1] = gs[2] = (u8)g_start;
            logf_("\"ev\":\"start_forced\",\"start\":%d,\"debug_only\":1", g_start);
            g_start = -1;                                      /* once */
        }
        new_scene(); log_identity("level_load", raw, h);
    }
    else if (StrStrIA(raw, "MAP_PSX.CIF")) log_identity("level_palette", raw, h);
    return h;
}

static u32 __cdecl h_FsRead(void *buf, u32 size, u32 count, int h) {
    void *ret = _ReturnAddress();
    u32 pos = G_FS_POS[h - 1], n = o_FsRead(buf, size, count, h);
    remember_read((u8 *)buf, n * size, h, pos);
    logf_("\"ev\":\"read\",\"handle\":%d,\"index\":%d,\"pos\":%u,\"bytes\":%u,\"buf\":\"0x%08X\","
          "\"caller\":\"%s+0x%X\",\"pc_caller\":\"%s+0x%X\",\"dll\":\"%s\"",
          h, h - 1, pos, n * size, (u32)buf, module_of(ret), rva(ret),
          g_pcCaller ? module_of(g_pcCaller) : "-", g_pcCaller ? rva(g_pcCaller) : 0, dll_caller(&buf));
    return n;
}

static void __cdecl h_FsSeek(int h, int off, int whence) {
    void *ret = _ReturnAddress();
    o_FsSeek(h, off, whence);
    logf_("\"ev\":\"seek\",\"handle\":%d,\"off\":%d,\"whence\":%d,\"pos\":%u,\"caller\":\"%s+0x%X\"",
          h, off, whence, G_FS_POS[h - 1], module_of(ret), rva(ret));
}

static int __cdecl h_FsClose(int h) {
    void *ret = _ReturnAddress();
    logf_("\"ev\":\"close\",\"handle\":%d,\"caller\":\"%s+0x%X\"", h, module_of(ret), rva(ret));
    return o_FsClose(h);
}

static int __cdecl h_PcOpen(const char *name, int mode) {
    int r;
    g_pcCaller = _ReturnAddress();
    r = o_PcOpen(name, mode);
    g_pcCaller = NULL;
    return r;
}

static int __cdecl h_PcRead(int fd, void *buf, int n) {
    int r;
    g_pcCaller = _ReturnAddress();
    r = o_PcRead(fd, buf, n);
    g_pcCaller = NULL;
    return r;
}

/* ---- ESF decode pair ---- */
typedef void (__cdecl *Adpcm)(const u8 *, short *, u32, u32 *);
static Adpcm o_Adpcm;

static void __cdecl h_Adpcm(const u8 *src, short *dst, u32 pcmBytes, u32 *state) {
    u32 st = *state;
    char rel[MAX_PATH], meta[1024];
    const Read *r = find_read(src);
    o_Adpcm(src, dst, pcmBytes, state);
    logf_("\"ev\":\"esf_decode\",\"name\":\"%s\",\"index\":%d,\"src\":\"0x%08X\",\"dst\":\"0x%08X\",\"pcm_bytes\":%u,"
          "\"rate\":%u,\"loop\":%u,\"bps8\":%u,\"state_in\":%u",
          g_lastOpenName, g_lastOpenHandle - 1, (u32)src, (u32)dst, pcmBytes,
          *(u16 *)(G_ESF_HDR + 8), G_ESF_HDR[12], G_ESF_HDR[13], st);
    if (g_sfxPairs < 4) {
        int idx = g_lastOpenHandle - 1;
        u32 adpcmLen = pcmBytes / 4;
        _snprintf(rel, MAX_PATH, "pairs\\s%02d_esf_%03d", g_scene, idx);
        { char d[MAX_PATH]; _snprintf(d, MAX_PATH, "%s\\%s", g_dir, rel); CreateDirectoryA(d, NULL); }
        _snprintf(rel, MAX_PATH, "pairs\\s%02d_esf_%03d\\header.bin", g_scene, idx); write_file(rel, G_ESF_HDR, 16);
        _snprintf(rel, MAX_PATH, "pairs\\s%02d_esf_%03d\\adpcm_in.bin", g_scene, idx); write_file(rel, src, adpcmLen);
        _snprintf(rel, MAX_PATH, "pairs\\s%02d_esf_%03d\\pcm_out.s16le", g_scene, idx); write_file(rel, dst, pcmBytes);
        _snprintf(meta, sizeof meta,
                  "{\"archive_index\":%d,\"name\":\"%s\",\"id\":\"%08X\",\"decoder\":\"HERCULES.EXE+0x2410\","
                  "\"input\":\"adpcm_in.bin (%u bytes, archive bytes 16..)\",\"input_read_pos\":%d,"
                  "\"output\":\"pcm_out.s16le\",\"format\":\"PCM s16le mono\",\"samples\":%u,\"rate\":%u,"
                  "\"state_in\":%u,\"tick\":%u,\"scene\":%d,\"level\":%u}\n",
                  idx, g_lastOpenName, G_FS_INDEX[idx * 3], adpcmLen, r ? (int)(r->pos + (src - r->buf)) : -1,
                  pcmBytes / 2, *(u16 *)(G_ESF_HDR + 8), st, g_tick, g_scene, *(const u8 *)0x00615c98);
        _snprintf(rel, MAX_PATH, "pairs\\s%02d_esf_%03d\\pair.json", g_scene, idx); write_file(rel, meta, strlen(meta));
        g_sfxPairs++;
    }
}

/* ---- VRAM uploads (rect = {x, y, w, h} shorts; w in 16-bit units) ---- */
typedef void (__cdecl *Upload)(short *, const u8 *);
typedef u32 (__cdecl *Upload2)(short *, u32, u32);
static Upload o_Upload;

static void __cdecl h_Upload(short *rect, const u8 *data) {
    void *ret = _ReturnAddress();
    const Read *r = find_read(data);
    u32 bytes = (u32)(rect[2] * 2) * (u32)rect[3];
    logf_("\"ev\":\"vram_upload\",\"x\":%d,\"y\":%d,\"w16\":%d,\"h\":%d,\"bytes\":%u,\"src\":\"0x%08X\","
          "\"from_index\":%d,\"from_pos\":%d,\"caller\":\"%s+0x%X\",\"dll\":\"%s\"",
          rect[0], rect[1], rect[2], rect[3], bytes, (u32)data,
          r ? r->handle - 1 : -1, r ? (int)(r->pos + (data - r->buf)) : -1, module_of(ret), rva(ret),
          dll_caller(&rect));
    if (g_imgPairs < g_maxPairs && bytes > 0 && bytes < 0x100000) {
        char rel[MAX_PATH], meta[768];
        int n = g_imgPairs++;
        _snprintf(rel, MAX_PATH, "pairs\\s%02d_vram_%03d", g_scene, n);
        { char d[MAX_PATH]; _snprintf(d, MAX_PATH, "%s\\%s", g_dir, rel); CreateDirectoryA(d, NULL); }
        _snprintf(rel, MAX_PATH, "pairs\\s%02d_vram_%03d\\upload.bin", g_scene, n); write_file(rel, data, bytes);
        _snprintf(meta, sizeof meta,
                  "{\"consumer\":\"HERCULES.EXE+0xa710 (VRAM upload, slot 0x204)\",\"caller\":\"%s+0x%X\","
                  "\"rect\":{\"x\":%d,\"y\":%d,\"w16\":%d,\"h\":%d},\"stride_bytes\":%d,\"bytes\":%u,"
                  "\"from_index\":%d,\"from_pos\":%d,\"format\":\"VRAM 16-bit units; 8bpp texels, 2 per unit\","
                  "\"palette_hash\":\"%08X\",\"tick\":%u,\"scene\":%d,\"level\":%u}\n",
                  module_of(ret), rva(ret), rect[0], rect[1], rect[2], rect[3], rect[2] * 2, bytes,
                  r ? r->handle - 1 : -1, r ? (int)(r->pos + (data - r->buf)) : -1, pal_hash(), g_tick, g_scene,
                  *(const u8 *)0x00615c98);
        _snprintf(rel, MAX_PATH, "pairs\\s%02d_vram_%03d\\pair.json", g_scene, n); write_file(rel, meta, strlen(meta));
    }
    o_Upload(rect, data);
}

/* ---- eng1.dll sprite frames ---- */
typedef void (__cdecl *DrawObj)(u8 *);
static DrawObj o_DrawObj;
static u32 g_eng1, g_bankTable = 0x5cee0;   /* base of the engine DLL now hooked (ENG1 or ENG3) */
static struct { u8 *obj; u32 key; } g_objs[256];

static void __cdecl h_DrawObj(u8 *obj) {
    u32 bank = obj[0x22], frame = *(u16 *)(obj + 0x34), key = (bank << 16) | frame;
    int i, slot = -1;
    for (i = 0; i < 256; i++) {
        if (g_objs[i].obj == obj) { slot = i; break; }
        if (slot < 0 && !g_objs[i].obj) slot = i;
    }
    if (slot >= 0 && !(g_objs[slot].obj == obj && g_objs[slot].key == key)) {
        u8 *ent = (u8 *)(g_eng1 + g_bankTable + bank * 0x14);   /* ENG1 0x5cee0, ENG3 0x39548 */
        const char *name = *(const char **)ent;
        u8 *data = *(u8 **)(ent + 0xc);
        const Read *r = data ? find_read(data) : NULL;
        char esc[128];
        json_str(esc, sizeof esc, (u32)name > g_eng1 && (u32)name < g_eng1 + 0x80000 ? name : "?");
        g_objs[slot].obj = obj; g_objs[slot].key = key;
        logf_("\"ev\":\"anim\",\"obj\":\"0x%08X\",\"bank\":%u,\"bank_name\":\"%s\",\"frame\":%u,\"frames_in_bank\":%u,"
              "\"x\":%d,\"y\":%d,\"pal\":%u,\"flip\":%u,\"type\":%u,\"bank_data\":\"0x%08X\",\"bank_index\":%d,\"bank_pos\":%d",
              (u32)obj, bank, esc, frame, *(u16 *)(ent + 4), *(short *)(obj + 0xc), *(short *)(obj + 0xe),
              obj[0x1c], (*(u32 *)(obj + 0x54) >> 4) & 1, *(u16 *)(obj + 0x2e),
              (u32)data, r ? r->handle - 1 : -1, r ? (int)(r->pos + (data - r->buf)) : -1);
    }
    o_DrawObj(obj);
}

typedef void *(__cdecl *ObjNew_t)(int, int, int, u32);
typedef int (__cdecl *ObjFree_t)(u8 *);
static ObjNew_t o_ObjNew; static ObjFree_t o_ObjFree;
static void *__cdecl h_ObjNew(int x, int y, int z, u32 kind) {
    void *o = o_ObjNew(x, y, z, kind);
    logf_("\"ev\":\"spawn\",\"obj\":\"0x%08X\",\"kind\":\"0x%04X\",\"x\":%d,\"y\":%d,\"z\":%d",
          (u32)o, kind & 0xffff, x >> 16, y >> 16, z >> 16);
    return o;
}
static int __cdecl h_ObjFree(u8 *o) {
    if (o) logf_("\"ev\":\"despawn\",\"obj\":\"0x%08X\",\"kind\":\"0x%04X\",\"x\":%d,\"y\":%d",
                 (u32)o, *(u16 *)(o + 0x2e) & 0x1fff, *(int *)o >> 16, *(int *)(o + 4) >> 16);
    return o_ObjFree(o);
}

static void log_palette(void) {
    static u32 last; static u8 lastB = 0x80; static int init;
    const u8 *bmi = *(const u8 **)0x0096e3f4;
    if (!bmi) return;
    u32 k = 2166136261u;
    for (int i = 0; i < 1024; i++) k = (k ^ bmi[0x28 + i]) * 16777619u;
    u8 b = *(const u8 *)0x00436a0c;
    if (!init || k != last || b != lastB)
        logf_("\"ev\":\"palette\",\"hash\":\"%08X\",\"brightness\":%u,\"color1\":\"%02X%02X%02X\"",
              k, b, bmi[0x28 + 6], bmi[0x28 + 5], bmi[0x28 + 4]);
    last = k; lastB = b; init = 1;
}

/* Frame/spawn hooks for whichever engine is loaded. ENG3 runs the same code at other addresses
   (tools/ghidra/fn_match.py): frame draw 0x8da0, create 0x1d670, free 0x1cb90, bank table 0x39548. */
static void hook_engine(const char *dll, const char *ev, u32 draw, u32 create, u32 freeo, u32 banks) {
    u32 b = (u32)GetModuleHandleA(dll);
    u8 *f;
    if (!b) return;
    f = (u8 *)(b + draw);
    if (f[0] == 0xE9) return;                          /* already hooked in this load */
    if (memcmp(f, "\x83\xec\x10\x33\xc0", 5)) return;   /* unexpected build */
    g_eng1 = b; g_bankTable = banks;
    memset(g_objs, 0, sizeof g_objs);
    o_DrawObj = (DrawObj)detour((u32)f, h_DrawObj, 5);
    if (!memcmp((u8 *)(b + create), "\x56\x57\x8b\x7c\x24\x18", 6))
        o_ObjNew = (ObjNew_t)detour(b + create, h_ObjNew, 6);
    if (!memcmp((u8 *)(b + freeo), "\x56\x8b\x74\x24\x08\x57", 6))
        o_ObjFree = (ObjFree_t)detour(b + freeo, h_ObjFree, 6);
    logf_("\"ev\":\"%s\",\"base\":\"0x%08X\"", ev, b);
}
/* Sound requests of the engine (S5.01): ENG1 0x6da0 / ENG3 AUD_PlaySfx 0xbc47 (u16 id, obj). The id is the handler's
   sound id (& 0x7ff indexes the level table gs+0x2f0); the following sfx_play event gives the EXE voice id. */
typedef void (__cdecl *SfxReq_t)(u32, u32);
static SfxReq_t o_SfxReq;
static void __cdecl h_SfxReq(u32 id, u32 obj) {
    void *ret = _ReturnAddress();
    logf_("\"ev\":\"sfx_request\",\"id\":%u,\"obj\":\"0x%08X\",\"kind\":%d,\"caller\":\"%s+0x%X\"",
          id & 0xffff, obj, obj && !IsBadReadPtr((void *)obj, 0x30) ? *(u16 *)(obj + 0x2e) : -1, module_of(ret), rva(ret));
    o_SfxReq(id, obj);
}
static void hook_sfxreq(const char *dll, u32 at) {
    u32 b = (u32)GetModuleHandleA(dll);
    if (!b || *(u8 *)(b + at) == 0xE9 || memcmp((u8 *)(b + at), "\x55\x8b\xec\x51\x53", 5)) return;
    o_SfxReq = (SfxReq_t)detour(b + at, h_SfxReq, 5);
}
static void hook_eng1(void) {
    hook_engine("eng1.dll", "hook_eng1", 0x3cd0, 0x27db0, 0x27260, 0x5cee0);
    hook_engine("eng3.dll", "hook_eng3", 0x8da0, 0x1d670, 0x1cb90, 0x39548);
    hook_sfxreq("eng1.dll", 0x6da0);
    hook_sfxreq("eng3.dll", 0xbc47);
}

/* ---- tick counter (RT_PollInput runs once at the start of every DLL tick) ---- */
typedef void (__cdecl *Poll)(void);
static Poll o_Poll;
static int g_heroLog;
static void log_hero(void) {
    u32 b = (u32)GetModuleHandleA("eng1.dll");
    u8 *h;
    if (!g_heroLog) return;
    if (!b) {
        /* ENG3: hero object 0x41d54, state index 0x41cb4 (HERO_Tick 0x174e0), track = obj+0x38 */
        u32 b3 = (u32)GetModuleHandleA("eng3.dll");
        if (!b3) return;
        h = *(u8 **)(b3 + 0x41d54);
        if (!h || IsBadReadPtr(h, 0x140)) return;
        logf_("\"ev\":\"hero\",\"engine\":\"ENG3\",\"x\":%d,\"y\":%d,\"z\":%d,\"vx\":%d,\"vy\":%d,\"state\":%u,"
              "\"track\":%u,\"frame\":%u,\"health\":%d",
              *(int *)h, *(int *)(h + 4), *(int *)(h + 8), *(int *)(h + 0xa8), *(int *)(h + 0xb8),
              *(u16 *)(b3 + 0x41cb4) & 0x7fff, *(u16 *)(h + 0x38), *(u16 *)(h + 0x34),
              *(short *)(*(u8 **)(b3 + 0x407a4) + 0x24));
        return;
    }
    h = *(u8 **)(b + 0x72860);
    if (!h || IsBadReadPtr(h, 0x140)) return;
    logf_("\"ev\":\"hero\",\"x\":%d,\"y\":%d,\"z\":%d,\"vx\":%d,\"vx_target\":%d,\"vy\":%d,\"up\":%u,"
          "\"facing\":%u,\"state\":%u,\"frame\":%u,\"script\":\"0x%08X\"",
          *(int *)h, *(int *)(h + 4), *(int *)(h + 8), *(int *)(h + 0xa8), *(int *)(h + 0xac), *(int *)(h + 0xb8),
          h[0x95], h[0x94], *(u16 *)(b + 0x72108), *(u16 *)(h + 0x34), *(u32 *)(h + 0x64));
}
static u32 g_vramTicks[16];
static int g_vramN;
/* ---- scripted pad + level harness ---- */
typedef struct { u32 from, to; u32 bits; } PadStep;
static PadStep g_pad[512]; static int g_padN, g_level = -1; static u32 g_firstPress = 0xffffffff;
static u8 g_padOrig[5]; static int g_padHandover; static u32 g_padEnd;
static u32 __cdecl h_ReadPad(void) {
    u32 bits = 0;
    if (g_padHandover && g_tick > g_padEnd) {          /* script over: give the pad back to the player */
        patch(0x408040, g_padOrig, 5);
        g_padHandover = 0;
        logf_("\"ev\":\"pad_handover\",\"after_tick\":%u", g_padEnd);
        return ((u32 (__cdecl *)(void))0x408040)();
    }
    for (int i = 0; i < g_padN; i++)
        if (g_tick >= g_pad[i].from && g_tick <= g_pad[i].to) bits |= g_pad[i].bits;
    return bits;
}
typedef void (__cdecl *CfgApply_t)(void);
static CfgApply_t o_CfgApply; static int g_difficulty = -1;
static void __cdecl h_CfgApply(void) {
    if (g_difficulty >= 0) *(u8 *)0x00b9e864 = (u8)g_difficulty;
    o_CfgApply();
}
typedef void (__cdecl *ModLoad_t)(void);
static ModLoad_t o_ModLoad;
static void __cdecl h_ModLoad(void) {
    o_ModLoad();
    new_scene();
    const u8 *gs = (const u8 *)0x00615c98;
    logf_("\"ev\":\"module_load\",\"module\":\"%s\",\"level\":%u,\"difficulty\":%u", cur_module(), gs[0], gs[0xe]);
}
typedef void (__cdecl *ModReq_t)(const char *, const char *);
static ModReq_t o_ModReq;
static void __cdecl h_ModReq(const char *dir, const char *name) {
    u8 *gs = (u8 *)0x00615c98;
    char jd[160], jn[160];
    json_str(jd, sizeof jd, dir ? dir : ""); json_str(jn, sizeof jn, name ? name : "");
    logf_("\"ev\":\"module_request\",\"dir\":\"%s\",\"name\":\"%s\",\"level\":%u,\"start\":%u", jd, jn, gs[0], gs[1]);
    if (g_level >= 0 && name && strstr(name, "ENGINE") && g_tick > g_firstPress) {
        gs[0] = (u8)g_level; gs[1] = gs[2] = 0;
        name = (g_level == 2 || g_level == 8 || g_level == 10) ? "ENGINE3" : "ENGINE1";
        logf_("\"ev\":\"level_forced\",\"level\":%d,\"name\":\"%s\"", g_level, name);
        g_level = -1;                                   /* once */
    }
    o_ModReq(dir, name);
}

/* state events (full-game S1.06): game-state identity bytes that changed since the last tick */
static void log_state(void) {
    static const int offs[] = { 0x0, 0x1, 0x2, 0x3, 0xc, 0xe, 0xf, 0x12 };
    static const char *names[] = { "level", "start", "restart", "mode", "lives", "difficulty", "difficulty_saved", "password" };
    static u8 last[8]; static int init;
    const u8 *gs = (const u8 *)0x00615c98;
    char buf[400]; int n = 0, changed = 0;
    for (int i = 0; i < 8; i++) {
        if (!init || gs[offs[i]] != last[i]) changed = 1;
        n += _snprintf(buf + n, sizeof buf - n, ",\"%s\":%u", names[i], gs[offs[i]]);
        last[i] = gs[offs[i]];
    }
    init = 1;
    if (changed) logf_("\"ev\":\"state\",\"module\":\"%s\",\"health\":%d%s,\"cfg_difficulty\":%u,\"cfg_lives\":%u",
                       cur_module(), *(const short *)(gs + 0x24), buf, *(const u8 *)0x00b9e864, *(const u8 *)0x00b9e870);
}

typedef struct { char name[24], mod[8]; u32 rva; int deref, off, size; u32 last; int seen; } Watch;
static Watch g_watch[16]; static int g_watchN;
static void parse_watch(void) {
    char b[1024];
    if (!GetEnvironmentVariableA("HERC_HOOK_WATCH", b, sizeof b)) return;
    for (char *t = strtok(b, ","); t && g_watchN < 16; t = strtok(NULL, ",")) {
        Watch *w = &g_watch[g_watchN];
        char *eq = strchr(t, '='), *plus, *at, *col;
        if (!eq || !(plus = strchr(eq, '+')) || !(col = strrchr(t, ':'))) continue;
        _snprintf(w->name, sizeof w->name, "%.*s", (int)(eq - t), t);
        _snprintf(w->mod, sizeof w->mod, "%.*s", (int)(plus - eq - 1), eq + 1);
        w->rva = strtoul(plus + 1, NULL, 16);
        at = strchr(plus, '@');
        w->deref = at != NULL; w->off = at ? strtol(at + 1, NULL, 16) : 0;
        w->size = atoi(col + 1);
        g_watchN++;
    }
}
static void log_watch(void) {
    for (int i = 0; i < g_watchN; i++) {
        Watch *w = &g_watch[i];
        u32 base = !strcmp(w->mod, "exe") ? 0x400000 : (u32)GetModuleHandleA(!strcmp(w->mod, "title") ? "title.dll" :
                   !strcmp(w->mod, "eng1") ? "eng1.dll" : !strcmp(w->mod, "eng3") ? "eng3.dll" : "?");
        if (!base) continue;
        const u8 *a = (const u8 *)(base + w->rva);
        if (w->deref) { a = *(const u8 * const *)a; if (!a || IsBadReadPtr(a + w->off, 4)) continue; a += w->off; }
        u32 v = w->size == 1 ? *a : w->size == 2 ? *(const u16 *)a : *(const u32 *)a;
        if (!w->seen || v != w->last) logf_("\"ev\":\"watch\",\"name\":\"%s\",\"value\":%u", w->name, v);
        w->last = v; w->seen = 1;
    }
}

/* ---- audio interface slots (cdecl; forward 4 dwords, the callee reads only its own) ---- */
typedef int (__cdecl *Slot4)(u32, u32, u32, u32);
static Slot4 o_aud[6];
static const u32 g_audSlot[6] = { 0x06c, 0x070, 0x074, 0x0d4, 0x0d8, 0x0dc };
static const char *g_audName[6] = { "AUD_LoadLevelSonix", "AUD_LoadMusic", "AUD_LoadSfx", "AUD_StartMusic",
                                    "AUD_StartMusic_title", "AUD_StopMusic" };
static int aud_call(int i, u32 a, u32 b, u32 c, u32 d) {
    char nm[160] = "";
    if (i == 0 && a && !IsBadReadPtr((void *)a, 4)) json_str(nm, sizeof nm, (const char *)a);
    int r = o_aud[i](a, b, c, d);
    logf_("\"ev\":\"audio\",\"call\":\"%s\",\"a\":%d,\"b\":%d,\"name\":\"%s\",\"ret\":%d,\"level\":%u,\"dll\":\"%s\"",
          g_audName[i], (int)a, (int)b, nm, r, *(const u8 *)0x00615c98, dll_caller(&a));
    return r;
}
static int __cdecl h_aud0(u32 a, u32 b, u32 c, u32 d) { return aud_call(0, a, b, c, d); }
static int __cdecl h_aud1(u32 a, u32 b, u32 c, u32 d) { return aud_call(1, a, b, c, d); }
static int __cdecl h_aud2(u32 a, u32 b, u32 c, u32 d) { return aud_call(2, a, b, c, d); }
static int __cdecl h_aud3(u32 a, u32 b, u32 c, u32 d) { return aud_call(3, a, b, c, d); }
static int __cdecl h_aud4(u32 a, u32 b, u32 c, u32 d) { return aud_call(4, a, b, c, d); }
static int __cdecl h_aud5(u32 a, u32 b, u32 c, u32 d) { return aud_call(5, a, b, c, d); }
static void hook_audio_slots(void) {
    static const Slot4 h[6] = { h_aud0, h_aud1, h_aud2, h_aud3, h_aud4, h_aud5 };
    u32 *table = (u32 *)0x004361b8;
    for (int i = 0; i < 6; i++) {
        u32 *e = table + g_audSlot[i] / 4;
        DWORD old;
        o_aud[i] = (Slot4)*e;
        VirtualProtect(e, 4, PAGE_READWRITE, &old);
        *e = (u32)h[i];
        VirtualProtect(e, 4, old, &old);
    }
}

static FILE *g_input;
static void log_input(void) {
    static int last = -1;
    const u8 *pad = *(const u8 **)0x00a8e78c;
    if (!g_input || !pad) return;
    int bits = (u16)~*(const u16 *)(pad + 2);
    if (bits != last) { fprintf(g_input, "{\"tick\":%u,\"bits\":%d}\n", g_tick, bits); fflush(g_input); last = bits; }
}

static void __cdecl h_Poll(void) {
    g_tick++;
    log_state();
    log_palette();
    log_watch();
    hook_eng1();
    log_hero();
    for (int i = 0; i < g_vramN; i++) {
        if (g_vramTicks[i] == g_tick) {
            char rel[64];
            _snprintf(rel, sizeof rel, "vram_%u.bin", g_tick);
            write_file(rel, (void *)0x0096e6e0, 0x100000);
            logf_("\"ev\":\"vram_dump\",\"file\":\"%s\"", rel);
        }
    }
    o_Poll();
    log_input();
}

/* ---- sound play ---- */
typedef short (__cdecl *Voice)(u32);
static Voice o_Voice;
static short __cdecl h_Voice(u32 sfx) {
    void *ret = _ReturnAddress();
    short ch = o_Voice(sfx);
    logf_("\"ev\":\"sfx_play\",\"sfx\":%u,\"channel\":%d,\"caller\":\"%s+0x%X\",\"dll\":\"%s\"",
          sfx, ch, module_of(ret), rva(ret), dll_caller(&sfx));
    return ch;
}

/* ---- OS-level file calls (IAT of HERCULES.EXE) ---- */
static HANDLE g_fsHandle = INVALID_HANDLE_VALUE;
typedef HANDLE (WINAPI *CreateFileA_t)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
typedef DWORD (WINAPI *SetFilePointer_t)(HANDLE, LONG, PLONG, DWORD);
typedef BOOL (WINAPI *ReadFile_t)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
static CreateFileA_t o_CreateFileA; static SetFilePointer_t o_SetFilePointer; static ReadFile_t o_ReadFile;

static HANDLE WINAPI h_CreateFileA(LPCSTR n, DWORD a, DWORD sm, LPSECURITY_ATTRIBUTES sa, DWORD cd, DWORD fl, HANDLE t) {
    void *ret = _ReturnAddress();
    char esc[300];
    HANDLE h = o_CreateFileA(n, a, sm, sa, cd, fl, t);
    const char *base = n ? strrchr(n, '\\') : NULL;
    base = base ? base + 1 : n;
    if (base && _stricmp(base, "hercules.fs") == 0) g_fsHandle = h;
    json_str(esc, sizeof esc, n);
    logf_("\"ev\":\"os_open\",\"name\":\"%s\",\"handle\":\"0x%X\",\"caller\":\"%s+0x%X\"",
          esc, (u32)h, module_of(ret), rva(ret));
    return h;
}
static DWORD WINAPI h_SetFilePointer(HANDLE h, LONG lo, PLONG hi, DWORD m) {
    void *ret = _ReturnAddress();
    DWORD r = o_SetFilePointer(h, lo, hi, m);
    if (h == g_fsHandle)
        logf_("\"ev\":\"os_seek\",\"off\":%ld,\"method\":%lu,\"result\":%lu,\"caller\":\"%s+0x%X\"",
              lo, m, r, module_of(ret), rva(ret));
    return r;
}
static BOOL WINAPI h_ReadFile(HANDLE h, LPVOID b, DWORD n, LPDWORD got, LPOVERLAPPED ov) {
    void *ret = _ReturnAddress();
    DWORD pos = h == g_fsHandle ? o_SetFilePointer(h, 0, NULL, FILE_CURRENT) : 0;
    BOOL ok = o_ReadFile(h, b, n, got, ov);
    if (h == g_fsHandle)
        logf_("\"ev\":\"os_read\",\"file_pos\":%lu,\"want\":%lu,\"got\":%lu,\"caller\":\"%s+0x%X\"",
              pos, n, got ? *got : 0, module_of(ret), rva(ret));
    return ok;
}

static void *iat_hook(const char *dll, const char *fn, void *hook) {
    u8 *base = (u8 *)GetModuleHandleA(NULL);
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
    return NULL;
}

/* ---- presented frames (GFX_Present -> StretchDIBits) ---- */
typedef int (WINAPI *StretchDIBits_t)(HDC, int, int, int, int, int, int, int, int, const void *, const BITMAPINFO *, UINT, DWORD);
static StretchDIBits_t o_StretchDIBits;
static FILE *g_frames;
static int WINAPI h_StretchDIBits(HDC dc, int xd, int yd, int wd, int hd, int xs, int ys, int ws, int hs,
                                  const void *bits, const BITMAPINFO *bmi, UINT usage, DWORD rop) {
    if (g_frames && ws == 320 && hs == 240 && bmi->bmiHeader.biBitCount == 8) {
        double t = now_ms();
        u32 tick = g_tick;
        fwrite("FRM0", 1, 4, g_frames);
        fwrite(&tick, 4, 1, g_frames);
        fwrite(&t, 8, 1, g_frames);
        fwrite(bmi->bmiColors, 4, 256, g_frames);
        fwrite(bits, 1, 320 * 240, g_frames);
    }
    return o_StretchDIBits(dc, xd, yd, wd, hd, xs, ys, ws, hs, bits, bmi, usage, rop);
}

/* ---- RT_FatalError (EXE 0x410460): log the message before the game exits ---- */
typedef void (__cdecl *Fatal_t)(const char *);
static Fatal_t o_Fatal;
static void __cdecl h_Fatal(const char *msg) {
    char esc[300];
    void *ret = _ReturnAddress();
    json_str(esc, sizeof esc, msg ? msg : "(null)");
    logf_("\"ev\":\"fatal\",\"message\":\"%s\",\"caller\":\"%s+0x%X\",\"dll\":\"%s\"", esc, module_of(ret), rva(ret),
          dll_caller(&msg));
    o_Fatal(msg);
}

/* ---- keep the game running unfocused ---- */
static DWORD WINAPI keep_active(LPVOID p) {
    (void)p;
    Sleep(2000);
    for (;;) { if (*G_ACTIVE < 2) *G_ACTIVE = 2; Sleep(5); }
}

static int env_int(const char *name, int def) {
    char b[32];
    if (!GetEnvironmentVariableA(name, b, sizeof b)) return def;
    return atoi(b);
}

static void install(void) {
    char path[MAX_PATH];
    InitializeCriticalSection(&g_cs);
    QueryPerformanceFrequency(&g_freq); QueryPerformanceCounter(&g_t0);
    if (!GetEnvironmentVariableA("HERC_HOOK_DIR", g_dir, MAX_PATH)) return;
    CreateDirectoryA(g_dir, NULL);
    _snprintf(path, MAX_PATH, "%s\\pairs", g_dir); CreateDirectoryA(path, NULL);
    _snprintf(path, MAX_PATH, "%s\\resource-usage.jsonl", g_dir);
    g_log = fopen(path, "w");
    g_maxPairs = env_int("HERC_HOOK_PAIRS", 16);
    g_heroLog = env_int("HERC_HOOK_HERO", 0);
    {
        char b[256], *tok;
        if (GetEnvironmentVariableA("HERC_HOOK_VRAM_TICKS", b, sizeof b))
            for (tok = strtok(b, ","); tok && g_vramN < 16; tok = strtok(NULL, ","))
                g_vramTicks[g_vramN++] = (u32)atoi(tok);
    }

    o_FsOpen = (FsOpen)detour(0x405130, h_FsOpen, 6);
    o_FsRead = (FsRead)detour(0x405210, h_FsRead, 5);
    o_FsSeek = (FsSeek)detour(0x4052c0, h_FsSeek, 5);
    o_FsClose = (FsClose)detour(0x4051f0, h_FsClose, 15);
    o_PcOpen = (PcOpen)detour(0x40a210, h_PcOpen, 5);
    o_PcRead = (PcRead)detour(0x40a2f0, h_PcRead, 5);
    o_Adpcm = (Adpcm)detour(0x402410, h_Adpcm, 7);
    o_Upload = (Upload)detour(0x40a710, h_Upload, 5);
    o_Poll = (Poll)detour(0x40bcf0, h_Poll, 5);
    o_Voice = (Voice)detour(0x40f940, h_Voice, 6);
    o_Fatal = (Fatal_t)detour(0x410460, h_Fatal, 5);
    o_SetFilePointer = SetFilePointer;
    o_CreateFileA = (CreateFileA_t)iat_hook("KERNEL32.dll", "CreateFileA", h_CreateFileA);
    if (!iat_hook("KERNEL32.dll", "SetFilePointer", h_SetFilePointer)) o_SetFilePointer = SetFilePointer;
    o_ReadFile = (ReadFile_t)iat_hook("KERNEL32.dll", "ReadFile", h_ReadFile);
    if (GetEnvironmentVariableA("HERC_HOOK_FRAMES", path, MAX_PATH) && path[0]) {
        g_frames = fopen(path, "wb");
        o_StretchDIBits = (StretchDIBits_t)iat_hook("GDI32.dll", "StretchDIBits", h_StretchDIBits);
        if (!o_StretchDIBits) o_StretchDIBits = StretchDIBits;
    }

    if (env_int("HERC_HOOK_WINDOWED", 0)) {
        static const u8 win[] = { 0x30, 0xC0, 0xA2, 0xD0, 0xE3, 0x96, 0x00, 0x90, 0x90 }; /* xor al,al; mov [96e3d0],al */
        patch(0x406ec0, win, sizeof win);
    }
    if (env_int("HERC_HOOK_NOINPUT", 0)) {
        static const u8 ret0[] = { 0x31, 0xC0, 0xC3 };
        patch(0x408040, ret0, sizeof ret0);
    }
    if (env_int("HERC_HOOK_ACTIVE", 0)) CreateThread(NULL, 0, keep_active, NULL, 0, NULL);
    {
        static char b[16384];
        if (GetEnvironmentVariableA("HERC_HOOK_PAD", b, sizeof b) && b[0]) {
            for (char *t = strtok(b, ","); t && g_padN < 512; t = strtok(NULL, ",")) {
                unsigned f, to, bits;
                if (sscanf(t, "%u-%u:%x", &f, &to, &bits) == 3) {
                    g_pad[g_padN].from = f; g_pad[g_padN].to = to; g_pad[g_padN].bits = bits; g_padN++;
                    if (f < g_firstPress) g_firstPress = f;
                    if (to > g_padEnd) g_padEnd = to;
                }
            }
            memcpy(g_padOrig, (void *)0x408040, 5);
            g_padHandover = env_int("HERC_HOOK_PAD_HANDOVER", 0) && !env_int("HERC_HOOK_NOINPUT", 0);
            u8 jmp[5] = { 0xE9 }; *(int *)(jmp + 1) = (int)((u8 *)h_ReadPad - (u8 *)0x408040 - 5);
            patch(0x408040, jmp, 5);
        }
        g_level = env_int("HERC_HOOK_LEVEL", -1);
        g_start = env_int("HERC_HOOK_START", -1);
        if (!memcmp((void *)0x405cd0, "\x8b\x4c\x24\x04\x81\xec\x00\x01\x00\x00", 10))
            o_ModReq = (ModReq_t)detour(0x405cd0, h_ModReq, 10);
        if (!env_int("HERC_HOOK_KEEPCONFIG", 0) && !memcmp((void *)0x404da0, "\x83\xec\x08", 3)) {
            static const u8 ret[] = { 0xC3 };
            patch(0x404da0, ret, 1);                  /* config save (copies gs+0xe back, RegSetValueEx) */
        }
        parse_watch();
        if (*(u32 *)0x004361b8 == 0x5ac00cac) hook_audio_slots();   /* interface table magic */
        g_difficulty = env_int("HERC_HOOK_DIFFICULTY", -1);
        { char ip[MAX_PATH]; _snprintf(ip, MAX_PATH, "%s\\input.jsonl", g_dir); g_input = fopen(ip, "w"); }
        if (g_difficulty >= 0 && !memcmp((void *)0x404ac0, "\xa1\x68\xe8\xb9\x00", 5))
            o_CfgApply = (CfgApply_t)detour(0x404ac0, h_CfgApply, 5);
        if (!memcmp((void *)0x405c20, "\xa1\x68\xc4\x91\x00", 5))
            o_ModLoad = (ModLoad_t)detour(0x405c20, h_ModLoad, 5);
    }
    logf_("\"ev\":\"hook\",\"windowed\":%d,\"active\":%d,\"noinput\":%d", env_int("HERC_HOOK_WINDOWED", 0),
          env_int("HERC_HOOK_ACTIVE", 0), env_int("HERC_HOOK_NOINPUT", 0));
}

BOOL WINAPI DllMain(HINSTANCE h, DWORD reason, LPVOID r) {
    (void)h; (void)r;
    if (reason == DLL_PROCESS_ATTACH) install();
    if (reason == DLL_PROCESS_DETACH && g_frames) { fclose(g_frames); g_frames = NULL; }
    if (reason == DLL_PROCESS_DETACH && g_log) { logf_("\"ev\":\"exit\""); fclose(g_log); g_log = NULL; }
    return TRUE;
}
