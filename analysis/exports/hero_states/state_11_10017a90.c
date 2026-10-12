
void FUN_10017a90(void)

{
  int iVar1;
  
  iVar1 = FUN_10029c20(DAT_10072860);
  if (iVar1 == 0) {
    FUN_10017340();
    return;
  }
  iVar1 = FUN_10016fa0();
  if (iVar1 == 0) {
    FUN_10017ae0();
    if (((*(byte *)(DAT_10070f24 + 0x5c) & 0x40) == 0) && (iVar1 = FUN_10016990(), iVar1 != 0)) {
      return;
    }
    iVar1 = FUN_10015d00();
    if (iVar1 != 0) {
      FUN_10016790();
      return;
    }
  }
  return;
}

