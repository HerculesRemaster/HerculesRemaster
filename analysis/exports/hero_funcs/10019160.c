
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_10019160(void)

{
  if (DAT_1005cf54 == 3) {
    DAT_10072108 = 0x19;
    *(undefined **)(DAT_10072860 + 100) = &DAT_10054b50;
  }
  else {
    FUN_10003520(5);
    DAT_10072108 = 0x19;
    *(undefined **)(DAT_10072860 + 100) = &DAT_10054c30;
  }
  *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
  *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
  FUN_10015d00();
  _DAT_10070c54 = 1;
  *(undefined1 *)(DAT_10070f24 + 1) = *(undefined1 *)(DAT_10070f24 + 2);
  return;
}

