
int FUN_0040c250(undefined4 param_1)

{
  int *piVar1;
  int iVar2;
  
  iVar2 = 0;
  piVar1 = &DAT_00a8e890;
  do {
    if (*piVar1 == 0) break;
    piVar1 = piVar1 + 1;
    iVar2 = iVar2 + 1;
  } while ((int)piVar1 < 0xa8e90c);
  (&DAT_00a8e890)[iVar2] = param_1;
  return iVar2 + 1;
}

