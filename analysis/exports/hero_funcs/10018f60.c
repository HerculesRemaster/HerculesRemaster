
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_10018f60(void)

{
  ushort uVar1;
  undefined2 extraout_var;
  uint uVar2;
  undefined2 extraout_var_00;
  int iVar3;
  int iVar4;
  int iVar5;
  
  *(char *)(DAT_10070f24 + 0xd) = *(char *)(DAT_10070f24 + 0xd) + '\x01';
  if (*(char *)(DAT_10070f24 + 0xc) != '\0') {
    *(char *)(DAT_10070f24 + 0xc) = *(char *)(DAT_10070f24 + 0xc) + -1;
  }
  if ((*(byte *)(DAT_10070f24 + 0x30) & 8) != 0) {
    *(char *)(DAT_10070f24 + 0xc) = *(char *)(DAT_10070f24 + 0xc) + '\x01';
  }
  if (*(char *)(DAT_10070f24 + 0xc) == '\0') {
    *(undefined1 *)(DAT_10070f24 + 3) = 1;
  }
  _DAT_10070c54 = 1;
  *(undefined1 *)(DAT_10070f24 + 5) = 1;
  *(undefined1 *)(DAT_10070f24 + 1) = *(undefined1 *)(DAT_10070f24 + 2);
  FUN_1002c4d0();
  FUN_10029c20(DAT_10072860);
  iVar3 = 0;
  iVar5 = 0;
  iVar4 = 0x100000;
  do {
    uVar1 = FUN_10030010(iVar4 + *DAT_10072860,DAT_10072860[1] + -0x20000,DAT_10072860[2],1);
    if (CONCAT22(extraout_var,uVar1) != 0) {
      iVar3 = iVar3 + 1;
    }
    uVar2 = FUN_1002a010(iVar4 + *DAT_10072860,DAT_10072860[1],DAT_10072860[2]);
    if ((uVar2 & 0xffff) != 0) {
      iVar3 = iVar3 + 1;
    }
    uVar1 = FUN_10030010(*DAT_10072860 - iVar4,DAT_10072860[1] + -0x20000,DAT_10072860[2],1);
    if (CONCAT22(extraout_var_00,uVar1) != 0) {
      iVar5 = iVar5 + 1;
    }
    uVar2 = FUN_1002a010(*DAT_10072860 - iVar4,DAT_10072860[1],DAT_10072860[2]);
    if ((uVar2 & 0xffff) != 0) {
      iVar5 = iVar5 + 1;
    }
    iVar4 = iVar4 + 0x100000;
  } while (iVar4 < 0x410000);
  DAT_10072108 = 0x16;
  *(undefined2 *)(DAT_10072860 + 0xc) = 0xffff;
  *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
  if ((iVar5 != 0) && (iVar3 != 0)) {
    DAT_10072860[0x19] = (int)&DAT_100549f0;
    FUN_10015d00();
    return;
  }
  if (iVar3 + iVar5 == 0) {
    uVar2 = FUN_1002c460(100);
    if (uVar2 < 0x32) {
      iVar5 = 1;
    }
  }
  if (iVar5 == 0) {
    if ((char)DAT_10072860[0x25] != '\0') {
      DAT_10072860[0x19] = (int)&DAT_10054aa0;
      FUN_10015d00();
      return;
    }
  }
  else if ((char)DAT_10072860[0x25] == '\0') {
    DAT_10072860[0x19] = (int)&DAT_10054aa0;
    FUN_10015d00();
    return;
  }
  DAT_10072860[0x19] = (int)&DAT_10054930;
  FUN_10015d00();
  return;
}

