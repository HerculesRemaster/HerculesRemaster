
void FUN_10016cf0(void)

{
  DAT_10072108 = 3;
  if (*(char *)(DAT_1007217c + 0x96) == '\0') {
    *(undefined **)(DAT_10072860 + 100) = &DAT_10053bc8;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
  }
  else {
    *(undefined **)(DAT_10072860 + 100) = &DAT_10053bb0;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
  }
  FUN_10015d00();
  DAT_10070d10 = DAT_10070d10 | 8;
  *(undefined4 *)(DAT_10072860 + 0xac) = 0;
  *(undefined4 *)(DAT_10072860 + 0xa8) = 0;
  *(undefined4 *)(DAT_10072860 + 0xb0) = 0;
  *(undefined4 *)(DAT_10072860 + 0xb8) = 0;
  *(undefined4 *)(DAT_10072860 + 0xbc) = 0;
  return;
}

