
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_100280f0(void)

{
  ushort *puVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  
  iVar4 = 0;
  DAT_10071860 = 0;
  if ((short)DAT_10068af0 != 999) {
    puVar5 = &DAT_10068af0;
    uVar2 = DAT_10068af0;
    do {
      if ((short)uVar2 == (short)*DAT_10070f24) break;
      puVar1 = (ushort *)(puVar5 + 0x12);
      uVar2 = (uint)*puVar1;
      puVar5 = puVar5 + 0x12;
      iVar4 = iVar4 + 1;
    } while (*puVar1 != 999);
  }
  iVar3 = iVar4 * 0x48;
  DAT_10071378 = (&DAT_10068af8)[iVar4 * 0x24];
  DAT_100713a8 = (&DAT_10068afe)[iVar4 * 0x24];
  DAT_100713b6 = (&DAT_10068b02)[iVar4 * 0x24];
  DAT_100713c4 = (&DAT_10068af8)[iVar4 * 0x24];
  DAT_10071308 = (&DAT_10068afe)[iVar4 * 0x24];
  DAT_10071374 = (&DAT_10068b02)[iVar4 * 0x24];
  DAT_10071388 = (&PTR_DAT_10068b08)[iVar4 * 0x12];
  DAT_10071370 = (&PTR_PTR_10068b0c)[iVar4 * 0x12];
  DAT_10071390 = (&PTR_DAT_10068b10)[iVar4 * 0x12];
  DAT_1007137c = (&PTR_PTR_10068b14)[iVar4 * 0x12];
  DAT_100713a4 = (&PTR_DAT_10068b18)[iVar4 * 0x12];
  DAT_100713ba = (&DAT_10068af6)[iVar4 * 0x24] + 5;
  DAT_100713b4 = *(undefined2 *)((int)&DAT_10068af0 + iVar3 + 2);
  DAT_1007139c = (&PTR_PTR_10068b1c)[iVar4 * 0x12];
  _DAT_100713b0 = (&PTR_PTR_10068b20)[iVar4 * 0x12];
  DAT_10071394 = *(undefined2 *)(iVar3 + 0x10068b04);
  DAT_100713a0 = *(undefined2 *)(iVar3 + 0x10068b00);
  DAT_10071432 = (&DAT_10068af4)[iVar4 * 0x24] + 5;
  DAT_100713ac = *(undefined2 *)(iVar3 + 0x10068b00);
  DAT_100713aa = *(undefined2 *)(iVar3 + 0x10068afa);
  DAT_100713b8 = *(undefined2 *)(iVar3 + 0x10068afa);
  DAT_10071384 = (&PTR_PTR_10068b24)[iVar4 * 0x12];
  DAT_100713c0 = (&PTR_PTR_10068b28)[iVar4 * 0x12];
  DAT_10071380 = *(undefined2 *)(iVar3 + 0x10068b06);
  DAT_1007138c = *(undefined2 *)(iVar3 + 0x10068afc);
  DAT_10071430 = *(undefined2 *)(iVar3 + 0x10068afc);
  DAT_10071398 = *(undefined4 *)(iVar3 + 0x10068b30);
  DAT_100713bc = *(undefined4 *)(iVar3 + 0x10068b34);
  return;
}


