
void FUN_1001fb90(void)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  
  if (DAT_1005ce48 != -1) {
    iVar1 = (int)*(short *)(DAT_10070d48 + 0x10 + DAT_10070d01 * 0xc);
    DAT_10070ca0 = DAT_10070ca0 + iVar1 * -0x40;
    *DAT_100734f4 = *DAT_100734f4 + iVar1 * -0x400000;
    DAT_10070cfc._2_1_ = DAT_1005ce48;
    FUN_10020830((int)DAT_1005ce48);
    FUN_10020830((int)DAT_1005ce48);
    DAT_1005ce48 = -1;
    iVar1 = 0;
    do {
      if (iVar1 != DAT_1005ce48) {
        iVar3 = 0;
        piVar2 = &DAT_10071e40;
        do {
          if (*piVar2 != 0) {
            FUN_1002c2d0((&DAT_10071e40)[iVar3 + iVar1 * 8]);
          }
          piVar2 = piVar2 + 1;
          iVar3 = iVar3 + 1;
        } while ((int)piVar2 < 0x10071e60);
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < 4);
  }
  DAT_10070d01 = DAT_10070c50;
  FUN_100208f0();
  return;
}


