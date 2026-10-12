
void FUN_0040a4c0(int param_1,int param_2,int param_3,uint param_4)

{
  int iVar1;
  
  iVar1 = FUN_0040a340();
  if (iVar1 != 1) {
    iVar1 = FUN_0040a340();
    if (iVar1 != 2) {
      *(uint *)(param_1 + 4) =
           -(uint)(param_2 != 0) & 0x400 | -(uint)(param_3 != 0) & 0x200 | param_4 & 0x9ff |
           0xe1000000;
      return;
    }
  }
  *(uint *)(param_1 + 4) =
       -(uint)(param_2 != 0) & 0x1000 | -(uint)(param_3 != 0) & 0x800 | param_4 & 0x27ff |
       0xe1000000;
  return;
}

