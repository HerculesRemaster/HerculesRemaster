
void FUN_00404b10(int param_1,int param_2)

{
  for (; -1 < param_2; param_2 = param_2 + -1) {
    if ((int)(uint)(&DAT_00b9e85c)[param_2] < param_1) {
      (&DAT_00b9e85c)[param_2] = (char)param_1;
    }
  }
  if (9 < DAT_00b9e85c) {
    DAT_00b9e85c = 9;
  }
  return;
}

