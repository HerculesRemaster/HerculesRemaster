
void __cdecl FUN_1002ed10(undefined4 *param_1,int param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  short sVar2;
  ushort uVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  ushort uVar7;
  int iVar8;
  ushort uVar9;
  undefined4 *puVar10;
  ushort uVar11;
  short sVar12;
  short *psVar13;
  short local_18;
  byte bStack_12;
  byte bStack_11;
  
  iVar5 = param_2;
  if (param_4 == 0) {
    local_18 = 0;
    param_2._0_2_ = 0;
    param_4._0_2_ = 0;
  }
  else {
    param_4._0_2_ = (short)*(undefined4 *)(param_2 + 0x48);
    local_18 = (short)*(undefined4 *)(param_2 + 0x38);
    param_2._0_2_ = (short)*(undefined4 *)(param_2 + 0x4c);
  }
  uVar3 = *(ushort *)(iVar5 + 0x2c);
  uVar4 = uVar3 & 7;
  uVar1 = *(undefined4 *)(DAT_10070d50 + (uint)(uVar3 >> 4) * 4);
  uVar11 = (ushort)uVar1 & 0xff;
  bStack_11 = (byte)((uint)uVar1 >> 0x18);
  uVar3 = (ushort)bStack_11;
  puVar10 = param_1 + 1;
  psVar13 = (short *)(DAT_10070f30 + 0x40);
  uVar6 = *param_1;
  uVar7 = (ushort)((uint)uVar1 >> 8) & 0xff;
  bStack_12 = (byte)((uint)uVar1 >> 0x10);
  uVar9 = (ushort)bStack_12;
  if (*(char *)(iVar5 + 0x24) != '\0') {
    do {
      sVar12 = (short)(char)uVar6;
      *psVar13 = sVar12 + (short)param_4;
      sVar2 = (short)((uint)uVar6 >> 8);
      psVar13[1] = sVar2 + (short)param_2;
      psVar13[2] = (char)((uint)uVar6 >> 0x18) + local_18;
      FUN_1002c690(psVar13);
      FUN_1002c6e0();
      iVar5 = ((int)sVar12 * ((int)(short)uVar11 - (int)(short)uVar7) >> 6) + (int)(short)uVar7;
      *(undefined4 *)(psVar13 + 2) =
           (&DAT_1006f4e8)
           [uVar4 * 0x40 +
            (((((((int)(short)uVar9 - (int)(short)uVar3) * (int)sVar12 >> 6) - iVar5) +
              (int)(short)uVar3) * (0x20 - sVar2) >> 6) + iVar5 >> 2)];
      FUN_1002c760(psVar13);
      uVar6 = *puVar10;
      psVar13 = psVar13 + 4;
      puVar10 = puVar10 + 1;
      param_3 = param_3 + -1;
    } while (param_3 != 0);
    return;
  }
  do {
    sVar12 = (short)(char)uVar6;
    *psVar13 = ((short)param_4 - sVar12) + 0x40;
    sVar2 = (short)((uint)uVar6 >> 8);
    psVar13[1] = sVar2 + (short)param_2;
    psVar13[2] = (char)((uint)uVar6 >> 0x18) + local_18;
    FUN_1002c690(psVar13);
    FUN_1002c6e0();
    iVar5 = (int)sVar12 * ((int)(short)uVar7 - (int)(short)uVar11);
    iVar8 = ((int)(iVar5 + (iVar5 >> 0x1f & 0x3fU)) >> 6) + (int)(short)uVar11;
    iVar5 = ((int)(short)uVar3 - (int)(short)uVar9) * (int)sVar12;
    iVar5 = ((((int)(iVar5 + (iVar5 >> 0x1f & 0x3fU)) >> 6) - iVar8) + (int)(short)uVar9) *
            (0x20 - sVar2);
    *(undefined4 *)(psVar13 + 2) =
         (&DAT_1006f4e8)
         [uVar4 * 0x40 + (((int)(iVar5 + (iVar5 >> 0x1f & 0x3fU)) >> 6) + iVar8 >> 2)];
    FUN_1002c760(psVar13);
    uVar6 = *puVar10;
    psVar13 = psVar13 + 4;
    puVar10 = puVar10 + 1;
    param_3 = param_3 + -1;
  } while (param_3 != 0);
  return;
}


