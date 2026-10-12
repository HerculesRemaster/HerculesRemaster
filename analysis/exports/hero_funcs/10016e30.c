
undefined4 FUN_10016e30(void)

{
  byte bVar1;
  
  if (*(char *)(DAT_10072860 + 0x94) == '\0') {
    bVar1 = *(byte *)(DAT_10070f24 + 0x5c) & 0x80;
  }
  else {
    bVar1 = *(byte *)(DAT_10070f24 + 0x5c) & 0x20;
  }
  if (bVar1 != 0) {
    FUN_10016e70();
    return 1;
  }
  return 0;
}

