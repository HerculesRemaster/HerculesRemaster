
void FUN_10026040(void)

{
  int iVar1;
  short *psVar2;
  short sVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  int *piVar7;
  ushort uVar8;
  ushort *puVar9;
  uint *puVar10;
  int iVar11;
  int local_14;
  short local_10;
  int *local_c;
  int *local_8;
  
  iVar5 = DAT_10071398;
  local_10 = 0;
  local_c._0_2_ = 0;
  if (DAT_10071398 != 0) {
    if (DAT_100713ae == 0) {
      uVar8 = (ushort)DAT_10070ed0;
      local_14 = 0;
      DAT_100713ae = uVar8;
      if (uVar8 != 0) {
        puVar10 = &DAT_1006feec;
        local_8 = &DAT_10070d5c;
        local_10 = 0;
        local_c._0_2_ = 0;
        do {
          iVar11 = *local_8;
          puVar10[-1] = 0;
          uVar6 = (uint)*(ushort *)(iVar11 + 8);
          *puVar10 = uVar6;
          if ((uVar6 != 0) &&
             (piVar7 = (int *)FUN_1002c3c0(puVar10 + -1,(uVar6 + 1) * 0x14,0), uVar8 = DAT_100713ae,
             piVar7 != (int *)0x0)) {
            iVar11 = 0;
            puVar9 = (ushort *)(*(short *)(*local_8 + 10) + 10 + *local_8);
            if (0 < (int)*puVar10) {
              do {
                iVar1 = iVar5 + (uint)puVar9[3] * 10;
                piVar7[5] = ((uint)*puVar9 + (int)*(short *)(iVar5 + (uint)puVar9[3] * 10)) *
                            0x10000;
                piVar7[6] = ((uint)puVar9[1] + (int)*(short *)(iVar1 + 2)) * 0x10000;
                piVar7[7] = ((uint)puVar9[2] + (int)*(short *)(iVar1 + 4)) * 0x10000;
                *(undefined2 *)(piVar7 + 8) = *(undefined2 *)(iVar1 + 6);
                *(undefined2 *)((int)piVar7 + 0x22) = 0;
                sVar3 = *(short *)(iVar1 + 8);
                *(short *)(piVar7 + 9) = sVar3;
                if (sVar3 == 0x2018) {
                  local_c._0_2_ = (short)local_c + 1;
                }
                if ((short)piVar7[9] == 0x2019) {
                  local_10 = local_10 + 1;
                }
                puVar9 = puVar9 + 4;
                iVar11 = iVar11 + 1;
                uVar8 = DAT_100713ae;
                piVar7 = piVar7 + 5;
              } while (iVar11 < (int)*puVar10);
            }
          }
          local_14 = local_14 + 1;
          local_8 = local_8 + 1;
          puVar10 = puVar10 + 2;
        } while (local_14 < (int)(uint)uVar8);
      }
      *(short *)(DAT_10070f24 + 0x16) = local_10;
      *(short *)(DAT_10070f24 + 0x1a) = (short)local_c;
      return;
    }
    local_14 = 0;
    if (DAT_100713ae != 0) {
      piVar7 = &DAT_1006feec;
      local_c = &DAT_10070d5c;
      uVar8 = DAT_100713ae;
      do {
        if ((int *)piVar7[-1] != (int *)0x0) {
          iVar11 = 0;
          puVar9 = (ushort *)(*(short *)(*local_c + 10) + 10 + *local_c);
          piVar4 = (int *)piVar7[-1];
          if (0 < *piVar7) {
            do {
              psVar2 = (short *)(iVar5 + (uint)puVar9[3] * 10);
              if ((*(byte *)(iVar5 + 7 + (uint)puVar9[3] * 10) & 2) != 0) {
                piVar4[5] = ((uint)*puVar9 + (int)*psVar2) * 0x10000;
                piVar4[6] = ((uint)puVar9[1] + (int)psVar2[1]) * 0x10000;
                piVar4[7] = ((uint)puVar9[2] + (int)psVar2[2]) * 0x10000;
                *(short *)(piVar4 + 8) = psVar2[3];
                *(undefined2 *)((int)piVar4 + 0x22) = 0;
                *(short *)(piVar4 + 9) = psVar2[4];
              }
              puVar9 = puVar9 + 4;
              iVar11 = iVar11 + 1;
              uVar8 = DAT_100713ae;
              piVar4 = piVar4 + 5;
            } while (iVar11 < *piVar7);
          }
        }
        local_14 = local_14 + 1;
        local_c = local_c + 1;
        piVar7 = piVar7 + 2;
      } while (local_14 < (int)(uint)uVar8);
    }
  }
  return;
}


