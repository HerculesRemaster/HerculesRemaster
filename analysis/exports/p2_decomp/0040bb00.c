
void FUN_0040bb00(short *param_1,undefined2 *param_2)

{
  short sVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  iVar6 = (int)*(short *)(PTR_DAT_004361cc + ((int)*param_1 & 0xfffU) * 2);
  sVar1 = *(short *)(PTR_DAT_004361cc + ((int)*param_1 - 0x400U & 0xfff) * 2);
  iVar4 = (int)*(short *)(PTR_DAT_004361cc + ((int)param_1[1] - 0x400U & 0xfff) * 2);
  iVar7 = (int)*(short *)(PTR_DAT_004361cc + ((int)param_1[1] & 0xfffU) * 2);
  iVar2 = (int)*(short *)(PTR_DAT_004361cc + ((int)param_1[2] & 0xfffU) * 2);
  iVar3 = (int)*(short *)(PTR_DAT_004361cc + ((int)param_1[2] - 0x400U & 0xfff) * 2);
  param_2[2] = (short)(iVar4 * iVar6 >> 0xc);
  param_2[5] = -sVar1;
  iVar5 = (int)(short)(iVar4 * sVar1 >> 0xc);
  param_2[8] = (short)(iVar7 * iVar6 >> 0xc);
  *param_2 = (short)(iVar3 * iVar5 + iVar2 * iVar7 >> 0xc);
  param_2[1] = (short)(iVar2 * iVar5 - iVar3 * iVar7 >> 0xc);
  param_2[3] = (short)(iVar3 * iVar6 >> 0xc);
  iVar5 = (int)(short)(sVar1 * iVar7 >> 0xc);
  param_2[4] = (short)(iVar2 * iVar6 >> 0xc);
  param_2[6] = (short)(iVar3 * iVar5 - iVar4 * iVar2 >> 0xc);
  param_2[7] = (short)(iVar2 * iVar5 + iVar3 * iVar4 >> 0xc);
  return;
}

