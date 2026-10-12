#include <io.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>

#ifdef GCC32HACK
#include "win32gcc.h"
#endif

/*
note that if you want to catch file names in the game you must
replace start of the filepath before you put filename to hash routine:
"O:\" => "M:\GRAFIX\MAPS\"
"P:\" => "M:\GRAFIX\CHOP\"
"Q:\" => "M:\LANGUAGE\SONIX\"
*/

/* instead "herchash.txt" file with file names use compacted and compressed file list which
   saves around 32 Kb in total size, not to mention removes requirement for the external file */
#include "namelist.c"

#pragma pack(push, 1)
typedef struct {
  uint32_t hash;
  uint32_t offs;
  uint32_t size;
} hfs_item;
#pragma pack(pop)

#define MAX_FS 1024
static hfs_item list[MAX_FS];

/* more optimized version compared to the one used by the game */
uint32_t herchash(char *s) {
uint32_t i, h;
  h = 0;
  if (s) {
    for (i = 0; s[i]; i++) {
      /* toupper() not required there since all characters in compacted name list
         already in uppercase, it's just here to show how original game code works */
      h += (toupper(s[i]) << ((i & 3) * 8));
    }
    h += i;
  }
  return(h);
}

/* search in compacted name list for hash */
void hercname(char *s, uint32_t hash, uint32_t len) {
uint8_t b;
size_t i;
int j;
  if (s && len) {
    /* for terminate zero byte */
    len--;
    /* position in name string buffer */
    j = 0;
    /* for each byte in compacted name list */
    for (i = 0; i < sizeof(packname); i++) {
      /* get byte */
      b = packname[i];
      /* if zero - it's the end of the current string */
      if (!b) {
        /* terminate string with the tail zero byte */
        s[j] = b;
        /* check hash and return if matched - string found */
        if (herchash(s) == hash) { return; }
      } else {
        /* highest bit set - command byte */
        if (b & 0x80) {
          if (b & 0x40) {
            /* both highest bits set - rollback string position */
            j -= (b & 0x3F);
          } else {
            /* only one bit set - advance string position */
            j += (b & 0x3F);
          }
          /* sanity checks */
          if (j <   0) { j =   0; }
          if (j > len) { j = len; }
        } else {
          /* higher bit not set - it's just single character - copy as is */
          if (j < len) {
            s[j] = b;
            j++;
          }
        }
      }
    }
    /* hash not found in compacted name list - use hash as file name */
    if (len > 8) {
      sprintf(s, "%08X", hash);
    } else {
      /* string too short even for HEX hash */
      *s = 0;
    }
  }
}

void makedirs(char *path) {
char *s;
  if (path) {
    for (s = path; *s; s++) {
      if ((*s == '\\') || (*s == '/')) {
        *s = 0;
        mkdir(path);
        *s = '\\';
      }
    }
  }
}

void *dataread(FILE *fl, uint32_t offs, uint32_t size) {
void *p;
  p = NULL;
  if (fl) {
    p = malloc(size);
    if (p) {
      memset(p, 0, size);
      fseek(fl, offs, SEEK_SET);
      fread(p, size, 1, fl);
    }
  }
  return(p);
}

void dumpfile(char *name, void *buff, uint32_t size) {
FILE *fl;
  if (name) {
    makedirs(name);
    fl = fopen(name, "wb");
    if (fl) {
      if (buff && size) {
        fwrite(buff, size, 1, fl);
      }
      fclose(fl);
    }
  }
}

int main(int argc, char *argv[]) {
uint32_t i, j;
uint8_t *p;
FILE *fl;
/* max length for known hashes is 56 characters (without zero tail byte) */
char s[64];
  printf(
    "Disney's Hercules .FS extractor v1.1\n"
    "(c) CTPAX-X Team 2012,2020\n"
    "http://www.CTPAX-X.org/\n\n"
  );
  if (argc != 2) {
    printf("Usage: unhercfs <hercules.fs>\n\n");
    return(1);
  }
  fl = fopen(argv[1], "rb");
  if (!fl) {
    printf("Error: can't open input file.\n\n");
    return(2);
  }
  /* unpack compacted name list */
  memset(packname, 0, sizeof(packname));
  /* +14 to skip COMPRESS.EXE file header which can be removed but its size neglible */
  lzss_unpack(&lzssname[14], packname, sizeof(lzssname) - 14, sizeof(packname));
  /* read TOC/FAT */
  memset(list, 0, sizeof(list));
  fread(list, sizeof(list), 1, fl);
  /*printf("fileoffs filesize filehash filename\n");*/
  for (i = 0; i < MAX_FS; i++) {
    if (!list[i].hash) { break; }
    hercname(s, list[i].hash, sizeof(s) / sizeof(s[0]));
    /*printf("%08X %08X %08X %s\n", list[i].offs, list[i].size, list[i].hash, s);*/
    printf("%s", s);
    p = (uint8_t *) dataread(fl, list[i].offs, list[i].size);
    /* fix .TGA bottom-top orientation */
    j = strlen(s);
    if ((j >= 4) && (!strcmp(&s[j - 4], ".TGA")) && (p) && (list[i].size > 0x11)) {
      p[0x11] |= 0x20; /* tga_head.ImageDescriptor */
    }
    dumpfile((s[1] == ':') ? &s[3] : s, p, list[i].size);
    printf("\n");
  }
  fclose(fl);
  printf("\ndone\n\n");
  return(0);
}
