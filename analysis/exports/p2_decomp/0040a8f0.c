
undefined4 FUN_0040a8f0(short *param_1,uint param_2,uint param_3)

{
  short sVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  undefined4 *puVar8;
  int iVar9;
  undefined4 *puVar10;
  
  sVar1 = param_1[1];
  if (sVar1 < 0x200) {
    uVar4 = (uint)sVar1;
    if ((int)uVar4 < (int)((int)param_1[3] + uVar4)) {
      do {
        iVar5 = ((((int)uVar4 < 0x100) - 1 & 0x800) + (uVar4 & 0xff)) * 0x100;
        uVar6 = (uint)param_1[2];
        uVar2 = (uint)*param_1;
        iVar9 = ((((int)param_3 < 0x100) - 1 & 0x800) + (param_3 & 0xff)) * 0x100;
        if (((((uVar2 + uVar6) - 1 ^ uVar2) & 0xffffff80) == 0) &&
           ((((param_2 - 1) + uVar6 ^ param_2) & 0xffffff80) == 0)) {
          puVar8 = (undefined4 *)
                   (&DAT_0096e6e0 + iVar5 + (uVar2 * 2 & 0x700) * 0x100 + (uVar2 * 2 & 0xff));
          puVar10 = (undefined4 *)
                    (&DAT_0096e6e0 + iVar9 + (param_2 * 2 & 0x700) * 0x100 + (param_2 * 2 & 0xff));
          for (uVar3 = (uVar6 & 0x7fffffff) >> 1; uVar3 != 0; uVar3 = uVar3 - 1) {
            *puVar10 = *puVar8;
            puVar8 = puVar8 + 1;
            puVar10 = puVar10 + 1;
          }
          for (uVar2 = uVar6 * 2 & 3; uVar2 != 0; uVar2 = uVar2 - 1) {
            *(undefined1 *)puVar10 = *(undefined1 *)puVar8;
            puVar8 = (undefined4 *)((int)puVar8 + 1);
            puVar10 = (undefined4 *)((int)puVar10 + 1);
          }
        }
        else {
          uVar3 = uVar2 * 2;
          uVar7 = param_2 * 2;
          if ((int)uVar3 < (int)((uVar2 + uVar6) * 2)) {
            do {
              uVar6 = uVar3 & 0x700;
              uVar2 = uVar3 & 0xff;
              uVar3 = uVar3 + 1;
              (&DAT_0096e6e0)[iVar9 + (uVar7 & 0x700) * 0x100 + (uVar7 & 0xff)] =
                   (&DAT_0096e6e0)[iVar5 + uVar6 * 0x100 + uVar2];
              uVar7 = uVar7 + 1;
            } while ((int)uVar3 < ((int)*param_1 + (int)param_1[2]) * 2);
          }
        }
        uVar4 = uVar4 + 1;
        param_3 = param_3 + 1;
      } while ((int)uVar4 < (int)param_1[1] + (int)param_1[3]);
    }
  }
  else {
    uVar4 = (uint)sVar1;
    if ((int)uVar4 < (int)((int)param_1[3] + uVar4)) {
      do {
        uVar6 = (uint)param_1[2];
        uVar2 = (uint)*param_1;
        iVar5 = ((uVar4 & 0x1ff) + 0x1000) * 0x100;
        iVar9 = ((param_3 & 0x1ff) + 0x1000) * 0x100;
        if (((((uVar2 + uVar6) - 1 ^ uVar2) & 0xffffff80) == 0) &&
           ((((param_2 - 1) + uVar6 ^ param_2) & 0xffffff80) == 0)) {
          puVar8 = (undefined4 *)
                   (&DAT_0096e6e0 + iVar5 + (uVar2 * 2 & 0x700) * 0x100 + (uVar2 * 2 & 0xff));
          puVar10 = (undefined4 *)
                    (&DAT_0096e6e0 + iVar9 + (param_2 * 2 & 0x700) * 0x100 + (param_2 * 2 & 0xff));
          for (uVar3 = (uVar6 & 0x7fffffff) >> 1; uVar3 != 0; uVar3 = uVar3 - 1) {
            *puVar10 = *puVar8;
            puVar8 = puVar8 + 1;
            puVar10 = puVar10 + 1;
          }
          for (uVar2 = uVar6 * 2 & 3; uVar2 != 0; uVar2 = uVar2 - 1) {
            *(undefined1 *)puVar10 = *(undefined1 *)puVar8;
            puVar8 = (undefined4 *)((int)puVar8 + 1);
            puVar10 = (undefined4 *)((int)puVar10 + 1);
          }
        }
        else {
          uVar3 = uVar2 * 2;
          uVar7 = param_2 * 2;
          if ((int)uVar3 < (int)((uVar2 + uVar6) * 2)) {
            do {
              (&DAT_0096e6e0)[iVar9 + (uVar7 & 0x700) * 0x100 + (uVar7 & 0xff)] =
                   (&DAT_0096e6e0)[iVar5 + (uVar3 & 0x700) * 0x100 + (uVar3 & 0xff)];
              uVar3 = uVar3 + 1;
              uVar7 = uVar7 + 1;
            } while ((int)uVar3 < ((int)*param_1 + (int)param_1[2]) * 2);
          }
        }
        uVar4 = uVar4 + 1;
        param_3 = param_3 + 1;
      } while ((int)uVar4 < (int)param_1[1] + (int)param_1[3]);
      FUN_0040bd80();
      return 0;
    }
  }
  FUN_0040bd80();
  return 0;
}

