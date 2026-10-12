// GPU renderer for the hybrid remaster (x86, inside the ddraw.dll proxy; renderer = gpu).
//
// The game DLLs hand the EXE a PSX-style ordering table each tick (GFX_DrawOTag, EXE 0x40ac60).
// The EXE rasterises it in software into a 320x240 8-bit frame. This module walks the same list
// (the original still runs, so game state is untouched) and redraws it with D3D11 at an integer
// multiple of 320x240. It reproduces the PC rasteriser's 8-bit rules exactly, just at a higher
// resolution: it draws palette INDICES into a persistent R8 target, then maps them through the
// palette in a final pass, and presents instead of GDI (GFX_Present 0x406620).
//
// Rules taken from the EXE rasteriser (docs/gpu-renderer.md):
//   list walk: p = *p, skipping OT slots (GFX_DrawOTag); code = ((u8*)p)[7], bit 1 = semi-transparent
//   0x24 POLY_FT3, 0x2c POLY_FT4, 0x3c POLY_GT4, 0x64 SPRT (w,h at +16), 0x74 SPRT_8, 0x7c SPRT_16
//   0xe1 draw mode (tpage = w & 0x1f, semi = (w >> 5) & 3, (w & 0x180) == 0x180 -> second VRAM MB)
//   VRAM: EXE 0x96e6e0, 8 columns of 256 bytes x 256 rows per half (upload 0xa710); 2nd MB at 0xa6e6e0
//   8-bit texel byte at x = (tpage>>1)*256 + u, y = ((tpage>>4)&1)*256 + v; texel 0 = transparent
//   sprites copy texels (0x40cab0); an axis-aligned quad with a same-size UV rect is a sprite too
//   polygon UV starts at the vertex texel (no half-texel offset), u/v wrap in the 256x256 page (0x40d420)
//   opaque polygon clut field: level = (clut >> 6) - 480 (0 if > 30); level > 0: fog row (0x92c390 +
//   level*256, 0x40cc50) when the CIF fog rows differ (0x96e3e0), else the averaged-UV texel fills it
//   semi-transparent: out = blend[mode][src*256 + dst], tables at 0x92e3d0 (0x40c8f0, 0x40e680)
//   semi polygon brightness (byte +4) != 0x80: src = shade[brightness][src], table 0x91c488 (0x40dd70)
//   clip rectangle 0xf3: polygon rows [y0, y1), sprite rows [0, y1)
//   frame clear: only when 0x96e698 is set, with palette index 0x96e424 (0x406ee0)
//   colour = palette[index] (BITMAPINFO colours at *(0x96e3f4) + 0x28)
//
// Replacement art (art_dir): a sprite block is keyed by FNV-1a 64 of (w, h, its w x h VRAM texels).
// <art_dir>/<key>_<w>x<h>.png, an 8-bit indexed PNG at k*w x k*h, is drawn into the same quad
// through the same index pipeline, so logical size, pivot and timing cannot change. art_dump writes
// every unique block once to <art_dir>/dump/ (indexed, current CIF palette) as the catalog.
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wincodec.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <string>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cwchar>
#include <cstdio>
#pragma warning(disable: 4996)

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

typedef uint8_t u8; typedef uint16_t u16; typedef uint32_t u32; typedef int16_t s16;

