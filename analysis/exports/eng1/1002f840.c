
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1002f840(void)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  int local_10;
  
  iVar6 = 0;
  iVar3 = (int)(short)((uint)DAT_100734f4[1] >> 0x10) >> 6;
  iVar1 = (short)((uint)DAT_100734f4[2] >> 0x10) + 0x40 >> 7;
  if (iVar1 < 0) {
    iVar1 = 0;
  }
  else if (DAT_10070ece + -1 < iVar1) {
    iVar1 = DAT_10070ece + -1;
  }
  iVar5 = ((int)DAT_10070ca0 >> 6) - (int)(short)(&DAT_10070e2c)[iVar1];
  if (iVar5 < 0) {
    iVar5 = 0;
  }
  else if ((short)(&DAT_10070e0c)[iVar1] + -1 < iVar5) {
    iVar5 = (short)(&DAT_10070e0c)[iVar1] + -1;
  }
  if (iVar3 < 0) {
    iVar3 = 0;
  }
  else if (DAT_10070d14 + -1 < iVar3) {
    iVar3 = DAT_10070d14 + -1;
  }
  piVar4 = (int *)((uint)*(ushort *)
                          (DAT_10070d54 +
                          (*(int *)((&DAT_10070dcc)[iVar1] + (iVar5 * DAT_10070d14 + iVar3) * 4) >>
                           0xb & 0x1fffU) * 4) * 0x10 + DAT_10070d58);
  uVar2 = piVar4[2] & 0xfff;
  uVar9 = (uint)piVar4[2] >> 0x18;
  if (DAT_10070d00 != '\0') {
    iVar6 = 0x20;
  }
  uVar8 = DAT_10070ca0 & 0x3f;
  iVar3 = *piVar4;
  local_10 = piVar4[1];
  uVar9 = ((int)(((int)((uVar2 + uVar9 * -0x10) * 0x100000) >> 0x14) * uVar8) >> 6) + iVar6 +
          uVar9 * 0x10 & 0xfff;
  if ((*(ushort *)((int)piVar4 + 0xe) & 0xc000) == 0) {
    iVar1 = ((iVar5 + (short)(&DAT_10070e2c)[iVar1]) - (*(ushort *)((int)piVar4 + 0xe) & 0x3fff)) *
            0x40;
    iVar3 = iVar3 + *(short *)(DAT_10070f34 + uVar2 * 2) * iVar1;
    local_10 = local_10 + *(short *)(DAT_10070f34 + (uVar2 - 0x400 & 0xfff) * 2) * iVar1;
  }
  iVar10 = (int)*(short *)(DAT_10070f34 + uVar9 * 2);
  iVar5 = (int)*(short *)(DAT_10070f34 + (uVar9 - 0x400 & 0xfff) * 2);
  iVar1 = (DAT_100734f4[2] >> 0x10) - DAT_10070d1c;
  iVar6 = (0x1000 - *(short *)(DAT_10070f34 + (DAT_10070cd4 & 0xfff) * 2)) * iVar1 >> 6;
  iVar1 = *(short *)(DAT_10070f34 + (DAT_10070cd4 + 0x400 & 0xfff) * 2) * iVar1 >> 6;
  if ((DAT_10070d10 & 0x10) != 0) {
    iVar7 = DAT_10070ca0 - (*DAT_100734f4 >> 0x10);
    iVar6 = iVar6 - (*(short *)(DAT_10070f34 + (DAT_10070cd4 - 0x400 & 0xfff) * 2) * iVar7 >> 6);
    iVar1 = iVar1 - ((0x1000 - *(short *)(DAT_10070f34 + (DAT_10070cd4 & 0xfff) * 2)) * iVar7 >> 6);
  }
  DAT_10070c60 = ((DAT_10070cb8 >> 10) * iVar10 - (DAT_10070cc0 >> 10) * iVar5 >> 6) +
                 iVar3 + iVar10 * uVar8 + (iVar1 * iVar10 - iVar6 * iVar5 >> 6);
  DAT_10070c68 = local_10 + iVar5 * uVar8 + (iVar1 * iVar5 + iVar6 * iVar10 >> 6) +
                 ((DAT_10070cb8 >> 10) * iVar5 + (DAT_10070cc0 >> 10) * iVar10 >> 6);
  DAT_10070c64 = (short)((uint)DAT_10070cbc >> 0x10) + DAT_10070ca4;
  if (uVar9 == 1) {
    uVar9 = 0;
  }
  _DAT_10070c6c = DAT_10070cd0;
  DAT_10070c70 = DAT_10070cd4 - uVar9;
  _DAT_10070c7c = (DAT_10070ca4 + -0x20 >> 6) + 2;
  _DAT_10070c78 = (int)(DAT_10070ca0 - 0x20) >> 6;
  FUN_1002c9d0(DAT_10070cdc,DAT_10070ce0);
  FUN_1002c9f0(DAT_10070ce4);
  return;
}


