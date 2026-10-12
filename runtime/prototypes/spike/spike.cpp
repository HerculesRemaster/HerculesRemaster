// P6 renderer spike: draws the decoded TRAINING level (all layers, unrolled path) and Hercules'
// run cycle at the monitor resolution with a fixed 33 ms simulation step (the game's RT_Tick rate),
// rendered every vsync with interpolation. Win32 + D3D11 + WIC only (Windows components).
// Data: runtime/prototypes/spike/data (tools/runtime/export_spike.py).
//   spike.exe [--seconds N] [--windowed]   (N > 0: run N seconds, write spike.log, exit)
// View: the original 320x240 world-unit view, integer-scaled (9x on 2160p) and pillarboxed.
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wincodec.h>
#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "user32.lib")

struct Vtx { float x, y, u, v; };
struct HeroFrame { int x, y, w, h, ox, oy, ticks; };

static ID3D11Device *dev; static ID3D11DeviceContext *ctx; static IDXGISwapChain *swap;
static ID3D11RenderTargetView *rtv; static HWND hwnd; static bool running = true;
static std::wstring dataDir;

// Save the current back buffer as PNG (WIC) - used for the automated check, no desktop capture.
static void SaveBackBuffer(const std::wstring &path) {
    ID3D11Texture2D *bb; swap->GetBuffer(0, IID_PPV_ARGS(&bb));
    D3D11_TEXTURE2D_DESC td; bb->GetDesc(&td);
    td.Usage = D3D11_USAGE_STAGING; td.BindFlags = 0; td.CPUAccessFlags = D3D11_CPU_ACCESS_READ; td.MiscFlags = 0;
    ID3D11Texture2D *st; dev->CreateTexture2D(&td, nullptr, &st); ctx->CopyResource(st, bb);
    D3D11_MAPPED_SUBRESOURCE m; ctx->Map(st, 0, D3D11_MAP_READ, 0, &m);
    IWICImagingFactory *f; CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&f));
    IWICStream *ws; f->CreateStream(&ws); ws->InitializeFromFilename(path.c_str(), GENERIC_WRITE);
    IWICBitmapEncoder *enc; f->CreateEncoder(GUID_ContainerFormatPng, nullptr, &enc); enc->Initialize(ws, WICBitmapEncoderNoCache);
    IWICBitmapFrameEncode *fr; enc->CreateNewFrame(&fr, nullptr); fr->Initialize(nullptr); fr->SetSize(td.Width, td.Height);
    std::vector<BYTE> px((size_t)m.RowPitch * td.Height);
    memcpy(px.data(), m.pData, px.size());
    for (size_t i = 0; i + 3 < px.size(); i += 4) std::swap(px[i], px[i + 2]);   // RGBA -> BGRA for the PNG encoder
    WICPixelFormatGUID fmt = GUID_WICPixelFormat32bppBGRA; fr->SetPixelFormat(&fmt);
    fr->WritePixels(td.Height, m.RowPitch, (UINT)px.size(), px.data());
    fr->Commit(); enc->Commit(); ctx->Unmap(st, 0);
    fr->Release(); enc->Release(); ws->Release(); f->Release(); st->Release(); bb->Release();
}

static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_DESTROY || (m == WM_KEYDOWN && w == VK_ESCAPE)) { running = false; PostQuitMessage(0); return 0; }
    return DefWindowProcW(h, m, w, l);
}

