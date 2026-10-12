
void FUN_10017d60(void)

{
  *(undefined4 *)(DAT_10072860 + 0xcc) = 0;
  *(undefined4 *)(DAT_10072860 + 0xd0) = 0;
  *(undefined4 *)(DAT_10072860 + 200) = 0;
  *(undefined4 *)(DAT_10072860 + 0xac) = 0;
  *(undefined4 *)(DAT_10072860 + 0xa8) = 0;
  *(undefined4 *)(DAT_10072860 + 0xb8) = 0;
  *(undefined4 *)(DAT_10072860 + 0xbc) = 0;
  *(undefined **)(DAT_1007217c + 0xf0) = &DAT_10054da0;
  *(undefined4 *)(DAT_1007217c + 0xf8) = 10;
  *(undefined2 *)(DAT_1007217c + 0x11e) = 0xf;
  *(undefined2 *)(DAT_1007217c + 0x72) = 0;
  DAT_10072108 = 0x1c;
  if ((DAT_10070f24[0x5c] & 0x10U) != 0) {
    *(undefined **)(DAT_10072860 + 100) = &DAT_10053a90;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    FUN_10015d00();
    *(undefined1 *)(DAT_10072860 + 0x96) = 0xff;
    *(int *)(DAT_1007217c + 0xf4) = (int)*(short *)(&DAT_10055240 + *DAT_10070f24 * 4);
    return;
  }
  *(undefined **)(DAT_10072860 + 100) = &DAT_10053ac8;
  *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
  *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
  FUN_10015d00();
  *(undefined1 *)(DAT_10072860 + 0x96) = 0;
  *(int *)(DAT_1007217c + 0xf4) = (int)*(short *)(&DAT_10055242 + *DAT_10070f24 * 4);
  return;
}

