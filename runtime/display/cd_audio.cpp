// CD audio emulation. HERCULES.EXE plays all of its music as Red Book track 2 through MCI
// (docs/music-transport.md): AUD_CdReady 0x9c30 ("open cdaudio shareable wait", "status cdaudio media
// present" / "status cdaudio ready" compared with "true"), AUD_CdPlaySegment 0x9e60 and AUD_CdResume
// 0x9f60 ("set cdaudio time format tmsf", "play cdaudio from %d to %d" with MCI TMSF integers),
// AUD_CdStop 0x9e10 ("stop cdaudio wait"), 0xa120 ("close cdaudio wait"). No other cdaudio string is
// referenced. When <game>\music\TrackNN.<ext> (or <game>\TrackNN.<ext>) exists, ddraw_proxy.c sends the
// cdaudio commands here instead of to MCI: status queries answer "true", play decodes [from, to) of the
// track file (Media Foundation: flac, wav, mp3, m4a, wma; ogg if a decoder is installed) and streams it
// to waveOut, stopping at `to` like the disc did. Without a track file the real MCI is used unchanged.
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mmreg.h>
#include <stdio.h>
#include <string.h>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

extern "C" void cd_log(const char *msg);                  // ddraw_proxy.c (hercules_display.log)

namespace {
char gameDir[MAX_PATH];
int volume = 100;
std::mutex mx;
std::condition_variable cv;
struct Req { int gen = 0; bool play = false; int track = 0; double from = 0, to = 0; } req;
bool started;

void logf(const char *fmt, ...) {
    char b[512]; va_list ap; va_start(ap, fmt); _vsnprintf(b, sizeof b - 1, fmt, ap); va_end(ap); b[sizeof b - 1] = 0;
    cd_log(b);
}

bool TrackFile(int track, wchar_t *out) {
    static const char *ext[] = { "flac", "wav", "mp3", "m4a", "wma", "ogg" };
    static const char *sub[] = { "\\music\\", "\\" };
    for (auto s : sub)
        for (auto e : ext) {
            char p[MAX_PATH]; _snprintf(p, MAX_PATH, "%s%sTrack%02d.%s", gameDir, s, track, e);
            if (GetFileAttributesA(p) != INVALID_FILE_ATTRIBUTES) { MultiByteToWideChar(CP_ACP, 0, p, -1, out, MAX_PATH); return true; }
        }
    return false;
}

// MCI TMSF integer: track | minutes << 8 | seconds << 16 | frames << 24 (75 frames per second)
void Tmsf(unsigned v, int *track, double *sec) {
    *track = v & 0xff;
    *sec = ((v >> 8) & 0xff) * 60.0 + ((v >> 16) & 0xff) + ((v >> 24) & 0xff) / 75.0;
}

// Plays one request until it ends or a newer request arrives (req.gen changes).
void PlaySegment(const Req r) {
    wchar_t path[MAX_PATH];
    if (!TrackFile(r.track, path)) { logf("cd: no file for track %d", r.track); return; }
    IMFSourceReader *rd = nullptr;
    if (FAILED(MFCreateSourceReaderFromURL(path, nullptr, &rd))) { logf("cd: cannot decode track %d (format not supported)", r.track); return; }
    IMFMediaType *mt = nullptr; MFCreateMediaType(&mt);
    mt->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio); mt->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    mt->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
    rd->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, FALSE);
    rd->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
    HRESULT hr = rd->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, mt);
    mt->Release();
    IMFMediaType *cur = nullptr; WAVEFORMATEX *wf = nullptr; UINT32 wfn = 0;
    if (FAILED(hr) || FAILED(rd->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &cur)) ||
        FAILED(MFCreateWaveFormatExFromMFMediaType(cur, &wf, &wfn))) {
        logf("cd: no PCM output for track %d", r.track); if (cur) cur->Release(); rd->Release(); return;
    }
    cur->Release();
    PROPVARIANT pos; PropVariantInit(&pos); pos.vt = VT_I8; pos.hVal.QuadPart = (LONGLONG)(r.from * 1e7);
    rd->SetCurrentPosition(GUID_NULL, pos);
    HANDLE ev = CreateEventA(nullptr, FALSE, FALSE, nullptr);
    HWAVEOUT wo = nullptr;
    if (waveOutOpen(&wo, WAVE_MAPPER, wf, (DWORD_PTR)ev, 0, CALLBACK_EVENT) != MMSYSERR_NOERROR) {
        logf("cd: waveOutOpen failed"); CoTaskMemFree(wf); rd->Release(); CloseHandle(ev); return;
    }
    DWORD v = (DWORD)(0xffff * volume / 100); waveOutSetVolume(wo, v | v << 16);
    const int NB = 4; const size_t cap = wf->nAvgBytesPerSec / 5;   // 4 x 200 ms queued
    WAVEHDR hdr[NB] = {}; std::vector<char> buf[NB];
    for (int i = 0; i < NB; i++) { buf[i].reserve(cap * 2); hdr[i].dwFlags = WHDR_DONE; }
    const double bps = wf->nAvgBytesPerSec; const int align = wf->nBlockAlign;
    double first = -1, last = 0; bool eos = false, done = false; int cur_ = 0;
    std::vector<char> pend;
    while (!eos) {
        { std::lock_guard<std::mutex> l(mx); if (req.gen != r.gen) break; }
        WAVEHDR &h = hdr[cur_];
        while (!(h.dwFlags & WHDR_DONE)) WaitForSingleObject(ev, 50);
        if (h.dwFlags & WHDR_PREPARED) waveOutUnprepareHeader(wo, &h, sizeof h);
        std::vector<char> &b = buf[cur_];
        b.assign(pend.begin(), pend.end()); pend.clear();
        while (b.size() < cap && !eos && !done) {
            DWORD fl = 0; LONGLONG ts = 0; IMFSample *s = nullptr;
            hr = rd->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &fl, &ts, &s);
            if (FAILED(hr) || (fl & MF_SOURCE_READERF_ENDOFSTREAM)) { if (s) s->Release(); eos = true; break; }
            if (!s) continue;                                    // stream tick / format change without data
            IMFMediaBuffer *mb = nullptr; s->ConvertToContiguousBuffer(&mb);
            BYTE *p; DWORD n; mb->Lock(&p, nullptr, &n);
            double t0 = ts / 1e7, t1 = t0 + n / bps;
            size_t a = 0, e = n;
            if (t0 < r.from) a = (size_t)((r.from - t0) * bps) / align * align;   // seek lands before `from`
            if (t1 > r.to) { e = (size_t)((r.to - t0) * bps) / align * align; eos = true; }
            if (e > a) {
                if (first < 0) first = t0 + a / bps;
                last = t0 + e / bps;
                b.insert(b.end(), (char *)p + a, (char *)p + e);
            }
            mb->Unlock(); mb->Release(); s->Release();
        }
        if (b.size() > cap) { pend.assign(b.begin() + cap, b.end()); b.resize(cap); }
        if (b.empty()) break;
        h = {}; h.lpData = b.data(); h.dwBufferLength = (DWORD)b.size();
        waveOutPrepareHeader(wo, &h, sizeof h); waveOutWrite(wo, &h, sizeof h);
        cur_ = (cur_ + 1) % NB;
        if (eos && !pend.empty()) eos = false, done = true;      // flush the remainder, read no more
        if (done && pend.empty()) break;
    }
    bool stopped;
    { std::lock_guard<std::mutex> l(mx); stopped = req.gen != r.gen; }
    if (stopped) waveOutReset(wo);
    for (int i = 0; i < NB; i++) {
        while ((hdr[i].dwFlags & WHDR_PREPARED) && !(hdr[i].dwFlags & WHDR_DONE)) {
            { std::lock_guard<std::mutex> l(mx); if (req.gen != r.gen && !stopped) { waveOutReset(wo); stopped = true; } }
            WaitForSingleObject(ev, 50);
        }
        if (hdr[i].dwFlags & WHDR_PREPARED) waveOutUnprepareHeader(wo, &hdr[i], sizeof hdr[i]);
    }
    waveOutClose(wo); CloseHandle(ev); CoTaskMemFree(wf); rd->Release();
    logf("cd: track %d %.2f-%.2f s played %.2f-%.2f s%s", r.track, r.from, r.to, first, last, stopped ? " (stopped)" : "");
}