namespace {
struct Vtx { float x, y, u, v, cu, cv; u32 info, rect; };
// info: tpage | fog level<<8 | alt<<15 | flat<<16 | shade<<17 | brightness<<18 | sprite addressing<<26 | art<<31
// rect: the primitive's texel rectangle u0 | v0<<8 | (w-1)<<16 | (h-1)<<24 (smoothing neighbours stay inside it)

ID3D11Device *dev; ID3D11DeviceContext *ctx; IDXGISwapChain *swap; ID3D11RenderTargetView *rtv;
ID3D11Texture2D *vramTex, *palTex, *fogTex, *blendTex, *shadeTex, *idxTex, *idxCopy, *colTex, *colCopy;
ID3D11ShaderResourceView *vramSrv, *palSrv, *fogSrv, *blendSrv, *shadeSrv, *idxSrv, *copySrv, *colSrv, *colCopySrv;
ID3D11RenderTargetView *idxRtv, *colRtv; ID3D11BlendState *opaqueBlend;
ID3D11VertexShader *vs, *vsFull; ID3D11PixelShader *psOpaque, *psSemi, *psResolve, *psOverlay, *psSolid, *psBright;
ID3D11Texture2D *ovTex; ID3D11ShaderResourceView *ovSrv; ID3D11BlendState *alphaBlend;
HDC ovDC; HBITMAP ovBmp; u32 *ovBits; const int OVW = 1100, OVH = 92;
bool devOverlay; char lastFile[160], lastMap[160] = "-"; int lastIndex = -1;
ID3D11InputLayout *layout; ID3D11Buffer *vb, *cb; ID3D11RasterizerState *raster; ID3D11BlendState *noBlend;
UINT vbCap; int bw, bh, scale = 1, iw, ih;
struct Art { ID3D11ShaderResourceView *srv; bool rgba; };
struct Run { int mode, n, spr; Art art; int cy0, cy1; };  // mode: semi 0..3 or 4 = opaque; spr: index into sprs; clip rows
struct Spr { u32 tpage; bool alt; int u, v, w, h, sx, sy; };   // sx, sy: screen position (dump index)
std::vector<Vtx> verts; std::vector<Run> runs; std::vector<Spr> sprs;
bool artOn, artDump; wchar_t artDir[MAX_PATH];
bool smoothOn;                                          // smooth = xbr: original texels magnified by xBR on palette indices; F10 toggles
std::unordered_map<uint64_t, std::wstring> artAvail;   // key -> folder holding its file (filled before the worker starts)
std::unordered_set<uint64_t> artDumped; std::unordered_map<uint64_t, Art> artCache;  // main thread
std::atomic<long long> artBytes{ 0 };                    // texture memory of loaded replacement art
bool artShow = true;                                    // F11: replacement art on/off (compare with the original)
struct KeyEnt { uint64_t key; int stamp; };
int stBlocks, stPoly, stFresh, stAvail, stArt;   // last frame: keyed blocks, of which polygons, re-hashed, with a file, drawn with art
std::unordered_map<uint64_t, KeyEnt> keyCache;                // UV rect -> content key, valid while its VRAM tiles are unchanged
std::vector<int> tileStamp(64 * 32); int vramFrame;   // 32x32-texel tiles of the 2048x1024 VRAM
struct ArtJob { uint64_t key; int w, h; std::vector<u8> blk, pal; char line[160]; };   // pal non-empty = dump job; line: index row
std::mutex artMx; std::condition_variable artCv; std::deque<ArtJob> artJobs;
std::vector<std::pair<uint64_t, Art>> artDone;                                 // worker -> main
std::vector<u8> lin(2048 * 1024);
static bool otFresh = true; static int otCalls, otCallsLast;  // OTs drawn since the last present (gpu_draw_ot)
struct OtRec { u8 code, r, g, b; short x0, y0, x1, y1; u16 tpage; u8 semi, cy0, cy1; };  // debug: primitive log
static std::vector<OtRec> otLog;                       // written with HERC_GPU_SHOT as gpu_<n>_ot.txt
u32 curTpage, curSemi; bool curAlt; int frames; int clipY0, clipY1;  // clip rows (0xf3): EXE 0xa909b4, 0x43a48c
bool ready, failed;

const char *kSrc = R"(
cbuffer C : register(b0) { float2 scale; float2 off; int fog; int mode; int smooth; int pad; };
Texture2D<uint> vram : register(t0); Texture2D<float4> pal : register(t1); Texture2D<uint> fogt : register(t2);
Texture2D<uint> blendt : register(t3); Texture2D<uint> dstI : register(t4); Texture2D<uint> shade : register(t5);
Texture2D<float4> ov : register(t6); Texture2D<uint> art : register(t7); Texture2D<float4> artc : register(t8);
Texture2D<float4> dstC : register(t9);
struct VI { float2 p : POSITION; float2 uv : TEXCOORD0; float2 c : TEXCOORD1; uint info : TEXCOORD2; uint rect : TEXCOORD3; };
struct PI { float4 p : SV_POSITION; float2 uv : TEXCOORD0; nointerpolation float2 c : TEXCOORD1; nointerpolation uint info : TEXCOORD2;
            nointerpolation uint rect : TEXCOORD3; };
PI VS(VI i) { PI o; o.p = float4(i.p * scale + off, 0, 1); o.uv = i.uv; o.c = i.c; o.info = i.info; o.rect = i.rect; return o; }
uint fetch(int2 t, uint info) {
    uint tp = info & 0xff; uint alt = (info >> 15) & 1;
    if (((info >> 26) & 1) != 0) { t.y += t.x >> 8; }   // sprite copy 0x40cab0: linear in the page, u carries into v
    t &= 255;                                           // polygons 0x40d420: page[(v & 255) << 8 | (u & 255)]
    return vram.Load(int3((tp >> 1 & 7) * 256 + t.x, ((tp >> 4) & 1) * 256 + t.y + alt * 512, 0));
}
// The EXE samples each pixel at its top-left corner and steps the span (0x40d420): pixel i of a
// span shows floor(U_left + i*dU). Where U decreases along the span (mirrored parts, mirrored level
// cells) that is one texel above floor(U) of the continuous mapping, so add 1 on that axis.
uint texel(float2 uv, uint info, int2 dir) { return fetch(int2(floor(uv)) + dir, info); }
// Smoothing (smooth = xbr): xBR level 2, no-blend (Hyllian's rules), evaluated per output pixel at its
// position inside the texel. Edges are weighted by palette luma, but the result is always one of the
// neighbouring palette indices, so fog, shade, blend tables and fades stay the EXE's. Index 0 (transparent)
// takes part, which rounds silhouettes. Neighbours are clamped to the primitive's texel rectangle, so
// other blocks packed next to it in VRAM never leak in.
float lum(uint t) { return t == 0 ? -1000.0 : dot(pal.Load(int3(t, 0, 0)).rgb, float3(76.2, 149.7, 29.1)); }
float4 df4(float4 a, float4 b) { return abs(a - b); }
bool4 eq4(float4 a, float4 b) { return df4(a, b) < 15.0; }
float4 wd(float4 a, float4 b, float4 c, float4 d, float4 e, float4 f, float4 g, float4 h) {
    return df4(a, b) + df4(a, c) + df4(d, e) + df4(d, f) + 4.0 * df4(g, h);
}
uint smoothTexel(float2 uv, uint info, int2 dir, uint rect) {
    int2 c = int2(floor(uv)) + dir;
    int2 r0 = int2(rect & 255, (rect >> 8) & 255), r1 = r0 + int2((rect >> 16) & 255, rect >> 24);
    int2 lo = min(r0, c), hi = max(r1, c);
    float2 fp = frac(uv);
#define T(x, y) fetch(clamp(c + int2(x, y), lo, hi), info)
    uint A1 = T(-1, -2), B1 = T(0, -2), C1 = T(1, -2);
    uint A0 = T(-2, -1), A = T(-1, -1), B = T(0, -1), C = T(1, -1), C4 = T(2, -1);
    uint D0 = T(-2, 0), D = T(-1, 0), E = fetch(c, info), F = T(1, 0), F4 = T(2, 0);
    uint G0 = T(-2, 1), G = T(-1, 1), H = T(0, 1), I = T(1, 1), I4 = T(2, 1);
    uint G5 = T(-1, 2), H5 = T(0, 2), I5 = T(1, 2);
#undef T
    float lA1 = lum(A1), lB1 = lum(B1), lC1 = lum(C1), lA0 = lum(A0), lA = lum(A), lB = lum(B), lC = lum(C), lC4 = lum(C4);
    float lD0 = lum(D0), lD = lum(D), lE = lum(E), lF = lum(F), lF4 = lum(F4), lG0 = lum(G0), lG = lum(G), lH = lum(H);
    float lI = lum(I), lI4 = lum(I4), lG5 = lum(G5), lH5 = lum(H5), lI5 = lum(I5);
    uint4 bI = uint4(B, D, H, F), cI = uint4(C, A, G, I), dI = uint4(D, H, F, B), eI = uint4(E, E, E, E);
    uint4 fI = uint4(F, B, D, H), gI = uint4(G, I, C, A), hI = uint4(H, F, B, D);
    float4 b = float4(lB, lD, lH, lF), cc = float4(lC, lA, lG, lI), d = float4(lD, lH, lF, lB), e = float4(lE, lE, lE, lE);
    float4 f = float4(lF, lB, lD, lH), g = float4(lG, lI, lC, lA), h = float4(lH, lF, lB, lD), i = float4(lI, lC, lA, lG);
    float4 i4 = float4(lI4, lC1, lA0, lG5), i5 = float4(lI5, lC4, lA1, lG0);
    float4 h5 = float4(lH5, lF4, lB1, lD0), f4 = float4(lF4, lB1, lD0, lH5);
    bool4 rst = (eI != fI) && (eI != hI) &&
               ((!eq4(f, b) && !eq4(h, d)) || (eq4(e, i) && !eq4(f, i4) && !eq4(h, i5)) || eq4(e, g) || eq4(e, cc));
    bool4 edr = (wd(e, cc, g, i, h5, f4, h, f) < wd(h, d, i5, f, i4, b, e, i)) && rst;
    bool4 edrL = (2.0 * df4(f, g) <= df4(h, cc)) && (eI != gI) && (dI != gI);
    bool4 edrU = (df4(f, g) >= 2.0 * df4(h, cc)) && (eI != cI) && (bI != cI);
    float4 fx = float4(1, -1, -1, 1) * fp.y + float4(1, 1, -1, -1) * fp.x;
    float4 fxL = float4(1, -1, -1, 1) * fp.y + float4(0.5, 2, -0.5, -2) * fp.x;
    float4 fxU = float4(1, -1, -1, 1) * fp.y + float4(2, 0.5, -2, -0.5) * fp.x;
    bool4 nc = (edr && fx > float4(1.5, 0.5, -0.5, 0.5)) || (edr && edrL && fxL > float4(1, 1, -0.5, 0)) ||
               (edr && edrU && fxU > float4(2, 0, -1, 0.5));
    bool4 px = df4(e, f) <= df4(e, h);
    uint res1 = E, res2 = E;
    if (nc.x) res1 = px.x ? F : H;
    if (nc.z) res1 = px.z ? D : B;
    if (nc.y) res2 = px.y ? B : F;
    if (nc.w) res2 = px.w ? H : D;
    return abs(lum(res1) - lE) <= abs(lum(res2) - lE) ? res2 : res1;
}
// A source pixel is either a palette index (original texels, indexed art) or a true colour with
// coverage alpha (RGBA art). Index rules are the EXE's; colour rules are the formulas its CIF tables
// were built from (docs/hires-pipeline.md): shade c*b/128, fog rows identity.
struct Src { uint idx; float4 c; bool tc; };
Src source(PI i) {
    Src s; s.idx = 0; s.tc = false;
    uint level = (i.info >> 8) & 0x1f; bool flat = ((i.info >> 16) & 1) != 0;
    bool shadeOn = ((i.info >> 17) & 1) != 0; float b = (float)((i.info >> 18) & 0xff);
    uint w, h;
    if ((i.info >> 30) == 3) {                          // RGBA art
        artc.GetDimensions(w, h);
        s.c = artc.Load(int3(min(int2(i.uv * float2(w, h)), int2(w - 1, h - 1)), 0));
        if (s.c.a < 0.5 / 255) discard;
        if (shadeOn) s.c.rgb = min(s.c.rgb * b / 128, 1);
        s.tc = true; return s;
    }
    uint t;
    if ((i.info >> 31) != 0) { art.GetDimensions(w, h); t = art.Load(int3(min(int2(i.uv * float2(w, h)), int2(w - 1, h - 1)), 0)); }
    else if (flat) t = texel(i.c, i.info, int2(0, 0));
    else {
        int2 dir = int2(ddx(i.uv.x) < 0 ? 1 : 0, ddy(i.uv.y) < 0 ? 1 : 0);
        t = smooth != 0 ? smoothTexel(i.uv, i.info, dir, i.rect) : texel(i.uv, i.info, dir);
    }
    if (t == 0) discard;
    if (level > 0 && fog != 0 && !flat) t = fogt.Load(int3(t, level, 0));
    if (shadeOn) t = shade.Load(int3(t, (uint)b, 0));  // 0x40dd70 / 0x40c700
    s.idx = t; s.c = float4(pal.Load(int3(t, 0, 0)).rgb, 1); return s;
}
// Targets: 0 = index (bit 8 = true colour), 1 = colour (always valid; alpha-blended for RGBA art).
struct PO { uint idx : SV_TARGET0; float4 c : SV_TARGET1; };
PO PSOpaque(PI i) { Src s = source(i); PO o; o.idx = s.tc ? 0x100 : s.idx; o.c = s.c; return o; }
float3 psx(int m, float3 a, float3 d) {                // PSX semi-transparency: 0 (a+d)/2, 1 a+d, 2 d-a, 3 d+a/4
    if (m == 0) return (a + d) * 0.5;
    if (m == 1) return min(a + d, 1);
    if (m == 2) return max(d - a, 0);
    return min(d + a * 0.25, 1);
}
PO PSSemi(PI i) {
    Src s = source(i);
    int3 p = int3(int2(i.p.xy), 0);
    uint di = dstI.Load(p); float4 dc = dstC.Load(p);
    PO o;
    if (!s.tc && di < 0x100) { o.idx = blendt.Load(int3(di, mode * 256 + s.idx, 0)); o.c = float4(pal.Load(int3(o.idx, 0, 0)).rgb, 1); }
    else { o.idx = 0x100; o.c = float4(lerp(dc.rgb, psx(mode, s.c.rgb, dc.rgb), s.c.a), 1); }
    return o;
}
struct FO { float4 p : SV_POSITION; float2 uv : TEXCOORD0; };
FO VSFull(uint id : SV_VertexID) { FO o; float2 t = float2((id << 1) & 2, id & 2); o.p = float4(t * float2(2, -2) + float2(-1, 1), 0, 1); o.uv = t; return o; }
float4 PSOverlay(FO i) : SV_TARGET { uint w, h; ov.GetDimensions(w, h); return ov.Load(int3(int2(i.uv * float2(w, h)), 0)); }
float4 PSSolid(FO i) : SV_TARGET { return float4(1, 0.2, 0.2, 1); }
// GFX_FrameBrightness (EXE 0x4070c0): before present every frame pixel becomes shade[b][pixel] when
// b (0x436a0c) != 0x80; it rewrites the persistent frame, so it is applied to the targets in place.
PO PSBright(FO i) {
    int3 p = int3(int2(i.p.xy), 0);
    uint v = dstI.Load(p); float4 c = dstC.Load(p);
    PO o;
    if (v >= 0x100) { o.idx = v; o.c = float4(min(c.rgb * mode / 128.0, 1), 1); }
    else { o.idx = shade.Load(int3(v, mode, 0)); o.c = float4(pal.Load(int3(o.idx, 0, 0)).rgb, 1); }
    return o;
}
float4 PSResolve(FO i) : SV_TARGET {
    uint w, h; dstI.GetDimensions(w, h);
    int3 p = int3(int2(i.uv * float2(w, h)), 0);
    uint v = dstI.Load(p);
    return v >= 0x100 ? float4(dstC.Load(p).rgb, 1) : float4(pal.Load(int3(v, 0, 0)).rgb, 1);
}
)";

bool Compile(const char *entry, const char *target, ID3DBlob **out) {
    ID3DBlob *err = nullptr;
    return SUCCEEDED(D3DCompile(kSrc, strlen(kSrc), nullptr, nullptr, nullptr, entry, target, 0, 0, out, &err));
}

void Tex(int w, int h, DXGI_FORMAT f, UINT bind, ID3D11Texture2D **t, ID3D11ShaderResourceView **s) {
    D3D11_TEXTURE2D_DESC td = {}; td.Width = w; td.Height = h; td.MipLevels = 1; td.ArraySize = 1; td.Format = f;
    td.SampleDesc.Count = 1; td.Usage = D3D11_USAGE_DEFAULT; td.BindFlags = bind;
    dev->CreateTexture2D(&td, nullptr, t); if (s) dev->CreateShaderResourceView(*t, nullptr, s);
}

