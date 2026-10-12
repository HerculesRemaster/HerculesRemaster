
void FUN_100176e0(void)

{
  DAT_10072108 = 8;
  *(undefined **)(DAT_10072860 + 100) = &DAT_10053da0;
  *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
  *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
  FUN_10015d00();
  DAT_1006ef70 = 0;
  *(undefined4 *)(DAT_10072860 + 0xa8) = 0;
  *(undefined4 *)(DAT_10072860 + 0x108) = 0;
  *(undefined4 *)(DAT_10072860 + 0xb0) = 0;
  *(uint *)(DAT_10072860 + 4) = *(uint *)(DAT_10072860 + 4) & 0xffff0000;
  if (((DAT_100718e0 != 0) && ((int)(uint)DAT_100718e0 < *(int *)(DAT_10072860 + 4) >> 0x10)) &&
     ((*DAT_10070f24 == '\x03' || (*DAT_10070f24 == '\x01')))) {
    FUN_10006da0(0x4276,DAT_1007217c);
  }
  *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) & 0xff7fffff;
  return;
}

