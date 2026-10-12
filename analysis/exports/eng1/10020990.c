
int __cdecl FUN_10020990(uint param_1,uint param_2,char *param_3)

{
  ushort uVar1;
  int iVar2;
  char *pcVar3;
  byte *pbVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  
  uVar5 = (uint)*param_3;
  pcVar3 = param_3 + 1;
  if (uVar5 == 0) {
    uVar1 = (ushort)*pcVar3;
    if ((uVar1 & 0x80) != 0) {
      return -1;
    }
    if ((uVar1 & 0x40) != 0) {
      return (uVar1 & 0x3f) << 1;
    }
    return ((int)(((uint)(ushort)(short)(char)(param_3[2] & 0x3f) * 2 + (uVar1 & 0x7fff) * -2) *
                  (param_1 & DAT_1005e0d4) * DAT_1005e0d8) >> 8) + (uVar1 & 0x7fff) * 2;
  }
  if ((uVar5 & 0xfffffff0) != 0) {
    uVar6 = uVar5 & 0xf;
    iVar2 = ((int)uVar5 >> 4) + -1;
    pcVar3 = pcVar3 + ((int)(*(int *)(&DAT_1005e0d0 + iVar2 * 0x10) * param_2) >> 8) *
                      (*(int *)(&DAT_1005e0dc + uVar6 * 0x10) + 1) +
                      ((int)(*(int *)(&DAT_1005e0d0 + uVar6 * 0x10) * param_1) >> 8);
    uVar1 = (ushort)*pcVar3;
    pbVar4 = (byte *)(pcVar3 + 1);
    if ((uVar1 & 0x80) != 0) {
      return -1;
    }
    if ((uVar1 & 0x40) != 0) {
      return (uVar1 & 0x3f) << 1;
    }
    iVar8 = ((&DAT_1005e0d4)[uVar6 * 4] & param_1) * (&DAT_1005e0d8)[uVar6 * 4];
    iVar7 = ((int)(((uint)(ushort)(short)(char)(*pbVar4 & 0x3f) * 2 + (uVar1 & 0x7fff) * -2) * iVar8
                  ) >> 8) + (uVar1 & 0x7fff) * 2;
    uVar5 = (uint)(ushort)(short)(char)(pbVar4[*(int *)(&DAT_1005e0dc + uVar6 * 0x10)] & 0x3f);
    return ((int)(((&DAT_1005e0d4)[iVar2 * 4] & param_2) * (&DAT_1005e0d8)[iVar2 * 4] *
                 ((((int)(((uint)(ushort)(short)(char)((pbVar4 + *(int *)(&DAT_1005e0dc +
                                                                         uVar6 * 0x10))[1] & 0x3f) *
                           2 + uVar5 * -2) * iVar8) >> 8) + uVar5 * 2) - iVar7)) >> 8) + iVar7;
  }
  uVar5 = uVar5 & 0xf;
  uVar1 = (ushort)pcVar3[(int)(*(int *)(&DAT_1005e0d0 + uVar5 * 0x10) * param_1) >> 8];
  if ((uVar1 & 0x80) != 0) {
    return -1;
  }
  if ((uVar1 & 0x40) != 0) {
    return (uVar1 & 0x3f) << 1;
  }
  return ((int)(((&DAT_1005e0d4)[uVar5 * 4] & param_1) *
                ((uint)(ushort)(short)(char)((pcVar3 + ((int)(*(int *)(&DAT_1005e0d0 + uVar5 * 0x10)
                                                             * param_1) >> 8))[1] & 0x3f) * 2 +
                (uVar1 & 0x7fff) * -2) * (&DAT_1005e0d8)[uVar5 * 4]) >> 8) + (uVar1 & 0x7fff) * 2;
}


