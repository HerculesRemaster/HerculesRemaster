
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_0040a150(char param_1,undefined4 *param_2)

{
  if (param_1 == '\x02') {
    _DAT_0096e634 = *param_2;
  }
  else if ((param_1 == '\x03') && (param_2 != (undefined4 *)0x0)) {
    _DAT_0096e634 = *param_2;
    return 1;
  }
  return 1;
}

