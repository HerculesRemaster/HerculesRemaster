
void FUN_10017000(void)

{
  DAT_10072108 = 4;
  *(undefined **)(DAT_10072860 + 100) = &DAT_10053cc0;
  *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
  *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
  FUN_10015d00();
  DAT_1006ef70 = 4;
  FUN_10017130();
  *(undefined4 *)(DAT_10072860 + 0x108) = 0x40000;
  *(undefined4 *)(DAT_10072860 + 0xb0) = 0x8000;
  return;
}

