
int __cdecl FUN_10020b50(int param_1,int param_2,int param_3)

{
  short *psVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint *puVar7;
  uint uVar8;
  int iVar9;
  ushort uVar10;
  uint uVar11;
  
  if (DAT_10070d01 == -1) {
    return 0x7fff7fff;
  }
  uVar3 = param_3 + 0x3fU & 0xffff;
  uVar8 = uVar3 >> 7;
  if ((int)uVar8 < 0) {
    uVar8 = 0;
  }
  else if ((int)((int)DAT_10070ece - 1U) < (int)uVar8) {
    uVar8 = (int)DAT_10070ece - 1U;
  }
  uVar4 = param_1 + (uint)(ushort)((&DAT_10070e2c)[uVar8] * -0x40);
  uVar2 = (uVar4 & 0xffff) >> 6;
  if ((ushort)(&DAT_10070e0c)[uVar8] <= (ushort)uVar2) {
    return 0x7fff7fff;
  }
  uVar11 = param_2 + 0xffe0U & 0xffff;
  uVar6 = uVar11 >> 6;
  if (DAT_10070d14 <= (ushort)uVar6) {
    return 0x7fff7fff;
  }
  uVar4 = uVar4 & 0x3f;
  param_1 = 0x7fff;
  uVar3 = 0x3f - (uVar3 >> 1 & 0x3f);
  iVar9 = uVar6 << 6;
  puVar7 = (uint *)((&DAT_10070dcc)[uVar8] + (uVar2 * (int)(short)DAT_10070d14 + uVar6) * 4);
  uVar10 = DAT_10070d14;
  do {
    uVar8 = *puVar7 >> 1 & 0x3ff;
    if (uVar8 != 0) {
      psVar1 = (short *)(DAT_10070d34 + -2 + uVar8 * 4);
      iVar5 = (int)*psVar1;
      if (iVar5 != 0) {
        uVar8 = uVar4;
        if ((*puVar7 & 1) != 0) {
          uVar8 = 0x3f - uVar4;
        }
        iVar5 = FUN_10020990(uVar8,uVar3,(char *)((int)psVar1 + iVar5));
        uVar10 = DAT_10070d14;
        if ((iVar5 != -1) && (iVar5 = (iVar9 - iVar5) - uVar11, -1 < iVar5)) break;
      }
    }
    puVar7 = puVar7 + 1;
    uVar6 = uVar6 + 1;
    iVar9 = iVar9 + 0x40;
    iVar5 = param_1;
  } while ((int)uVar6 < (int)(short)uVar10);
  param_1 = iVar5;
  iVar9 = uVar11 + uVar6 * -0x40;
  do {
    uVar8 = *puVar7 >> 1 & 0x3ff;
    if (uVar8 != 0) {
      psVar1 = (short *)(DAT_10070d34 + -2 + uVar8 * 4);
      iVar5 = (int)*psVar1;
      if (iVar5 != 0) {
        uVar8 = uVar4;
        if ((*puVar7 & 1) != 0) {
          uVar8 = 0x3f - uVar4;
        }
        iVar5 = FUN_10020990(uVar8,uVar3,(char *)((int)psVar1 + iVar5));
        if ((iVar5 != -1) && (-1 < iVar5 + iVar9)) {
          return (iVar5 + iVar9) * 0x10000 + param_1;
        }
      }
    }
    puVar7 = puVar7 + -1;
    uVar6 = uVar6 - 1;
    iVar9 = iVar9 + 0x40;
    if ((int)uVar6 < 0) {
      return param_1 + 0x7fff0000;
    }
  } while( true );
}


