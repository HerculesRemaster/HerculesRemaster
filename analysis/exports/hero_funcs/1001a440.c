
void __cdecl FUN_1001a440(int *param_1)

{
  int iVar1;
  undefined2 *puVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  
  param_1[0x14] = param_1[0x14] | 0x20800;
  param_1[0x15] = param_1[0x15] | 0x20700000;
  *(undefined1 *)((int)param_1 + 0x23) = 4;
  param_1[2] = *(int *)(DAT_10072860 + 8);
  *param_1 = *(short *)(&DAT_10054c9c + (uint)DAT_1006ef78 * 10) * 0x10000 + *DAT_1007217c;
  param_1[1] = *(short *)(&DAT_10054c9e + (uint)DAT_1006ef78 * 10) * 0x10000 + DAT_1007217c[1];
  *(undefined2 *)(param_1 + 0x1d) = *(undefined2 *)(&DAT_10054ca0 + (uint)DAT_1006ef78 * 10);
  iVar1 = DAT_1007217c[0xe];
  *(undefined2 *)((int)param_1 + 0x4a) = 0xe0;
  *(short *)(param_1 + 0xe) = (short)iVar1;
  param_1[0x19] = (int)&DAT_100554f0;
  *(undefined1 *)((int)param_1 + 0x22) = 2;
  FUN_10029260((int)param_1);
  FUN_10028820(param_1);
  FUN_10026700((int)param_1,0x10055548);
  DAT_10071fd2 = 0;
  puVar2 = &DAT_10071ee6;
  DAT_10071fd0 = *(undefined2 *)(&DAT_10054ca0 + (uint)DAT_1006ef78 * 10);
  *(undefined2 *)(param_1 + 0x1d) = DAT_10071fd0;
  uVar3 = CONCAT22(DAT_10071fd0,DAT_10071fd0);
  puVar4 = &DAT_10071fe0;
  for (iVar1 = 10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar4 = uVar3;
    puVar4 = puVar4 + 1;
  }
  do {
    puVar2[-3] = 0;
    *puVar2 = 0;
    puVar2[-2] = 0;
    puVar2[1] = 0;
    puVar2[-1] = 0;
    puVar2[2] = 0;
    puVar2 = puVar2 + 6;
  } while ((int)puVar2 < 0x10071fd6);
  if ((*DAT_10070f24 != '\t') && (-0x50 < DAT_10070d18)) {
    *(short *)(DAT_10072020 + 0x36) = *(short *)(DAT_10072020 + 0x36) + -3;
  }
  *(undefined2 *)(param_1 + 0x26) = *(undefined2 *)((int)param_1 + 0x36);
  return;
}