bool Init(HWND h) {
    RECT r; GetClientRect(h, &r); bw = r.right - r.left; bh = r.bottom - r.top;
    if (bw <= 0 || bh <= 0) return false;
    DXGI_SWAP_CHAIN_DESC sd = {};
    // 3 buffers: under independent flip (borderless fullscreen) the last frame is still scanned out when the next starts
    sd.BufferCount = 3; sd.BufferDesc.Width = bw; sd.BufferDesc.Height = bh;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = h; sd.SampleDesc.Count = 1; sd.Windowed = TRUE; sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
                                             D3D11_SDK_VERSION, &sd, &swap, &dev, nullptr, &ctx))) return false;
    ID3D11Texture2D *bb; swap->GetBuffer(0, IID_PPV_ARGS(&bb)); dev->CreateRenderTargetView(bb, nullptr, &rtv); bb->Release();
    ID3DBlob *b[8] = {};
    if (!Compile("VS", "vs_4_0", &b[0]) || !Compile("PSOpaque", "ps_4_0", &b[1]) || !Compile("PSSemi", "ps_4_0", &b[2]) ||
        !Compile("VSFull", "vs_4_0", &b[3]) || !Compile("PSResolve", "ps_4_0", &b[4]) ||
        !Compile("PSOverlay", "ps_4_0", &b[5]) || !Compile("PSSolid", "ps_4_0", &b[6]) || !Compile("PSBright", "ps_4_0", &b[7])) return false;
    dev->CreatePixelShader(b[7]->GetBufferPointer(), b[7]->GetBufferSize(), nullptr, &psBright);
    dev->CreatePixelShader(b[5]->GetBufferPointer(), b[5]->GetBufferSize(), nullptr, &psOverlay);
    dev->CreatePixelShader(b[6]->GetBufferPointer(), b[6]->GetBufferSize(), nullptr, &psSolid);
    dev->CreateVertexShader(b[0]->GetBufferPointer(), b[0]->GetBufferSize(), nullptr, &vs);
    dev->CreatePixelShader(b[1]->GetBufferPointer(), b[1]->GetBufferSize(), nullptr, &psOpaque);
    dev->CreatePixelShader(b[2]->GetBufferPointer(), b[2]->GetBufferSize(), nullptr, &psSemi);
    dev->CreateVertexShader(b[3]->GetBufferPointer(), b[3]->GetBufferSize(), nullptr, &vsFull);
    dev->CreatePixelShader(b[4]->GetBufferPointer(), b[4]->GetBufferSize(), nullptr, &psResolve);
    D3D11_INPUT_ELEMENT_DESC il[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 2, DXGI_FORMAT_R32_UINT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 3, DXGI_FORMAT_R32_UINT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 } };
    dev->CreateInputLayout(il, 5, b[0]->GetBufferPointer(), b[0]->GetBufferSize(), &layout);
    scale = (std::min)(bw / 320, bh / 240); if (scale < 1) scale = 1;
    { char e[16]; if (GetEnvironmentVariableA("HERC_GPU_SCALE", e, sizeof e) && atoi(e) > 0) scale = atoi(e); }  // perf: 4K targets in a small window
    iw = 320 * scale; ih = 240 * scale;
    Tex(2048, 1024, DXGI_FORMAT_R8_UINT, D3D11_BIND_SHADER_RESOURCE, &vramTex, &vramSrv);
    Tex(256, 1, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE, &palTex, &palSrv);
    Tex(256, 32, DXGI_FORMAT_R8_UINT, D3D11_BIND_SHADER_RESOURCE, &fogTex, &fogSrv);
    Tex(256, 1024, DXGI_FORMAT_R8_UINT, D3D11_BIND_SHADER_RESOURCE, &blendTex, &blendSrv);
    Tex(256, 256, DXGI_FORMAT_R8_UINT, D3D11_BIND_SHADER_RESOURCE, &shadeTex, &shadeSrv);
    Tex(iw, ih, DXGI_FORMAT_R16_UINT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, &idxTex, &idxSrv);
    Tex(iw, ih, DXGI_FORMAT_R16_UINT, D3D11_BIND_SHADER_RESOURCE, &idxCopy, &copySrv);
    Tex(iw, ih, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, &colTex, &colSrv);
    Tex(iw, ih, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE, &colCopy, &colCopySrv);
    dev->CreateRenderTargetView(idxTex, nullptr, &idxRtv); dev->CreateRenderTargetView(colTex, nullptr, &colRtv);
    { D3D11_BLEND_DESC ob = {}; ob.IndependentBlendEnable = TRUE;   // index target unblended, colour target alpha-blended
      ob.RenderTarget[0].RenderTargetWriteMask = 0xF;
      D3D11_RENDER_TARGET_BLEND_DESC &t = ob.RenderTarget[1]; t.BlendEnable = TRUE;
      t.SrcBlend = D3D11_BLEND_SRC_ALPHA; t.DestBlend = D3D11_BLEND_INV_SRC_ALPHA; t.BlendOp = D3D11_BLEND_OP_ADD;
      t.SrcBlendAlpha = D3D11_BLEND_ONE; t.DestBlendAlpha = D3D11_BLEND_ZERO; t.BlendOpAlpha = D3D11_BLEND_OP_ADD;
      t.RenderTargetWriteMask = 0xF; dev->CreateBlendState(&ob, &opaqueBlend); }
    Tex(OVW, OVH, DXGI_FORMAT_B8G8R8A8_UNORM, D3D11_BIND_SHADER_RESOURCE, &ovTex, &ovSrv);
    { D3D11_BLEND_DESC ab = {}; D3D11_RENDER_TARGET_BLEND_DESC &t = ab.RenderTarget[0]; t.BlendEnable = TRUE;
      t.SrcBlend = D3D11_BLEND_SRC_ALPHA; t.DestBlend = D3D11_BLEND_INV_SRC_ALPHA; t.BlendOp = D3D11_BLEND_OP_ADD;
      t.SrcBlendAlpha = D3D11_BLEND_ONE; t.DestBlendAlpha = D3D11_BLEND_ZERO; t.BlendOpAlpha = D3D11_BLEND_OP_ADD;
      t.RenderTargetWriteMask = 0xF; dev->CreateBlendState(&ab, &alphaBlend); }
    { BITMAPINFO bi = {}; bi.bmiHeader.biSize = sizeof bi.bmiHeader; bi.bmiHeader.biWidth = OVW; bi.bmiHeader.biHeight = -OVH;
      bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; ovDC = CreateCompatibleDC(nullptr);
      ovBmp = CreateDIBSection(ovDC, &bi, DIB_RGB_COLORS, (void **)&ovBits, nullptr, 0); SelectObject(ovDC, ovBmp);
      SelectObject(ovDC, CreateFontA(26, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, 0, NONANTIALIASED_QUALITY, FIXED_PITCH, "Consolas")); }
    float zero[4] = { 0, 0, 0, 1 }; ctx->ClearRenderTargetView(idxRtv, zero); ctx->ClearRenderTargetView(colRtv, zero);
    D3D11_BUFFER_DESC cbd = {}; cbd.ByteWidth = 32; cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC; cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; dev->CreateBuffer(&cbd, nullptr, &cb);
    D3D11_BLEND_DESC bd = {}; bd.RenderTarget[0].RenderTargetWriteMask = 0xF; dev->CreateBlendState(&bd, &noBlend);
    D3D11_RASTERIZER_DESC rd = {}; rd.FillMode = D3D11_FILL_SOLID; rd.CullMode = D3D11_CULL_NONE; rd.ScissorEnable = TRUE;
    dev->CreateRasterizerState(&rd, &raster);
    return true;
}

// Copy the EXE's VRAM into lin, comparing first: only changed rows are copied, the 32x32 tiles they touch
// are stamped with this frame (sprite cells are re-unpacked often, level pages stay put, so cached block
// keys are reused until their texels change), and only the dirty rows of each 256-wide column are uploaded.
bool vramInit; int dirtyY0[32], dirtyY1[32];        // per (half, column): rows y0..y1 changed this frame
void Unswizzle() {
    const u8 *v = (const u8 *)0x0096e6e0;
    vramFrame++;
    for (int half = 0; half < 4; half++)          // halves 0,1 = first MB (y 0..511), 2,3 = second MB
        for (int c = 0; c < 8; c++) {
            int &y0 = dirtyY0[half * 8 + c], &y1 = dirtyY1[half * 8 + c];
            y0 = 256; y1 = -1;
            for (int y = 0; y < 256; y++) {
                const u8 *src = v + half * 0x80000 + c * 0x10000 + y * 256;
                u8 *dst = &lin[(size_t)(half * 256 + y) * 2048 + c * 256];
                if (vramInit && !memcmp(dst, src, 256)) continue;
                for (int t = 0; t < 8; t++)
                    if (!vramInit || memcmp(dst + t * 32, src + t * 32, 32)) tileStamp[((half * 256 + y) >> 5) * 64 + c * 8 + t] = vramFrame;
                memcpy(dst, src, 256);
                if (y < y0) y0 = y;
                y1 = y;
            }
        }
    vramInit = true;
}

void UploadVram() {
    for (int i = 0; i < 32; i++) {
        if (dirtyY1[i] < dirtyY0[i]) continue;
        UINT x = (i & 7) * 256, y0 = (i >> 3) * 256 + dirtyY0[i], y1 = (i >> 3) * 256 + dirtyY1[i] + 1;
        D3D11_BOX b = { x, y0, 0, x + 256, y1, 1 };
        ctx->UpdateSubresource(vramTex, 0, &b, &lin[(size_t)y0 * 2048 + x], 2048, 0);
    }
}

inline u32 Rect(int u, int v, int w, int h) {
    return (u32)(u & 255) | (u32)(v & 255) << 8 | (u32)((std::min)((std::max)(w, 1), 256) - 1) << 16 | (u32)((std::min)((std::max)(h, 1), 256) - 1) << 24;
}
inline void Push(float x, float y, float u, float v, float cu, float cv, u32 info, u32 rect) { verts.push_back({ x, y, u, v, cu, cv, info, rect }); }

void AddRun(int mode, int n, int spr, int cy0, int cy1) {
    if (spr < 0 && !runs.empty() && runs.back().mode == mode && mode == 4 && runs.back().spr < 0 &&
        runs.back().cy0 == cy0 && runs.back().cy1 == cy1) runs.back().n += n;
    else runs.push_back({ mode, n, spr, { nullptr, false }, cy0, cy1 });
}

