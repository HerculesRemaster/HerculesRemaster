
void FUN_00410760(void)

{
  undefined *puVar1;
  
  *(undefined2 *)(PTR_DAT_004361bc + 0x9c) = 0;
  PTR_DAT_004361bc[0xdf] = 0;
  *(undefined2 *)(PTR_DAT_004361bc + 0xdc) = 0;
  puVar1 = PTR_DAT_004361bc;
  *(undefined4 *)(PTR_DAT_004361bc + 0x7c) = *(undefined4 *)(PTR_DAT_004361bc + 0x6c);
  *(undefined4 *)(puVar1 + 0x80) = *(undefined4 *)(puVar1 + 0x70);
  *(undefined4 *)(puVar1 + 0x84) = *(undefined4 *)(puVar1 + 0x74);
  *(undefined4 *)(puVar1 + 0x88) = *(undefined4 *)(puVar1 + 0x78);
  PTR_DAT_004361bc[0xf] = PTR_DAT_004361bc[0xe];
  return;
}

