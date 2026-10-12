
void FUN_10019a10(void)

{
  int iVar1;
  
  iVar1 = DAT_10072860[0x2a];
  if (iVar1 != DAT_10072860[0x2b]) {
    if (DAT_10072860[0x2b] < iVar1) {
      DAT_10072860[0x2a] = iVar1 - DAT_10072860[0x2d];
      iVar1 = DAT_10072860[0x2b];
      if (iVar1 <= DAT_10072860[0x2a]) goto LAB_10019a78;
    }
    else {
      DAT_10072860[0x2a] = DAT_10072860[0x2c] + iVar1;
      iVar1 = DAT_10072860[0x2b];
      if (DAT_10072860[0x2a] <= iVar1) goto LAB_10019a78;
    }
    DAT_10072860[0x2a] = iVar1;
  }
LAB_10019a78:
  iVar1 = DAT_10072860[0x2a];
  if ((char)DAT_10072860[0x25] != '\0') {
    iVar1 = -iVar1;
  }
  *DAT_10072860 = *DAT_10072860 + iVar1;
  FUN_10019c30();
  return;
}