// Sprite copy (0x40cab0 raw, 0x40c8f0 blend, 0x40c700 shade + blend): texel (u + i, v + j) for
// pixel (x + i, y + j), clipped to x [0, 320) and y [0, clip y1). Shade only on the semi path.
void SpriteAt(int x, int y, int u, int v, int w, int h, u32 tpage, bool alt, int mode, int bright) {
    if (w <= 0 || h <= 0) return;
    u32 info = (tpage & 0x1f) | (alt ? 1u << 15 : 0) | (1u << 26) | (bright >= 0 ? (1u << 17) | ((u32)bright << 18) : 0);
    float X[4] = { (float)x, (float)(x + w), (float)x, (float)(x + w) }, Y[4] = { (float)y, (float)y, (float)(y + h), (float)(y + h) };
    float U[4] = { (float)u, (float)(u + w), (float)u, (float)(u + w) }, V[4] = { (float)v, (float)v, (float)(v + h), (float)(v + h) };
    float A[4] = { 0, 1, 0, 1 }, B[4] = { 0, 0, 1, 1 };   // replacement-art coordinates (cu, cv)
    int tri[6] = { 0, 1, 2, 1, 3, 2 };
    u32 rect = Rect(u, v, w, h);
    for (int k : tri) Push(X[k], Y[k], U[k], V[k], A[k], B[k], info, rect);
    int spr = -1;
    if (artOn) { spr = (int)sprs.size(); sprs.push_back({ tpage & 0x1f, alt, u, v, w, h, x, y }); }
    AddRun(mode, 6, spr, 0, clipY1);
}

// Textured polygon (GFX_DrawOTag 0xac60 -> 0x40d420 opaque / 0x40e680 semi). Rules from the EXE:
// - an axis-aligned quad whose UV rect has the same size (not mirrored) is drawn by the sprite copy;
// - edges start at the vertex with UV = texel << 16 (no half-texel offset); rows y0 .. y1-1, spans
//   [floor(xl), floor(xr)); the texel is fetched with u, v wrapped to the 256x256 page;
// - opaque: fog level (clut >> 6) - 480 when the CIF fog rows differ (0x96e3e0), else a flat fill
//   with the texel at the averaged UV (0x40c2e0); never shaded;
// - semi: brightness byte (+4) != 0x80 -> blend[mode][shade[b][src]*256 + dst] (0x40dd70), else
//   blend only; no fog;
// - rows are clipped to the 0xf3 clip rectangle, x to the screen.
void Poly(const u8 *p, int n, const int *xy, const int *uvo, int tpo, int clo, bool semi) {
    u16 tp = *(const u16 *)(p + tpo), cl = *(const u16 *)(p + clo);
    bool alt = (tp & 0x180) == 0x180;
    int mode = semi ? ((tp >> 5) & 3) : 4, bright = p[4];
    int X[4], Y[4], U[4], V[4];
    for (int k = 0; k < n; k++) {
        X[k] = *(const s16 *)(p + xy[k]); Y[k] = *(const s16 *)(p + xy[k] + 2);
        U[k] = p[uvo[k]]; V[k] = p[uvo[k] + 1];
    }
    if (n == 4 && X[0] <= X[3] && Y[0] <= Y[3] && Y[0] == Y[1] && Y[3] == Y[2] && X[0] == X[2] && X[1] == X[3] &&
        U[0] <= U[3] && V[0] <= V[3] && V[0] == V[1] && V[3] == V[2] && U[0] == U[2] && U[1] == U[3] &&
        U[3] - U[0] == X[1] - X[0] && V[3] - V[0] == Y[2] - Y[0]) {
        SpriteAt(X[0], Y[0], U[0], V[0], X[1] - X[0], Y[2] - Y[0], tp & 0x1f, alt, mode, semi && bright != 0x80 ? bright : -1);
        return;
    }
    int level = 0; bool flat = false;
    if (!semi) {
        level = (cl >> 6) - 480; if (level < 0 || level > 30) level = 0;
        flat = level > 0 && *(const u8 *)0x0096e3e0 == 0;
    }
    // The EXE rasterisers (0x40d420 opaque, 0x40e680 semi, 0x40dd70, 0x40cc50 fog) draw a polygon only when its
    // vertex extents are below 0x800 in x and y. Near-plane polygons projected to huge coordinates (ENG3:
    // about -3105..8352 x -20386..28766, CYCLOPS) are dropped there, while the GPU would cover the whole screen.
    // The flat path (0x40c2e0, not yet read) keeps the old behaviour.
    if (!flat) {
        int x0 = X[0], x1 = X[0], y0 = Y[0], y1 = Y[0];
        for (int k = 1; k < n; k++) { x0 = (std::min)(x0, X[k]); x1 = (std::max)(x1, X[k]); y0 = (std::min)(y0, Y[k]); y1 = (std::max)(y1, Y[k]); }
        if (x1 - x0 >= 0x800 || y1 - y0 >= 0x800) return;
    }
    // flat-fill texel: (u0 + u1 + u2 + u3) >> 2 with u3 = u2 for triangles (0xac60 copies the last vertex)
    float cu = (float)((U[0] + U[1] + U[2] + (n == 4 ? U[3] : U[2])) >> 2), cv = (float)((V[0] + V[1] + V[2] + (n == 4 ? V[3] : V[2])) >> 2);
    u32 info = (tp & 0x1f) | (level << 8) | (alt ? 1u << 15 : 0) | (flat ? 1u << 16 : 0) |
               (semi && bright != 0x80 ? (1u << 17) | ((u32)bright << 18) : 0);
    // Replacement-art key and coordinates (cu, cv carry them; flat polygons are never replaced).
    // ENG1 part quads (ANIM_BuildSpriteQuads 0x4010): mirrored parts span UV u-1 .. u+w-1 over a mirrored
    // rect (u = 0: 0 .. w-1), unmirrored parts ending at u = 256 span u .. u+w-1 over w pixels. The EXE
    // samples exactly the part's w texels in both cases, so they are keyed like the unmirrored part and
    // the art is placed by screen position. Other polygons key the texels in their UV bounding box.
    float A[4] = {}, B[4] = {};
    int ku = 0, kv = 0, kw = 0, kh = 0;
    bool keyed = artOn && !flat;
    if (!flat) {                                        // the key rect is also the smoothing rect
        int w = X[1] - X[0], h = Y[2] - Y[0];
        bool rect = n == 4 && Y[0] == Y[1] && Y[2] == Y[3] && X[0] == X[2] && X[1] == X[3] && V[0] == V[1] && V[2] == V[3] &&
                    U[0] == U[2] && U[1] == U[3] && h > 0 && V[2] - V[0] == h;
        if (rect && w < 0 && (U[1] - U[0] == -w || (U[0] == 0 && U[1] == -w - 1))) {
            kw = -w; ku = U[1] - U[0] == kw ? U[0] + 1 : 0;
            for (int k = 0; k < 4; k++) { A[k] = (float)(X[0] - X[k]) / kw; B[k] = (float)(Y[k] - Y[0]) / h; }
        } else if (rect && w > 0 && U[1] - U[0] == w - 1) {
            kw = w; ku = U[0];
            for (int k = 0; k < 4; k++) { A[k] = (float)(X[k] - X[0]) / kw; B[k] = (float)(Y[k] - Y[0]) / h; }
        } else {
            int u1 = 0, v1 = 0; ku = 255; kv = 255;
            for (int k = 0; k < n; k++) {
                ku = (std::min)(ku, U[k]); kv = (std::min)(kv, V[k]); u1 = (std::max)(u1, U[k]); v1 = (std::max)(v1, V[k]);
            }
            kw = u1 - ku + 1; kh = v1 - kv + 1;
            for (int k = 0; k < n; k++) { A[k] = (float)(U[k] - ku) / kw; B[k] = (float)(V[k] - kv) / kh; }
        }
        if (!kh) { kv = V[0]; kh = h; }
    }
    int tri[6] = { 0, 1, 2, 1, 3, 2 };
    u32 rect = flat ? 0 : Rect(ku, kv, kw, kh);
    for (int k = 0; k < (n == 4 ? 6 : 3); k++) {
        int i = tri[k];
        Push((float)X[i], (float)Y[i], (float)U[i], (float)V[i], keyed ? A[i] : cu, keyed ? B[i] : cv, info, rect);
    }
    int spr = -1;
    if (keyed) { spr = (int)sprs.size(); sprs.push_back({ (u32)(tp & 0x1f), alt, ku, kv, kw, kh, (std::min)((std::min)(X[0], X[1]), X[2]), (std::min)((std::min)(Y[0], Y[1]), Y[2]) }); }
    AddRun(mode, n == 4 ? 6 : 3, spr, clipY0, clipY1);
}

void Sprite(const u8 *p, int w, int h, bool semi) {
    SpriteAt(*(const s16 *)(p + 8), *(const s16 *)(p + 10), p[12], p[13], w, h, curTpage, curAlt, semi ? (int)curSemi : 4, -1);
}

void SavePng(const wchar_t *path, const u8 *bgra, int w, int h, int pitch) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    IWICImagingFactory *f = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&f)))) return;
    IWICStream *ws; f->CreateStream(&ws); ws->InitializeFromFilename(path, GENERIC_WRITE);
    IWICBitmapEncoder *enc; f->CreateEncoder(GUID_ContainerFormatPng, nullptr, &enc); enc->Initialize(ws, WICBitmapEncoderNoCache);
    IWICBitmapFrameEncode *fr; enc->CreateNewFrame(&fr, nullptr); fr->Initialize(nullptr); fr->SetSize(w, h);
    WICPixelFormatGUID fmt = GUID_WICPixelFormat32bppBGRA; fr->SetPixelFormat(&fmt);
    fr->WritePixels(h, pitch, pitch * h, (BYTE *)bgra); fr->Commit(); enc->Commit();
    fr->Release(); enc->Release(); ws->Release(); f->Release();
}

