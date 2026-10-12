
undefined4 FUN_00403d30(void)

{
  int iVar1;
  
  iVar1 = FUN_0040a350(*(undefined4 *)(PTR_DAT_004361bc + 0xb8));
  if (iVar1 != 0) {
    FUN_004063c0(s___Failed_00436044);
    return 0;
  }
  *(undefined4 *)(PTR_DAT_004361bc + 0xac) = 0;
  *(undefined2 *)(PTR_DAT_004361bc + 0x9c) = 0;
  FUN_004063c0(s___Closed_00436038);
  return 1;
}

