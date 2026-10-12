
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl FUN_1001f870(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar3 = 0;
  if ((param_1 == 0) ||
     ((*(short *)(DAT_10070f24 + 0x9c) == 0 && (*(char *)(DAT_10070f24 + 0xdf) == '\0')))) {
    iVar4 = DAT_10070ca0 >> 6;
    iVar5 = 0;
    iVar2 = DAT_10070d48;
    if ('\0' < DAT_10070ecf) {
      iVar6 = 0;
      do {
        iVar1 = (int)*(short *)(iVar2 + 4 + iVar6);
        if (*(short *)(iVar2 + 8 + iVar6) + iVar1 <= iVar4) {
          iVar3 = iVar5;
        }
        if ((((iVar4 < iVar1) || (*(short *)(iVar2 + 6 + iVar6) + iVar1 < iVar4)) &&
            (iVar5 != DAT_10070d01)) && ((&DAT_10071e40)[iVar5 + DAT_10070cfc._2_1_ * 8] != 0)) {
          FUN_1002c2d0((&DAT_10071e40)[iVar5 + DAT_10070cfc._2_1_ * 8]);
          iVar2 = DAT_10070d48;
        }
        iVar5 = iVar5 + 1;
        iVar6 = iVar6 + 0xc;
      } while (iVar5 < DAT_10070ecf);
    }
    if (('\0' < DAT_10070ed0) &&
       ((int)*(short *)(iVar2 + iVar3 * 0xc + 4) + (int)*(short *)(iVar2 + 10 + iVar3 * 0xc) ==
        (int)_DAT_10070d12)) {
      DAT_10070cfc._2_1_ = DAT_10070cfc._2_1_ + '\x01';
      FUN_10020830((int)DAT_10070cfc._2_1_);
      FUN_1001f9e0(0,1);
      DAT_10070cfc._2_1_ = DAT_10070cfc._2_1_ + -1;
      FUN_10020830((int)DAT_10070cfc._2_1_);
      DAT_1005ce48 = 1;
      return;
    }
    if ((&DAT_10071e40)[iVar3 + DAT_10070cfc._2_1_ * 8] == 0) {
      if (DAT_10070d01 != -1) {
        FUN_1001f9e0(iVar3,1);
        return;
      }
      FUN_1001f9e0(iVar3,0);
      return;
    }
    DAT_10070d01 = (char)iVar3;
    FUN_100208f0();
  }
  return;
}


