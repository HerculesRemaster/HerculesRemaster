
undefined4 FUN_10019cf0(void)

{
  int iVar1;
  
  iVar1 = *(int *)(DAT_10072860 + 8) >> 0x10;
  if (iVar1 < DAT_10071c5c) {
    *(int *)(DAT_10072860 + 8) = (int)DAT_10071c5c << 0x10;
    return 1;
  }
  if (DAT_10071bc4 < iVar1) {
    *(int *)(DAT_10072860 + 8) = (int)DAT_10071bc4 << 0x10;
    return 1;
  }
  return 0;
}

