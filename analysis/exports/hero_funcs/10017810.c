
undefined4 FUN_10017810(void)

{
  undefined4 *puVar1;
  bool bVar2;
  int iVar3;
  undefined3 extraout_var;
  int *piVar4;
  short local_c [6];
  
  iVar3 = FUN_1002a880(DAT_1007217c,local_c,3);
  if ((iVar3 != 0) && (iVar3 = 0, 0 < DAT_10071a20)) {
    piVar4 = &DAT_10071a24;
    do {
      puVar1 = (undefined4 *)*piVar4;
      bVar2 = FUN_1002a810(*puVar1,puVar1[1],puVar1[2],local_c);
      if (CONCAT31(extraout_var,bVar2) != 0) {
        *DAT_10072860 = *puVar1;
        DAT_10072860[1] = puVar1[1] + 0x300000;
        DAT_10072108 = 9;
        DAT_10072860[0x19] = &DAT_10054810;
        *(undefined2 *)(DAT_10072860 + 0xd) = 0x126;
        *(undefined2 *)(DAT_10072860 + 0xc) = 1;
        *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
        return 1;
      }
      iVar3 = iVar3 + 1;
      piVar4 = piVar4 + 1;
    } while (iVar3 < DAT_10071a20);
    return 0;
  }
  return 0;
}

