
bool FUN_10019650(void)

{
  ushort uVar1;
  undefined2 extraout_var;
  undefined2 extraout_var_00;
  undefined2 extraout_var_01;
  int iVar2;
  
  iVar2 = (-(uint)((char)DAT_10072860[0x25] != '\0') & 0xffc80000) + 0x1c0000;
  uVar1 = FUN_10030010(iVar2 + *DAT_10072860,DAT_10072860[1] + -0x200000,DAT_10072860[2],1);
  if (CONCAT22(extraout_var,uVar1) != 0) {
    return false;
  }
  uVar1 = FUN_10030010(iVar2 + *DAT_10072860,DAT_10072860[1] + -0x100000,DAT_10072860[2],1);
  if (CONCAT22(extraout_var_00,uVar1) != 0) {
    return false;
  }
  uVar1 = FUN_10030010(iVar2 + *DAT_10072860,DAT_10072860[1],DAT_10072860[2],1);
  return (bool)('\x01' - (CONCAT22(extraout_var_01,uVar1) != 0));
}

