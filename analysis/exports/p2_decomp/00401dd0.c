
undefined * FUN_00401dd0(int param_1)

{
  short sVar1;
  
  if ((int)DAT_004352b0 < 0) {
    *(undefined4 *)(PTR_DAT_004361bc + 0x1b00) = 0;
  }
  if (*(int *)(PTR_DAT_004361bc + 0x1b00) != 0) {
    return (undefined *)0x0;
  }
  if ((*(uint *)(PTR_DAT_004361bc + 0x30) & 0x20000) != 0) {
    return (undefined *)0x1;
  }
  if (*(short *)(PTR_DAT_004361bc + 0x9c) == 1) {
    return (undefined *)0x1;
  }
  if (PTR_DAT_004361bc[0xdf] != '\0') {
    return (undefined *)0x1;
  }
  DAT_0043f744 = param_1 + 1000;
  DAT_0043e530 = &PTR_s__XA_XA1__1_004352b8 + param_1 * 4;
  sVar1 = FUN_0040f940(DAT_0043f744);
  FUN_0040fbf0(0xffffffff,(int)sVar1,8000,8000);
  *(undefined4 *)(PTR_DAT_004361bc + 0x1b00) = 2;
  DAT_004352b0 = DAT_0043e530[3];
  return DAT_0043e530[3];
}

