
undefined4
FUN_00403d90(undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4,int param_5)

{
  undefined4 uVar1;
  
  if (*(short *)(PTR_DAT_004361bc + 0x9c) == 1) {
    return 1;
  }
  FUN_00401d60();
  FUN_00410f40(&DAT_00615c10,&DAT_00436024,param_1,param_2);
  uVar1 = FUN_0040a210(&DAT_00615c10,0,0);
  *(undefined4 *)(PTR_DAT_004361bc + 0xb8) = uVar1;
  if (*(int *)(PTR_DAT_004361bc + 0xb8) == -1) {
    FUN_004063c0(s_Unable_to_open__s___d__00436050,&DAT_00615c10,param_5);
    return 3;
  }
  if (param_3 != 0) {
    FUN_0040a280(*(int *)(PTR_DAT_004361bc + 0xb8),param_3 << 0xb,0);
  }
  *(undefined4 *)(PTR_DAT_004361bc + 0xa4) = param_4;
  *(undefined2 *)(PTR_DAT_004361bc + 0x9c) = 1;
  *(short *)(PTR_DAT_004361bc + 0xa0) =
       (short)((int)(param_5 + 0x7ff + (param_5 + 0x7ff >> 0x1f & 0x7ffU)) >> 0xb);
  *(undefined2 *)(PTR_DAT_004361bc + 0x9e) = *(undefined2 *)(PTR_DAT_004361bc + 0xa0);
  *(undefined4 *)(PTR_DAT_004361bc + 0xbc) = 0;
  return 2;
}

