
void FUN_004015d0(void)

{
  undefined2 extraout_var;
  
  if (*(int *)(PTR_DAT_004361bc + 0x1afc) != -1) {
    thunk_FUN_00409e10(CONCAT22((short)((uint)PTR_DAT_004361bc >> 0x10),
                                *(undefined2 *)(PTR_DAT_004361bc + 0x1afc)));
    FUN_0040be30();
    FUN_0040be30(CONCAT22(extraout_var,*(undefined2 *)(PTR_DAT_004361bc + 0x1afc)));
    FUN_0040be30();
    return;
  }
  return;
}

