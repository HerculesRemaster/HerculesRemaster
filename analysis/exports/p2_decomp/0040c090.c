
undefined4 FUN_0040c090(undefined4 param_1,short param_2)

{
  short sVar1;
  undefined2 uVar2;
  undefined2 extraout_var;
  int iVar3;
  
  uVar2 = 0;
  if ((&DAT_00a8e790)[param_2] != 0) {
    iVar3 = 0x44;
    do {
      sVar1 = *(short *)((&DAT_00a8e790)[param_2] + iVar3);
      uVar2 = (undefined2)((uint)(&DAT_00a8e790)[param_2] >> 0x10);
      if (sVar1 != 0) {
        FUN_0040f3d0((int)sVar1);
        uVar2 = extraout_var;
      }
      iVar3 = iVar3 + 2;
    } while (iVar3 < 0x144);
  }
  return CONCAT22(uVar2,param_2);
}

