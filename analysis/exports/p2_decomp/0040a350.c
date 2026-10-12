
undefined4 FUN_0040a350(int param_1)

{
  undefined4 uVar1;
  
  if (((param_1 <= DAT_0096e638) && (0 < param_1)) && (*(int *)(&DAT_0096e63c + param_1 * 4) != 0))
  {
    uVar1 = FS_Close(*(int *)(&DAT_0096e63c + param_1 * 4));
    if (0 < DAT_0096e638) {
      DAT_0096e638 = DAT_0096e638 + -1;
    }
    *(undefined4 *)(&DAT_0096e63c + param_1 * 4) = 0;
    return uVar1;
  }
  return 0xffffffff;
}

