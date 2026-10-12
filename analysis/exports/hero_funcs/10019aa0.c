
void FUN_10019aa0(void)

{
  int iVar1;
  
  iVar1 = *(int *)(DAT_10072860 + 0xb8);
  if (iVar1 != *(int *)(DAT_10072860 + 0xbc)) {
    if (*(int *)(DAT_10072860 + 0xbc) < iVar1) {
      *(int *)(DAT_10072860 + 0xb8) = iVar1 - *(int *)(DAT_10072860 + 0xc4);
      iVar1 = *(int *)(DAT_10072860 + 0xbc);
      if (iVar1 <= *(int *)(DAT_10072860 + 0xb8)) goto LAB_10019b08;
    }
    else {
      *(int *)(DAT_10072860 + 0xb8) = *(int *)(DAT_10072860 + 0xc0) + iVar1;
      iVar1 = *(int *)(DAT_10072860 + 0xbc);
      if (*(int *)(DAT_10072860 + 0xb8) <= iVar1) goto LAB_10019b08;
    }
    *(int *)(DAT_10072860 + 0xb8) = iVar1;
  }
LAB_10019b08:
  if (*(char *)(DAT_10072860 + 0x95) == '\0') {
    *(int *)(DAT_10072860 + 4) = *(int *)(DAT_10072860 + 4) + *(int *)(DAT_10072860 + 0xb8);
  }
  else {
    *(int *)(DAT_10072860 + 4) = *(int *)(DAT_10072860 + 4) - *(int *)(DAT_10072860 + 0xb8);
  }
  FUN_10019d30();
  return;
}

