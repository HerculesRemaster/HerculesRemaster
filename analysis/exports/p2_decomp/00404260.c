
uint FUN_00404260(uint param_1)

{
  uint uVar1;
  
  if (param_1 == 0) {
    return 0;
  }
  uVar1 = FUN_00404280();
  return uVar1 % param_1;
}

