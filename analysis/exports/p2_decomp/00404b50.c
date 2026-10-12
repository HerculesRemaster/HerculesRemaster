
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint FUN_00404b50(uint param_1)

{
  if (param_1 != 0xffffffff) {
    if (param_1 == 0xfffffffe) {
      return (DAT_00615c90 << 8 | _DAT_00b9e864 & 0xff) << 8 | DAT_00b9e870;
    }
    if (param_1 == 0) {
      DAT_00615c90 = param_1;
      return 0;
    }
    DAT_00615c90 = ((int)(uint)(byte)(&DAT_00b9e85c)[_DAT_00b9e864 & 0xff] < (int)param_1) - 1 &
                   param_1;
  }
  return DAT_00615c90;
}

