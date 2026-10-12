
undefined4 __cdecl FUN_1002df20(int param_1)

{
  uint uVar1;
  uint uVar2;
  bool bVar3;
  byte bVar4;
  ushort uVar5;
  uint uVar6;
  uint *puVar7;
  uint uVar8;
  undefined4 *puVar9;
  int iVar10;
  int iVar11;
  undefined4 *puVar12;
  int iVar13;
  undefined4 *puVar14;
  uint local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  undefined4 *local_8;
  undefined4 *local_4;
  
  *(byte *)(param_1 + 0x24) = *(byte *)(param_1 + 0x20) & 1;
  local_1c = 0;
  uVar6 = *(uint *)(param_1 + 0x20) >> 1 & 0x3ff;
  iVar11 = (int)*(short *)(DAT_10070d34 + -4 + uVar6 * 4) + uVar6;
  iVar10 = DAT_10070d34 + -4 + iVar11 * 4;
  local_c = (int)*(short *)(DAT_10070d34 + -4 + iVar11 * 4);
  uVar6 = (uint)*(byte *)(iVar10 + 2);
  puVar12 = DAT_10073484;
  if (uVar6 != 0) {
    local_20 = (uint)*(byte *)(iVar10 + 3);
    puVar9 = (undefined4 *)(iVar10 + 4);
    if (*(int *)(DAT_10070f24 + 0x40) == 0) {
      uVar8 = *(uint *)(param_1 + 0x20) >> 0xb & 0x1fff;
      uVar5 = *(ushort *)(DAT_10070d54 + uVar8 * 4);
      iVar11 = DAT_10070d54 + uVar8 * 4;
      *(ushort *)(param_1 + 0x2a) = uVar5;
      iVar13 = *(int *)(param_1 + 4) + (uint)uVar5 * 0x10;
      *(int *)(param_1 + 8) = iVar13;
      bVar4 = (byte)((ushort)*(undefined2 *)(iVar13 + 0xe) >> 0xe);
      *(byte *)(param_1 + 0x25) = bVar4;
      *(undefined2 *)(param_1 + 0x2c) = *(undefined2 *)(iVar11 + 2);
      if (bVar4 == 0) {
        iVar11 = FUN_1002e3b0(param_1);
        if (iVar11 == -1) {
          return 0;
        }
        if ((*(byte *)(param_1 + 0x2c) & 8) == 0) {
          FUN_1002ed10(puVar9,param_1,uVar6,iVar11);
          local_1c = 1;
        }
        else if (iVar11 == 0) {
          FUN_1002e600(puVar9,param_1,uVar6);
        }
        else {
          FUN_1002e6c0(puVar9,param_1,uVar6);
        }
      }
      else if (local_c == 1) {
        FUN_1002ef60(param_1);
        FUN_1002f120(puVar9,param_1,uVar6);
        *(char *)(param_1 + 0x15) = *(char *)(param_1 + 0x15) + '\x01';
      }
      else {
        iVar11 = FUN_1002e7a0(param_1);
        if (iVar11 == -1) {
          return 0;
        }
        if ((*(byte *)(param_1 + 0x2c) & 8) == 0) {
          FUN_1002f340(puVar9,param_1,uVar6);
          local_1c = 1;
        }
        else {
          FUN_1002ea50(puVar9,param_1,uVar6);
        }
      }
      iVar13 = *(short *)(param_1 + 0x10) + -0x40;
      iVar10 = iVar10 + uVar6 * 4;
      local_10 = DAT_10071ec4 + (0x521 - *(short *)(param_1 + 0x10)) * 4;
      local_c = DAT_10070d30;
      iVar11 = DAT_10070f30 + 0x40;
      uVar6 = iVar13 >> (*(byte *)(param_1 + 0x26) & 0x1f);
      bVar3 = (*(byte *)(param_1 + 0x26) & 0x1f) != 0;
      if ((bVar3 || -1 < iVar13) && (!bVar3 || -1 < (int)uVar6)) {
        if ((int)(uint)*(byte *)(param_1 + 0x27) < (int)uVar6) {
          uVar6 = (uint)*(byte *)(param_1 + 0x27);
        }
      }
      else {
        uVar6 = 0;
      }
      uVar6 = uVar6 * 0x400000 + 0x78300000;
      puVar12 = DAT_10073484;
      local_18 = iVar11;
      if (local_1c == 0) {
        local_8 = (undefined4 *)
                  ((&DAT_1006f4e8)
                   [((*(ushort *)(param_1 + 0x2c) & 0x3fc0) >> 6) +
                    (*(ushort *)(param_1 + 0x2c) & 7) * 0x40] | 0x2c000000);
        do {
          uVar8 = *(uint *)(iVar10 + 8);
          FUN_1002c670(*(undefined4 *)(iVar11 + (uVar8 & 0x3f) * 8),
                       *(undefined4 *)(((int)uVar8 >> 3 & 0x1f8U) + iVar11),
                       *(undefined4 *)(((int)uVar8 >> 9 & 0x1f8U) + iVar11));
          FUN_1002c6d0();
          local_4 = *(undefined4 **)(((int)uVar8 >> 0xf & 0x1f8U) + iVar11);
          FUN_1002c740(&local_14);
          if (*(char *)(param_1 + 0x24) == '\0') {
            if (-1 < local_14) {
LAB_1002e2f7:
              FUN_1002c770(puVar12);
              puVar12[8] = local_4;
              puVar12[1] = ((int)uVar8 >> 0x18 & 0x20U) << 0x14 | (uint)local_8;
              uVar1 = *(uint *)(iVar10 + 0xc);
              puVar9 = (undefined4 *)(local_10 - (uVar1 >> 0x16 & 0x3fc));
              *puVar12 = *puVar9;
              *puVar9 = puVar12;
              if (((int)uVar8 >> 0x18 & 0x80U) == 0) {
                puVar7 = (uint *)((uVar1 & 0xffff) + local_c);
                uVar8 = puVar7[1];
                uVar1 = puVar7[2];
                uVar2 = *puVar7;
                *(short *)(puVar12 + 7) = (short)uVar1;
                puVar12[5] = uVar8;
                puVar12[3] = uVar2 | uVar6;
                *(short *)(puVar12 + 9) = (short)(uVar1 >> 0x10);
              }
              else {
                uVar5 = CONCAT11(0xff,(char)uVar1);
                *(ushort *)(puVar12 + 7) = uVar5;
                *(ushort *)(puVar12 + 9) = uVar5;
                puVar12[3] = uVar6 | uVar5;
                puVar12[5] = uVar5 | 0x9c0000;
              }
              puVar12 = puVar12 + 10;
            }
          }
          else if (local_14 < 1) goto LAB_1002e2f7;
          local_20 = local_20 - 1;
          iVar10 = iVar10 + 8;
        } while (local_20 != 0);
      }
      else {
        local_14 = iVar10;
        if (local_1c == 1) {
          do {
            uVar8 = *(uint *)(local_14 + 8);
            local_14 = local_14 + 8;
            local_8 = (undefined4 *)(local_18 + (uVar8 & 0x3f) * 8);
            local_4 = (undefined4 *)(((int)uVar8 >> 9 & 0x1f8U) + local_18);
            puVar9 = (undefined4 *)(((int)uVar8 >> 3 & 0x1f8U) + local_18);
            FUN_1002c670(*local_8,*puVar9,*local_4);
            FUN_1002c6d0();
            puVar14 = (undefined4 *)(((int)uVar8 >> 0xf & 0x1f8U) + local_18);
            FUN_1002c740(&local_1c);
            if (*(char *)(param_1 + 0x24) == '\0') {
              if (-1 < local_1c) {
LAB_1002e17d:
                FUN_1002c790(puVar12);
                puVar12[0xb] = *puVar14;
                puVar12[1] = local_8[1] | ((int)((int)uVar8 >> 0x18 & 0x20U | 0x3c0) >> 4) << 0x18;
                puVar12[4] = puVar9[1];
                puVar12[7] = local_4[1];
                puVar12[10] = puVar14[1];
                uVar1 = *(uint *)(local_14 + 4);
                puVar9 = (undefined4 *)(local_10 - (uVar1 >> 0x16 & 0x3fc));
                *puVar12 = *puVar9;
                *puVar9 = puVar12;
                if (((int)uVar8 >> 0x18 & 0x80U) == 0) {
                  puVar7 = (uint *)((uVar1 & 0xffff) + local_c);
                  uVar8 = puVar7[1];
                  uVar1 = puVar7[2];
                  uVar2 = *puVar7;
                  *(short *)(puVar12 + 9) = (short)uVar1;
                  puVar12[6] = uVar8;
                  puVar12[3] = uVar2 | uVar6;
                  *(short *)(puVar12 + 0xc) = (short)(uVar1 >> 0x10);
                }
                else {
                  uVar5 = CONCAT11(0xff,(char)uVar1);
                  *(ushort *)(puVar12 + 9) = uVar5;
                  *(ushort *)(puVar12 + 0xc) = uVar5;
                  puVar12[3] = uVar6 | uVar5;
                  puVar12[6] = uVar5 | 0x9c0000;
                }
                puVar12 = puVar12 + 0xd;
              }
            }
            else if (local_1c < 1) goto LAB_1002e17d;
            local_20 = local_20 - 1;
            if (local_20 == 0) {
              DAT_10073484 = puVar12;
              return 0;
            }
          } while( true );
        }
      }
    }
  }
  DAT_10073484 = puVar12;
  return 0;
}


