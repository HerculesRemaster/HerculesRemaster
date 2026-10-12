
void FUN_004043a0(int param_1,uint param_2,uint param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  
  param_3 = (int)(param_3 + 3) >> 2;
  puVar10 = (undefined4 *)(param_2 + param_3 * 4);
  puVar8 = (undefined4 *)(param_1 + param_3 * 4);
  if (7 < (int)param_3) {
    param_2 = param_3 >> 3;
    puVar9 = puVar8;
    puVar11 = puVar10;
    do {
      uVar1 = puVar11[-3];
      puVar10 = puVar11 + -8;
      puVar8 = puVar9 + -8;
      uVar2 = puVar11[-2];
      uVar3 = puVar11[-6];
      uVar4 = puVar11[-5];
      uVar5 = puVar11[-4];
      uVar6 = puVar11[-1];
      uVar7 = *puVar10;
      puVar9[-7] = puVar11[-7];
      puVar9[-3] = uVar1;
      puVar9[-2] = uVar2;
      puVar9[-1] = uVar6;
      puVar9[-6] = uVar3;
      param_3 = param_3 - 8;
      param_2 = param_2 - 1;
      *puVar8 = uVar7;
      puVar9[-5] = uVar4;
      puVar9[-4] = uVar5;
      puVar9 = puVar8;
      puVar11 = puVar10;
    } while (param_2 != 0);
  }
  if (0 < (int)param_3) {
    do {
      puVar9 = puVar10 + -1;
      puVar10 = puVar10 + -1;
      puVar8 = puVar8 + -1;
      param_3 = param_3 - 1;
      *puVar8 = *puVar9;
    } while (param_3 != 0);
  }
  return;
}

