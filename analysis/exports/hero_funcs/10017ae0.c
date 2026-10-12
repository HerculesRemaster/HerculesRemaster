
undefined4 FUN_10017ae0(void)

{
  int iVar1;
  
  if ((*(char *)(DAT_10072860 + 0x94) == -1) && ((*(byte *)(DAT_10070f24 + 0x5c) & 0x20) != 0)) {
    *(undefined1 *)(DAT_10072860 + 0x94) = 0;
    *(uint *)(DAT_1007217c + 0x54) =
         *(byte *)(DAT_1007217c + 0x94) & 0x10 | *(uint *)(DAT_1007217c + 0x54) & 0xffffffef;
    iVar1 = *(int *)(DAT_1007217c + 0x60);
    if (iVar1 != 0) {
      *(uint *)(iVar1 + 0x54) =
           *(byte *)(DAT_1007217c + 0x94) & 0x10 | *(uint *)(iVar1 + 0x54) & 0xffffffef;
      return 1;
    }
  }
  else {
    if ((*(char *)(DAT_10072860 + 0x94) != '\0') || ((*(byte *)(DAT_10070f24 + 0x5c) & 0x80) == 0))
    {
      return 0;
    }
    *(undefined1 *)(DAT_10072860 + 0x94) = 0xff;
    *(uint *)(DAT_1007217c + 0x54) =
         *(byte *)(DAT_1007217c + 0x94) & 0x10 | *(uint *)(DAT_1007217c + 0x54) & 0xffffffef;
    iVar1 = *(int *)(DAT_1007217c + 0x60);
    if (iVar1 != 0) {
      *(uint *)(iVar1 + 0x54) =
           *(byte *)(DAT_1007217c + 0x94) & 0x10 | *(uint *)(iVar1 + 0x54) & 0xffffffef;
    }
  }
  return 1;
}

