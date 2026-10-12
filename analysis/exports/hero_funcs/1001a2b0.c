
void __cdecl FUN_1001a2b0(int *param_1)

{
  int iVar1;
  
  if (param_1 == DAT_10072018) {
    param_1[0x19] = (int)&DAT_100554a8;
    *(short *)((int)param_1 + 0x3e) = (short)DAT_1007217c[4] + 0x20;
  }
  else {
    param_1[0x19] = (int)&DAT_100554c0;
    *(short *)((int)param_1 + 0x3e) = (short)DAT_1007217c[4] + -0x20;
  }
  *param_1 = *DAT_1007217c + -0x80000;
  param_1[1] = DAT_1007217c[1] + -0x300000;
  iVar1 = *(int *)(DAT_10072860 + 8);
  *(undefined2 *)((int)param_1 + 0x3a) = 0x100;
  param_1[2] = iVar1;
  *(undefined2 *)(param_1 + 0x1c) = 0xa0;
  *(undefined2 *)((int)param_1 + 0x72) = 0;
  *(undefined2 *)(param_1 + 0x1d) = 0;
  *(undefined1 *)((int)param_1 + 0x22) = 2;
  FUN_10029260((int)param_1);
  FUN_10028820(param_1);
  param_1[0x14] = param_1[0x14] | 0x20000;
  param_1[0x15] = param_1[0x15] | 0x2005;
  *(undefined1 *)((int)param_1 + 0x23) = 5;
  *(undefined2 *)((int)param_1 + 0x4a) = 0xa0;
  iVar1 = DAT_1007217c[0x25];
  *(undefined1 *)((int)param_1 + 0x95) = 0xff;
  *(char *)(param_1 + 0x25) = (char)iVar1;
  return;
}

