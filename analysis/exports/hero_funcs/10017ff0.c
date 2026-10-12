
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_10017ff0(void)

{
  if ((*(ushort *)(DAT_10070f24 + 0x6c) & *(ushort *)(DAT_10070f24 + 0x5c)) == 0) {
    return 0;
  }
  _DAT_1006ef78 = _DAT_1006ef78 & 0xffff0000;
  _DAT_10072024 = 0;
  DAT_1007215a = 0;
  if (DAT_1006efb4 == '\x01') {
    DAT_10072108 = 0x2b;
    *(undefined **)(DAT_10072860 + 100) = &DAT_10054260;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    FUN_10015d00();
    DAT_1007216a = 0xd9;
    *DAT_10072020 =
         *(short *)(&DAT_10054c9c + (_DAT_1006ef78 & 0xffff) * 10) * 0x10000 + *DAT_1007217c;
    DAT_10072020[1] =
         *(short *)(&DAT_10054c9e + (_DAT_1006ef78 & 0xffff) * 10) * 0x10000 + DAT_1007217c[1];
    *(undefined2 *)(DAT_10072020 + 0x1d) =
         *(undefined2 *)(&DAT_10054ca0 + (_DAT_1006ef78 & 0xffff) * 10);
    if (*(char *)(DAT_10072860 + 0x94) != '\0') {
      _DAT_1006ef78 = CONCAT22(DAT_1006ef78_2,0x13);
    }
    DAT_1006ef74._0_2_ = *(undefined2 *)(&DAT_10054c98 + (_DAT_1006ef78 & 0xffff) * 10);
  }
  else if (DAT_1006efb4 == '\x02') {
    DAT_10072108 = 0x2c;
    *(undefined **)(DAT_10072860 + 100) = &DAT_100542a0;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    FUN_10015d00();
    DAT_1007216a = 0xd9;
    if (*(char *)(DAT_10072860 + 0x94) != '\0') {
      _DAT_1006ef78 = CONCAT22(DAT_1006ef78_2,5);
    }
    DAT_1006ef74._0_2_ = *(undefined2 *)(&DAT_10054d60 + (_DAT_1006ef78 & 0xffff) * 10);
  }
  else if (DAT_1006efb4 == '\x03') {
    DAT_10072108 = 0x2d;
    *(undefined **)(DAT_10072860 + 100) = &DAT_100543d0;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    FUN_10015d00();
  }
  else {
    if (DAT_1007211c != 0) {
      _DAT_10072024 = 0;
      DAT_1007215a = 0;
      return 0;
    }
    DAT_10072108 = 0xc;
    *(undefined **)(DAT_10072860 + 100) = &DAT_10054120;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    FUN_10015d00();
    DAT_1007211c = 1;
  }
  *(undefined4 *)(DAT_10072860 + 0xa8) = 0;
  return 1;
}

