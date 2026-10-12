

undefined4 __cdecl FUN_10031540(int param_1,short *param_2,undefined4 param_3)



{

  undefined4 uVar1;

  undefined4 uVar2;

  ushort uVar3;

  int iVar4;

  uint uVar5;

  byte *pbVar6;

  undefined2 *puVar7;

  short *psVar8;

  uint uVar9;

  undefined4 *puVar10;

  undefined4 *puVar11;

  undefined2 *puVar12;

  undefined4 *puVar13;

  int iStack_30;

  undefined4 *puStack_2c;

  undefined4 *puStack_28;

  uint local_24;

  int iStack_20;

  uint uStack_1c;

  undefined **local_18;

  undefined4 uStack_14;

  uint local_10;

  int local_c;

  int local_8;

  int local_4;

  

  local_c = DAT_10071ec4 + 0x1384;

  DAT_10070c44 = 0;

  iVar4 = (int)*(short *)(param_1 + 0x10) >> 5;

  if (iVar4 < 1) {

    iVar4 = 1;

  }

  else if (0x1d < iVar4) {

    iVar4 = 0x1d;

  }

  local_24 = iVar4 * 0x400000 + 0x78300000;

  puVar7 = (undefined2 *)(DAT_10070d28 + *param_2 * 4);

  psVar8 = param_2 + (uint)*(byte *)(param_2 + 1) * 4 + 5;

  if (*(int *)(param_2 + 2) == 0) {

    FUN_1002c720(param_1 + 0x78);

  }

  else {

    FUN_10031d30(param_1,*(int *)(param_2 + 2));

  }

  local_8 = DAT_10070d30;

  *(byte *)(param_1 + 0x1f) = *(byte *)(param_1 + 0x1f) | 2;

  iVar4 = DAT_10070f30 + 0x18;

  local_10 = *(uint *)(param_1 + 0x54) & 0x40;

  puVar11 = DAT_10073484;

  if (*(int *)(DAT_10070f24 + 0x40) == 0) {

    local_18 = &PTR_FUN_1005cec0 + *(byte *)(param_1 + 0x23);

    do {

      local_4 = (int)*psVar8;

      (*(code *)*local_18)(param_1,psVar8 + 1,local_4);

      iStack_20 = CONCAT22(iStack_20._2_2_,*puVar7);

      uStack_1c = CONCAT22(uStack_1c._2_2_,puVar7[1]);

      puVar7 = puVar7 + -2;

      do {

        puVar12 = puVar7;

        uVar5 = *(uint *)(puVar12 + 4);

        puStack_28 = (undefined4 *)(iVar4 + (uVar5 & 0x3f) * 8);

        puVar13 = (undefined4 *)((uVar5 >> 3 & 0x1f8) + iVar4);

        puStack_2c = (undefined4 *)((uVar5 >> 9 & 0x1f8) + iVar4);

        uVar9 = uVar5 >> 0x18;

        puVar10 = (undefined4 *)((uVar5 >> 0xf & 0x1f8) + iVar4);

        if ((uVar9 & 8) == 0) {

LAB_100316e1:

          FUN_1002c670(*puStack_28,*puVar13,*puVar10);

          FUN_1002c6d0();

          FUN_1002c740(&iStack_30);

          if (-1 < iStack_30) {

            FUN_1002c780(puVar11);

            goto LAB_10031774;

          }

          if ((uVar9 & 0x40) != 0) {

            uStack_14 = *puVar10;

            uVar1 = *puVar13;

            FUN_1002c670(uVar1,*puStack_2c,uStack_14);

            FUN_1002c6d0();

            puVar11[2] = *puStack_28;

            puVar11[4] = uVar1;

            puVar11[6] = uStack_14;

            FUN_1002c740(&iStack_30);

            if (-1 < iStack_30) goto LAB_10031774;

          }

        }

        else {

          if ((uVar9 & 1) == 0) {

            if (((uVar9 & 4) == 0) || (DAT_1006ffbc != 0)) goto LAB_100316e1;

            goto LAB_10031828;

          }

          puVar11[2] = *puStack_28;

          puVar11[4] = *puVar13;

          puVar11[6] = *puVar10;

LAB_10031774:

          puVar11[8] = *puStack_2c;

          if (local_10 == 0) {

            uVar5 = (int)(puStack_2c[1] + puStack_28[1]) >> 3;

          }

          else {

            uVar5 = (int)(puVar10[1] + puVar13[1] + puStack_2c[1] + puStack_28[1]) >> 4;

          }

          if (0x4bf < uVar5) {

            uVar5 = 0x4bf;

          }

          puVar10 = (undefined4 *)(local_c + uVar5 * -4);

          *puVar11 = *puVar10;

          *puVar10 = puVar11;

          puVar11[1] = param_3;

          uVar5 = *(uint *)(puVar12 + 6);

          if ((uVar5 & 0xff000000) == 0) {

            uVar1 = *(undefined4 *)(uVar5 + 4 + local_8);

            uVar2 = *(undefined4 *)(uVar5 + 8 + local_8);

            uVar5 = *(uint *)(uVar5 + local_8);

            *(short *)(puVar11 + 7) = (short)uVar2;

            puVar11[5] = uVar1;

            puVar11[3] = uVar5 | local_24;

            *(short *)(puVar11 + 9) = (short)((uint)uVar2 >> 0x10);

          }

          else {

            uVar3 = CONCAT11(0xff,(char)uVar5);

            puVar11[3] = uVar3 | local_24;

            puVar11[5] = uVar3 | 0x9c0000;

            *(ushort *)(puVar11 + 7) = uVar3;

            *(ushort *)(puVar11 + 9) = uVar3;

          }

          puVar11 = puVar11 + 10;

        }

LAB_10031828:

        iStack_20 = iStack_20 + 0xffff;

        puVar7 = puVar12 + 4;

      } while ((short)iStack_20 != 0);

      if ((short)uStack_1c == 0) {

        DAT_10073484 = puVar11;

        return 1;

      }

      psVar8 = psVar8 + local_4 * 2 + 2;

      pbVar6 = (byte *)(puVar12 + 8);

      if ((uStack_1c & 0xff00) != 0) {

        uVar5 = uStack_1c >> 8 & 0xff;

        do {

          puVar10 = (undefined4 *)(iVar4 + (*pbVar6 & 0x3f) * 8);

          *DAT_10070c40 = *puVar10;

          DAT_10070c40[1] = puVar10[1];

          DAT_10070c40 = DAT_10070c40 + 2;

          pbVar6 = pbVar6 + 1;

          uVar5 = uVar5 - 1;

        } while (uVar5 != 0);

      }

      puVar7 = (undefined2 *)((uint)(pbVar6 + 3) & 0xfffffffc);

    } while ((char)uStack_1c != '\0');

  }

  DAT_10073484 = puVar11;

  return 1;

}




