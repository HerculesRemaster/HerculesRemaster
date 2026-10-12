
void FUN_00401d80(void)

{
  if (((-1 < DAT_004352b0) && (*(int *)(PTR_DAT_004361bc + 0x1b00) == 2)) &&
     (DAT_004352b0 = DAT_004352b0 + -1, DAT_004352b0 < 0)) {
    FUN_0040a150(9,0,0);
    *(undefined4 *)(PTR_DAT_004361bc + 0x1b00) = 0;
    FUN_0040be30();
    return;
  }
  return;
}

