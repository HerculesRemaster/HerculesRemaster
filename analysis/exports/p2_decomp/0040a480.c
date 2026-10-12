
void FUN_0040a480(int param_1,int param_2)

{
  if (param_2 != 0) {
    *(byte *)(param_1 + 7) = *(byte *)(param_1 + 7) | 2;
    return;
  }
  *(byte *)(param_1 + 7) = *(byte *)(param_1 + 7) & 0xfd;
  return;
}

