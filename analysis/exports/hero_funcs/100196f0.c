
bool FUN_100196f0(void)

{
  ushort uVar1;
  undefined2 extraout_var;
  undefined2 extraout_var_00;
  undefined2 extraout_var_01;
  uint uVar2;
  int iVar3;
  
  iVar3 = (-(uint)((char)DAT_10072860[0x25] != '\0') & 0x380000) - 0x1c0000;
  uVar1 = FUN_10030010(iVar3 + *DAT_10072860,DAT_10072860[1] + -0x200000,DAT_10072860[2],1);
  if (CONCAT22(extraout_var,uVar1) != 0) {
    return false;
  }
  uVar1 = FUN_10030010(iVar3 + *DAT_10072860,DAT_10072860[1] + -0x100000,DAT_10072860[2],1);
  if (CONCAT22(extraout_var_00,uVar1) != 0) {
    return false;
  }
  uVar1 = FUN_10030010(iVar3 + *DAT_10072860,DAT_10072860[1],DAT_10072860[2],1);
  if (CONCAT22(extraout_var_01,uVar1) != 0) {
    return false;
  }
  uVar2 = FUN_1002a010(iVar3 + *DAT_10072860,DAT_10072860[1],DAT_10072860[2]);
  return (bool)('\x01' - (uVar2 != 0));
}

