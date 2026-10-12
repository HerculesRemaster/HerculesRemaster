
void FUN_0040bcf0(void)

{
  ushort uVar1;
  
  if (DAT_00a8e78c != (undefined1 *)0x0) {
    uVar1 = FUN_00408040();
    *(ushort *)(DAT_00a8e78c + 2) = ~uVar1;
    DAT_00a8e78c[1] = 0x40;
    *DAT_00a8e78c = 0;
  }
  FUN_0040f820();
  FUN_0040bdb0();
  FUN_0040be40();
  return;
}