void Worker() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    MFStartup(MF_VERSION, MFSTARTUP_LITE);
    int seen = 0;
    for (;;) {
        Req r;
        { std::unique_lock<std::mutex> l(mx); cv.wait(l, [&] { return req.gen != seen; }); r = req; seen = r.gen; }
        if (r.play) PlaySegment(r);
    }
}

void Post(bool play, int track = 0, double from = 0, double to = 0) {
    std::lock_guard<std::mutex> l(mx);
    req.gen++; req.play = play; req.track = track; req.from = from; req.to = to;
    if (!started) { started = true; std::thread(Worker).detach(); }
    cv.notify_all();
}
}  // namespace

// dir: the game folder; vol: 0..100. Returns 1 if a track 2 file exists (emulation on).
extern "C" int cd_audio_init(const char *dir, int vol) {
    strncpy(gameDir, dir, MAX_PATH - 1); volume = vol < 0 ? 0 : vol > 100 ? 100 : vol;
    wchar_t p[MAX_PATH];
    if (!TrackFile(2, p)) return 0;
    char a[MAX_PATH]; WideCharToMultiByte(CP_ACP, 0, p, -1, a, MAX_PATH, nullptr, nullptr);
    logf("cd: emulating cdaudio with %s (volume %d)", a, volume);
    return 1;
}

// One cdaudio command string; returns the MCIERROR (0 = ok) and fills ret like MCI.
extern "C" unsigned cd_audio_command(const char *cmd, char *ret, unsigned len) {
    if (ret && len) ret[0] = 0;
    if (!_strnicmp(cmd, "status cdaudio media present", 28) || !_strnicmp(cmd, "status cdaudio ready", 20)) {
        if (ret && len >= 5) strcpy(ret, "true");
    } else if (!_strnicmp(cmd, "play cdaudio from ", 18)) {
        unsigned a = 0, b = 0; int ta, tb; double fa, fb;
        if (sscanf(cmd + 18, "%u to %u", &a, &b) != 2) return 274;   // MCIERR_MISSING_PARAMETER
        Tmsf(a, &ta, &fa); Tmsf(b, &tb, &fb);
        if (tb != ta) fb = 1e9;                                       // to the end of the start track
        logf("cd: play track %d %.2f-%.2f s", ta, fa, fb);
        Post(true, ta, fa, fb);
    } else if (!_strnicmp(cmd, "stop cdaudio", 12) || !_strnicmp(cmd, "close cdaudio", 13) || !_strnicmp(cmd, "pause cdaudio", 13)) {
        Post(false);
    }
    return 0;                                                         // open / set time format / others: accepted
}
