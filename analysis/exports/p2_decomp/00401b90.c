
void FUN_00401b90(int param_1)

{
  *(undefined4 *)(PTR_DAT_004361bc + 0x1b30) = 1;
  if (*(int *)(PTR_DAT_004361bc + 0x1afc) != -1) {
    if (param_1 == 0) {
      *(undefined4 *)(PTR_DAT_004361bc + 0x1b2c) = 0x69;
      return;
    }
    *(undefined4 *)(PTR_DAT_004361bc + 0x1b2c) = 0;
  }
  return;
}

