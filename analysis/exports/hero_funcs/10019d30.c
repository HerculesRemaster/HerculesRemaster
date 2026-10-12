
void FUN_10019d30(void)

{
  int iVar1;
  
  if (*(int *)(DAT_10072860 + 4) < 0x400000) {
    *(undefined4 *)(DAT_10072860 + 4) = 0x400000;
  }
  iVar1 = (uint)*(ushort *)((&DAT_10070d5c)[DAT_10070cfc._2_1_] + 6) * 0x400000;
  if (iVar1 < *(int *)(DAT_10072860 + 4)) {
    *(int *)(DAT_10072860 + 4) = iVar1;
  }
  return;
}

