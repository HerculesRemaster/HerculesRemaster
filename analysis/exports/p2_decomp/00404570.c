
uint * FUN_00404570(int *param_1,int param_2,uint param_3)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint *puVar6;
  uint uVar7;
  uint local_8;
  
  uVar7 = 0;
  if (*param_1 != 0) {
    FUN_004063c0(s_Malloc_FAILED___Address__x_data___004360cc,param_1,*param_1);
  }
  uVar5 = 0;
  local_8 = 0;
  if ((param_3 & 0x10) != 0) {
    uVar5 = 0x40000000;
    local_8 = 0x40000000;
  }
  uVar2 = param_3 & 3;
  puVar6 = (uint *)0x0;
  uVar4 = (param_2 + 3U >> 2) + 2;
  puVar1 = (uint *)(&DAT_00ba29c0)[uVar2];
  while (puVar1 < (uint *)(&DAT_00ba29d0)[uVar2]) {
    uVar5 = *puVar1;
    if (((uVar5 & 0x80000000) == 0x80000000) && (uVar5 = uVar5 & 0x3fffffff, uVar4 <= uVar5)) {
      puVar6 = puVar1;
      uVar7 = uVar5;
    }
    puVar1 = puVar1 + uVar5;
    uVar5 = local_8;
  }
  if (puVar6 != (uint *)0x0) {
    uVar3 = uVar7 - uVar4;
    if (uVar7 == uVar4) {
      *puVar6 = uVar5 | uVar4;
      puVar6[1] = (uint)param_1;
    }
    else {
      *puVar6 = uVar3 | 0x80000000;
      puVar6[uVar3] = uVar5 | uVar4;
      puVar6[uVar3 + 1] = (uint)param_1;
    }
    puVar6 = puVar6 + uVar3 + 2;
    FUN_004063c0(s_Malloc_Heap_d___0x_x_Size__d_004360ac,uVar2,puVar6,uVar4 * 4 + -8);
    *param_1 = (int)puVar6;
    return puVar6;
  }
  if ((param_3 & 0x20) == 0) {
    *param_1 = 0;
  }
  return (uint *)0x0;
}