static ID3D11ShaderResourceView *LoadPng(const std::wstring &path, int *ow, int *oh) {
    IWICImagingFactory *f = nullptr; IWICBitmapDecoder *d = nullptr; IWICBitmapFrameDecode *fr = nullptr;
    IWICFormatConverter *cv = nullptr;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&f));
    if (!f || FAILED(f->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &d))) return nullptr;
    d->GetFrame(0, &fr); f->CreateFormatConverter(&cv);
    cv->Initialize(fr, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom);
    UINT w, h; cv->GetSize(&w, &h);
    std::vector<uint8_t> px((size_t)w * h * 4);
    cv->CopyPixels(nullptr, w * 4, (UINT)px.size(), px.data());
    D3D11_TEXTURE2D_DESC td = {}; td.Width = w; td.Height = h; td.MipLevels = 1; td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM; td.SampleDesc.Count = 1; td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    D3D11_SUBRESOURCE_DATA sd = { px.data(), w * 4, 0 };
    ID3D11Texture2D *tex; dev->CreateTexture2D(&td, &sd, &tex);
    ID3D11ShaderResourceView *srv; dev->CreateShaderResourceView(tex, nullptr, &srv); tex->Release();
    cv->Release(); fr->Release(); d->Release(); f->Release();
    *ow = (int)w; *oh = (int)h;
    return srv;
}

static const char *kShader = R"(
cbuffer Cam : register(b0) { float2 camPos; float2 scale; float2 offset; float2 pad; };
Texture2D tex : register(t0); SamplerState smp : register(s0);
struct VSIn { float2 pos : POSITION; float2 uv : TEXCOORD; };
struct PSIn { float4 pos : SV_POSITION; float2 uv : TEXCOORD; };
PSIn VS(VSIn i) { PSIn o; float2 p = (i.pos - camPos) * scale + offset; o.pos = float4(p, 0, 1); o.uv = i.uv; return o; }
float4 PS(PSIn i) : SV_TARGET { float4 c = tex.Sample(smp, i.uv); if (c.a < 0.5) discard; return c; }
)";

