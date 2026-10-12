
void FUN_0040b740(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = (int)DAT_00a8e708;
  iVar2 = (int)DAT_00a8e70e;
  iVar3 = (int)DAT_00a8e714;
  DAT_00a8e71c = (int)(short)((short)(DAT_00a8e75e * iVar2 + DAT_00a8e75c * iVar1 +
                                      DAT_00a8e760 * iVar3 >> 0xc) + (short)DAT_00a8e770);
  DAT_00a8e728 = (int)(short)((short)(DAT_00a8e764 * iVar2 + DAT_00a8e762 * iVar1 +
                                      DAT_00a8e766 * iVar3 >> 0xc) + (short)DAT_00a8e774);
  DAT_00a8e734 = (int)(short)((short)(DAT_00a8e76a * iVar2 + DAT_00a8e768 * iVar1 +
                                      DAT_00a8e76c * iVar3 >> 0xc) + (short)DAT_00a8e778);
  if (DAT_00a8e734 < 1) {
    DAT_00a8e734 = 1;
  }
  return;
}

