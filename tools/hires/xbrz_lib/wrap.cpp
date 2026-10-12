// C entry point for tools/hires/xbrz_index.py (ctypes): xBRZ on a 32-bit ARGB image.
#include "xbrz.h"

extern "C" __declspec(dllexport) void xbrz_scale(int factor, const uint32_t *src, uint32_t *trg, int w, int h,
                                                 double equalColorTolerance) {
    xbrz::ScalerCfg cfg;
    cfg.equalColorTolerance = equalColorTolerance;
    xbrz::scale(factor, src, trg, w, h, xbrz::ColorFormat::ARGB, cfg);
}
