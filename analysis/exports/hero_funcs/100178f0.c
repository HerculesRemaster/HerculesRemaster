
void FUN_100178f0(void)

{
  int iVar1;
  
  DAT_1007144c = 0;
  iVar1 = FUN_10015d00();
  if (iVar1 != 0) {
    DAT_10072108 = 5;
    *(undefined **)(DAT_10072860 + 100) = &DAT_100548e8;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    FUN_10015d00();
    DAT_1006ef70 = 5;
    *(undefined1 *)(DAT_10072860 + 0x95) = 0xff;
    *(undefined4 *)(DAT_10072860 + 0x108) = 0x40000;
    *(undefined4 *)(DAT_10072860 + 0xb8) = 0xf0000;
    *(undefined4 *)(DAT_10072860 + 0xc0) = 0x7000;
    *(undefined4 *)(DAT_10072860 + 0xc4) = 0xe000;
    *(undefined4 *)(DAT_10072860 + 0xbc) = 0xd0000;
    *(undefined4 *)(DAT_10072860 + 0xac) = 0x40000;
    *(undefined4 *)(DAT_10072860 + 0xa8) = 0x40000;
    *(undefined4 *)(DAT_10072860 + 0xb0) = 0x4000;
    if (DAT_10072110 != 0) {
      *(undefined4 *)(DAT_10072860 + 0xac) = 0x8000;
      *(undefined4 *)(DAT_10072860 + 0xa8) = 0x8000;
      *(undefined4 *)(DAT_10072860 + 0xb0) = 0x1000;
    }
    DAT_1006ef7c = 0;
    *(undefined4 *)(DAT_10072860 + 0x108) = *(undefined4 *)(DAT_10072860 + 0xa8);
  }
  return;
}

