
void __cdecl FUN_1001a0f0(int *param_1)

{
  bool bVar1;
  ushort uVar2;
  int *piVar3;
  int iVar4;
  undefined2 extraout_var_01;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  void *this;
  void *this_00;
  int *piVar5;
  byte bVar6;
  
  piVar3 = (int *)param_1[0x17];
  FUN_10028820(param_1);
  *(short *)(param_1 + 0x1d) = (short)param_1[0x1d] + -0x80;
  if (piVar3 == (int *)0x0) {
    piVar3 = FUN_1002aea0(param_1,0x30,0x140);
  }
  else if (((piVar3[0x14] & 0x200U) == 0) || (*(short *)((int)piVar3 + 0x2e) == 0)) {
    piVar3 = (int *)0x0;
  }
  if (piVar3 == (int *)0x0) {
    bVar6 = 0x40;
    piVar5 = (int *)0x0;
  }
  else {
    bVar6 = 3;
    piVar5 = piVar3;
  }
  FUN_1002b5e0(param_1,piVar5,bVar6);
  param_1[0x17] = (int)piVar3;
  iVar4 = FUN_10029f50(param_1);
  if (iVar4 != 0) {
    FUN_10030290(this,param_1,1);
  }
  uVar2 = FUN_10030010(*param_1,param_1[1],param_1[2],1);
  if (CONCAT22(extraout_var_01,uVar2) != 0) {
    FUN_10030290(this_00,param_1,1);
  }
  if (piVar3 == (int *)0x0) {
    bVar1 = FUN_1002aa30((int)param_1,0x40,0x40);
    if (CONCAT31(extraout_var_00,bVar1) != 0) {
      FUN_10027860(param_1);
    }
  }
  else {
    bVar1 = FUN_1002aa30((int)param_1,0x100,0x100);
    if (CONCAT31(extraout_var,bVar1) != 0) {
      FUN_10027860(param_1);
      return;
    }
  }
  return;
}

