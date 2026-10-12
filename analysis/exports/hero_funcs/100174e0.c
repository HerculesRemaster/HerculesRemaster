
void FUN_100174e0(void)

{
  int iVar1;
  bool bVar2;
  ushort uVar3;
  ushort uVar4;
  undefined3 extraout_var;
  undefined2 extraout_var_00;
  undefined2 extraout_var_01;
  int iVar5;
  
  DAT_10072860[0x2b] = 0;
  if (DAT_10072108 == 0x13) {
    DAT_10072860[0x2a] = 0x30000;
    DAT_10072860[0x2b] = 0x30000;
  }
  else if ((*(byte *)(DAT_10070f24 + 0x5c) & 0xa0) != 0) {
    DAT_10072860[0x2b] = DAT_10072860[0x42];
  }
  if ((char)DAT_10072860[0x25] == '\0') {
    if ((*(byte *)(DAT_10070f24 + 0x5c) & 0x80) == 0) goto LAB_10017605;
    if (DAT_1006ef70 == 5) {
      if (0xbfff < DAT_10072860[0x2a]) goto LAB_100175ca;
      DAT_10072860[0x2c] = 0x6000;
      *(undefined1 *)(DAT_10072860 + 0x25) = 0xff;
    }
    else {
      *(undefined1 *)(DAT_10072860 + 0x25) = 0xff;
    }
  }
  else {
    if ((*(byte *)(DAT_10070f24 + 0x5c) & 0x20) == 0) goto LAB_10017605;
    if (DAT_1006ef70 == 5) {
      if (0xbfff < DAT_10072860[0x2a]) {
LAB_100175ca:
        DAT_10072860[0x2b] = 0x4000;
        DAT_10072860[0x2d] = 0x5000;
        goto LAB_10017605;
      }
      DAT_10072860[0x2c] = 0x6000;
      *(undefined1 *)(DAT_10072860 + 0x25) = 0;
    }
    else {
      *(undefined1 *)(DAT_10072860 + 0x25) = 0;
    }
  }
  *(uint *)(DAT_1007217c + 0x54) =
       *(byte *)(DAT_1007217c + 0x94) & 0x10 | *(uint *)(DAT_1007217c + 0x54) & 0xffffffef;
LAB_10017605:
  if (DAT_10072108 == 0x13) {
    FUN_10019a10();
    return;
  }
  FUN_10019a10();
  bVar2 = FUN_10019650();
  if (CONCAT31(extraout_var,bVar2) == 0) {
    *DAT_10072860 = DAT_10072860[0x27];
  }
  uVar3 = FUN_10030010(*DAT_10072860 + -0x1c0000,DAT_10072860[1] + -0x10000,DAT_10072860[2],1);
  uVar4 = FUN_10030010(*DAT_10072860 + 0x1c0000,DAT_10072860[1] + -0x10000,DAT_10072860[2],1);
  if (CONCAT22(extraout_var_00,uVar3) != 0) {
    if (CONCAT22(extraout_var_01,uVar4) != 0) {
      return;
    }
    if (CONCAT22(extraout_var_00,uVar3) != 0) {
      *DAT_10072860 = *DAT_10072860 + 0x20000;
    }
  }
  if (CONCAT22(extraout_var_01,uVar4) != 0) {
    *DAT_10072860 = *DAT_10072860 + -0x20000;
  }
  iVar5 = FUN_10019970(0);
  if (iVar5 != 0) {
    iVar1 = *DAT_10072860;
    if (iVar1 < *(int *)(&DAT_10071a20)[iVar5]) {
      *DAT_10072860 = iVar1 + -0x20000;
      return;
    }
    *DAT_10072860 = iVar1 + 0x20000;
  }
  return;
}

