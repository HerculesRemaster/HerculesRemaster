
void FUN_10019c30(void)

{
  int iVar1;
  int iVar2;
  
  if ((DAT_10070d10 & 1) == 0) {
    if (*DAT_10072860 < 0x400000) {
      *DAT_10072860 = 0x400000;
    }
    iVar1 = (uint)*(ushort *)((&DAT_10070d5c)[DAT_10070cfc._2_1_] + 4) * 0x400000;
    if (iVar1 + -0x400000 < *DAT_10072860) {
      *DAT_10072860 = iVar1 + -0x40;
    }
    iVar1 = *DAT_10072860;
    if ((0xc00000 < iVar1) &&
       (iVar1 < (int)((*(ushort *)((&DAT_10070d5c)[DAT_10070cfc._2_1_] + 4) - 3) * 0x400000))) {
      iVar2 = (DAT_10070e2c + 3) * 0x400000;
      if (iVar1 < iVar2) {
        *DAT_10072860 = iVar2;
      }
      iVar1 = (DAT_10070e0c + -3 + (int)DAT_10070e2c) * 0x400000;
      if (iVar1 < *DAT_10072860) {
        *DAT_10072860 = iVar1;
      }
    }
  }
  return;
}

