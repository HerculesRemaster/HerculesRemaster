
undefined4 FUN_00403b50(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  int iVar2;
  
  FUN_00401d60();
  FUN_00410f40(&DAT_00615c10,&DAT_00436024,param_1,param_2);
  uVar1 = FUN_0040a210(&DAT_00615c10,0,0);
  *(undefined4 *)(PTR_DAT_004361bc + 0xb8) = uVar1;
  iVar2 = *(int *)(PTR_DAT_004361bc + 0xb8);
  while( true ) {
    if (iVar2 == -1) {
      FUN_004063c0(s_Unable_to_open__s_00435ff4,&DAT_00615c10);
      return 0;
    }
    FUN_004063c0(s_Loading__s___0x_x_size__d_00436008,&DAT_00615c10,param_3,param_4);
    FUN_0040a2f0(*(undefined4 *)(PTR_DAT_004361bc + 0xb8),param_3,param_4);
    iVar2 = FUN_0040a350(*(undefined4 *)(PTR_DAT_004361bc + 0xb8));
    if (iVar2 == 0) break;
    uVar1 = FUN_0040a210(&DAT_00615c10,0,0);
    *(undefined4 *)(PTR_DAT_004361bc + 0xb8) = uVar1;
    iVar2 = *(int *)(PTR_DAT_004361bc + 0xb8);
  }
  *(undefined2 *)(PTR_DAT_004361bc + 0xdc) = 0;
  FUN_004063c0(s___Loaded_00435fe8);
  return param_4;
}

