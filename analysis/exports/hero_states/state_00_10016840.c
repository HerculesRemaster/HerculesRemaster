
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_10016840(void)

{
  int iVar1;
  
  if (DAT_10072170 != 0) {
    FUN_10019160();
    return;
  }
  if (DAT_10072110 != 0) {
    if (DAT_1007201c != 0) {
      DAT_10072108 = 0x2f;
      DAT_10072860[0x19] = (int)&DAT_10054558;
      *(undefined2 *)(DAT_10072860 + 0xc) = 0xffff;
      *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
      FUN_10015d00();
      return;
    }
    FUN_10018f60();
    return;
  }
  if ((undefined *)DAT_10072860[0x19] != &DAT_10053930) {
    *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) & 0xff7fffff;
  }
  DAT_1007210c = 0;
  _DAT_10072012 = 0;
  iVar1 = FUN_10029c20(DAT_10072860);
  if (iVar1 == 0) {
    FUN_10017340();
    return;
  }
  if ((*DAT_10070f24 != '\x06') || (DAT_100734f4 == DAT_1007217c)) {
    if (DAT_1007201c == 0) {
      iVar1 = FUN_10017ff0();
      if (iVar1 != 0) {
        return;
      }
      iVar1 = FUN_10017ba0();
      if (iVar1 != 0) {
        return;
      }
      iVar1 = FUN_10017d00();
      if (iVar1 != 0) {
        return;
      }
      iVar1 = FUN_10017a20();
    }
    else {
      iVar1 = FUN_10018bb0();
    }
    if (iVar1 != 0) {
      return;
    }
    if (DAT_10071840 == 0) {
      iVar1 = FUN_10016fa0();
      if (iVar1 != 0) {
        return;
      }
      iVar1 = FUN_10016990();
      if (iVar1 != 0) {
        return;
      }
      iVar1 = FUN_10018b10();
    }
    else {
      if ((DAT_10072860[2] & 0x7f0000U) != 0) goto LAB_10016981;
      iVar1 = FUN_10016fa0();
      if (iVar1 != 0) {
        return;
      }
      iVar1 = FUN_10016990();
      if (iVar1 != 0) {
        return;
      }
      iVar1 = FUN_10018b10();
    }
    if (iVar1 == 0) {
LAB_10016981:
      FUN_10015d00();
      return;
    }
  }
  return;
}

