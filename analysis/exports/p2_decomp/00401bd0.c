
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00401bd0(undefined2 param_1,undefined2 param_2,int param_3,int param_4,undefined2 param_5)

{
  FUN_00401d00();
  *(undefined2 *)(PTR_DAT_004361bc + 0x130) = param_1;
  *(undefined2 *)(PTR_DAT_004361bc + 0x132) = param_2;
  *(undefined2 *)(PTR_DAT_004361bc + 0x138) = param_5;
  FUN_00404300(&DAT_00ba2da0,&DAT_00ba2e1c);
  DAT_00ba2e34 = 0;
  DAT_00ba2db8 = 0;
  DAT_00ba2e32 = 1;
  DAT_00ba2db6 = 1;
  PTR_DAT_004361bc[0x134] = 0;
  PTR_DAT_004361bc[0x135] = 0;
  PTR_DAT_004361bc[0x136] = 0;
  _DAT_0043f748 = &DAT_00ba2da0;
  FUN_0040ac20(&DAT_00ba2e10,2);
  FUN_0040ac20(&DAT_00ba2e8c,2);
  PTR_DAT_004361bc[0x129] = 1;
  *(undefined2 *)(PTR_DAT_004361bc + 0x12e) = 0x80;
  *(undefined2 *)(PTR_DAT_004361bc + 300) = 0x80;
  if (param_4 < 0) {
    *(undefined2 *)(PTR_DAT_004361bc + 0x12e) = 0;
  }
  if (param_3 < 0) {
    *(undefined2 *)(PTR_DAT_004361bc + 300) = 0;
  }
  PTR_DAT_004361bc[0x12a] = (char)param_3;
  PTR_DAT_004361bc[299] = (char)param_4;
  return;
}