int WINAPI wWinMain(HINSTANCE inst, HINSTANCE, PWSTR cmd, int) {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    int seconds = 0; bool windowed = false;
    { int argc; LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
      for (int i = 1; i < argc; i++) {
          if (!wcscmp(argv[i], L"--seconds") && i + 1 < argc) seconds = _wtoi(argv[++i]);
          if (!wcscmp(argv[i], L"--windowed")) windowed = true;
      } }
    wchar_t exe[MAX_PATH]; GetModuleFileNameW(nullptr, exe, MAX_PATH);
    dataDir = exe; dataDir = dataDir.substr(0, dataDir.find_last_of(L'\\')); dataDir += L"\\..\\data\\";

    int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);
    if (windowed) { sw = 1280; sh = 960; }
    WNDCLASSW wc = {}; wc.lpfnWndProc = WndProc; wc.hInstance = inst; wc.lpszClassName = L"HercSpike";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW); RegisterClassW(&wc);
    hwnd = CreateWindowExW(0, wc.lpszClassName, L"Hercules spike", windowed ? WS_OVERLAPPEDWINDOW | WS_VISIBLE : WS_POPUP | WS_VISIBLE,
                           0, 0, sw, sh, nullptr, nullptr, inst, nullptr);
    if (!windowed) ShowCursor(FALSE);

    DXGI_SWAP_CHAIN_DESC sd = {}; sd.BufferCount = 2; sd.BufferDesc.Width = sw; sd.BufferDesc.Height = sh;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd; sd.SampleDesc.Count = 1; sd.Windowed = TRUE; sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
                                             &sd, &swap, &dev, nullptr, &ctx))) return 1;
    ID3D11Texture2D *bb; swap->GetBuffer(0, IID_PPV_ARGS(&bb)); dev->CreateRenderTargetView(bb, nullptr, &rtv); bb->Release();

    // shaders
    ID3DBlob *vsb, *psb, *err;
    D3DCompile(kShader, strlen(kShader), nullptr, nullptr, nullptr, "VS", "vs_4_0", 0, 0, &vsb, &err);
    D3DCompile(kShader, strlen(kShader), nullptr, nullptr, nullptr, "PS", "ps_4_0", 0, 0, &psb, &err);
    ID3D11VertexShader *vs; ID3D11PixelShader *ps;
    dev->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, &vs);
    dev->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, &ps);
    D3D11_INPUT_ELEMENT_DESC il[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 } };
    ID3D11InputLayout *layout; dev->CreateInputLayout(il, 2, vsb->GetBufferPointer(), vsb->GetBufferSize(), &layout);

    // data
    int aw, ah, hw, hh;
    ID3D11ShaderResourceView *atlas = LoadPng(dataDir + L"atlas.png", &aw, &ah);
    ID3D11ShaderResourceView *heroTex = LoadPng(dataDir + L"hero.png", &hw, &hh);
    if (!atlas || !heroTex) { MessageBoxW(hwnd, L"run tools/runtime/export_spike.py first", L"spike", MB_OK); return 1; }
    std::vector<Vtx> level;
    { std::ifstream f(dataDir + L"quads.bin", std::ios::binary); uint32_t n; f.read((char *)&n, 4);
      for (uint32_t q = 0; q < n; q++) {
          float p[8], uv[8], depth; f.read((char *)p, 32); f.read((char *)uv, 32); f.read((char *)&depth, 4);
          int idx[6] = { 0, 1, 2, 1, 3, 2 };   // PSX quad v0 v1 v2 v3 = TL TR BL BR
          for (int k : idx) level.push_back({ p[k * 2], p[k * 2 + 1], uv[k * 2], uv[k * 2 + 1] });
      } }
    std::vector<HeroFrame> frames;
    { std::ifstream f(dataDir + L"hero.txt"); HeroFrame fr;
      while (f >> fr.x >> fr.y >> fr.w >> fr.h >> fr.ox >> fr.oy >> fr.ticks) frames.push_back(fr); }
    int cols, rows, layers, nq, runSpeedFx, tickMs, col0;
    { std::ifstream f(dataDir + L"meta.txt"); f >> cols >> rows >> layers >> nq >> runSpeedFx >> tickMs >> col0; }
    const double runSpeed = runSpeedFx / 65536.0;

    D3D11_BUFFER_DESC bd = {}; bd.ByteWidth = (UINT)(level.size() * sizeof(Vtx)); bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vd = { level.data(), 0, 0 };
    ID3D11Buffer *vb; dev->CreateBuffer(&bd, &vd, &vb);
    D3D11_BUFFER_DESC hbd = {}; hbd.ByteWidth = 6 * sizeof(Vtx); hbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    hbd.Usage = D3D11_USAGE_DYNAMIC; hbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ID3D11Buffer *hvb; dev->CreateBuffer(&hbd, nullptr, &hvb);
    D3D11_BUFFER_DESC cbd = {}; cbd.ByteWidth = 32; cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC; cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ID3D11Buffer *cb; dev->CreateBuffer(&cbd, nullptr, &cb);
    D3D11_SAMPLER_DESC smp = {}; smp.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    smp.AddressU = smp.AddressV = smp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    ID3D11SamplerState *sampler; dev->CreateSamplerState(&smp, &sampler);
    D3D11_RASTERIZER_DESC rd = {}; rd.FillMode = D3D11_FILL_SOLID; rd.CullMode = D3D11_CULL_NONE;  // mirrored cells flip winding
    ID3D11RasterizerState *raster; dev->CreateRasterizerState(&rd, &raster);

    // viewport: 320x240 world units, integer scale, centred (pillarbox)
    int scale = (std::min)(sw / 320, sh / 240); if (scale < 1) scale = 1;
    D3D11_VIEWPORT vp = { (float)((sw - 320 * scale) / 2), (float)((sh - 240 * scale) / 2), 320.0f * scale, 240.0f * scale, 0, 1 };

    // simulation state (fixed 33 ms ticks like EXE RT_Tick)
    double heroX = 200, prevX = 200; const double heroY = 800; int frameIdx = 0, frameLeft = frames.empty() ? 1 : frames[0].ticks;
    double maxX = col0 * 64.0 + cols * 64.0 - 200;
    LARGE_INTEGER freq, t0, last; QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&t0); last = t0;
    double acc = 0; long long presents = 0, ticks = 0; double worstMs = 0;
    std::vector<double> frameMs;
    MSG msg;
    while (running) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        LARGE_INTEGER now; QueryPerformanceCounter(&now);
        double dt = (double)(now.QuadPart - last.QuadPart) * 1000.0 / freq.QuadPart; last = now;
        if (presents > 2) { frameMs.push_back(dt); worstMs = (std::max)(worstMs, dt); }
        acc += dt; if (acc > 2 * tickMs) acc = 2 * tickMs;
        while (acc >= tickMs) {          // one game tick
            acc -= tickMs; ticks++;
            prevX = heroX; heroX += runSpeed; if (heroX > maxX) { heroX = 200; prevX = 200; }
            if (--frameLeft <= 0 && !frames.empty()) { frameIdx = (frameIdx + 1) % frames.size(); frameLeft = frames[frameIdx].ticks; }
        }
        double alpha = acc / tickMs, x = prevX + (heroX - prevX) * alpha;
        float camX = (float)(x - 160), camY = (float)(heroY - 170);
        struct { float cx, cy, sx, sy, ox, oy, p0, p1; } cam = { camX, camY, 2.0f / 320, -2.0f / 240, -1, 1, 0, 0 };
        D3D11_MAPPED_SUBRESOURCE ms; ctx->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms); memcpy(ms.pData, &cam, sizeof cam); ctx->Unmap(cb, 0);

        float black[4] = { 0, 0, 0, 1 };
        ctx->OMSetRenderTargets(1, &rtv, nullptr); ctx->ClearRenderTargetView(rtv, black); ctx->RSSetViewports(1, &vp); ctx->RSSetState(raster);
        ctx->IASetInputLayout(layout); ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ctx->VSSetShader(vs, nullptr, 0); ctx->PSSetShader(ps, nullptr, 0); ctx->VSSetConstantBuffers(0, 1, &cb);
        ctx->PSSetSamplers(0, 1, &sampler);
        UINT stride = sizeof(Vtx), off = 0;
        ctx->IASetVertexBuffers(0, 1, &vb, &stride, &off); ctx->PSSetShaderResources(0, 1, &atlas);
        ctx->Draw((UINT)level.size(), 0);
        if (!frames.empty()) {
            const HeroFrame &fr = frames[frameIdx];
            float x0 = (float)(std::floor(x) - fr.ox), y0 = (float)(heroY - fr.oy), x1 = x0 + fr.w, y1 = y0 + fr.h;
            float u0 = (float)fr.x / hw, u1 = (float)(fr.x + fr.w) / hw, v0 = 0, v1 = (float)fr.h / hh;
            Vtx q[6] = { {x0,y0,u0,v0},{x1,y0,u1,v0},{x0,y1,u0,v1},{x1,y0,u1,v0},{x1,y1,u1,v1},{x0,y1,u0,v1} };
            ctx->Map(hvb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms); memcpy(ms.pData, q, sizeof q); ctx->Unmap(hvb, 0);
            ctx->IASetVertexBuffers(0, 1, &hvb, &stride, &off); ctx->PSSetShaderResources(0, 1, &heroTex); ctx->Draw(6, 0);
        }
        if (seconds > 0 && (now.QuadPart - t0.QuadPart) / freq.QuadPart >= seconds) {
            SaveBackBuffer(dataDir + L"..\\spike.png");
            running = false;
        }
        swap->Present(1, 0); presents++;
    }
    if (seconds > 0) {
        std::sort(frameMs.begin(), frameMs.end());
        double p95 = frameMs.empty() ? 0 : frameMs[(size_t)(frameMs.size() * 0.95)];
        double total = (double)(last.QuadPart - t0.QuadPart) / freq.QuadPart;
        FILE *f = _wfopen((dataDir + L"..\\spike.log").c_str(), L"w");
        if (f) {
            fprintf(f, "resolution %dx%d scale %d viewport %.0fx%.0f at %.0f,%.0f\nquads %d vertices %zu\n"
                       "seconds %.2f presents %lld (%.2f fps) ticks %lld (%.2f Hz)\nframe ms p95 %.2f worst %.2f\n",
                    sw, sh, scale, vp.Width, vp.Height, vp.TopLeftX, vp.TopLeftY, nq, level.size(),
                    total, presents, presents / total, ticks, ticks / total, p95, worstMs);
            fclose(f);
        }
    }
    return 0;
}
