
void FUN_10016c40(void)

{
  if (DAT_1007201c == 0) {
    if (*(int *)(DAT_10072860 + 0xa8) < 0x40000) {
      FUN_10016790();
      goto LAB_10016ca7;
    }
    DAT_10072108 = 3;
    *(undefined **)(DAT_10072860 + 100) = &DAT_10053b90;
  }
  else {
    DAT_10072108 = 3;
    *(undefined **)(DAT_10072860 + 100) = &DAT_10053bf0;
  }
  *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
  *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
  FUN_10015d00();
LAB_10016ca7:
  DAT_1006efa8 = 0;
  *(undefined4 *)(DAT_10072860 + 0xac) = 0;
  *(undefined4 *)(DAT_10072860 + 0xa8) = 0;
  *(undefined4 *)(DAT_10072860 + 0xb0) = 0;
  *(undefined4 *)(DAT_10072860 + 0xb8) = 0;
  *(undefined4 *)(DAT_10072860 + 0xbc) = 0;
  return;
}

