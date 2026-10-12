
void FUN_00404510(uint *param_1,uint param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  
  FUN_004063c0(s_Initheap_Heap_d___0x_x_size__d_0043608c,param_3,param_1,param_2);
  uVar2 = (param_2 >> 2) - 2;
  (&DAT_00ba29c0)[param_3] = param_1;
  (&DAT_00ba29d0)[param_3] = param_1 + uVar2;
  *param_1 = uVar2 | 0x80000000;
  param_1 = param_1 + 2;
  iVar1 = (param_2 >> 2) - 3;
  do {
    *param_1 = 1;
    param_1 = param_1 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}

