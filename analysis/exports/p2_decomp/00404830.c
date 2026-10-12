
void FUN_00404830(int param_1)

{
  uint *puVar1;
  uint *puVar2;
  uint uVar3;
  
  puVar1 = (uint *)(&DAT_00ba29c0)[param_1];
  puVar2 = (uint *)(&DAT_00ba29d0)[param_1];
  while (puVar1 < puVar2) {
    uVar3 = *puVar1;
    if (((uVar3 & 0x80000000) == 0x80000000) && ((puVar1[uVar3] & 0x80000000) == 0x80000000)) {
      *puVar1 = (puVar1[uVar3] & 0x3fffffff) + uVar3;
    }
    else {
      puVar1 = puVar1 + uVar3;
    }
  }
  return;
}

