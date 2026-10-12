
short FUN_0040c040(undefined4 param_1,short param_2)

{
  if ((param_2 < -1) || (0xf < param_2)) {
    return -1;
  }
  if (param_2 == -1) {
    param_2 = 0;
    do {
      if ((&DAT_00a8e790)[param_2] == 0) break;
      param_2 = param_2 + 1;
    } while (param_2 < 0x10);
    if (0xf < param_2) {
      return -1;
    }
  }
  (&DAT_00a8e790)[param_2] = param_1;
  return param_2;
}

