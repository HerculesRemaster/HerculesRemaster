
void FUN_10016a60(void)

{
  DAT_10072108 = 1;
  if (DAT_1007201c == 0) {
    *(undefined **)(DAT_10072860 + 100) = &DAT_10053a20;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    FUN_10015d00();
    *(undefined4 *)(DAT_10072860 + 0xac) = 0x80000;
    *(undefined4 *)(DAT_10072860 + 0xb0) = 0x8000;
    *(undefined4 *)(DAT_10072860 + 0xa8) = 0x40000;
  }
  else {
    *(undefined **)(DAT_10072860 + 100) = &DAT_10053b00;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    FUN_10015d00();
    *(undefined4 *)(DAT_10072860 + 0xac) = 0x40000;
    *(undefined4 *)(DAT_10072860 + 0xb0) = 0x2000;
    *(undefined4 *)(DAT_10072860 + 0xa8) = 0x20000;
  }
  *(undefined4 *)(DAT_10072860 + 0xb8) = 0;
  *(undefined4 *)(DAT_10072860 + 0xbc) = 0;
  return;
}

