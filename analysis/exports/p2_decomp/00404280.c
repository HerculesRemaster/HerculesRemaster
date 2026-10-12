
undefined4 FUN_00404280(void)

{
  uint uVar1;
  
  *(int *)(PTR_DAT_004361bc + 0x48) =
       *(int *)(PTR_DAT_004361bc + 0x48) + *(int *)(PTR_DAT_004361bc + 0x4c);
  uVar1 = *(uint *)(PTR_DAT_004361bc + 0x48);
  if (0xffff < uVar1) {
    *(uint *)(PTR_DAT_004361bc + 0x48) = uVar1 & 0xffff;
    *(uint *)(PTR_DAT_004361bc + 0x4c) =
         *(int *)(PTR_DAT_004361bc + 0x48) + 1 + *(int *)(PTR_DAT_004361bc + 0x4c) & 0xffff;
    return *(undefined4 *)(PTR_DAT_004361bc + 0x4c);
  }
  *(uint *)(PTR_DAT_004361bc + 0x4c) = *(int *)(PTR_DAT_004361bc + 0x4c) + uVar1 & 0xffff;
  return *(undefined4 *)(PTR_DAT_004361bc + 0x4c);
}

