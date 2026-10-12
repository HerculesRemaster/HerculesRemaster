
void FUN_00401e90(void)

{
  if (*(int *)(PTR_DAT_004361bc + 0x1b00) != 0) {
    FUN_0040fa10(DAT_0043f744,0);
    FUN_0040a150(9,0,0);
    *(undefined4 *)(PTR_DAT_004361bc + 0x1b00) = 0;
    DAT_004352b0 = 0;
    FUN_0040be30();
    return;
  }
  return;
}

