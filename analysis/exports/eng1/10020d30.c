
uint __cdecl FUN_10020d30(int param_1,int param_2,uint param_3)

{
  ushort uVar1;
  uint in_EAX;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  ushort *puVar6;
  
  if (DAT_10070d01 == -1) {
    return in_EAX & 0xffff0000;
  }
  iVar3 = (int)((param_3 & 0xffff) + 0x40) >> 7;
  if (iVar3 < 0) {
    iVar3 = 0;
  }
  else if (DAT_10070ece + -1 < iVar3) {
    iVar3 = DAT_10070ece + -1;
  }
  uVar5 = param_1 + (uint)(ushort)((&DAT_10070e2c)[iVar3] * -0x40);
  uVar2 = (uVar5 & 0xffff) >> 6;
  if ((ushort)(&DAT_10070e0c)[iVar3] <= (ushort)uVar2) {
    return 0;
  }
  uVar4 = (param_2 + 0x20U & 0xffff) >> 6;
  if (DAT_10070d14 <= (ushort)uVar4) {
    return 0;
  }
  uVar2 = *(uint *)((&DAT_10070dcc)[iVar3] + (uVar2 * (int)(short)DAT_10070d14 + uVar4) * 4) >> 0x18
          & 0x7f;
  if (uVar2 == 0) {
    return 0;
  }
  puVar6 = (ushort *)(DAT_10070d40 + -4 + uVar2 * 4 + *(int *)(DAT_10070d40 + -4 + uVar2 * 4));
  uVar1 = *puVar6;
  uVar2 = (uint)uVar1;
  puVar6 = puVar6 + 1;
  iVar3 = 0;
  if (uVar2 != 0) {
    uVar4 = uVar2 & 0xf;
    uVar2 = (uVar2 & 0xff) >> 4;
    if ((uVar1 & 0xf) != 0) {
      puVar6 = (ushort *)
               ((int)puVar6 + ((int)(*(int *)(&DAT_1005e0d0 + uVar4 * 0x10) * (uVar5 & 0x3f)) >> 8))
      ;
    }
    if (uVar2 != 0) {
      puVar6 = (ushort *)
               ((int)puVar6 +
               ((int)(*(int *)(&DAT_1005e0d0 + uVar2 * 0x10) * (param_2 + 0x20U & 0x3f)) >> 8) *
               *(int *)(&DAT_1005e0dc + uVar4 * 0x10));
    }
    iVar3 = 0;
    if (uVar1 >> 8 != 0) {
      iVar3 = ((int)(*(int *)(&DAT_1005e0d0 + (uint)(uVar1 >> 8) * 0x10) *
                    ((int)((param_3 & 0xffff) + 0x3f) >> 1 & 0x3fU)) >> 8) *
              *(int *)(&DAT_1005e0dc + uVar4 * 0x10) * *(int *)(&DAT_1005e0dc + uVar2 * 0x10);
      puVar6 = (ushort *)((int)puVar6 + iVar3);
    }
  }
  return CONCAT22((short)((uint)iVar3 >> 0x10),(short)(char)*puVar6);
}