// HERC_GPU_SHOT=<dir> + HERC_GPU_SHOT_FRAME=<n>[,<n>...]: at each present n save gpu_<n>.png (back
// buffer) and soft_<n>.png (the EXE's own 8-bit 320x240 frame, g_frameBits 0x96e3d8, with its palette).
void MaybeShot() {
    static bool init; static wchar_t dir[MAX_PATH]; static std::vector<int> want;
    if (!init) {
        init = true; char b[256];
        if (GetEnvironmentVariableA("HERC_GPU_SHOT_FRAME", b, sizeof b) && GetEnvironmentVariableW(L"HERC_GPU_SHOT", dir, MAX_PATH))
            for (char *t = strtok(b, ","); t; t = strtok(nullptr, ",")) want.push_back(atoi(t));
    }
    bool hit = false; for (int w : want) hit |= w == frames;
    if (!hit) return;
    int target = frames;
    wchar_t path[MAX_PATH];
    ID3D11Texture2D *bb; swap->GetBuffer(0, IID_PPV_ARGS(&bb));
    D3D11_TEXTURE2D_DESC td; bb->GetDesc(&td);
    td.Usage = D3D11_USAGE_STAGING; td.BindFlags = 0; td.CPUAccessFlags = D3D11_CPU_ACCESS_READ; td.MiscFlags = 0;
    ID3D11Texture2D *st; dev->CreateTexture2D(&td, nullptr, &st); ctx->CopyResource(st, bb);
    D3D11_MAPPED_SUBRESOURCE m; ctx->Map(st, 0, D3D11_MAP_READ, 0, &m);
    std::vector<u8> px((size_t)m.RowPitch * td.Height); memcpy(px.data(), m.pData, px.size()); ctx->Unmap(st, 0);
    for (size_t i = 0; i + 3 < px.size(); i += 4) { u8 t = px[i]; px[i] = px[i + 2]; px[i + 2] = t; }
    swprintf(path, MAX_PATH, L"%ls\\gpu_%d.png", dir, target); SavePng(path, px.data(), td.Width, td.Height, m.RowPitch);
    st->Release(); bb->Release();
    const u8 *bits = *(const u8 **)0x0096e3d8, *bmi = *(const u8 **)0x0096e3f4;
    if (bits && bmi) {
        std::vector<u8> soft(320 * 240 * 4);
        for (int i = 0; i < 320 * 240; i++) { const u8 *c = bmi + 0x28 + bits[i] * 4; soft[i*4] = c[0]; soft[i*4+1] = c[1]; soft[i*4+2] = c[2]; soft[i*4+3] = 255; }
        swprintf(path, MAX_PATH, L"%ls\\soft_%d.png", dir, target); SavePng(path, soft.data(), 320, 240, 320 * 4);
    }
    swprintf(path, MAX_PATH, L"%ls\\gpu_%d_ot.txt", dir, target);
    if (FILE *o = _wfopen(path, L"w")) {
        fprintf(o, "i code rgb x0 y0 x1 y1 tpage semi clip_y0 clip_y1 (OT walk order, back to front)\n");
        for (size_t i = 0; i < otLog.size(); i++) {
            const OtRec &r = otLog[i];
            fprintf(o, "%u %02x %02x%02x%02x %d %d %d %d %u %u %u %u\n", (unsigned)i, r.code, r.r, r.g, r.b, r.x0, r.y0, r.x1, r.y1,
                    r.tpage, r.semi, r.cy0, r.cy1);
        }
        fclose(o);
    }
    swprintf(path, MAX_PATH, L"%ls\\gpu_%d.txt", dir, target);
    FILE *f = _wfopen(path, L"w");
    if (f) {
        int semi = 0; for (auto &r : runs) semi += r.mode != 4;
        fprintf(f, "clear_flag %u clear_index %u clear_rgb %u,%u,%u fog %u verts %u runs %u semi_runs %d scale %d\n",
                *(const u8 *)0x0096e698, *(const u8 *)0x0096e424, *(const u8 *)0x0096e699, *(const u8 *)0x0096e69a,
                *(const u8 *)0x0096e69b, *(const u8 *)0x0096e3e0, (unsigned)verts.size(), (unsigned)runs.size(), semi, scale);
        fprintf(f, "art blocks %d poly %d fresh %d avail %d drawn %d ot_calls %d\n", stBlocks, stPoly, stFresh, stAvail, stArt, otCallsLast);
        fprintf(f, "frame_brightness %u\n", *(const u8 *)0x00436a0c);
        fprintf(f, "module %s map %s\n", GetModuleHandleA("eng1.dll") ? "ENG1" : GetModuleHandleA("eng3.dll") ? "ENG3" :
                GetModuleHandleA("title.dll") ? "TITLE" : "?", lastMap);
        fclose(f);
    }
}
// Dev overlay text: frame, module, hero state/frame/position (ENG1 globals), last archive file.
short heroBox[6]; bool heroBoxOn;   // hero collision box in screen pixels (x0, y0, x1, y1)
void DrawOverlayText(int *heroSx, int *heroSy) {
    char l1[200], l2[200], l3[220];
    const u8 *gs = (const u8 *)0x00615c98;                      // g_gameState (shared with the DLLs)
    HMODULE e1 = GetModuleHandleA("eng1.dll"), e3 = GetModuleHandleA("eng3.dll"), ti = GetModuleHandleA("title.dll");
    sprintf(l1, "frame %d  module %s  gpu x%d", *(const int *)(gs + 0x38), e1 ? "ENG1" : e3 ? "ENG3" : ti ? "TITLE" : "?", scale);
    sprintf(l2, "hero: -"); *heroSx = *heroSy = -9999; heroBoxOn = false;
    if (e1) {
        const u8 *h = *(const u8 **)((const u8 *)e1 + 0x72860);
        if (h && !IsBadReadPtr(h, 0x140)) {
            sprintf(l2, "hero state %u frame %u  x %d y %d z %d  vx %.2f vy %.2f",
                    *(const u16 *)((const u8 *)e1 + 0x72108), *(const u16 *)(h + 0x34), *(const int *)h >> 16,
                    *(const int *)(h + 4) >> 16, *(const int *)(h + 8) >> 16, *(const int *)(h + 0xa8) / 65536.0,
                    *(const int *)(h + 0xb8) / 65536.0);
            *heroSx = *(const s16 *)(h + 0xc); *heroSy = *(const s16 *)(h + 0xe);
            // world box at hero+0x88 {x0, y0, z0, x1, y1, z1} (ENG1 0x2a880 builds it, 0x2a7a0 tests overlap),
            // placed on screen relative to the hero's world (+0, +4) and screen (+0xc, +0xe) position
            const s16 *b = (const s16 *)(h + 0x88); int wx = *(const int *)h >> 16, wy = *(const int *)(h + 4) >> 16;
            heroBox[0] = (short)(*heroSx + b[0] - wx); heroBox[1] = (short)(*heroSy + b[1] - wy);
            heroBox[2] = (short)(*heroSx + b[3] - wx); heroBox[3] = (short)(*heroSy + b[4] - wy);
            heroBoxOn = b[3] > b[0] && b[4] > b[1];
        }
    }
    sprintf(l3, "last file: %s (#%d)", lastIndex >= 0 ? lastFile : "-", lastIndex);
    for (int i = 0; i < OVW * OVH; i++) ovBits[i] = 0xA0000000;
    SetBkMode(ovDC, TRANSPARENT); SetTextColor(ovDC, RGB(255, 255, 120));
    TextOutA(ovDC, 6, 2, l1, (int)strlen(l1)); TextOutA(ovDC, 6, 31, l2, (int)strlen(l2)); TextOutA(ovDC, 6, 60, l3, (int)strlen(l3));
    GdiFlush();
    for (int i = 0; i < OVW * OVH; i++) if ((ovBits[i] & 0x00FFFFFF) != 0) ovBits[i] |= 0xFF000000;
    ctx->UpdateSubresource(ovTex, 0, nullptr, ovBits, OVW * 4, 0);
}
void BlockTexels(const Spr &s, std::vector<u8> &out) {
    out.resize((size_t)s.w * s.h);
    int bx = (s.tpage >> 1 & 7) * 256, by = ((s.tpage >> 4) & 1) * 256 + (s.alt ? 512 : 0);
    for (int y = 0; y < s.h; y++)
        for (int x = 0; x < s.w; x++)
            out[(size_t)y * s.w + x] = lin[(size_t)(by + (std::min)(s.v + y, 255)) * 2048 + bx + (std::min)(s.u + x, 255)];
}

uint64_t BlockKey(const Spr &s, const std::vector<u8> &blk) {
    uint64_t k = 1469598103934665603ull;
    auto mix = [&](u8 b) { k = (k ^ b) * 1099511628211ull; };
    mix((u8)s.w); mix((u8)(s.w >> 8)); mix((u8)s.h); mix((u8)(s.h >> 8));
    for (u8 b : blk) mix(b);
    return k;
}

IWICImagingFactory *Wic() {
    static IWICImagingFactory *f;
    if (!f) { CoInitializeEx(nullptr, COINIT_MULTITHREADED); CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&f)); }
    return f;
}

void SaveIndexedPng(const wchar_t *path, const u8 *idx, int w, int h, const u8 *bmi) {
    IWICImagingFactory *f = Wic(); if (!f) return;
    WICColor cols[256];
    for (int i = 0; i < 256; i++)
        cols[i] = bmi ? 0xFF000000u | bmi[0x28 + i*4 + 2] << 16 | bmi[0x28 + i*4 + 1] << 8 | bmi[0x28 + i*4] : 0xFF000000u | i * 0x010101u;
    IWICPalette *pal; f->CreatePalette(&pal); pal->InitializeCustom(cols, 256);
    IWICStream *ws; f->CreateStream(&ws); ws->InitializeFromFilename(path, GENERIC_WRITE);
    IWICBitmapEncoder *enc; f->CreateEncoder(GUID_ContainerFormatPng, nullptr, &enc); enc->Initialize(ws, WICBitmapEncoderNoCache);
    IWICBitmapFrameEncode *fr; enc->CreateNewFrame(&fr, nullptr); fr->Initialize(nullptr); fr->SetSize(w, h);
    WICPixelFormatGUID fmt = GUID_WICPixelFormat8bppIndexed; fr->SetPixelFormat(&fmt); fr->SetPalette(pal);
    fr->WritePixels(h, w, w * h, (BYTE *)idx); fr->Commit(); enc->Commit();
    fr->Release(); enc->Release(); ws->Release(); pal->Release();
}

