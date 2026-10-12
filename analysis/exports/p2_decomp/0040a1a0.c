
undefined4 FUN_0040a1a0(void)

{
  int iVar1;
  undefined4 *puVar2;
  
  if (DAT_0043744c != 0) {
    DAT_0043744c = 0;
    DAT_0096e638 = 0;
    puVar2 = &DAT_0096e640;
    for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar2 = 0;
      puVar2 = puVar2 + 1;
    }
  }
  iVar1 = 1;
  puVar2 = &DAT_0096e644;
  do {
    if (puVar2[-1] != 0) {
      FUN_0040a350(iVar1);
    }
    puVar2 = puVar2 + 1;
    iVar1 = iVar1 + 1;
  } while ((int)puVar2 < 0x96e681);
  DAT_0096e638 = 0;
  return 0;
}

