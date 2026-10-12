
uint FUN_0040c1c0(short param_1,undefined2 *param_2,undefined2 *param_3)

{
  if (param_2 != (undefined2 *)0x0) {
    *param_2 = *(undefined2 *)(&DAT_00a8e7d0 + param_1 * 4);
  }
  if (param_3 != (undefined2 *)0x0) {
    *param_3 = *(undefined2 *)(&DAT_00a8e830 + param_1 * 4);
  }
  return (uint)param_3 & 0xffff0000;
}

