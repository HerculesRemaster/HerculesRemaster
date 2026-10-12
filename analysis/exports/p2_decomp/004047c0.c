
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004047c0(int param_1)

{
  uint *puVar1;
  uint *puVar2;
  uint uVar3;
  
  puVar1 = (uint *)(&DAT_00ba29c0)[param_1];
  puVar2 = (uint *)(&DAT_00ba29d0)[param_1];
  DAT_00ba29ec = *puVar1 & 0x3fffffff;
  _DAT_00ba29e8 = puVar1;
  for (; puVar1 < puVar2; puVar1 = puVar1 + uVar3) {
    uVar3 = *puVar1;
    if (((uVar3 & 0xc0000000) == 0) && (*(int *)puVar1[1] != 0)) {
      FUN_00404750(*(int *)puVar1[1]);
    }
  }
  FUN_00404890(param_1);
  return;
}

