
void FUN_0040bfe0(short param_1)

{
  int iVar1;
  int iVar2;
  
  if (((-1 < param_1) && (param_1 < 0x10)) && (iVar2 = (int)param_1, (&DAT_00a8e790)[iVar2] != 0)) {
    iVar1 = 0x44;
    do {
      if (*(short *)((&DAT_00a8e790)[iVar2] + iVar1) != 0) {
        FUN_0040f710((int)*(short *)((&DAT_00a8e790)[iVar2] + iVar1));
      }
      iVar1 = iVar1 + 2;
    } while (iVar1 < 0x144);
    (&DAT_00a8e790)[iVar2] = 0;
  }
  return;
}

