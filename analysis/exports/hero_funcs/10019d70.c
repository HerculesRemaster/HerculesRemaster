
void FUN_10019d70(void)

{
  int iVar1;
  
  if (*(char *)(DAT_10072860 + 0x95) != '\0') {
    FUN_10029da0(DAT_1007217c);
    *(int *)(DAT_10072860 + 0xb8) = *(int *)(DAT_10072860 + 0xb8) - *(int *)(DAT_10072860 + 0xc4);
    if (*(int *)(DAT_10072860 + 0xb8) < 0) {
      *(undefined4 *)(DAT_10072860 + 0xb8) = 0;
      *(undefined1 *)(DAT_10072860 + 0x95) = 0;
      *(undefined4 *)(DAT_10072860 + 0xbc) = 0xd0000;
      *(undefined4 *)(DAT_10072860 + 0xc0) = 0x6000;
      if (DAT_10072108 != 0x16) {
        if ((DAT_10072108 != 4) || ((*(byte *)(DAT_10070f24 + 0x5c) & 0x40) == 0)) {
          DAT_10072108 = 7;
          return;
        }
        DAT_10072108 = 0x18;
        *(undefined **)(DAT_10072860 + 100) = &DAT_10053e28;
        *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
        *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
        FUN_10015d00();
        *(undefined4 *)(DAT_10072860 + 0xc0) = 0x10000;
        *(undefined4 *)(DAT_10072860 + 0xbc) = 0x200000;
        DAT_1007217c[0x15] = DAT_1007217c[0x15] & 0xff7fffff;
      }
    }
    else {
      iVar1 = FUN_10019970(0x10);
      if (iVar1 == 0) {
        *(int *)(DAT_10072860 + 4) = *(int *)(DAT_10072860 + 4) - *(int *)(DAT_10072860 + 0xb8);
        FUN_10019d30();
        return;
      }
    }
    return;
  }
  *(int *)(DAT_10072860 + 0xb8) = *(int *)(DAT_10072860 + 0xb8) + *(int *)(DAT_10072860 + 0xc0);
  if (*(int *)(DAT_10072860 + 0xbc) < *(int *)(DAT_10072860 + 0xb8)) {
    *(int *)(DAT_10072860 + 0xb8) = *(int *)(DAT_10072860 + 0xbc);
  }
  *(int *)(DAT_10072860 + 4) = *(int *)(DAT_10072860 + 4) + *(int *)(DAT_10072860 + 0xb8);
  if ((int)((uint)*(ushort *)((&DAT_10070d5c)[DAT_10070cfc._2_1_] + 6) * 0x400000) <
      *(int *)(DAT_10072860 + 4)) {
    if (DAT_10072108 != 0x16) {
      FUN_100170c0();
    }
    *(uint *)(DAT_10072860 + 4) = (uint)*(ushort *)((&DAT_10070d5c)[DAT_10070cfc._2_1_] + 6) << 0x16
    ;
    *(undefined4 *)(DAT_10072860 + 0xb8) = 0x80000;
    return;
  }
  FUN_10019d30();
  return;
}

