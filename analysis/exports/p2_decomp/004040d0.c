
undefined4 FUN_004040d0(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  FUN_00410f40(&DAT_00615c10,&DAT_00436024,param_1,param_2);
  uVar1 = FUN_0040a210(&DAT_00615c10,0,0);
  *(undefined4 *)(PTR_DAT_004361bc + 0xb8) = uVar1;
  iVar2 = *(int *)(PTR_DAT_004361bc + 0xb8);
  while( true ) {
    if (iVar2 == -1) {
      return 0;
    }
    uVar1 = FUN_0040a280(iVar2,0,2);
    iVar2 = FUN_0040a350(*(undefined4 *)(PTR_DAT_004361bc + 0xb8));
    if (iVar2 == 0) break;
    uVar1 = FUN_0040a210(&DAT_00615c10,0,0);
    *(undefined4 *)(PTR_DAT_004361bc + 0xb8) = uVar1;
    iVar2 = *(int *)(PTR_DAT_004361bc + 0xb8);
  }
  return uVar1;
}

