
void __cdecl FUN_10019f50(int *param_1)

{
  short *psVar1;
  undefined1 uVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  undefined4 *puVar6;
  
  FUN_10029260((int)param_1);
  param_1[0x14] = param_1[0x14] | 0x20000;
  param_1[0x15] = param_1[0x15] | 0x2005;
  sVar3 = *(short *)(DAT_1007217c + 0x10);
  *(undefined1 *)((int)param_1 + 0x23) = 5;
  *(undefined2 *)((int)param_1 + 0x4a) = 0xe0;
  *(short *)((int)param_1 + 0x3e) = sVar3 + -0x10;
  uVar2 = *(undefined1 *)(DAT_1007217c + 0x94);
  *(undefined1 *)((int)param_1 + 0x95) = 0xff;
  *(undefined1 *)(param_1 + 0x25) = uVar2;
  *param_1 = *param_1 + *(short *)(&DAT_10054d64 + (uint)DAT_1006ef78 * 10) * 0x10000;
  param_1[1] = param_1[1] + *(short *)(&DAT_10054d66 + (uint)DAT_1006ef78 * 10) * 0x10000;
  param_1[2] = *(int *)(DAT_10072860 + 8);
  uVar5 = (int)(uint)CONCAT11((char)(DAT_1006ef78 >> 8),(&DAT_10054d60)[(uint)DAT_1006ef78 * 10]) >>
          2 & 0x3e;
  iVar4 = *(int *)(&DAT_10055448 + uVar5 * 4);
  param_1[0x2b] = 0xf0000;
  param_1[0x2a] = iVar4;
  param_1[0x2c] = 0xa000;
  param_1[0x2d] = 0xf000;
  param_1[0x2e] = *(int *)(&DAT_1005544c + uVar5 * 4);
  param_1[0x2f] = 0xa0000;
  param_1[0x30] = 0x8000;
  param_1[0x31] = 0xa000;
  param_1[0x32] = 0;
  param_1[0x33] = 0xa0000;
  param_1[0x34] = 0x8000;
  param_1[0x35] = 0xa000;
  puVar6 = FUN_1002aea0(param_1,0x30,0x140);
  param_1[0x17] = (int)puVar6;
  FUN_100075e3(0x4811,(int)param_1,DAT_1007217c);
  if ((&DAT_10072140)[DAT_1006efb4] == 0) {
    psVar1 = (short *)(DAT_10070f24 + 0x24 + DAT_1006efb4 * 2);
    *psVar1 = *psVar1 + -0xc;
    if (*(short *)(DAT_10070f24 + 0x24 + DAT_1006efb4 * 2) < 1) {
      *(undefined2 *)(DAT_10070f24 + 0x24 + DAT_1006efb4 * 2) = 0;
      DAT_1006efb4 = '\0';
    }
  }
  return;
}

