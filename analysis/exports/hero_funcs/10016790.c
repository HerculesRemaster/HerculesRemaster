
void FUN_10016790(void)

{
  if (DAT_10072170 != 0) {
    FUN_10019160();
    return;
  }
  if (DAT_10072110 != 0) {
    if (DAT_1007201c == 0) {
      FUN_10018f60();
      return;
    }
    DAT_10072108 = 0x2f;
    *(undefined **)(DAT_10072860 + 100) = &DAT_10054558;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    FUN_10015d00();
    return;
  }
  DAT_10072108 = 0;
  *(undefined **)(DAT_10072860 + 100) = &DAT_10053820;
  *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
  *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
  FUN_10015d00();
  *(undefined4 *)(DAT_10072860 + 0x4c) = 2;
  *(undefined1 *)(DAT_10072860 + 0x96) = 0xff;
  return;
}

