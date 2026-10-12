
uint FUN_00401ee0(void)

{
  int iVar1;
  uint uVar2;
  undefined2 *puVar3;
  
  FUN_0040a340(&DAT_0043e738,1);
  puVar3 = &DAT_0043eb38;
  do {
    puVar3 = puVar3 + 4;
  } while ((int)puVar3 < 0x43ef38);
  iVar1 = FUN_0040f7c0(DAT_0043f744);
  if (iVar1 != 0) {
    uVar2 = FUN_00410a40();
    return uVar2 & 0x1f;
  }
  return 0;
}

