
void FUN_0040a280(int param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  
  if (((DAT_0096e638 < param_1) || (param_1 < 1)) || (*(int *)(&DAT_0096e63c + param_1 * 4) == 0)) {
    RT_FatalError(s_PClseek_1__00437468);
  }
  cVar1 = '\0';
  if (param_3 != 0) {
    cVar1 = (param_3 != 1) + '\x01';
  }
  FS_Seek(*(undefined4 *)(&DAT_0096e63c + param_1 * 4),param_2,cVar1);
  FS_Tell(*(undefined4 *)(&DAT_0096e63c + param_1 * 4));
  return;
}

