
void FUN_0040c100(void)

{
  int iVar1;
  undefined4 *puVar2;
  
  puVar2 = &DAT_00a8e790;
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  puVar2 = &DAT_00a8e890;
  for (iVar1 = 0x1f; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  DAT_0043a480 = 0x7f;
  DAT_0043a484 = 0x7f;
  FUN_0040ff40(0);
  return;
}

