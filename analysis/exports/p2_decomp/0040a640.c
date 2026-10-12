
uint FUN_0040a640(undefined4 param_1,uint param_2,uint param_3,int param_4,uint param_5,
                 undefined2 param_6,undefined2 param_7)

{
  int iVar1;
  undefined2 uStack_8;
  undefined2 uStack_6;
  undefined2 uStack_4;
  undefined2 uStack_2;
  
  uStack_4 = param_6;
  uStack_8 = (undefined2)param_4;
  uStack_6 = (undefined2)param_5;
  uStack_2 = param_7;
  FUN_0040a710(&uStack_8,param_1);
  iVar1 = FUN_0040a340();
  if (iVar1 != 1) {
    iVar1 = FUN_0040a340();
    if (iVar1 != 2) {
      return (((param_2 & 3) << 2 | param_3 & 3) << 3 | param_5 & 0x200) << 2 |
             (int)(param_4 >> 2 & 0xf0U | param_5 & 0x100) >> 4;
    }
  }
  return (int)(param_4 >> 3 & 0x78U | param_5 & 0x300) >> 3 |
         ((param_2 & 3) << 2 | param_3 & 3) << 7;
}