// <artDir>/<key>_<w>x<h>.png: an 8-bit indexed PNG becomes an R8_UINT index texture (exact palette
// indices); any other format becomes RGBA8 true colour. Null if missing or not the same integer
// multiple of the logical block size on both axes.
Art LoadArt(uint64_t key, int w, int h) {
    auto where = artAvail.find(key);
    Art art = { nullptr, false };
    if (where == artAvail.end()) return art;
    wchar_t path[MAX_PATH]; swprintf(path, MAX_PATH, L"%ls\\%016llx_%dx%d.png", where->second.c_str(), (unsigned long long)key, w, h);
    IWICImagingFactory *f = Wic(); if (!f) return art;
    IWICBitmapDecoder *d = nullptr; IWICBitmapFrameDecode *fr = nullptr;
    if (SUCCEEDED(f->CreateDecoderFromFilename(path, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &d)) && SUCCEEDED(d->GetFrame(0, &fr))) {
        UINT tw, th; WICPixelFormatGUID fmt; fr->GetSize(&tw, &th); fr->GetPixelFormat(&fmt);
        if (tw % w == 0 && th % h == 0 && tw / w == th / h) {
            art.rgba = fmt != GUID_WICPixelFormat8bppIndexed;
            UINT bpp = art.rgba ? 4 : 1;
            std::vector<u8> px((size_t)tw * th * bpp);
            if (art.rgba) {
                IWICFormatConverter *cv = nullptr;
                if (SUCCEEDED(f->CreateFormatConverter(&cv)) &&
                    SUCCEEDED(cv->Initialize(fr, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom)))
                    cv->CopyPixels(nullptr, tw * 4, (UINT)px.size(), px.data());
                if (cv) cv->Release();
            } else fr->CopyPixels(nullptr, tw, (UINT)px.size(), px.data());
            D3D11_TEXTURE2D_DESC td = {}; td.Width = tw; td.Height = th; td.MipLevels = 1; td.ArraySize = 1;
            td.Format = art.rgba ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8_UINT; td.SampleDesc.Count = 1;
            td.Usage = D3D11_USAGE_IMMUTABLE; td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            D3D11_SUBRESOURCE_DATA sd = { px.data(), tw * bpp, 0 }; ID3D11Texture2D *t;
            if (SUCCEEDED(dev->CreateTexture2D(&td, &sd, &t))) { dev->CreateShaderResourceView(t, nullptr, &art.srv); t->Release(); artBytes += (long long)tw * th * bpp; }
        }
    }
    if (fr) fr->Release();
    if (d) d->Release();
    return art;
}

bool KeyValid(const Spr &s, int stamp) {
    int bx = (s.tpage >> 1 & 7) * 256, by = ((s.tpage >> 4) & 1) * 256 + (s.alt ? 512 : 0);
    int x0 = (bx + s.u) >> 5, x1 = (bx + (std::min)(s.u + s.w - 1, 255)) >> 5;
    int y0 = (by + s.v) >> 5, y1 = (by + (std::min)(s.v + s.h - 1, 255)) >> 5;
    for (int ty = y0; ty <= y1; ty++)
        for (int tx = x0; tx <= x1; tx++)
            if (tileStamp[ty * 64 + tx] > stamp) return false;
    return true;
}

// Worker: loads replacement PNGs and writes dump PNGs off the render thread (D3D11 device creation
// calls are thread-safe). Until a load finishes the block is drawn with its original texels.
void ArtWorker() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    for (;;) {
        ArtJob j;
        { std::unique_lock<std::mutex> l(artMx); artCv.wait(l, [] { return !artJobs.empty(); }); j = std::move(artJobs.front()); artJobs.pop_front(); }
        if (!j.pal.empty()) {
            wchar_t path[MAX_PATH]; swprintf(path, MAX_PATH, L"%ls\\dump\\%016llx_%dx%d.png", artDir, (unsigned long long)j.key, j.w, j.h);
            SaveIndexedPng(path, j.blk.data(), j.w, j.h, j.pal.data());
            static FILE *idx;                                // dump/index.csv: where each key was first drawn
            if (!idx) { swprintf(path, MAX_PATH, L"%ls\\dump\\index.csv", artDir); idx = _wfopen(path, L"a");
                        if (idx) fputs("key,w,h,tpage,alt,u,v,screen_x,screen_y,frame,module\n", idx); }
            if (idx) { fputs(j.line, idx); fflush(idx); }
        } else {
            Art a = LoadArt(j.key, j.w, j.h);
            std::lock_guard<std::mutex> l(artMx); artDone.push_back({ j.key, a });
        }
    }
}

void QueueArt(ArtJob &&j) {
    { std::lock_guard<std::mutex> l(artMx); artJobs.push_back(std::move(j)); }
    artCv.notify_one();
}

// After Unswizzle: key each textured run, queue dumps and loads, and switch runs whose replacement
// art is loaded to it (vertex flag bit 31, uv rewritten to 0..1 across the block).
void ResolveArt(const u8 *bmi) {
    { std::lock_guard<std::mutex> l(artMx); for (auto &d : artDone) artCache[d.first] = d.second; artDone.clear(); }
    std::vector<u8> blk;
    stBlocks = stPoly = stFresh = stAvail = stArt = 0;
    int first = 0;
    for (auto &r : runs) {
        if (r.spr >= 0) {
            const Spr &s = sprs[r.spr];
            stBlocks++; stPoly += (verts[first].info >> 17) & 1;
            uint64_t rect = s.tpage | (uint64_t)s.alt << 5 | (uint64_t)s.u << 8 | (uint64_t)s.v << 16 | (uint64_t)s.w << 24 | (uint64_t)s.h << 34;
            auto kc = keyCache.find(rect);
            uint64_t key;
            bool fresh = kc == keyCache.end() || !KeyValid(s, kc->second.stamp);
            if (fresh) {
                stFresh++;
                BlockTexels(s, blk); key = BlockKey(s, blk); keyCache[rect] = { key, vramFrame };
                if (artDump && artDumped.insert(key).second) {
                    ArtJob j{ key, s.w, s.h, blk, std::vector<u8>(0x28 + 1024), {} };
                    sprintf(j.line, "%016llx,%d,%d,%u,%d,%d,%d,%d,%d,%d,%s\n", (unsigned long long)key, s.w, s.h, s.tpage, s.alt,
                            s.u, s.v, s.sx, s.sy, frames, GetModuleHandleA("eng1.dll") ? "ENG1" : GetModuleHandleA("eng3.dll") ? "ENG3" : "TITLE");
                    if (bmi) memcpy(j.pal.data() + 0x28, bmi + 0x28, 1024);
                    QueueArt(std::move(j));
                }
            } else key = kc->second.key;
            if (artShow && artAvail.count(key)) {
                stAvail++;
                auto it = artCache.find(key);
                if (it == artCache.end()) { artCache.emplace(key, Art{ nullptr, false }); QueueArt({ key, s.w, s.h, {}, {} }); }
                else if ((r.art = it->second).srv != nullptr && ++stArt)
                    for (int k = first; k < first + r.n; k++) {
                        Vtx &v = verts[k];
                        v.u = v.cu; v.v = v.cv; v.info |= 1u << 31 | (r.art.rgba ? 1u << 30 : 0);   // art coordinates set by SpriteAt / Poly
                    }
            }
        }
        first += r.n;
    }
    size_t o = 0;                                           // merge opaque runs that ended up without art
    for (size_t i = 0; i < runs.size(); i++) {
        Run &r = runs[i];
        if (o && r.mode == 4 && !r.art.srv && runs[o - 1].mode == 4 && !runs[o - 1].art.srv &&
              runs[o - 1].cy0 == r.cy0 && runs[o - 1].cy1 == r.cy1) runs[o - 1].n += r.n;
        else runs[o++] = r;
    }
    runs.resize(o);
}
}  // namespace

