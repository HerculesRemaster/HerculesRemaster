
int FUN_00404690(int param_1,int param_2,undefined4 param_3,int param_4)

{
  uint uVar1;
  uint *puVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  
  if (param_1 != 0) {
    uVar1 = *(int *)(param_1 + -8) * 4 - 8;
    uVar4 = param_2 + 3U & 0xfffffffc;
    if (uVar4 == uVar1) {
      return 0;
    }
    if (uVar4 < uVar1) {
      uVar1 = *(uint *)(param_1 + -8);
      puVar3 = *(undefined4 **)(param_1 + -4);
      uVar4 = (param_2 + 3U >> 2) + 2;
      uVar5 = (uVar1 & 0x3fffffff) - uVar4;
      puVar2 = (uint *)(param_1 + -8 + uVar5 * 4);
      *(uint *)(param_1 + -8) = uVar5 | 0x80000000;
      *puVar2 = uVar1 & 0x40000000 | uVar4;
      if (param_4 == 0) {
        FUN_00410fb0(puVar2 + 1,(uint *)(param_1 + -4),uVar4 * 4 + -4);
      }
      else {
        puVar2[1] = *(uint *)(param_1 + -4);
      }
      *puVar3 = puVar2 + 2;
      FUN_00404830(param_3);
      FUN_004063c0(s_ReMalloc_Heap_d___0x_x_Size__d_004360f0,param_3,puVar2 + 2,uVar4 * 4 + -8);
    }
  }
  return param_1;
}

