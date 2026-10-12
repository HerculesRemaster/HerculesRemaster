
void FUN_0040b920(void)

{
  int iVar1;
  
  FUN_0040b740();
  iVar1 = DAT_00a8e754 << 0x10;
  if (DAT_00a8e734 != 0) {
    iVar1 = iVar1 / DAT_00a8e734;
  }
  DAT_00a8e740 = (short)((uint)(iVar1 * DAT_00a8e71c) >> 0x10) + (short)DAT_00a8e74c;
  DAT_00a8e746 = (short)((uint)(iVar1 * DAT_00a8e728) >> 0x10) + (short)DAT_00a8e750;
  return;
}

