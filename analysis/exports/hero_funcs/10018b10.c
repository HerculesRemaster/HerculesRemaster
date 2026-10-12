
undefined4 FUN_10018b10(void)

{
  ushort uVar1;
  
  if (DAT_1007210c == 0) {
    return 0;
  }
  uVar1 = *(ushort *)(DAT_1007210c + 0x2e);
  if ((uVar1 != 2) && ((uVar1 < 4 || (5 < uVar1)))) {
    return 0;
  }
  *(undefined2 *)(DAT_1007210c + 0x9a) = 1;
  DAT_1007201c = (uint)*(ushort *)(DAT_1007210c + 0x2e);
  DAT_1006ef98 = 0;
  DAT_1006ef9c = 0;
  DAT_10072108 = 0x14;
  *(undefined **)(DAT_10072860 + 100) = &DAT_100544c0;
  *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
  *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
  FUN_10015d00();
  return 1;
}

