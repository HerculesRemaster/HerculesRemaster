
void FUN_100208f0(void)

{
  short sVar1;
  short *psVar2;
  short *psVar3;
  uint uVar4;
  int iVar5;
  int *piVar6;
  short *psVar7;
  
  psVar2 = (short *)(&DAT_10071e40)[(int)DAT_10070d01 + DAT_10070cfc._2_1_ * 8];
  iVar5 = 0;
  sVar1 = *psVar2;
  uVar4 = (int)DAT_10070ece + 2U & 0xfffffffe;
  if (0 < DAT_10070ece) {
    psVar3 = &DAT_10070e0c;
    piVar6 = &DAT_10070dcc;
    psVar7 = psVar2;
    do {
      psVar7 = psVar7 + 1;
      *psVar3 = *psVar7;
      psVar3[0x10] = sVar1;
      *piVar6 = (int)(psVar2 + uVar4);
      iVar5 = iVar5 + 1;
      piVar6 = piVar6 + 1;
      uVar4 = uVar4 + (int)DAT_10070d14 * (int)*psVar3 * 2;
      psVar3 = psVar3 + 1;
    } while (iVar5 < DAT_10070ece);
  }
  return;
}


