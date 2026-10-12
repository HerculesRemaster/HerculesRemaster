
void FUN_0040a710(short *param_1,undefined4 *param_2)

{
  undefined1 uVar1;
  short sVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  
  sVar2 = param_1[1];
  if (sVar2 < 0x200) {
    uVar5 = (uint)sVar2;
    if ((int)uVar5 < (int)((int)param_1[3] + uVar5)) {
      do {
        uVar7 = (uint)param_1[2];
        uVar3 = (uint)*param_1;
        iVar6 = ((((int)uVar5 < 0x100) - 1 & 0x800) + (uVar5 & 0xff)) * 0x100;
        if ((((uVar3 + uVar7) - 1 ^ uVar3) & 0xffffff80) == 0) {
          puVar8 = param_2;
          puVar9 = (undefined4 *)
                   (&DAT_0096e6e0 + iVar6 + (uVar3 * 2 & 0x700) * 0x100 + (uVar3 * 2 & 0xff));
          for (uVar4 = (uVar7 & 0x7fffffff) >> 1; uVar4 != 0; uVar4 = uVar4 - 1) {
            *puVar9 = *puVar8;
            puVar8 = puVar8 + 1;
            puVar9 = puVar9 + 1;
          }
          for (uVar3 = uVar7 * 2 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
            *(undefined1 *)puVar9 = *(undefined1 *)puVar8;
            puVar8 = (undefined4 *)((int)puVar8 + 1);
            puVar9 = (undefined4 *)((int)puVar9 + 1);
          }
          param_2 = (undefined4 *)((int)param_2 + param_1[2] * 2);
        }
        else {
          uVar4 = uVar3 * 2;
          if ((int)uVar4 < (int)((uVar3 + uVar7) * 2)) {
            do {
              uVar1 = *(undefined1 *)param_2;
              param_2 = (undefined4 *)((int)param_2 + 1);
              (&DAT_0096e6e0)[iVar6 + (uVar4 & 0x700) * 0x100 + (uVar4 & 0xff)] = uVar1;
              uVar4 = uVar4 + 1;
            } while ((int)uVar4 < ((int)*param_1 + (int)param_1[2]) * 2);
          }
        }
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)param_1[1] + (int)param_1[3]);
    }
  }
  else {
    uVar5 = (uint)sVar2;
    if ((int)uVar5 < (int)((int)param_1[3] + uVar5)) {
      do {
        uVar7 = (uint)param_1[2];
        uVar3 = (uint)*param_1;
        iVar6 = ((uVar5 & 0x1ff) + 0x1000) * 0x100;
        if ((((uVar3 + uVar7) - 1 ^ uVar3) & 0xffffff80) == 0) {
          puVar8 = param_2;
          puVar9 = (undefined4 *)
                   (&DAT_0096e6e0 + iVar6 + (uVar3 * 2 & 0x700) * 0x100 + (uVar3 * 2 & 0xff));
          for (uVar4 = (uVar7 & 0x7fffffff) >> 1; uVar4 != 0; uVar4 = uVar4 - 1) {
            *puVar9 = *puVar8;
            puVar8 = puVar8 + 1;
            puVar9 = puVar9 + 1;
          }
          for (uVar3 = uVar7 * 2 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
            *(undefined1 *)puVar9 = *(undefined1 *)puVar8;
            puVar8 = (undefined4 *)((int)puVar8 + 1);
            puVar9 = (undefined4 *)((int)puVar9 + 1);
          }
          param_2 = (undefined4 *)((int)param_2 + param_1[2] * 2);
        }
        else {
          uVar4 = uVar3 * 2;
          if ((int)uVar4 < (int)((uVar3 + uVar7) * 2)) {
            do {
              uVar1 = *(undefined1 *)param_2;
              param_2 = (undefined4 *)((int)param_2 + 1);
              (&DAT_0096e6e0)[iVar6 + (uVar4 & 0x700) * 0x100 + (uVar4 & 0xff)] = uVar1;
              uVar4 = uVar4 + 1;
            } while ((int)uVar4 < ((int)*param_1 + (int)param_1[2]) * 2);
          }
        }
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)param_1[1] + (int)param_1[3]);
      FUN_0040bd80();
      return;
    }
  }
  FUN_0040bd80();
  return;
}