// dir: art folder; dump: write the catalog; families: comma list of subfolders to load
// (sprite, level, ui, screen; atlas.py packs into these). Files directly in dir are always loaded.
extern "C" void gpu_set_art(const char *dir, int dump, const char *families) {
    MultiByteToWideChar(CP_ACP, 0, dir, -1, artDir, MAX_PATH);
    artDump = dump != 0;
    wchar_t d[MAX_PATH];
    if (artDump) { CreateDirectoryW(artDir, nullptr); swprintf(d, MAX_PATH, L"%ls\\dump", artDir); CreateDirectoryW(d, nullptr); }
    std::vector<std::wstring> folders = { artDir };
    char fam[256]; strncpy(fam, families ? families : "", sizeof fam - 1); fam[sizeof fam - 1] = 0;
    for (char *t = strtok(fam, ", "); t; t = strtok(nullptr, ", ")) {
        wchar_t w[64]; MultiByteToWideChar(CP_ACP, 0, t, -1, w, 64);
        folders.push_back(std::wstring(artDir) + L"\\" + w);
    }
    for (auto &f : folders) {
        swprintf(d, MAX_PATH, L"%ls\\*.png", f.c_str());
        WIN32_FIND_DATAW fd; HANDLE h = FindFirstFileW(d, &fd);
        if (h == INVALID_HANDLE_VALUE) continue;
        do { unsigned long long k; if (swscanf(fd.cFileName, L"%16llx_", &k) == 1) artAvail.emplace(k, f); } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    artOn = artDump || !artAvail.empty();
    if (artOn) std::thread(ArtWorker).detach();
}

extern "C" void gpu_set_dev(int on) { devOverlay = on != 0; }
extern "C" void gpu_set_smooth(int on) { smoothOn = on != 0; }
extern "C" void gpu_last_file(const char *name, int index) {
    strncpy(lastFile, name ? name : "", sizeof lastFile - 1); lastIndex = index;
    if (name && (strstr(name, "MAP_PSX") || strstr(name, "map_psx"))) strncpy(lastMap, name, sizeof lastMap - 1);
}

// A frame can draw several OTs (GFX_DrawOTag per OT; ENG3 draws the scene and a HUD/overlay OT in some frames).
// The EXE draws each one over the same 8-bit frame without clearing, so the primitive lists accumulate until the
// next present; only the first OT after a present starts new lists. (Clearing per OT dropped the scene and left
// only the HUD on the clear colour: analysis/runtime/levels/eng3_gpu, GAUNTLET 4300 / CYCLOPS 3950.)
extern "C" void gpu_draw_ot(void *ot) {
    if (failed) return;
    if (otFresh) { verts.clear(); runs.clear(); sprs.clear(); otLog.clear(); otFresh = false; otCalls = 0; }
    otCalls++;
    clipY0 = *(const int *)0x00a909b4; clipY1 = *(const int *)0x0043a48c;   // clip state carries over between frames
    int *p = (int *)ot, *slot = p + 1;
    for (;;) {
        p = (int *)*p;
        while (p == slot) { p = (int *)*p; slot++; }
        if (!p) break;
        const u8 *b = (const u8 *)p;
        u8 code = b[7];
        bool semi = (code & 2) != 0;
        u8 c = code & 0xfd;
        if ((c & 0xe0) == 0x20 || c == 0x64 || c == 0x74 || c == 0x7c) {   // debug log: polygons and sprites
            static const int o3[3] = { 8, 16, 24 }, o4[4] = { 8, 16, 24, 32 }, og[4] = { 8, 20, 32, 44 }, f4[4] = { 8, 12, 16, 20 };
            const int *o = c == 0x24 ? o3 : c == 0x2c ? o4 : c == 0x3c ? og : c == 0x28 ? f4 : c == 0x20 ? f4 : c == 0x30 ? o4 : c == 0x38 ? o4 : o3;
            int n = (c == 0x24 || c == 0x20 || c == 0x30) ? 3 : c >= 0x64 ? 1 : 4;
            OtRec r{ code, b[4], b[5], b[6], 32767, 32767, -32768, -32768, (u16)curTpage, (u8)semi, (u8)clipY0, (u8)clipY1 };
            for (int i = 0; i < n; i++) {
                short x = *(const s16 *)(b + o[i]), y = *(const s16 *)(b + o[i] + 2);
                r.x0 = (std::min)(r.x0, x); r.y0 = (std::min)(r.y0, y); r.x1 = (std::max)(r.x1, x); r.y1 = (std::max)(r.y1, y);
            }
            if (c >= 0x64) { int w = c == 0x64 ? *(const s16 *)(b + 16) : c == 0x74 ? 8 : 16, h = c == 0x64 ? *(const s16 *)(b + 18) : w;
                             r.x1 = (short)(r.x0 + w); r.y1 = (short)(r.y0 + h); }
            if (otLog.size() < 20000) otLog.push_back(r);
        }
        if (code == 0xe1) {
            u32 w = *(const u32 *)(b + 4);
            curTpage = w & 0x1f; curSemi = (w >> 5) & 3; curAlt = (w & 0x180) == 0x180;
        } else if (code == 0xf3) {                           // clip rectangle (GFX_DrawOTag): only rows matter
            int y0 = b[6]; if (y0 > 0xef) y0 -= 0xf0;
            int y1 = *(const s16 *)(b + 10) + y0;
            clipY0 = (std::min)((std::max)(y0, 0), 240); clipY1 = (std::min)((std::max)(y1, 0), 240);
        } else if (c == 0x24) {
            static const int xy[3] = { 8, 16, 24 }, uv[3] = { 12, 20, 28 };
            Poly(b, 3, xy, uv, 22, 14, semi);
        } else if (c == 0x2c) {
            static const int xy[4] = { 8, 16, 24, 32 }, uv[4] = { 12, 20, 28, 36 };
            Poly(b, 4, xy, uv, 22, 14, semi);
        } else if (c == 0x3c) {
            static const int xy[4] = { 8, 20, 32, 44 }, uv[4] = { 12, 24, 36, 48 };
            Poly(b, 4, xy, uv, 26, 14, semi);
        } else if (c == 0x64) {
            Sprite(b, *(const s16 *)(b + 16), *(const s16 *)(b + 18), semi);
        } else if (c == 0x74) {
            Sprite(b, 8, 8, semi);
        } else if (c == 0x7c) {
            Sprite(b, 16, 16, semi);
        }
    }
}

// Called instead of the GDI present. Returns 0 if the GPU path is unavailable (GDI fallback).
// HERC_PERF=<file>: one CSV row per present (tools/perf/report.py): frame, game tick counter (game
// state +0x38), ms since the previous present, renderer CPU ms (until Present), Present ms (vsync
// wait), GPU ms (timestamp queries, read 3 frames later, -1 if not ready), vertices, runs, art MB.
struct Perf {
    FILE *f; LARGE_INTEGER freq, last; ID3D11Query *dis[4], *t0[4], *t1[4]; int slot;
    double row[4][12]; bool pending[4];
} perf;

void PerfInit() {
    char path[MAX_PATH];
    if (!GetEnvironmentVariableA("HERC_PERF", path, MAX_PATH) || !(perf.f = fopen(path, "w"))) return;
    fputs("frame,tick,interval_ms,cpu_ms,present_ms,gpu_ms,verts,runs,art_mb,cpu_upload_ms,cpu_art_ms,cpu_runs_ms,cpu_resolve_ms,fresh\n", perf.f);
    QueryPerformanceFrequency(&perf.freq);
    for (int i = 0; i < 4; i++) {
        D3D11_QUERY_DESC q = { D3D11_QUERY_TIMESTAMP_DISJOINT, 0 }; dev->CreateQuery(&q, &perf.dis[i]);
        q.Query = D3D11_QUERY_TIMESTAMP; dev->CreateQuery(&q, &perf.t0[i]); dev->CreateQuery(&q, &perf.t1[i]);
    }
}

void PerfFlush(int slot, bool wait) {
    if (!perf.pending[slot]) return;
    D3D11_QUERY_DATA_TIMESTAMP_DISJOINT dj; UINT64 a, b; double gpu = -1;
    UINT fl = wait ? 0 : D3D11_ASYNC_GETDATA_DONOTFLUSH;
    if (ctx->GetData(perf.dis[slot], &dj, sizeof dj, fl) == S_OK && ctx->GetData(perf.t0[slot], &a, sizeof a, fl) == S_OK &&
        ctx->GetData(perf.t1[slot], &b, sizeof b, fl) == S_OK && !dj.Disjoint)
        gpu = (double)(b - a) * 1000.0 / (double)dj.Frequency;
    double *r = perf.row[slot];
    fprintf(perf.f, "%.0f,%.0f,%.3f,%.3f,%.3f,%.3f,%.0f,%.0f,%.1f,%.3f,%.3f,%.3f,%.3f,%d\n", r[0], r[1], r[2], r[3], r[4], gpu,
            r[5], r[6], artBytes / 1048576.0, r[7], r[8], r[9], r[10], (int)r[11]);
    perf.pending[slot] = false;
}

extern "C" int gpu_present(HWND h) {
    if (failed) return 0;
    if (!ready) { ready = Init(h); if (!ready) { failed = true; return 0; } PerfInit(); }
    otFresh = true; otCallsLast = otCalls;                 // this present consumes the lists of every OT since the last one
    LARGE_INTEGER pt0; QueryPerformanceCounter(&pt0);
    int ps = perf.slot;
    if (perf.f) { PerfFlush(ps, false); ctx->Begin(perf.dis[ps]); ctx->End(perf.t0[ps]); }
    Unswizzle();
    UploadVram();
    const u8 *bmi = *(const u8 **)0x0096e3f4;
    if (bmi) {
        u8 pal[1024];
        for (int i = 0; i < 256; i++) { pal[i*4] = bmi[0x28 + i*4 + 2]; pal[i*4+1] = bmi[0x28 + i*4 + 1]; pal[i*4+2] = bmi[0x28 + i*4]; pal[i*4+3] = 255; }
        ctx->UpdateSubresource(palTex, 0, nullptr, pal, 1024, 0);
    }
    static u8 fog[256 * 32];
    memcpy(fog + 256, (const void *)0x0092c490, 31 * 256);  // shader row = level (EXE reads 0x92c390 + level*256)
    ctx->UpdateSubresource(fogTex, 0, nullptr, fog, 256, 0);
    ctx->UpdateSubresource(blendTex, 0, nullptr, (const void *)0x0092e3d0, 256, 0);  // [mode*256 + src][dst]
    ctx->UpdateSubresource(shadeTex, 0, nullptr, (const void *)0x0091c488, 256, 0);  // [brightness][index]
    {   // F11 toggles replacement art (original vs remastered), only while the game window has focus
        static bool was; bool now = (GetAsyncKeyState(VK_F11) & 0x8000) != 0;
        if (now && !was && GetForegroundWindow() == h) artShow = !artShow;
        was = now;
        static bool wasS; bool nowS = (GetAsyncKeyState(VK_F10) & 0x8000) != 0;   // F10: smoothing on/off
        if (nowS && !wasS && GetForegroundWindow() == h) smoothOn = !smoothOn;
        wasS = nowS;
    }
    LARGE_INTEGER pa; QueryPerformanceCounter(&pa);
    if (artOn) ResolveArt(bmi);
    LARGE_INTEGER pb; QueryPerformanceCounter(&pb);
    if (verts.size() > vbCap) {
        if (vb) vb->Release();
        vbCap = (UINT)verts.size() * 2;
        D3D11_BUFFER_DESC bd = {}; bd.ByteWidth = vbCap * sizeof(Vtx); bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.Usage = D3D11_USAGE_DYNAMIC; bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; dev->CreateBuffer(&bd, nullptr, &vb);
    }
    // 1) indices at iw x ih
    if (*(const u8 *)0x0096e698) {                         // the EXE clears only when this is set
        u8 ci = *(const u8 *)0x0096e424;
        float clr[4] = { (float)ci, 0, 0, 0 };                // integer formats take the value as float
        ctx->ClearRenderTargetView(idxRtv, clr);
        float cc[4] = { 0, 0, 0, 1 };
        if (bmi) { cc[0] = bmi[0x28 + ci * 4 + 2] / 255.f; cc[1] = bmi[0x28 + ci * 4 + 1] / 255.f; cc[2] = bmi[0x28 + ci * 4] / 255.f; }
        ctx->ClearRenderTargetView(colRtv, cc);
    }
    D3D11_VIEWPORT ivp = { 0, 0, (float)iw, (float)ih, 0, 1 };
    D3D11_RECT isc = { 0, 0, iw, ih };
    ctx->RSSetViewports(1, &ivp); ctx->RSSetScissorRects(1, &isc); ctx->RSSetState(raster);
    ctx->OMSetBlendState(noBlend, nullptr, 0xffffffff);
    if (!verts.empty()) {
        D3D11_MAPPED_SUBRESOURCE m;
        ctx->Map(vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &m); memcpy(m.pData, verts.data(), verts.size() * sizeof(Vtx)); ctx->Unmap(vb, 0);
        UINT stride = sizeof(Vtx), off = 0;
        ctx->IASetVertexBuffers(0, 1, &vb, &stride, &off); ctx->IASetInputLayout(layout);
        ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ctx->VSSetShader(vs, nullptr, 0);
        int first = 0;
        for (auto &r : runs) {
            struct { float sx, sy, ox, oy; int fog, mode, p0, p1; } c = { 2.0f / 320, -2.0f / 240, -1, 1,
                *(const u8 *)0x0096e3e0, r.mode & 3, smoothOn ? 1 : 0, 0 };
            ctx->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &m); memcpy(m.pData, &c, sizeof c); ctx->Unmap(cb, 0);
            ctx->VSSetConstantBuffers(0, 1, &cb); ctx->PSSetConstantBuffers(0, 1, &cb);
            ID3D11ShaderResourceView *none[10] = {};
            ctx->PSSetShaderResources(0, 10, none);
            ID3D11RenderTargetView *mrt[2] = { idxRtv, colRtv };
            ID3D11ShaderResourceView *artSrv[2] = { r.art.rgba ? nullptr : r.art.srv, r.art.rgba ? r.art.srv : nullptr };
            if (r.mode == 4) {
                ctx->OMSetRenderTargets(2, mrt, nullptr); ctx->OMSetBlendState(opaqueBlend, nullptr, 0xffffffff);
                ID3D11ShaderResourceView *srv[6] = { vramSrv, palSrv, fogSrv, nullptr, nullptr, shadeSrv }; ctx->PSSetShaderResources(0, 6, srv);
                ctx->PSSetShader(psOpaque, nullptr, 0);
            } else {
                ctx->OMSetRenderTargets(0, nullptr, nullptr);
                // destination for the blend: only the run's screen box (+1 texel, clipped to its clip rows) is read
                // by psSemi, so copy that region instead of both full targets (Q-05: ENG3 frames have up to
                // hundreds of semi runs; full copies made the 9x renderer GPU-bound)
                float bx0 = 1e9f, by0 = 1e9f, bx1 = -1e9f, by1 = -1e9f;
                for (int k = first; k < first + r.n; k++) {
                    bx0 = (std::min)(bx0, verts[k].x); bx1 = (std::max)(bx1, verts[k].x);
                    by0 = (std::min)(by0, verts[k].y); by1 = (std::max)(by1, verts[k].y);
                }
                D3D11_BOX box;
                box.left = (UINT)(std::max)(0, (int)floorf(bx0 * scale) - 1); box.right = (UINT)(std::min)(iw, (int)ceilf(bx1 * scale) + 1);
                box.top = (UINT)(std::max)((int)r.cy0 * scale, (int)floorf(by0 * scale) - 1);
                box.bottom = (UINT)(std::min)((std::min)(ih, (int)r.cy1 * scale), (int)ceilf(by1 * scale) + 1);
                box.front = 0; box.back = 1;
                if (box.right > box.left && box.bottom > box.top) {
                    ctx->CopySubresourceRegion(idxCopy, 0, box.left, box.top, 0, idxTex, 0, &box);
                    ctx->CopySubresourceRegion(colCopy, 0, box.left, box.top, 0, colTex, 0, &box);
                }
                ctx->OMSetRenderTargets(2, mrt, nullptr); ctx->OMSetBlendState(noBlend, nullptr, 0xffffffff);
                ID3D11ShaderResourceView *srv[6] = { vramSrv, palSrv, fogSrv, blendSrv, copySrv, shadeSrv }; ctx->PSSetShaderResources(0, 6, srv);
                ctx->PSSetShaderResources(9, 1, &colCopySrv);
                ctx->PSSetShader(psSemi, nullptr, 0);
            }
            ctx->PSSetShaderResources(7, 2, artSrv);
            D3D11_RECT rs = { 0, (LONG)(r.cy0 * scale), iw, (LONG)(r.cy1 * scale) };
            ctx->RSSetScissorRects(1, &rs);
            ctx->Draw(r.n, first); first += r.n;
        }
    }
    LARGE_INTEGER pc; QueryPerformanceCounter(&pc);
    // frame brightness (fades): EXE 0x4070c0 after the list, before present
    u8 fb = *(const u8 *)0x00436a0c;
    if (fb != 0x80) {
        ID3D11ShaderResourceView *n10[10] = {};
        ctx->PSSetShaderResources(0, 10, n10);
        ctx->OMSetRenderTargets(0, nullptr, nullptr);
        ctx->CopyResource(idxCopy, idxTex); ctx->CopyResource(colCopy, colTex);
        ID3D11RenderTargetView *mrt[2] = { idxRtv, colRtv };
        ctx->OMSetRenderTargets(2, mrt, nullptr); ctx->OMSetBlendState(noBlend, nullptr, 0xffffffff);
        D3D11_RECT all = { 0, 0, iw, ih }; ctx->RSSetViewports(1, &ivp); ctx->RSSetScissorRects(1, &all);
        struct { float sx, sy, ox, oy; int fog, mode, p0, p1; } c = { 0, 0, 0, 0, 0, fb, 0, 0 };
        D3D11_MAPPED_SUBRESOURCE m; ctx->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &m); memcpy(m.pData, &c, sizeof c); ctx->Unmap(cb, 0);
        ctx->PSSetConstantBuffers(0, 1, &cb);
        ID3D11ShaderResourceView *srv[10] = { nullptr, palSrv, nullptr, nullptr, copySrv, shadeSrv, nullptr, nullptr, nullptr, colCopySrv };
        ctx->PSSetShaderResources(0, 10, srv);
        ctx->IASetInputLayout(nullptr); ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ctx->VSSetShader(vsFull, nullptr, 0); ctx->PSSetShader(psBright, nullptr, 0);
        ctx->Draw(3, 0);
    }
    // 2) resolve indices -> palette colours into the centred, integer-scaled viewport
    ID3D11ShaderResourceView *none[10] = {};
    ctx->PSSetShaderResources(0, 10, none);
    ctx->OMSetBlendState(noBlend, nullptr, 0xffffffff);
    float black[4] = { 0, 0, 0, 1 };
    ctx->OMSetRenderTargets(1, &rtv, nullptr); ctx->ClearRenderTargetView(rtv, black);
    D3D11_VIEWPORT vp = { (float)((bw - iw) / 2), (float)((bh - ih) / 2), (float)iw, (float)ih, 0, 1 };
    if (iw > bw || ih > bh) vp = { 0, 0, (float)bw, (float)bh, 0, 1 };   // HERC_GPU_SCALE larger than the window: shrink
    D3D11_RECT sc = { (LONG)vp.TopLeftX, (LONG)vp.TopLeftY, (LONG)(vp.TopLeftX + iw), (LONG)(vp.TopLeftY + ih) };
    ctx->RSSetViewports(1, &vp); ctx->RSSetScissorRects(1, &sc);
    ctx->IASetInputLayout(nullptr); ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->VSSetShader(vsFull, nullptr, 0); ctx->PSSetShader(psResolve, nullptr, 0);
    ID3D11ShaderResourceView *rs[10] = { nullptr, palSrv, nullptr, nullptr, idxSrv, nullptr, nullptr, nullptr, nullptr, colSrv };
    ctx->PSSetShaderResources(0, 10, rs);
    ctx->Draw(3, 0);
    ctx->PSSetShaderResources(0, 10, none);
    if (devOverlay) {
        int hx, hy;
        DrawOverlayText(&hx, &hy);
        ctx->OMSetBlendState(alphaBlend, nullptr, 0xffffffff);
        D3D11_VIEWPORT ov = { vp.TopLeftX + 8, vp.TopLeftY + 8, (float)OVW, (float)OVH, 0, 1 };
        D3D11_RECT ovs = { (LONG)ov.TopLeftX, (LONG)ov.TopLeftY, (LONG)(ov.TopLeftX + OVW), (LONG)(ov.TopLeftY + OVH) };
        ctx->RSSetViewports(1, &ov); ctx->RSSetScissorRects(1, &ovs); ctx->PSSetShader(psOverlay, nullptr, 0);
        ctx->PSSetShaderResources(6, 1, &ovSrv); ctx->Draw(3, 0);
        ID3D11ShaderResourceView *n1 = nullptr; ctx->PSSetShaderResources(6, 1, &n1);
        ctx->OMSetBlendState(noBlend, nullptr, 0xffffffff);
        if (hx > -9000) {
            float px = vp.TopLeftX + hx * scale, py = vp.TopLeftY + hy * scale, a = 4.0f * scale;
            D3D11_VIEWPORT bars[2] = { { px - a, py - 1, 2 * a, 3, 0, 1 }, { px - 1, py - a, 3, 2 * a, 0, 1 } };
            ctx->PSSetShader(psSolid, nullptr, 0);
            for (auto &bv : bars) {
                D3D11_RECT r = { (LONG)bv.TopLeftX, (LONG)bv.TopLeftY, (LONG)(bv.TopLeftX + bv.Width), (LONG)(bv.TopLeftY + bv.Height) };
                ctx->RSSetViewports(1, &bv); ctx->RSSetScissorRects(1, &r); ctx->Draw(3, 0);
            }
            if (heroBoxOn) {
                float x0 = vp.TopLeftX + heroBox[0] * scale, y0 = vp.TopLeftY + heroBox[1] * scale;
                float x1 = vp.TopLeftX + heroBox[2] * scale, y1 = vp.TopLeftY + heroBox[3] * scale;
                D3D11_VIEWPORT edges[4] = { { x0, y0, x1 - x0, 2, 0, 1 }, { x0, y1 - 2, x1 - x0, 2, 0, 1 },
                                            { x0, y0, 2, y1 - y0, 0, 1 }, { x1 - 2, y0, 2, y1 - y0, 0, 1 } };
                for (auto &e : edges) {
                    D3D11_RECT r = { (LONG)e.TopLeftX, (LONG)e.TopLeftY, (LONG)(e.TopLeftX + e.Width), (LONG)(e.TopLeftY + e.Height) };
                    ctx->RSSetViewports(1, &e); ctx->RSSetScissorRects(1, &r); ctx->Draw(3, 0);
                }
            }
        }
    }
    MaybeShot();
    LARGE_INTEGER pt1; QueryPerformanceCounter(&pt1);
    if (perf.f) { ctx->End(perf.t1[ps]); ctx->End(perf.dis[ps]); }
    swap->Present(1, 0);
    if (perf.f) {
        LARGE_INTEGER pt2; QueryPerformanceCounter(&pt2);
        double ms = 1000.0 / (double)perf.freq.QuadPart;
        double *r = perf.row[ps];
        r[0] = frames; r[1] = *(const int *)(0x00615c98 + 0x38);
        r[2] = perf.last.QuadPart ? (pt0.QuadPart - perf.last.QuadPart) * ms : 0;
        r[7] = (pa.QuadPart - pt0.QuadPart) * ms; r[8] = (pb.QuadPart - pa.QuadPart) * ms;
        r[11] = stFresh;
        r[9] = (pc.QuadPart - pb.QuadPart) * ms; r[10] = (pt1.QuadPart - pc.QuadPart) * ms;
        r[3] = (pt1.QuadPart - pt0.QuadPart) * ms; r[4] = (pt2.QuadPart - pt1.QuadPart) * ms; r[5] = (double)verts.size(); r[6] = (double)runs.size();
        perf.pending[ps] = true; perf.last = pt0; perf.slot = (ps + 1) & 3;
        if ((frames & 63) == 0) fflush(perf.f);
    }
    frames++;
    return 1;
}

extern "C" int gpu_frames(void) { return frames; }
