
undefined4 FUN_004048a0(int param_1,uint param_2)

{
  uint *puVar1;
  uint *puVar2;
  uint uVar3;
  uint uVar4;
  undefined4 *puVar5;
  int iVar6;
  int iVar7;
  uint local_c;
  
  local_c = 0;
  iVar6 = FUN_004049f0(param_1);
  iVar6 = iVar6 + -1;
  if (iVar6 < 0) {
    return 1;
  }
  do {
    iVar7 = 0;
    for (puVar2 = (uint *)(&DAT_00ba29c0)[param_1]; puVar2 < (uint *)(&DAT_00ba29d0)[param_1];
        puVar2 = puVar2 + *puVar2) {
      if ((*puVar2 & 0x80000000) != 0x80000000) {
        if (iVar6 == iVar7) {
          uVar3 = *puVar2;
          if ((uVar3 & 0x40000000) != 0) break;
          uVar4 = puVar2[uVar3];
          if ((uVar4 & 0x80000000) != 0x80000000) break;
          puVar5 = (undefined4 *)puVar2[1];
          puVar1 = puVar2 + uVar4;
          if ((param_2 == 0) || (local_c + uVar3 * 4 <= param_2)) {
            FUN_004043a0(puVar1,puVar2,uVar3 * 4);
            local_c = local_c + (uVar3 & 0x3fffffff);
            *puVar2 = uVar4 & 0x3fffffff | 0x80000000;
            *puVar1 = uVar3 & 0x3fffffff;
            puVar1[1] = (uint)puVar5;
            *puVar5 = puVar1 + 2;
            FUN_00404830(param_1);
            FUN_004063c0(s_CompactMalloc_Heap_d___0x_x_Size_00436130,param_1,puVar1 + 2,
                         uVar3 * 4 + -8);
            if (local_c < 0x8001) break;
          }
          return 0;
        }
        iVar7 = iVar7 + 1;
      }
    }
    iVar6 = iVar6 + -1;
    if (iVar6 < 0) {
      return 1;
    }
  } while( true );
}

