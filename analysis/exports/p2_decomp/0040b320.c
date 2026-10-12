
void FUN_0040b320(void)

{
  int iVar1;
  short *psVar2;
  int *piVar3;
  int *piVar4;
  int iVar5;
  
  psVar2 = &DAT_00a8e70e;
  piVar3 = &DAT_00a8e71c;
  do {
    *piVar3 = (int)(short)((short)((int)DAT_00a8e75e * (int)*psVar2 +
                                   (int)DAT_00a8e75c * (int)psVar2[-3] +
                                   (int)DAT_00a8e760 * (int)psVar2[3] >> 0xc) + (short)DAT_00a8e770)
    ;
    piVar3[3] = (int)(short)((short)((int)DAT_00a8e764 * (int)*psVar2 +
                                     (int)DAT_00a8e762 * (int)psVar2[-3] +
                                     (int)DAT_00a8e766 * (int)psVar2[3] >> 0xc) +
                            (short)DAT_00a8e774);
    iVar5 = (int)(short)((short)((int)DAT_00a8e76a * (int)*psVar2 +
                                 (int)DAT_00a8e768 * (int)psVar2[-3] +
                                 (int)DAT_00a8e76c * (int)psVar2[3] >> 0xc) + (short)DAT_00a8e778);
    piVar3[6] = iVar5;
    iVar1 = DAT_00a8e754 << 0x10;
    if (iVar5 != 0) {
      iVar1 = iVar1 / iVar5;
    }
    piVar4 = piVar3 + 1;
    psVar2[0x19] = (short)((uint)(iVar1 * *piVar3) >> 0x10) + (short)DAT_00a8e74c;
    psVar2[0x1c] = (short)((uint)(iVar1 * piVar3[3]) >> 0x10) + (short)DAT_00a8e750;
    psVar2 = psVar2 + 1;
    piVar3 = piVar4;
  } while ((int)piVar4 < 0xa8e728);
  return;
}

