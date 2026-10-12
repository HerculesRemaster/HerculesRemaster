
undefined4 FUN_10017d00(void)

{
  if (DAT_10071840 != 0) {
    if (((*(byte *)(DAT_10070f24 + 0x5c) & 0x10) != 0) &&
       (*(int *)(DAT_10072860 + 8) >> 0x10 < (int)DAT_10071bc4)) {
      FUN_10017d60();
      return 1;
    }
    if (((*(byte *)(DAT_10070f24 + 0x5c) & 0x40) != 0) &&
       ((int)DAT_10071c5c < *(int *)(DAT_10072860 + 8) >> 0x10)) {
      FUN_10017d60();
      return 1;
    }
  }
  return 0;
}

