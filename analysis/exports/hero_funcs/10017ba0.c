
undefined4 FUN_10017ba0(void)

{
  if ((*(ushort *)(DAT_10070f24 + 0x70) & *(ushort *)(DAT_10070f24 + 0x5c)) == 0) {
    if ((*(ushort *)(DAT_10070f24 + 0x72) & *(ushort *)(DAT_10070f24 + 0x5c)) == 0) {
      return 0;
    }
    DAT_10072108 = 0x2a;
    *(undefined **)(DAT_10072860 + 100) = &DAT_10053f80;
  }
  else {
    DAT_10072108 = 0x29;
    *(undefined **)(DAT_10072860 + 100) = &DAT_10053ea0;
  }
  *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
  *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
  FUN_10015d00();
  *(undefined4 *)(DAT_10072860 + 0xa8) = 0;
  return 1;
}

