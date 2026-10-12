#pragma pack(push, 1)
typedef struct {
  int16_t pred_val;
  int8_t  step_idx;
} adpcmnfo;

typedef struct {
  uint32_t dwRIFFSign;
  uint32_t dwRIFFSize;
  uint32_t dwWAVESign;
  uint32_t dwfmt_Sign;
  uint32_t dwfmt_Size;
  /* PCMWAVEFORMAT */
  /* WAVEFORMAT */
  uint16_t wFormatTag;
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  /* /WAVEFORMAT */
  uint16_t wBitsPerSample;
  /* /PCMWAVEFORMAT */
  uint32_t dwdataSign;
  uint32_t dwdataSize;
} wav_head;
#pragma pack(pop)

void FillWaveHeader(wav_head *hdr, uint32_t len, uint32_t sps, uint16_t chn, uint16_t bps) {
  if (hdr) {
    memset(hdr, 0, sizeof(hdr[0]));
    hdr->dwRIFFSign = 0x46464952;
    hdr->dwRIFFSize = sizeof(hdr[0]) - 8 + len;
    hdr->dwWAVESign = 0x45564157;
    hdr->dwfmt_Sign = 0x20746D66;
    hdr->dwfmt_Size = 16;
    hdr->wFormatTag = 1;
    hdr->nChannels  = chn;
    hdr->nSamplesPerSec = sps;
    hdr->wBitsPerSample = bps;
    hdr->nBlockAlign = hdr->nChannels * (hdr->wBitsPerSample / 8);
    hdr->nAvgBytesPerSec = hdr->nBlockAlign * hdr->nSamplesPerSec;
    hdr->dwdataSign = 0x61746164;
    hdr->dwdataSize = len;
  }
}

/* ADPCM routines */

static const char adpcm_index[8] = {-1, -1, -1, -1, 2, 4, 6, 8};

static const short adpcm_step[89] = {
  7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
  19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
  50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
  130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
  337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
  876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
  2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
  5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
  15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

int16_t adpcm_decoder(uint8_t code, adpcmnfo *state) {
int16_t pred_diff, step;
int8_t sign;
int32_t i;
  sign = code & 0x8;
  code &= 0x7;
  step = adpcm_step[state->step_idx];
  pred_diff = step >> 3;
  for (i = 0x4; i; i >>= 1) {
    if (code & i) { pred_diff += step; }
    step >>= 1;
  }
  i = state->pred_val;
  i += (sign) ? -pred_diff : pred_diff;
  if (i > 32767) { i = 32767; }
  else { if (i < -32768) { i = -32768; } }
  state->pred_val = i;
  state->step_idx += adpcm_index[code];
  if (state->step_idx < 0) { state->step_idx = 0; }
  else { if (state->step_idx > 88) { state->step_idx = 88; } }
  return(state->pred_val);
}

uint8_t *IMA2WAV(uint8_t *data, uint32_t size, uint32_t sdhz) {
adpcmnfo info;
uint32_t i;
int16_t *w;
uint8_t *p;
  p = NULL;
  if (data && size) {
    p = (uint8_t *) malloc(sizeof(wav_head) + (size * 4));
    if (p) {
      FillWaveHeader((wav_head *) p, size * 4, sdhz, 1, 16);
      w = (int16_t *) &p[sizeof(wav_head)];
      memset(&info, 0, sizeof(info));
      for (i = 0; i < size; i++) {
        *w = adpcm_decoder(data[i] >> 4, &info); w++;
        *w = adpcm_decoder(data[i]     , &info); w++;
      }
    }
  }
  return(p);
}
