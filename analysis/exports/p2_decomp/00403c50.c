
undefined4 FUN_00403c50(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
  if (*(short *)(PTR_DAT_004361bc + 0x9c) == 1) {
    return 0;
  }
  FUN_00401d60();
  FUN_00410f40(&DAT_00615c10,&DAT_00436024,param_1,param_2);
  uVar1 = FUN_0040a210(&DAT_00615c10,0,0);
  *(undefined4 *)(PTR_DAT_004361bc + 0xb8) = uVar1;
  if (*(int *)(PTR_DAT_004361bc + 0xb8) == -1) {
    FUN_004063c0(s_Unable_to_open__s_00435ff4,&DAT_00615c10);
    return 0;
  }
  uVar1 = FUN_004063c0(s_Opening__s_0043602c,&DAT_00615c10);
  return uVar1;
}

