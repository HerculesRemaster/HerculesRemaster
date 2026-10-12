
uint FUN_0040a3f0(uint param_1,uint param_2,int param_3,uint param_4)

{
  int iVar1;
  
  iVar1 = FUN_0040a340();
  if (iVar1 != 1) {
    iVar1 = FUN_0040a340();
    if (iVar1 != 2) {
      return ((param_2 & 3 | (param_1 & 3) << 2) << 3 | param_4 & 0x200) << 2 |
             (int)(param_4 & 0x100 | param_3 >> 2 & 0xf0U) >> 4;
    }
  }
  return (int)(param_3 >> 3 & 0x78U | param_4 & 0x300) >> 3 |
         (param_2 & 3 | (param_1 & 3) << 2) << 7;
}

