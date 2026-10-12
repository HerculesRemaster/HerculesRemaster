
uint FUN_0040be60(short param_1,short param_2,undefined4 *param_3)

{
  if ((((&DAT_00a8e790)[param_1] != 0) && (-1 < param_2)) && (param_2 < 0x80)) {
    *param_3 = 0;
    param_3[1] = 0;
    param_3[2] = 0;
    param_3[3] = 0;
    *(undefined2 *)((int)param_3 + 6) =
         *(undefined2 *)((&DAT_00a8e790)[param_1] + 0x44 + param_2 * 2);
    return (uint)param_3 & 0xffff0000;
  }
  return CONCAT22((short)((uint)(&DAT_00a8e790)[param_1] >> 0x10),0xffff);
}

