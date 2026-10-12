
void FUN_10016da0(void)

{
  int iVar1;
  
  FUN_10029c20(DAT_10072860);
  if (((DAT_1007201c == 0) && (DAT_10072170 == 0)) && (DAT_10072110 == 0)) {
    iVar1 = FUN_10017ff0();
    if (iVar1 != 0) {
      return;
    }
    iVar1 = FUN_10017ba0();
    if (iVar1 != 0) {
      return;
    }
    if ((DAT_10072860[2] >> 0x10 == (int)DAT_10071c5c) ||
       (DAT_10072860[2] >> 0x10 == (int)DAT_10071bc4)) {
      iVar1 = FUN_10016fa0();
      if (iVar1 != 0) {
        return;
      }
      iVar1 = FUN_10016990();
      if (iVar1 != 0) {
        return;
      }
    }
    iVar1 = FUN_10017d00();
    if (iVar1 != 0) {
      return;
    }
    iVar1 = FUN_10017a20();
    if (iVar1 != 0) {
      return;
    }
  }
  iVar1 = FUN_10015d00();
  if (iVar1 == 0) {
    return;
  }
  FUN_10016790();
  return;
}

