
void FUN_00404750(uint param_1)

{
  uint uVar1;
  uint *puVar2;
  int iVar3;
  
  if (param_1 != 0) {
    iVar3 = 0;
    puVar2 = &DAT_00ba29d0;
    while ((param_1 <= puVar2[-4] || (*puVar2 <= param_1))) {
      puVar2 = puVar2 + 1;
      iVar3 = iVar3 + 1;
      if (0xba29df < (int)puVar2) {
        return;
      }
    }
    uVar1 = *(uint *)(param_1 - 8);
    *(uint *)(param_1 - 8) = uVar1 | 0x80000000;
    FUN_004063c0(s_Free_id__d___0x_x_Size____d_00436110,iVar3,param_1,uVar1 * 4 + -8);
    FUN_00404830(iVar3);
    if (*(undefined4 **)(param_1 - 4) != (undefined4 *)0x0) {
      **(undefined4 **)(param_1 - 4) = 0;
    }
  }
  return;
}

