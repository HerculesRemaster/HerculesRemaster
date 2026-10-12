
void FUN_10017060(void)

{
  DAT_10072108 = 2;
  *(undefined **)(DAT_10072860 + 100) = &DAT_10053d10;
  *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
  *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
  FUN_10015d00();
  DAT_1006ef70 = 2;
  FUN_10017130();
  FUN_10029620(DAT_10072860);
  *(undefined4 *)(DAT_10072860 + 0x108) = *(undefined4 *)(DAT_10072860 + 0xa8);
  return;
}

