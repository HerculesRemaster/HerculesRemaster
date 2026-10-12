
void FUN_10016e70(void)

{
  bool bVar1;
  
  if (DAT_1007201c == 0) {
    bVar1 = DAT_10072108 == 1;
    DAT_10072108 = 6;
    if (bVar1) {
      *(undefined **)(DAT_10072860 + 100) = &DAT_10053c18;
      *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
      *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
      FUN_10015d00();
      return;
    }
    *(undefined **)(DAT_10072860 + 100) = &DAT_10053c00;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    FUN_10015d00();
    return;
  }
  DAT_10072108 = 6;
  if (*(char *)(DAT_10072860 + 0x94) != '\0') {
    *(undefined **)(DAT_10072860 + 100) = &DAT_10053c80;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    FUN_10015d00();
    return;
  }
  *(undefined **)(DAT_10072860 + 100) = &DAT_10053c40;
  *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
  *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
  FUN_10015d00();
  return;
}

