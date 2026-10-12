
void FUN_00410690(void)

{
  undefined *puVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_1f;
  
  puVar3 = &local_1f;
  for (iVar2 = 7; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  *(undefined2 *)puVar3 = 0;
  *(undefined1 *)((int)puVar3 + 2) = 0;
  FUN_00404510(DAT_00ba29e0,DAT_00ba29e4,0);
  FUN_0040bce0(PTR_DAT_004361bc[9]);
  FUN_0040a340(0);
  FUN_0040be30(0);
  FUN_0040bdf0();
  FUN_0040a340(0);
  FUN_00404180();
  FUN_0040bd40();
  puVar1 = PTR_DAT_004361bc;
  *(undefined4 *)(PTR_DAT_004361bc + 0x6c) = DAT_0043a640;
  *(undefined4 *)(puVar1 + 0x70) = DAT_0043a644;
  *(undefined4 *)(puVar1 + 0x74) = DAT_0043a648;
  *(undefined4 *)(puVar1 + 0x78) = DAT_0043a64c;
  FUN_00410760(0);
  FUN_0040ac00(&DAT_0043a630,0,0,0);
  FUN_0040a1a0();
  FUN_0040a340();
  return;
}

