
void FUN_00403f00(void)

{
  short sVar1;
  int iVar2;
  
  if (*(int *)(PTR_DAT_004361bc + 0xbc) != 0) {
    *(int *)(PTR_DAT_004361bc + 0xbc) = *(int *)(PTR_DAT_004361bc + 0xbc) + -1;
    if (*(int *)(PTR_DAT_004361bc + 0xbc) == 0) {
      *(undefined2 *)(PTR_DAT_004361bc + 0x9c) = 0;
    }
  }
  if (*(short *)(PTR_DAT_004361bc + 0x9c) == 1) {
    if (*(int *)(PTR_DAT_004361bc + 0xd8) == -1) {
      *(undefined2 *)(PTR_DAT_004361bc + 0xdc) = 0x100;
      if (*(code **)(PTR_DAT_004361bc + 0xb4) != (code *)0x0) {
        (**(code **)(PTR_DAT_004361bc + 0xb4))(0);
      }
      *(undefined2 *)(PTR_DAT_004361bc + 0x9c) = 0;
      *(undefined4 *)(PTR_DAT_004361bc + 0xd8) = 0;
      return;
    }
    sVar1 = *(short *)(PTR_DAT_004361bc + 0x9e);
    iVar2 = 5;
    if (sVar1 < 5) {
      *(undefined2 *)(PTR_DAT_004361bc + 0x9e) = 0;
      iVar2 = (int)sVar1;
    }
    else {
      *(short *)(PTR_DAT_004361bc + 0x9e) = sVar1 + -5;
    }
    if (iVar2 != 0) {
      FUN_0040a2f0(*(undefined4 *)(PTR_DAT_004361bc + 0xb8),*(undefined4 *)(PTR_DAT_004361bc + 0xa4)
                   ,iVar2 * 0x800);
      *(int *)(PTR_DAT_004361bc + 0xa4) = *(int *)(PTR_DAT_004361bc + 0xa4) + iVar2 * 0x800;
    }
    if (*(short *)(PTR_DAT_004361bc + 0x9e) == 0) {
      *(undefined2 *)(PTR_DAT_004361bc + 0xdc) = 0;
      if (PTR_DAT_004361bc[0xe0] == '\0') {
        FUN_00403ea0();
        *(undefined2 *)(PTR_DAT_004361bc + 0x9c) = 4;
        *(undefined4 *)(PTR_DAT_004361bc + 0xbc) = 0xc;
      }
      if (*(code **)(PTR_DAT_004361bc + 0xb0) != (code *)0x0) {
        (**(code **)(PTR_DAT_004361bc + 0xb0))(0);
      }
    }
  }
  return;
}

