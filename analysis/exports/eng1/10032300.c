
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl FUN_10032300(undefined4 *param_1)

{
  uint uVar1;
  ushort uVar2;
  ushort uVar3;
  short *psVar4;
  short sVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  int *piVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  int iVar14;
  int iVar15;
  short local_24;
  short local_20;
  int local_1c;
  uint local_14;
  undefined2 local_10;
  undefined4 local_c;
  uint local_8;
  int local_4;
  
  iVar6 = DAT_10070d58;
  sVar5 = (short)((uint)param_1[2] >> 0x10);
  iVar15 = sVar5 - DAT_10070ca8;
  if (iVar15 < 0x20) {
    return -1;
  }
  local_20 = sVar5 - (short)DAT_10070d1c;
  iVar11 = sVar5 + 0x40 >> 7;
  if (iVar11 < 0) {
    iVar11 = 0;
  }
  else if (DAT_10070ece + -1 < iVar11) {
    iVar11 = DAT_10070ece + -1;
  }
  uVar13 = (int)(short)((uint)param_1[1] >> 0x10) >> 6;
  uVar8 = ((int)(short)((uint)*param_1 >> 0x10) >> 6) - (int)(short)(&DAT_10070e2c)[iVar11];
  if ((int)(short)(&DAT_10070e0c)[iVar11] - 1U < uVar8) {
    uVar8 = (int)(short)(&DAT_10070e0c)[iVar11] - 1U;
  }
  uVar1 = (int)DAT_10070d14 - 1;
  if (uVar1 < uVar13) {
    uVar13 = uVar1;
  }
  iVar11 = (uint)*(ushort *)
                  (DAT_10070d54 +
                  (*(int *)((&DAT_10070dcc)[iVar11] + ((int)DAT_10070d14 * uVar8 + uVar13) * 4) >>
                   0xb & 0x1fffU) * 4) * 0x10 + DAT_10070d58;
  uVar2 = *(ushort *)(iVar11 + 0xc);
  *(undefined2 *)(param_1 + 0xb) = 0;
  uVar8 = DAT_10070c70;
  uVar3 = *(ushort *)(iVar11 + 0xe);
  piVar9 = (int *)((uint)uVar2 * 0x10 + iVar6);
  uVar13 = uVar3 & 0x3fff;
  uVar3 = uVar3 >> 0xe;
  local_c = uVar13;
  if (uVar3 == 0) {
    iVar6 = (piVar9[2] & 0xfffU) + DAT_10070c70;
    local_24 = -(short)iVar6;
    if (DAT_10070c4c != piVar9) {
      iVar14 = (int)*(short *)(DAT_10070f34 + (DAT_10070c70 & 0xfff) * 2);
      iVar12 = (int)*(short *)(DAT_10070f34 + (DAT_10070c70 - 0x400 & 0xfff) * 2);
      iVar10 = piVar9[1] - DAT_10070c68 >> 6;
      iVar11 = *piVar9 - DAT_10070c60 >> 6;
      iVar7 = iVar11 * iVar14 - iVar10 * iVar12;
      _DAT_1005cea4 = CONCAT31((int3)(iVar7 >> 0x19),(char)(iVar7 >> 0x11)) & 0xfffffffe;
      _DAT_1005cea8 = 0;
      local_8 = local_8 & 0xffff0000;
      local_c = CONCAT22(local_24,DAT_10070c6c);
      _DAT_1005ceac = ((iVar15 - local_20) + (iVar10 * iVar14 + iVar11 * iVar12 >> 0x12)) * 2;
      DAT_10070c4c = piVar9;
      FUN_1002c920(&local_c,&DAT_1005ce90);
      FUN_1002c940(&DAT_1005ce90,&DAT_1005ceb0);
    }
    FUN_1002c700(&DAT_1005ce90);
    FUN_1002c710(&DAT_1005ce90);
    psVar4 = DAT_10070f30;
    *DAT_10070f30 = (short)((uint)*param_1 >> 0x10) - (short)(uVar13 << 6);
    psVar4[1] = (short)((uint)param_1[1] >> 0x10) - (short)DAT_10070c64;
    psVar4[2] = local_20;
    FUN_1002c690(psVar4);
    FUN_1002c6e0();
    return iVar6 * -2;
  }
  iVar6 = ((uint)piVar9[2] >> 0x18) + DAT_10070c70;
  if (DAT_10070c48 != piVar9) {
    iVar7 = piVar9[1] - DAT_10070c68 >> 6;
    iVar12 = (int)*(short *)(DAT_10070f34 + (DAT_10070c70 - 0x400 & 0xfff) * 2);
    iVar11 = *piVar9 - DAT_10070c60 >> 6;
    iVar10 = (int)*(short *)(DAT_10070f34 + (DAT_10070c70 & 0xfff) * 2);
    if (uVar3 == 1) {
      uVar13 = iVar6 + 0x400;
    }
    else {
      uVar13 = iVar6 - 0x400;
    }
    DAT_1005ce84 = (iVar11 * iVar10 - iVar7 * iVar12 >> 6) +
                   *(short *)(DAT_10070f34 + (uVar13 & 0xfff) * 2) * 0x28c >> 0xb & 0xfffffffe;
    DAT_1005ce88 = 0;
    local_10 = 0;
    local_14 = (uint)DAT_10070c6c;
    DAT_1005ce8c = ((iVar11 * iVar12 + iVar7 * iVar10 >> 6) +
                    *(short *)(DAT_10070f34 + (uVar13 - 0x400 & 0xfff) * 2) * 0x28c >> 0xb &
                   0xfffffffeU) + (iVar15 - local_20) * 2;
    DAT_10070c48 = piVar9;
    FUN_1002c920(&local_14,&DAT_1005ce70);
    FUN_1002c940(&DAT_1005ce70,&DAT_1005ceb0);
  }
  uVar13 = local_c;
  FUN_1002c700(&DAT_1005ce70);
  sVar5 = (short)((uint)*param_1 >> 0x10);
  if (uVar3 == 1) {
    iVar6 = (uVar13 * 0x40 - (int)sVar5) - iVar6;
    iVar15 = local_20 + -0x28c;
    local_1c = iVar6 + -0x400;
  }
  else {
    iVar6 = ((int)sVar5 + uVar13 * -0x40) - iVar6;
    iVar15 = local_20 + 0x28c;
    local_1c = iVar6 + 0x400;
  }
  if ((param_1[0x15] & 0x1000) == 0) {
    FUN_1002c710(&DAT_1005ce70);
  }
  else {
    iVar11 = (int)*(short *)(DAT_10070f34 + (uVar8 - 0x400 & 0xfff) * 2);
    iVar7 = (int)*(short *)(DAT_10070f34 + (uVar8 & 0xfff) * 2);
    local_c = (*(short *)(param_1 + 5) * iVar7 - *(short *)(param_1 + 6) * iVar11 >> 0xc) +
              DAT_1005ce84;
    local_8 = *(short *)((int)param_1 + 0x16) + DAT_1005ce88;
    local_4 = (*(short *)(param_1 + 5) * iVar11 + *(short *)(param_1 + 6) * iVar7 >> 0xc) +
              DAT_1005ce8c;
    FUN_1002c720(&local_c);
  }
  psVar4 = DAT_10070f30;
  *DAT_10070f30 = (short)(*(short *)(DAT_10070f34 + (-local_1c & 0xfffU) * 2) * iVar15 >> 0xc);
  psVar4[2] = (short)(*(short *)(DAT_10070f34 + (-local_1c - 0x400U & 0xfff) * 2) * iVar15 >> 0xc);
  psVar4[1] = (short)((uint)param_1[1] >> 0x10) - (short)DAT_10070c64;
  FUN_1002c690(psVar4);
  FUN_1002c6e0();
  return iVar6 * 2;
}


