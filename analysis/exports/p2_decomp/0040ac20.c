
undefined4 FUN_0040ac20(undefined4 *param_1,int param_2)

{
  undefined4 *puVar1;
  
  param_2 = param_2 + -1;
  puVar1 = param_1;
  if (0 < param_2) {
    do {
      param_1 = puVar1 + 1;
      param_2 = param_2 + -1;
      *puVar1 = param_1;
      puVar1 = param_1;
    } while (param_2 != 0);
  }
  *param_1 = 0;
  return 0;
}

