
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_10020670(void)

{
  int iVar1;
  ushort uVar2;
  ushort uVar3;
  undefined4 uVar4;
  uint *puVar5;
  uint uVar6;
  int *piVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  undefined1 local_10;
  undefined1 local_c;
  
  uVar10 = (uint)*(ushort *)(DAT_10070ed4 + 8);
  uVar2 = *(ushort *)(DAT_10070ed4 + 10);
  uVar8 = (uint)*(ushort *)(DAT_10070ed4 + 0xc);
  uVar6 = (uint)*(ushort *)(DAT_10070ed4 + 0xe);
  uVar9 = (uint)*(ushort *)(DAT_10070ed4 + 0x10);
  uVar3 = *(ushort *)(DAT_10070ed4 + 0x12);
  DAT_10070ed0 = (undefined1)*(ushort *)(DAT_10070ed4 + 8);
  local_10 = (undefined1)*(ushort *)(DAT_10070ed4 + 0x10);
  DAT_10070ed1 = local_10;
  local_c = (undefined1)uVar2;
  DAT_10070ecd = (undefined1)*(ushort *)(DAT_10070ed4 + 0xe);
  DAT_10070ed2 = (undefined1)uVar3;
  DAT_10070ed3 = local_c;
  DAT_10070ecc = 0;
  DAT_10070d28 = DAT_10070ed4 + 0x24 + (*(uint *)(DAT_10070ed4 + 0x24) & 0xfffffffc);
  DAT_10070d30 = DAT_10070ed4 + 0x28 + (*(uint *)(DAT_10070ed4 + 0x28) & 0xfffffffc);
  uVar4 = *(undefined4 *)(DAT_10070ed4 + 0x2c);
  puVar5 = (uint *)(DAT_10070ed4 + 0x30);
  DAT_10070cf0._0_1_ = (undefined1)uVar4;
  DAT_10070cf0._1_1_ = (undefined1)((uint)uVar4 >> 8);
  DAT_10070cf0._2_1_ = (undefined1)((uint)uVar4 >> 0x10);
  if (uVar10 != 0) {
    piVar7 = &DAT_10070d5c;
    do {
      iVar1 = (int)puVar5 + (*puVar5 & 0xfffffffc);
      puVar5 = puVar5 + 1;
      *piVar7 = iVar1;
      uVar10 = uVar10 - 1;
      piVar7 = piVar7 + 1;
    } while (uVar10 != 0);
  }
  DAT_10070d58 = (int)puVar5 + (*puVar5 & 0xfffffffc);
  DAT_10070d54 = (int)(puVar5 + 1) + (puVar5[1] & 0xfffffffc);
  DAT_10070d50 = (int)(puVar5 + 2) + (puVar5[2] & 0xfffffffc);
  _DAT_10070d44 = (int)(puVar5 + 3) + (puVar5[3] & 0xfffffffc);
  DAT_10070d34 = puVar5 + 4;
  DAT_10070d2c = puVar5 + uVar8 + 3;
  DAT_10070d38 = DAT_10070d34 + uVar6 + uVar8;
  DAT_10070d3c = DAT_10070d34 + uVar9 + uVar6 + uVar8;
  DAT_10070d4c = DAT_10070d34 + uVar3 + uVar9 + uVar6 + uVar8;
  DAT_10070d40 = DAT_10070d34 + (uint)uVar2 + (uint)uVar3 + uVar9 + uVar6 + uVar8;
  DAT_10070d00 = (&DAT_1005df0b)[*DAT_10070f24 * 0x1c];
  FUN_10020830((int)DAT_10070cfc._2_1_);
  return;
}


