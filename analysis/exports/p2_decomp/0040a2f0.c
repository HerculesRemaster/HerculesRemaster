
void FUN_0040a2f0(int param_1,undefined4 param_2,undefined4 param_3)

{
  if (((DAT_0096e638 < param_1) || (param_1 < 1)) || (*(int *)(&DAT_0096e63c + param_1 * 4) == 0)) {
    RT_FatalError(s_PCread_1__00437474);
  }
  FS_Read(param_2,1,param_3,*(undefined4 *)(&DAT_0096e63c + param_1 * 4));
  return;
}

