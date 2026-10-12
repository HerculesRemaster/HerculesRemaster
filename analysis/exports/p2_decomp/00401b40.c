
void FUN_00401b40(void)

{
  undefined *puVar1;
  undefined4 uVar2;
  
  *(undefined4 *)(PTR_DAT_004361bc + 0x1b2c) = 0x69;
  *(undefined4 *)(PTR_DAT_004361bc + 0x1b28) = 0x69;
  puVar1 = PTR_DAT_004361bc;
  *(undefined4 *)(PTR_DAT_004361bc + 0x1b30) = 1;
  uVar2 = CONCAT22((short)((uint)puVar1 >> 0x10),*(undefined2 *)(PTR_DAT_004361bc + 0x1b28));
  FUN_0040c200(uVar2,uVar2);
  return;
}

