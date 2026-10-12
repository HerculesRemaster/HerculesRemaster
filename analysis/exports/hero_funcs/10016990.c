
undefined4 FUN_10016990(void)

{
  int iVar1;
  
  if ((*(byte *)(DAT_10070f24 + 0x5c) & 0x80) == 0) {
    if ((*(byte *)(DAT_10070f24 + 0x5c) & 0x20) != 0) {
      if ((*(byte *)(DAT_10072860 + 0x54) & 0x10) == 0) {
        iVar1 = FUN_10019900();
        if (iVar1 != 0) {
          FUN_10016a60();
          *(undefined1 *)(DAT_10072860 + 0x94) = 0;
          return 1;
        }
      }
      else {
        iVar1 = FUN_10019900();
        if ((iVar1 != 0) || (*(char *)(DAT_10072860 + 0x94) != '\0')) {
          *(undefined1 *)(DAT_10072860 + 0x94) = 0xff;
          FUN_10016e70();
          return 1;
        }
      }
    }
  }
  else if (((byte)*(undefined4 *)(DAT_10072860 + 0x54) & 0x10) == 0x10) {
    iVar1 = FUN_10019820();
    if (iVar1 != 0) {
      FUN_10016a60();
      *(undefined1 *)(DAT_10072860 + 0x94) = 0xff;
      return 1;
    }
  }
  else {
    iVar1 = FUN_10019820();
    if ((iVar1 != 0) || (*(char *)(DAT_10072860 + 0x94) != -1)) {
      *(undefined1 *)(DAT_10072860 + 0x94) = 0;
      FUN_10016e70();
      return 1;
    }
  }
  return 0;
}

