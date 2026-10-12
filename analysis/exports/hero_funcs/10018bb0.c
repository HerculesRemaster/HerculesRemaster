
undefined4 FUN_10018bb0(void)

{
  if ((*(ushort *)(DAT_10070f24 + 0x6c) & *(ushort *)(DAT_10070f24 + 0x5c)) == 0) {
    return 0;
  }
  if ((*(byte *)(DAT_10070f24 + 0x5c) & 0x10) == 0) {
    DAT_10072108 = 0x2f;
    *(undefined **)(DAT_10072860 + 100) = &DAT_10054558;
  }
  else {
    if (DAT_1007201c == 4) {
      return 0;
    }
    DAT_10072108 = 0x2f;
    *(undefined **)(DAT_10072860 + 100) = &DAT_100545e8;
  }
  *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
  *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
  FUN_10015d00();
  return 1;
}

