
void FUN_00404300(int param_1,int param_2)

{
  FUN_0040a340(param_1,0,0,0x140,0xf0);
  FUN_0040a340(param_2,0,0x100,0x140,0xf0);
  FUN_0040a340(param_1 + 0x5c,0,0x100,0x140,0xf0);
  FUN_0040a340(param_2 + 0x5c,0,0,0x140,0xf0);
  *(undefined2 *)(param_2 + 100) = 0;
  *(undefined2 *)(param_1 + 100) = 0;
  *(undefined2 *)(param_2 + 0x66) = 0;
  *(undefined2 *)(param_1 + 0x66) = 0;
  *(undefined2 *)(param_2 + 0x6a) = 0xf0;
  *(undefined2 *)(param_1 + 0x6a) = 0xf0;
  *(undefined2 *)(param_2 + 0x68) = 0;
  *(undefined2 *)(param_1 + 0x68) = 0;
  return;
}

