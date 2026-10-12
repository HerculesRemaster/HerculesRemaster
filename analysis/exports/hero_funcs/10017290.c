
undefined4 FUN_10017290(void)

{
  if (((DAT_1006ef7c != 0) &&
      ((*(ushort *)(DAT_10070f24 + 0x6c) & *(ushort *)(DAT_10070f24 + 0x5c)) != 0)) &&
     (DAT_1007211c == 0)) {
    *(undefined **)(DAT_10072860 + 100) = &DAT_100541f8;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    DAT_1007211c = 1;
    DAT_1006ef7c = 0;
    return 1;
  }
  if ((DAT_1006ef7c != 0) &&
     ((*(ushort *)(DAT_10070f24 + 0x70) & *(ushort *)(DAT_10070f24 + 0x5c)) != 0)) {
    *(undefined **)(DAT_10072860 + 100) = &DAT_100540c0;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    DAT_1006ef7c = 0;
    return 1;
  }
  return 0;
}

