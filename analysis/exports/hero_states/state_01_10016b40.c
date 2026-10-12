
void FUN_10016b40(void)

{
  int iVar1;
  
  if ((DAT_10072170 != 0) || (DAT_10072110 != 0)) {
    FUN_10016c40();
    return;
  }
  if (DAT_1007201c == 0) {
    iVar1 = FUN_10017ff0();
    if (iVar1 != 0) {
      return;
    }
    iVar1 = FUN_10017ba0();
    if (iVar1 != 0) {
      return;
    }
    iVar1 = FUN_10016fd0();
    if (iVar1 != 0) {
      return;
    }
  }
  FUN_10015d00();
  if ((char)DAT_10072860[0x25] == '\0') {
    if ((*(byte *)(DAT_10070f24 + 0x5c) & 0x20) == 0) {
      if ((*(byte *)(DAT_10070f24 + 0x5c) & 0x80) == 0) {
        FUN_10016c40();
        return;
      }
      FUN_10016e70();
      return;
    }
    FUN_10019a10();
    iVar1 = FUN_10029c20(DAT_10072860);
    if (iVar1 == 0) {
      FUN_10017340();
      return;
    }
    iVar1 = FUN_10019890();
  }
  else {
    if ((*(byte *)(DAT_10070f24 + 0x5c) & 0x80) == 0) {
      if ((*(byte *)(DAT_10070f24 + 0x5c) & 0x20) == 0) {
        FUN_10016c40();
        return;
      }
      FUN_10016e70();
      return;
    }
    FUN_10019a10();
    iVar1 = FUN_10029c20(DAT_10072860);
    if (iVar1 == 0) {
      FUN_10017340();
      return;
    }
    iVar1 = FUN_100197b0();
  }
  if (iVar1 == 0) {
    *DAT_10072860 = DAT_10072860[0x27];
    FUN_10016c40();
    FUN_10029c20(DAT_10072860);
  }
  return;
}

