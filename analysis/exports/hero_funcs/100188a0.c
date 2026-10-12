
undefined4 FUN_100188a0(void)

{
  ushort uVar1;
  undefined2 extraout_var;
  
  if (DAT_10072108 != 0x13) {
    uVar1 = FUN_10030010((-(uint)((char)DAT_10072860[0x25] != '\0') & 0xffb80000) + 0x240000 +
                         *DAT_10072860,DAT_10072860[1] + -0x300000,DAT_10072860[2],0x3000);
    if (CONCAT22(extraout_var,uVar1) != 0) {
      FUN_10018900();
      return 1;
    }
  }
  return 0;
}

