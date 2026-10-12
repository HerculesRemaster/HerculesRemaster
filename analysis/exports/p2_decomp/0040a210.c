
int FUN_0040a210(undefined4 param_1,int param_2)

{
  int *piVar1;
  undefined4 uVar2;
  int iVar3;
  
  if (0xf < DAT_0096e638) {
    RT_FatalError(s_PCopen_1__0043745c);
  }
  if (param_2 != 0) {
    RT_FatalError(s_PCopen_2__00437450);
  }
  iVar3 = 0;
  piVar1 = &DAT_0096e640;
  do {
    if (*piVar1 == 0) break;
    piVar1 = piVar1 + 1;
    iVar3 = iVar3 + 1;
  } while ((int)piVar1 < 0x96e680);
  uVar2 = FS_Open(param_1,&DAT_00435fe4);
  (&DAT_0096e640)[iVar3] = uVar2;
  DAT_0096e638 = DAT_0096e638 + 1;
  return iVar3 + 1;
}

