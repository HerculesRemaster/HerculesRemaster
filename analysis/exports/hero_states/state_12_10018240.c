
void FUN_10018240(void)

{
  int iVar1;
  
  DAT_1006efa4 = 0x32;
  iVar1 = FUN_10029c20(DAT_10072860);
  if (iVar1 == 0) {
    FUN_10017340();
    return;
  }
  if (((undefined *)DAT_10072860[0x19] == &DAT_100541b0) && (iVar1 = FUN_10017ff0(), iVar1 != 0)) {
    return;
  }
  iVar1 = FUN_10016fa0();
  if (iVar1 == 0) {
    FUN_10017ae0();
    iVar1 = FUN_10015d00();
    if (iVar1 != 0) {
      FUN_10016790();
      DAT_1007211c = 0;
    }
    return;
  }
  DAT_1006ef7c = 0;
  *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) & 0xff7fffff;
  return;
}

