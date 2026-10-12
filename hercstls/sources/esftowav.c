#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>

#ifdef GCC32HACK
#include "win32gcc.h"
#endif

#pragma pack(push, 1)
typedef struct {
  uint32_t sign; /* ESF\0x03 */
  uint32_t size; /* unpacked PCM buffer size in bytes (size / 4 = datasize) */
  uint16_t sdhz; /* 22050 Hz or 11025 Hz */
  uint16_t pad1;
  uint8_t  loop; /* 1 - loop file or 0 - do not loop (? not sure about this one) */
  uint8_t  bps8; /* 1 - downsample output to 8 bits per sample: s8 = 127 - (s16 >> 8) */
  uint16_t pad2;
} esf_head;
#pragma pack(pop)

#include "imatowav.c"

char *basename(char *s) {
char *r;
  if (s) {
    for (r = s; *r; r++) {
      if ((*r == '/') || (*r == '\\')) {
        s = &r[1];
      }
    }
  }
  return(s);
}

void newext(char *name, char *ext) {
char *p;
  if (name && ext) {
    for (; (*name == '.'); name++);
    for (p = NULL; *name; name++) {
      if (*name == '.') {
        p = name;
      }
    }
    strcpy(p ? p : name, ext);
  }
}

int main(int argc, char *argv[]) {
uint8_t *p, *u;
esf_head eh;
char s[260]; /* MAX_PATH */
FILE *fl;
  printf(
    "Disney's Hercules .ESF to .WAV converter v1.1\n"
    "(c) CTPAX-X Team 2012,2020\n"
    "http://www.CTPAX-X.org/\n\n"
  );
  if (argc != 2) {
    printf("Usage: esftowav <filename.esf>\n\n");
    return(1);
  }
  strcpy(s, basename(argv[1]));
  fl = fopen(argv[1], "rb");
  if (!fl) {
    printf("Error: can't open input file.\n\n");
    return(2);
  }
  memset(&eh, 0, sizeof(eh));
  fread(&eh, sizeof(eh), 1, fl);
  fseek(fl, 0, SEEK_END);
  eh.sign ^= 0x03465345;
  if ((eh.sign) || (ftell(fl) != (sizeof(eh) + (eh.size / 4)))) {
    fclose(fl);
    printf("Error: invalid input file (%s).\n\n", eh.sign ? "sign" : "size");
    return(3);
  }
  p = (uint8_t *) malloc(eh.size / 4);
  if (!p) {
    fclose(fl);
    printf("Error: not enough memory (input).\n\n");
    return(4);
  }
  fseek(fl, sizeof(eh), SEEK_SET);
  fread(p, eh.size / 4, 1, fl);
  fclose(fl);
  u = IMA2WAV(p, eh.size / 4, eh.sdhz);
  free(p);
  if (!u) {
    printf("Error: not enough memory (output).\n\n");
    return(5);
  }
  printf("%s ->", s);
  newext(s, ".wav");
  fl = fopen(s, "wb");
  if (fl) {
    fwrite(u, *((uint32_t *)&u[4]) + 8, 1, fl);
    fclose(fl);
  }
  free(u);
  printf(" %s\n\n", s);
  return(0);
}
